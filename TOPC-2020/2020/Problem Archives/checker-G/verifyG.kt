import java.io.File
import kotlin.system.exitProcess
import kotlin.math.abs

fun testIf(v: Boolean, msg: String = "Should be EOF") {
    if (!v) throw Exception(msg)
}

fun verify(input: List<String>, ans: List<String>) {
    val (r,c) = input[0].split(" ").map{it.toInt()}
    val maxID = ans[0].toInt()
    val out = ArrayList<String>()
    var buf: String? 
    while(out.size <= r + 2) {
        buf = readLine()
        if (buf == null) break
        out.add(buf)
    }
    testIf(out.size == r+1, "Wrong number of lines")

    val outMaxID = out[0].toInt()    
    testIf(maxID <= outMaxID, "Not maximum")

    var cnt = 0
    for (i in 1..r) {
        testIf(out[i].length == c, "Wrong length of row $i$")
        for (j in 0 until c) {
            if (input[i][j] == '.'){
                if (out[i][j] == 'C') {
                    if (i < r) testIf(out[i+1][j] != 'C', "Adjacent! ($i,${j+1})-(${i+1},${j+1})")
                    if (j > 0) testIf(out[i][j-1] != 'C', "Adjacent! ($i,$j)-($i,${j+1})")
                    cnt++
                }
                else { // must be '.'
                    testIf(out[i][j] == '.', "Wrong at ($i,${j+1})")
                }
            }
            else { // must be '*'
                testIf(out[i][j] == '*', "Wrong at ($i,${j+1})")
            }
        }
    }
    testIf(cnt >= outMaxID, "not enough C")
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
