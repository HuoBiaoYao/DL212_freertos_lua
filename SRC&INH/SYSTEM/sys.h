#ifndef __SYS_H
#define __SYS_H	
#include "stm32l1xx.h"
 
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(unsigned int addr);	//���ö�ջ��ַ

#endif
