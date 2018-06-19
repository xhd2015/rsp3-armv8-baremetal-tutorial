# 关于
引入Descriptor4KBLx(x=0,1,2,3)等的结构,在include/arch/vmsa_descriptors.h目录下。

重新定义了RAM_SIZE。

本节引入了虚拟内存启用的整个过程，由于虚拟内存的调试困难，因此，我们可
使用QEMU的日志来记录异常信息，增加了-d int -D qemu.log的选项。

# 测试程序
建立0,1,2共3级页表，使用平面映射到高端地址

访问0xffffff_0000000000 高端地址，正确则意味着MMU启用成功了。

# 状态
已完成
树莓派：已测试
