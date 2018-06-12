#include "DL212.h"
#include "string.h" 
#include "stdlib.h"
#include "stdio.h"
#include "Datalogger_Protocols.h"
#include "arithmetic_expression.h"
#include "hal.h"
#include "hw.h"
#include "delay.h"
#include "stm32l1xx_it.h"

struct _NUM sNUM; 
 
float Var[7]; 
 
 
void Battery(double *para){ 
	*para = psSE_FUNC->bat_read();
}
  
void VGet(double *para){ 
  unsigned int dest,reps,chan,time,i; 
	
	dest = (unsigned int)(*para); 
	reps = (unsigned int)(*(para+1)); 
	chan = (unsigned int)(*(para+2)); 
	time = (unsigned int)(*(para+3)); 
	delay_ms(time); 
  for(i=0;i<reps;i++){ 
    Var[dest+i] = psSE_FUNC->read(chan+i); 
	} 
}
 
unsigned int  C1_Dest=0  ,C2_Dest=0  ,P_SW_Dest=0  ,F_Mea_Dest=0;
unsigned int  C1_Time=0  ,C2_Time=0  ,P_SW_Time=0  ,F_Mea_Time=0; 
unsigned char C1_Option=0,C2_Option=0,P_SW_Option=0,F_Mea_Option=0;
void PulseCount(double *para){ 
  unsigned int dest,pchan,config,option,time_ms,i,count; 
	
	/*dest = (unsigned int)(*para); 
	pchan = (unsigned int)(*(para+1)); 
	config = (unsigned int)(*(para+2)); 
	option = (unsigned int)(*(para+3)); 
	time_ms = (unsigned int)(*(para+4));*/ 
	dest = 1; pchan = 4; config = 1; option = 1; time_ms = 2000; 
	switch(pchan){
	  case 1://C1
			C1_Dest=dest,C1_Time=time_ms*2,C1_Option=(unsigned char)option;//C1上升下降沿都检测，所以time_ms*2用于计算
			TIM7_Init(time_ms); 
		break;
		case 2://C2
			C2_Dest=dest,C2_Time=time_ms,C2_Option=(unsigned char)option;
			TIM4_Init(time_ms); 
		break;
		case 3://P_SW
			P_SW_Dest=dest,P_SW_Time=time_ms*2,P_SW_Option=(unsigned char)option;//P_SW上升下降沿都检测，所以time_ms*2用于计算
			TIM6_Init(time_ms); 
		break;
		case 4://F_Mea
			F_Mea_Dest=dest,F_Mea_Time=time_ms*2,F_Mea_Option=(unsigned char)option;//F_Mea上升下降沿都检测，所以time_ms*2用于计算
		  psSE_FUNC->chan_open(1); 
		  TIM10_Init(time_ms); 
		break;
		case 5://F_Mea
			F_Mea_Dest=dest,F_Mea_Time=time_ms*2,F_Mea_Option=(unsigned char)option;
		  psSE_FUNC->chan_open(2); 
		  TIM10_Init(time_ms); 
		break;
		case 6://F_Mea
			F_Mea_Dest=dest,F_Mea_Time=time_ms*2,F_Mea_Option=(unsigned char)option;
		  psSE_FUNC->chan_open(3); 
		  TIM10_Init(time_ms); 
		break;
		case 7://F_Mea
			F_Mea_Dest=dest,F_Mea_Time=time_ms*2,F_Mea_Option=(unsigned char)option;
		  psSE_FUNC->chan_open(4); 
		  TIM10_Init(time_ms); 
		break;
		default:
		break;
	}  
} 
 
void ExciteV(double *para){ 
 
} 

void SDI12(double *para){
  unsigned int dest,sdiport,sdiaddress,sdicommand;
  
	
}
   
void SetFlagHigh(double *para){

}

void SetFlagLow(double *para){

}

 
 
float SquareRootFloat(float number){
	float x,y,*pt1;
  unsigned int i;
	int *pt2;  
  const float f = 1.5;  
	
  x = number * 0.5;
  y = number;
  pt2 = (int *)&y;
  i = *pt2;
  i  = 0x5f3759df - ( i >> 1 );//0x5f375a86可能更好，没测试过
  pt1=(float *)&i;
  y = *pt1;
  y  = y * ( f - ( x * y * y ) );
  y  = y * ( f - ( x * y * y ) );
  y = 1/y;

  return  y;
}



