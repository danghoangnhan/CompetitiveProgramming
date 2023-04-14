fun main() = readLine()!!.split(" ").zip(listOf(56,24,14,6)).map{(x,y) -> x.toInt()*y}.sum().let{println(it)}
