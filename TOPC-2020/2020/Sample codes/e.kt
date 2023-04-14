fun hamming(x: List<Int>, y: List<Int>) = (0..19).count{x[it] != y[it]}

fun gray(p: Int): List<Int> {
    if (p == 1) return listOf(0,1)
    val code = gray(p-1)
    return ArrayList<Int>(code).apply{
        addAll(code.map{it+1.shl(p-1)}.asReversed())}
}

fun solve(src: List<Int>, dst: List<Int>, step: Int, ham: Int) {
    val buf = StringBuilder()
    val s = step - ham
    val h = s / 2
    var diff = ArrayList<Int>((0..19).filter{src[it] != dst[it]})
    val index = ArrayList<Int>((0..19).toList())
    index[18] = diff[0].also{index[diff[0]] = 18}
    if (diff.size > 1 && 19 !in diff) {
        index[19] = if (diff.last() == 18) diff[0] else diff.last()
        index[diff.last()] = 19
    }
    val code = gray(19)
    for (i in 1..h) {
        val pat = (0..19).map{code[i].ushr(it).and(1)}
        buf.append((0..19).map{src[it] xor pat[index[it]]}.joinToString(""))
        buf.append("\n")
    }
    for (i in (1.shl(19)-h-1) until (1 shl 19)) {
        val pat = (0..19).map{code[i].ushr(it).and(1)}
        buf.append((0..19).map{src[it] xor pat[index[it]]}.joinToString(""))
        buf.append("\n")
    }
    val cur = ArrayList<Int>(src)
    cur[diff[0]] = 1 - cur[diff[0]]
    for (i in diff.reversed()) {
        if (cur[i] != dst[i]) {
            cur[i] = 1 - cur[i]
            buf.append(cur.joinToString(""))
            buf.append("\n")
        }
    }
    print(buf)
}

fun main() {
    val src = readLine()!!.map{it - '0'}
    val dst = readLine()!!.map{it - '0'}
    var step = readLine()!!.toInt()
    val h = hamming(src, dst)
    when {
        h > step -> { println("-1") }
        (h - step) % 2 != 0 -> { println("-1") }
        h == 0 -> { return }
        else -> {solve(src, dst, step,h)}
    }
}
