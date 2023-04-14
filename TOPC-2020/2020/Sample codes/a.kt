fun main() {
    var (x, y) = readLine()!!.split(" ").map{it.toInt()}
    while (x != 0 || y != 0) {
        println("$x $y")
        if (x > 0) x-- else y--
    }
    for (i in 0..8 step 2) {
        for (j in 0..9) println("$i $j")
        for (j in 9 downTo 0) println("${i+1} $j")
    }
}
