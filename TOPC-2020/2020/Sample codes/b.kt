fun main() {
    val (p1, q1, p2, q2) = readLine()!!.split(" ").map{it.toLong()}
    val q = readLine()!!.toInt()
    for (rnd in 1..q) {
        val x = readLine()!!.toLong()
        val (a, b) = when {
            x > q1 -> Pair(x - q1, x - p1)
            x < p1 -> Pair(p1 - x, q1 - x)
            else -> Pair(0L, maxOf(x - p1, q1 - x))
        }
        val (c, d) = when {
            x > q2 -> Pair(x - q2, x - p2)
            x < p2 -> Pair(p2 - x, q2 - x)
            else -> Pair(0L, maxOf(x - p2, q2 - x))
        }
        val ans = if (b <= c || d <= a) b-a + d-c else maxOf(b, d) - minOf(a, c)
        println(ans)
    }
}
