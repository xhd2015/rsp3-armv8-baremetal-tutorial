# 关于
定义了MemoryChunk和MemoryManager类，MemoryManager目前不包括reallocate函数。

实现了许多C语言的函数：assert宏的函数`__assert_func`(GNU Header), `abort`(cstdlib)

新增了gcd和lcm两个数学函数,alignAhead,alignBackward函数

重新定义了链接脚本，定义了汇编符号\_\_free\_ram\_start等

新增了`CALL_CONST_EQUIV`宏

# 状态
已完成