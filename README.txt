Xilinx Common Bare-Metal API

1. 要啥模块在 bsp.h 打开啥模块，打开了如果不用也不影响编译结果，基本的配置集中到 config.h
2. 根据 bsp 信息，使能相应的模块或功能，尽可能兼容 microblaze/zynq/zynqmp/r5/ppc
3. 各模块尽量解耦，可调用函数接口和全局变量放在头文件(extern)，模块内尽量用static函数

* clk_wiz 不做自动计算
* csi_rx 不初始化也可以正常用，只是没有初始化后的句柄
* vdma 这里没有放中断设置和处理代码，一般用不需要
* vtc 还没有利用xvidc已经定义的时序，可以改进为使用库定义的相关用法
* xgpio_i2c 在linux环境使用不如xiic方便
* qspi_flash 改进
* IAP升级只保留两种简单升级，暂不进行md5校验等升级交互。也暂不进行分区升级和指定地址升级。这样可以直接用 python 脚本或 NetAssist.exe 进行升级，网络环境时应应该比较好。
* 上位机没有必要做直接地址读写操作，可以保留eeprom有关的设置和 配置表 更新保存编辑什么的
* 还是有必要做golden区，放一个通用的可升级固件可打印无其他功能的程序
* IAP 刷新之后复位控制添加
* Modbus(主/从)，Can 通讯等添加
* 串口打印菜单的设计, 修改 eeprom有关的设置和 配置表，i2c/spi等调试操作
* 
* 
* 
* 
