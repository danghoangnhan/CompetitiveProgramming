import java.io.File
import kotlin.system.exitProcess
import kotlin.math.abs

fun testIf(v: Boolean, msg: String = "Should be EOF") {
    if (!v) throw Exception(msg)
}

fun ham(s: String, t: String): Int {
    var ret = 0
    for (i in 0 until 20) 
        if (s[i] != t[i])
            ret++
    return ret
}

fun verify(input: List<String>, ans: List<String>) {
    val src = input[0]
    val dest = input[1]
    val D = input[2].toInt()
    val out = ArrayList<String>()
    var buf: String?
    while (out.size <= D+1) {
        buf = readLine()
        if (buf == null) break
        out.add(buf)
        if (buf == "-1") continue
        testIf(buf.length == 20, "Wrong length")
        testIf(buf.all{it == '0' || it == '1'}, "Non-binary")
    }
    if (ans[0] == "-1" && out.size == 1 && out[0] == "-1") return // accept
    testIf(out.size == D, "Wrong number of steps")
    testIf(ham(src, out[0]) == 1, "Wrong first step: ham($src,${out[0]})==${ham(src,out[0])}")
    testIf(dest == out[D-1], "Wrong last step")
    for (i in 1 until D) 
        testIf(ham(out[i],out[i-1]) == 1, "Wrong step $i")
    testIf(out.toSet().size == D, "Repeating")
}

// usage: judge_in judge_ans feedback_dir [options] < team_out

fun main(args: Array<String>) {
    try{
        verify(File(args[0]).readLines(), File(args[1]).readLines())
    } catch (e: Exception) {
        println("Wrong Answer")
        println("$e")
        exitProcess(43)
    }
    println("Correct")
    exitProcess(42)
}
