#include <msp430.h>
#include <stdio.h>

#define ADC10CTL0_INIT (ADC10SHT_2 + ADC10ON + ADC10IE) // ADC10 ���ƼĴ��� 0 �ĳ�ʼֵ
#define ADC10CTL1_INIT (INCH_11 + ADC10DIV_3) // ADC10 ���ƼĴ��� 1 �ĳ�ʼֵ
#define CALBC1_INIT CALBC1_1MHZ // У׼ DCOCLKDIV Ƶ��Ϊ 1MHz �ĳ�ʼֵ
#define CALDCO_INIT CALDCO_1MHZ // У׼ DCOCLK Ƶ��Ϊ 1MHz �ĳ�ʼֵ
#define UCA0BR0_INIT 104 // USCI_A0 �����ʿ��ƼĴ��� 0 �ĳ�ʼֵ��������Ϊ 9600
#define UCA0BR1_INIT 0 // USCI_A0 �����ʿ��ƼĴ��� 1 �ĳ�ʼֵ��������Ϊ 9600
#define UCA0MCTL_INIT UCBRS0 // USCI_A0 ���ƿ��ƼĴ����ĳ�ʼֵ��������Ϊ 9600
#define TACCR0_INIT 10000 // ��ʱ�� A ����/�ȽϼĴ��� 0 �ĳ�ʼֵ����ʱ������Ϊ 10ms

void init(void); // ��ʼ�� MSP430G2553 �ĺ���
void startADC(void); // ���� ADC ת���ĺ���
void sendByte(unsigned char byte); // ����һ���ֽڵ����ڵĺ���
void sendString(char *string); // ����һ���ַ��������ڵĺ���

unsigned int adcResult; // �洢 ADC ת������ı���
char buffer[16]; // �洢��ʽ������Ļ�����

int main(void)
{
    init(); // ��ʼ�� MSP430G2553
    __enable_interrupt(); // ����ȫ���ж�

    while(1)
    {
        startADC(); // ���� ADC ת��
        __bis_SR_register(CPUOFF + GIE); // ����͹���ģʽ���ȴ� ADC �жϻ���
        sprintf(buffer, "%u\r\n", adcResult); // �� ADC �����ʽ��Ϊ�ַ������Իس����н�β
        sendString(buffer); // �����ַ���������
    }
}

// ��ʼ�� MSP430G2553 �ĺ���
void init(void)
{
    WDTCTL = WDTPW + WDTHOLD; // ֹͣ���Ź���ʱ��

    if (CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xFF) // ���У׼�����Ƿ񱻲���
    {
        while(1); // ����ǣ��������ѭ��
    }

    BCSCTL1 = CALBC1_INIT; // ���� DCOCLKDIV Ƶ��
    DCOCTL = CALDCO_INIT; // ���� DCOCLK Ƶ��

    P1SEL = BIT1 + BIT2 ; // ���� P1.1 �� P1.2 Ϊ UART ģʽ
    P1SEL2 = BIT1 + BIT2 ; // ���� P1.1 �� P1.2 Ϊ UART ģʽ

    UCA0CTL1 |= UCSSEL_2; // ���� USCI_A0 ʱ��ԴΪ SMCLK
    UCA0BR0 = UCA0BR0_INIT; // ���� USCI_A0 �����ʿ��ƼĴ��� 0
    UCA0BR1 = UCA0BR1_INIT; // ���� USCI_A0 �����ʿ��ƼĴ��� 1
    UCA0MCTL = UCA0MCTL_INIT; // ���� USCI_A0 ���ƿ��ƼĴ���
    UCA0CTL1 &= ~UCSWRST; // ��ʼ�� USCI_A0 ״̬��

    ADC10CTL0 = ADC10CTL0_INIT; // ���� ADC10 ���ƼĴ��� 0
    ADC10CTL1 = ADC10CTL1_INIT; // ���� ADC10 ���ƼĴ��� 1

    TACCR0 = TACCR0_INIT; // ���ö�ʱ�� A ����/�ȽϼĴ��� 0
    TACCTL0 = CCIE; // ������ʱ�� A ����/�Ƚ��ж�
    TACTL = TASSEL_2 + MC_1; // ���ö�ʱ�� A ʱ��ԴΪ SMCLK������ģʽΪ������ģʽ
}

// ���� ADC ת���ĺ���
void startADC(void)
{
    ADC10CTL0 |= ENC + ADC10SC; // ʹ�� ADC ת������ʼ����
}

// ����һ���ֽڵ����ڵĺ���
void sendByte(unsigned char byte)
{
    while (!(IFG2 & UCA0TXIFG)); // �ȴ� USCI_A0 ���ͻ�����Ϊ��
    UCA0TXBUF = byte; // ���ֽ�д�� USCI_A0 ���ͻ�����
}

// ����һ���ַ��������ڵĺ���
void sendString(char *string)
{
    while (*string) // �����ַ����е�ÿ���ַ�
    {
        sendByte(*string++); // ���͵�ǰ�ַ���ָ����һ���ַ�
    }
}

// ADC10 �жϷ������
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    adcResult = ADC10MEM; // ��ȡ ADC ת�����
    __bic_SR_register_on_exit(CPUOFF); // �˳��͹���ģʽ
}

// ��ʱ�� A �жϷ������
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    startADC(); // ���� ADC ת��
}
