# MSP430g2553_VoltTrack
## 1.时钟
### 介绍
MSP430G2553的时钟模块是一个复杂而灵活的系统，它可以为CPU和片上外设提供不同的时钟源和输出。时钟模块主要包括以下几个部分：
### 时钟模块
四个时钟源：内部低速振荡器 (VLO)、外部低速时钟振荡器 (LFXT1)、外部高速时钟振荡器 (XT2)和数控时钟振荡器 (DCO)。这些时钟源可以通过配置相应的寄存器来选择和调节，以满足不同的功耗和精度要求。（我们用的MSP430G2553是没有XT2CLK时钟源的）

• LFXT1CLK：低频/高频振荡器，可与 32768Hz 的低频手表晶体或外部时钟源或者 400kHz 至 16MHz 范围内
的标准晶体、谐振器或外部时钟源一同使用。

• XT2CLK：可选高频振荡器，可与 400kHz 至 16MHz 范围内的标准晶体、谐振器或外部时钟源一同使用。

• DCOCLK：内部数控振荡器 (DCO)。

• VLOCLK：具有 12kHz 典型频率的内部超低功耗低频振荡器。

### 时钟输出
三个时钟输出：主系统时钟 (MCLK)、子系统时钟 (SMCLK)和辅助时钟 (ACLK)。这些时钟输出可以从任意一个时钟源获取，并可以进行进一步的分频，以适应不同的设备工作频率。MCLK为CPU提供运行时钟，SMCLK为高速设备提供高速时钟，ACLK为低速设备提供低速时钟。

• ACLK：辅助时钟。ACLK 是由软件选择来作为 LFXT1CLK 或 VLOCLK。ACLK 进行 1、2、4 或 8 分频。
ACLK 可由软件选作各个外围模块。

• MCLK：主时钟。可通过软件将 MCLK 选择为 LFXT1CLK、VLOCLK、XT2CLK（如果片上可用）或 
DCOCLK。MCLK 进行 1、2、4 或 8 分频。MCLK 用于 CPU 和系统。

• SMCLK：子主时钟。可通过软件将 SMCLK 选择为 LFXT1CLK、VLOCLK、XT2CLK（如果片上可用）或 
DCOCLK。SMCLK 进行 1、2、4 或 8 分频。可通过软件针对各个外设模块选择 SMCLK。

### 寄存器
-   四个时钟寄存器：DCOCTL、BCSCTL1、BCSCTL2和BCSCTL3。这些寄存器用于控制时钟源的选择、分频、开关和稳定性等参数。
地址 首字母缩写   寄存器名称         类型           复位                
56h   DCOCTL   DCO 控制           读取/写入     PUC 后为 60h        
57h   BCSCTL1 基本时钟系统控制 1   读取/写入     POR 后为 87h
58h   BCSCTL2 基本时钟系统控制 2   读取/写入     PUC 后为 00h    
53h   BCSCTL3 基本时钟系统控制 3   读取/写入     PUC 后为 05h
00h   IE1     SFR 中断使能 1      读取/写入     PUC 后为 00h 
02h   IFG1    SFR 中断标志 1      读取/写入     PUC 后为 02h 
### 频率点
官方校准好的频率点有1MHZ,8MHZ,12MHZ,16MHZ,用这些官方频率点可以获得一个比较稳定的频率，具体配置为:

> DCOCTL = CALDCO_16MHZ;
> BCSCTL1 = CALBC1_16MHZ;
> //可以把DCO频率设置为16MHZ
