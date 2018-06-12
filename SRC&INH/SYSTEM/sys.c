#include "sys.h"
#include "stdio.h"

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(unsigned int addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
//#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//__use_no_semihosting was requested, but _ttywrch was 
void _ttywrch(int ch)
{
ch = ch;
}
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
  while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (unsigned char) ch;      
	return ch;
}
#endif 
