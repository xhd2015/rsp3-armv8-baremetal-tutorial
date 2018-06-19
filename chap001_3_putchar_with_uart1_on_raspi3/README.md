# 关于
本节主要引入assert,abort,puts,MemoryMappedDevice，GPIO，delayCPU,位操作等一系列函数，使得树莓派
能够暂时输出信息。但是正确的输出还需要额外的初始化。

注意：从此工程开始，后面的工程均能通过make install将文件复制到
树莓派上运行。注意，连接树莓派与电脑需要参见额外的文档。

注意：这里必须引入assert了，因为assert是后面一些重要函数的基础。


# 状态
已完成