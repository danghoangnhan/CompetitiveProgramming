import java.io.File
import kotlin.system.exitProcess
import kotlin.math.abs

fun testIf(v: Boolean, msg: String = "Should be EOF") {
    if (!v) throw Exception(msg)
}

fun verify(input: List<String>) { //, ans: List<String>) {
    val out = ArrayList<String>()
    var buf: String?
    while (out.size <= 10_002) {
        buf = readLine()
        if (buf == null) break
        out.add(buf)
        testIf(buf.split(" ").size <= 2, "More than two tokens")
    }
    testIf(out.size <= 10_001, "Too long")
    val (initX, initY) = input[0].split(" ").map{it.toInt()}
    var (prevX, prevY) = out[0].split(" ").map{it.toInt()}
    testIf(initX == prevX && initY == prevY, "Wrong beginning position")
    val visited = HashSet<String>()
    visited.add("$prevX $prevY")
    for (i in 1 until out.size) {
        var (x, y) = out[i].split(" ").map{it.toInt()}
        testIf(x in 0..9 && y in 0..9, "out of board")
        testIf(abs(x-prevX) + abs(y-prevY) == 1, "not adjacent")
        visited.add("$x $y")
        prevX = x
        prevY = y
    }
    testIf(visited.size == 100, "Some cell is not visited")
}

// usage: judge_in judge_ans feedback_dir [options] < team_out

fun main(args: Array<String>) {
    try{
        verify(File(args[0]).readLines()) //, File(args[1]).readLines())
    } catch (e: Exception) {
        println("Wrong Answer")
        println("$e")
        exitProcess(43)
    }
    println("Correct")
    exitProcess(42)
}
