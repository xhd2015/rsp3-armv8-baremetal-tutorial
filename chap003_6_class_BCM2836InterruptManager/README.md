# 关于
树莓派3的中断控制器分成两个部分：一个是BCM2835的控制器，一个是BCM2836的控制器。

BCM2836是主控制器，大部分主要中断来自于BCM2835，由BCM2836控制。

引自BCM2836文档：
```
The ARM control logic is a module which performs the following functions:
 Provide a 64-bit Timer
 Route the various interrupts to the cores
 Route the GPU interrupts to the core
 Provide mailboxes between the processors
 Provide extra interrupt timer
```

ARM控制逻辑实际上是把来自GPU的中断路由到CPU。

需要增加这个的原因在于，中断只有一个，而CPU有多个，需要确定路由到
哪一个。

就目前而言，我们都是默认路由到CPU0，因此无需BCM2836InterruptManager也可以。
所以本章暂时跳过。

# 状态
跳过，以后若有需要再完成。