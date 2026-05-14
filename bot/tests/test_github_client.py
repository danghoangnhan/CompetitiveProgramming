import httpx
import pytest

from bot.github_issue import create_issue


class FakeTransport(httpx.BaseTransport):
    def __init__(self, response_factory):
        self._factory = response_factory
        self.calls: list[httpx.Request] = []

    def handle_request(self, request: httpx.Request) -> httpx.Response:
        self.calls.append(request)
        return self._factory(request, len(self.calls))


def test_create_issue_posts_to_correct_url_with_token():
    transport = FakeTransport(
        lambda req, n: httpx.Response(201, json={"number": 42}, request=req)
    )
    client = httpx.Client(transport=transport, base_url="https://api.github.com")
    n = create_issue(
        client,
        repo="owner/repo",
        token="t0ken",
        title="hello",
        body="world",
        labels=["a", "b"],
    )
    assert n == 42
    req = transport.calls[0]
    assert str(req.url) == "https://api.github.com/repos/owner/repo/issues"
    assert req.headers["Authorization"] == "Bearer t0ken"
    assert req.headers["Accept"] == "application/vnd.github+json"
    import json as _json
    payload = _json.loads(req.content)
    assert payload == {"title": "hello", "body": "world", "labels": ["a", "b"]}


def test_create_issue_retries_on_5xx_then_succeeds():
    def factory(req, n):
        if n < 3:
            return httpx.Response(503, request=req)
        return httpx.Response(201, json={"number": 99}, request=req)

    transport = FakeTransport(factory)
    client = httpx.Client(transport=transport, base_url="https://api.github.com")
    n = create_issue(client, "owner/repo", "t", "x", "y", labels=[], sleep=lambda s: None)
    assert n == 99
    assert len(transport.calls) == 3


def test_create_issue_raises_after_exhausting_retries():
    transport = FakeTransport(lambda req, n: httpx.Response(503, request=req))
    client = httpx.Client(transport=transport, base_url="https://api.github.com")
    with pytest.raises(RuntimeError, match="GitHub create_issue failed after 3 attempts"):
        create_issue(client, "owner/repo", "t", "x", "y", labels=[], sleep=lambda s: None)


def test_create_issue_does_not_retry_on_4xx():
    transport = FakeTransport(lambda req, n: httpx.Response(422, json={"message": "bad"},
                                                            request=req))
    client = httpx.Client(transport=transport, base_url="https://api.github.com")
    with pytest.raises(RuntimeError, match="GitHub create_issue failed: 422"):
        create_issue(client, "owner/repo", "t", "x", "y", labels=[], sleep=lambda s: None)
    assert len(transport.calls) == 1
