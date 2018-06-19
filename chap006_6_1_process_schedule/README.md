# 关于
本节引入了VirtualMap的改进版本，该VirtualMap基于L3可对页表进行扩展，满足一般性的内存扩展需求。

本节引入了两个基本的VirtualMap子类：SimpleExtensiveVirtualMap -- 根据L2的表项数目扩展内存页
ProcessVirtualMap -- 前者的一个具体化类型，将页表分为多个部分


SimpleExtensiveVirtualMap主要方法是increaseMap和increasePages
前者用于将已有内存映射到当前虚拟地址的尾部，后者则申请新的内存页。

引入数学函数ceilDev

# 测试程序

# 状态
已完成
树莓派：已测试