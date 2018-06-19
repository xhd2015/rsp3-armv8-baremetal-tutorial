# 关于
新增了createShell系统调用和众多vfs管理命令

改变了ProcessVirtualMap renew的返回值

新增了memcpy

MemoryManager改变了检查逻辑，仅当初始化时检查。

# 测试程序

# 状态
已完成
树莓派：未测试

# 构建
没有Debug目录时,按照下面的顺序构建：3-prepare 1-prepare 2-prepare 1-output.bin