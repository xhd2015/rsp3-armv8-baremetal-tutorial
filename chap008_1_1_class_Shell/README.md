# 关于
在内核init代码中，增加了允许访问时钟的控制，包括EL2和EL1两个级别的控制。

loader中增加了允许EL0访问时钟的权限，实现sleep命令

引入stoi, strlen

loader中加入strlen.cpp的引用（因为generic/util.cpp使用到)

改变了VirtualFile，VirtualFileSystem的部分逻辑。

在Process的析构中，增加了_pid的回收。
# 测试程序

# 状态
已完成
树莓派：已测试

# 构建
没有Debug目录时,按照下面的顺序构建：3-prepare 1-prepare 2-prepare 1-output.bin