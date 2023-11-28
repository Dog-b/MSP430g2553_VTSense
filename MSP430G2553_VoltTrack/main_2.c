#include <msp430.h>
#include <stdio.h>

#define ADC10CTL0_INIT (ADC10SHT_2 + ADC10ON + ADC10IE) // ADC10 控制寄存器 0 的初始值
#define ADC10CTL1_INIT (INCH_10 + ADC10DIV_3) // ADC10 控制寄存器 1 的初始值
#define CALBC1_INIT CALBC1_1MHZ // 校准 DCOCLKDIV 频率为 1MHz 的初始值
#define CALDCO_INIT CALDCO_1MHZ // 校准 DCOCLK 频率为 1MHz 的初始值
#define UCA0BR0_INIT 104 // USCI_A0 波特率控制寄存器 0 的初始值，波特率为 9600
#define UCA0BR1_INIT 0 // USCI_A0 波特率控制寄存器 1 的初始值，波特率为 9600
#define UCA0MCTL_INIT UCBRS0 // USCI_A0 调制控制寄存器的初始值，波特率为 9600
#define TACCR0_INIT 10000 // 定时器 A 捕获/比较寄存器 0 的初始值，定时器周期为 10ms

void init(void); // 初始化 MSP430G2553 的函数
void startADC(void); // 启动 ADC 转换的函数
void sendByte(unsigned char byte); // 发送一个字节到串口的函数
void sendString(char *string); // 发送一个字符串到串口的函数

unsigned int adcResult; // 存储 ADC 转换结果的变量
char buffer[16]; // 存储格式化输出的缓冲区

int main(void)
{
    init(); // 初始化 MSP430G2553
    __enable_interrupt(); // 开启全局中断

    while(1)
    {
        startADC(); // 启动 ADC 转换
        __bis_SR_register(CPUOFF + GIE); // 进入低功耗模式，等待 ADC 中断唤醒
        sprintf(buffer, "%u\r\n", adcResult); // 将 ADC 结果格式化为字符串，以回车换行结尾
        sendString(buffer); // 发送字符串到串口
    }
}

// 初始化 MSP430G2553 的函数
void init(void)
{
    WDTCTL = WDTPW + WDTHOLD; // 停止看门狗定时器

    if (CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xFF) // 检查校准常数是否被擦除
    {
        while(1); // 如果是，则进入死循环
    }

    BCSCTL1 = CALBC1_INIT; // 设置 DCOCLKDIV 频率
    DCOCTL = CALDCO_INIT; // 设置 DCOCLK 频率

    P1SEL = BIT1 + BIT2 ; // 设置 P1.1 和 P1.2 为 UART 模式
    P1SEL2 = BIT1 + BIT2 ; // 设置 P1.1 和 P1.2 为 UART 模式

    UCA0CTL1 |= UCSSEL_2; // 设置 USCI_A0 时钟源为 SMCLK
    UCA0BR0 = UCA0BR0_INIT; // 设置 USCI_A0 波特率控制寄存器 0
    UCA0BR1 = UCA0BR1_INIT; // 设置 USCI_A0 波特率控制寄存器 1
    UCA0MCTL = UCA0MCTL_INIT; // 设置 USCI_A0 调制控制寄存器
    UCA0CTL1 &= ~UCSWRST; // 初始化 USCI_A0 状态机

    ADC10CTL0 = ADC10CTL0_INIT; // 设置 ADC10 控制寄存器 0
    ADC10CTL1 = ADC10CTL1_INIT; // 设置 ADC10 控制寄存器 1
    ADC10CTL0|=SREF_1+REFON;
    ADC10CTL0|= ADC10SHT_3+MSC; //过采样率设置为 64 个采样周期，打开 AD 转换

    TACCR0 = TACCR0_INIT; // 设置定时器 A 捕获/比较寄存器 0
    TACCTL0 = CCIE; // 开启定时器 A 捕获/比较中断
    TACTL = TASSEL_2 + MC_1; // 设置定时器 A 时钟源为 SMCLK，工作模式为增计数模式
}

// 启动 ADC 转换的函数
void startADC(void)
{
    ADC10CTL0 |= ENC + ADC10SC; // 使能 ADC 转换并开始采样
}

// 发送一个字节到串口的函数
void sendByte(unsigned char byte)
{
    while (!(IFG2 & UCA0TXIFG)); // 等待 USCI_A0 发送缓冲区为空
    UCA0TXBUF = byte; // 将字节写入 USCI_A0 发送缓冲区
}

// 发送一个字符串到串口的函数
void sendString(char *string)
{
    while (*string) // 遍历字符串中的每个字符
    {
        sendByte(*string++); // 发送当前字符并指向下一个字符
    }
}

// ADC10 中断服务程序
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    adcResult = ADC10MEM; // 读取 ADC 转换结果
    __bic_SR_register_on_exit(CPUOFF); // 退出低功耗模式
}

// 定时器 A 中断服务程序
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    startADC(); // 启动 ADC 转换
}
