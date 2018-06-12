#include "main.h" 
#include "hw.h"
#include "hal.h"
#include "delay.h" 
#include "task.h"
#include "semphr.h"
#include "my_usb.h"
#include "DL212_easy_mode.h"
 #include "math.h"
//Modbus Includes
#include "usart_idle_dma_modbus.h"   
#include "portfunction.h"
//lua includes
#include "lua_task.h"
 
TaskHandle_t Task_Start_Handler; 
TaskHandle_t Task1_Handler; 
TaskHandle_t Task2_Handler; 
TaskHandle_t Task3_Handler; 
TaskHandle_t QueryTask_Handler; 

QueueHandle_t xQueue; 
SemaphoreHandle_t BinarySemaphore_MB; 
SemaphoreHandle_t BinarySemaphore_USB;  
SemaphoreHandle_t xSemaphore; 

TimerHandle_t OneShotTimer_Handle; 

int main(void){ 
	unsigned int i; 
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //����ϵͳ�ж����ȼ�����4	 	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE); 
	USART1_Config(9600);
	delay_init(); //��ʱ������ʼ��   
	//delay_ms(3000); 
  //DBGMCU_Config(DBGMCU_SLEEP,ENABLE);
	//������ʼ����
  xTaskCreate((TaskFunction_t )Task_Start,           //������
              (const char*    )"start_task",         //��������
              (uint16_t       )32,                   //�����ջ��С
              (void*          )NULL,                 //���ݸ��������Ĳ���
              (UBaseType_t    )1,                    //�������ȼ�
              (TaskHandle_t*  )&Task_Start_Handler); //������              
  vTaskStartScheduler();          //�����������

  return 0;
} 

//��ʼ����������
void Task_Start(void *pvParameters){
  taskENTER_CRITICAL();           //�����ٽ���
  xQueue = xQueueCreate(10,sizeof(char));
	BinarySemaphore_MB = xSemaphoreCreateBinary(); 
  BinarySemaphore_USB = xSemaphoreCreateBinary(); 
 	xSemaphore = xSemaphoreCreateMutex(); 
  if(xQueue==NULL || BinarySemaphore_MB==NULL || BinarySemaphore_USB==NULL || xSemaphore==NULL){
	  while(1); 
	} 
	My_USB_Init(); 
	psSE_FUNC->init(); 
	psFram_Func->init(); 
  psFlash_Func->init();  
	psSW12_Func->init();//SW12���ƿڳ�ʼ��
	psC_Pulse_Func->init(0);//C1��Ĭ�ϳ�ʼ��Ϊ�������
	psC_Pulse_Func->init(1);//C2��Ĭ�ϳ�ʼ��Ϊ�������  
 // sDL212_State.battery = psSE_FUNC->bat_read();
 // psSE_FUNC->vref_read(); 
  I2C_RTC_Init();
  TIM2_TI2FP2_Init();//PSW-----PA1(TIM2_CH2)
  TIM3_ETR_Init();   //C2------PD2(TIM3_ETR)
  TIM9_TI2FP2_Init();//C1------PA3(TIM9_CH2) 
	TIM5_TI1FP1_Init();//F_Mea---PA0(TIM5_CH1)  
	/*SDI12_C1_SEND_DISABLE();
	psSDI12_Func->init(0);
	psSDI12_Func->init(1);
	psC_RS232_Func->init(0,0);*/  
	OneShotTimer_Handle = xTimerCreate((const char*)"OneShotTimer",
		                                 (TickType_t)200,//200ms
	                                   (UBaseType_t)pdFALSE,
	                                   (void*)2,
															       (TimerCallbackFunction_t)OneShotCallback);
    xTaskCreate((TaskFunction_t )Task1, 
                (const char*    )"task1", 
                (uint16_t       )128, 
                (void*          )NULL, 
                (UBaseType_t    )1,	
                (TaskHandle_t*  )&Task1_Handler); 

    xTaskCreate((TaskFunction_t )Task2,
                (const char*    )"task2",   
                (uint16_t       )128, 
                (void*          )NULL,
                (UBaseType_t    )1,
                (TaskHandle_t*  )&Task2_Handler); 
	  xTaskCreate((TaskFunction_t )Task3,     
                (const char*    )"task3",   
                (uint16_t       )512,  
                (void*          )NULL,
                (UBaseType_t    )1,
                (TaskHandle_t*  )&Task3_Handler); 
		/*xTaskCreate((TaskFunction_t )Query_Task,     
                (const char*    )"query_task",   
                (uint16_t       )128,
                (void*          )NULL,
                (UBaseType_t    )3,
                (TaskHandle_t*  )&QueryTask_Handler);*/ 						
    vTaskDelete(Task_Start_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

unsigned int DL212_EasyMode_Scan_Count=0; 
void Task1(void *pvParameters){ 
	TickType_t xLastWakeTime; 
	const TickType_t xPeriod=pdMS_TO_TICKS(1000); 
	
	DL212_EasyMode_Init();
	xLastWakeTime = xTaskGetTickCount(); 
  while(1){ 
    vTaskDelayUntil(&xLastWakeTime,xPeriod); 
		if(DL212_EasyMode){
      DL212_EasyMode_Scan(); 
      DL212_EasyMode_Scan_Count++; 
			memcpy(usRegInputBuf,sEMData.value,28);
		}
	} 
} 

void Task2(void *pvParameters){ 
	BaseType_t err=pdFALSE; 

  while(1){ 
    if(USB_DETECT()){  
      if(bDeviceState == CONFIGURED){ 
        CDC_Receive_DATA();
        if(sUSB_Para.packet_rec){
          DL212_EasyMode_Config(); 
          //USB_Send(sUSB_Para.rx_buf,sUSB_Para.rec_len); 
          sUSB_Para.packet_rec = 0; 
          sUSB_Para.rec_len = 0; 
        } 
      } 
	  } 
		else{ 
		  if(BinarySemaphore_USB != NULL){ 
        if(xSemaphoreTake(BinarySemaphore_USB,portMAX_DELAY) == pdTRUE){ 
	        SystemInit(); 
				} 
			} 
		} 
  } 
} 
 
void Task3(void *pvParameters){
	portTickType xLastWakeTime; 
  unsigned char i=0;
	
  if(sEMData.usart_baudrate){
	  sMBSlave.init(sEMData.usart_baudrate);   
	}
	else{
	  sMBSlave.init(9600); 
	}
  lua_main();
	while(1){ 
	  if(BinarySemaphore_MB != NULL){ 
      if(xSemaphoreTake(BinarySemaphore_MB,portMAX_DELAY) == pdTRUE){ 
				if(1 == sEMData.usart_mode){
					ucMBAddress = sEMData.modbus_address;
				  sMBSlave.poll();
				}
        else{
				  memset(USART_DMA_RxBuf,0,USART1_DMA_Rec_Cnt);
				  USART1_DMA_Rec_Cnt =0;
				}
	    }
      else{
			  vTaskDelay(1000);
			}
    }
	}
} 

char InfoBuffer[1];				//������Ϣ������
void Query_Task(void *pvParameters){
  unsigned int TotalRunTime;
	UBaseType_t ArraySize,x;
	TaskStatus_t *StatusArray;
	
	//��һ��������uxTaskGetSystemState()��ʹ��
	printf("/********��һ��������uxTaskGetSystemState()��ʹ��**********/\r\n");
	ArraySize=uxTaskGetNumberOfTasks();		//��ȡϵͳ��������
	StatusArray=pvPortMalloc(ArraySize*sizeof(TaskStatus_t));//�����ڴ�
	if(StatusArray!=NULL)					//�ڴ�����ɹ�
	{
		ArraySize=uxTaskGetSystemState((TaskStatus_t* 	)StatusArray, 	//������Ϣ�洢����
									   (UBaseType_t		)ArraySize, 	//������Ϣ�洢�����С
								       (uint32_t*		)&TotalRunTime);//����ϵͳ�ܵ�����ʱ��
		printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
		for(x=0;x<ArraySize;x++){
			//ͨ�����ڴ�ӡ����ȡ����ϵͳ������й���Ϣ�������������ơ�
			//�������ȼ��������š�
			printf("%s\t\t%d\t\t\t%d\t\t\t\r\n",				
					StatusArray[x].pcTaskName,
					(int)StatusArray[x].uxCurrentPriority,
					(int)StatusArray[x].xTaskNumber);	
		}
	}
	vPortFree(StatusArray);	//�ͷ��ڴ�
	printf("/**************************����***************************/\r\n");
	//�ڶ���������vTaskGetInfo()��ʹ��
	TaskHandle_t TaskHandle;	
	TaskStatus_t TaskStatus;
	
	printf("/************�ڶ���������vTaskGetInfo()��ʹ��**************/\r\n");
	TaskHandle=xTaskGetHandle("task1");			//������������ȡ��������
	//��ȡTask1��������Ϣ
	vTaskGetInfo((TaskHandle_t	)TaskHandle, 		//������
				       (TaskStatus_t*	)&TaskStatus, 		//������Ϣ�ṹ��
				       (BaseType_t	)pdTRUE,			//����ͳ�������ջ��ʷ��Сʣ���С
			         (eTaskState	)eInvalid);			//�����Լ���ȡ��������׳̬
	//ͨ�����ڴ�ӡ��ָ��������й���Ϣ��
	printf("������:                %s\r\n",TaskStatus.pcTaskName);
	printf("������:              %d\r\n",(int)TaskStatus.xTaskNumber);
	printf("����׳̬:              %d\r\n",TaskStatus.eCurrentState);
	printf("����ǰ���ȼ�:        %d\r\n",(int)TaskStatus.uxCurrentPriority);
	printf("��������ȼ�:          %d\r\n",(int)TaskStatus.uxBasePriority);
	printf("�����ջ����ַ:        %#x\r\n",(int)TaskStatus.pxStackBase);
	printf("�����ջ��ʷʣ����Сֵ:%d\r\n",TaskStatus.usStackHighWaterMark);
	printf("/**************************����***************************/\r\n");
	//������������eTaskGetState()��ʹ��	
	eTaskState TaskState;
	char TaskInfo[10];
	printf("/***********������������eTaskGetState()��ʹ��*************/\r\n");
	TaskHandle=xTaskGetHandle("query_task");		//������������ȡ��������
	TaskState=eTaskGetState(TaskHandle);			//��ȡquery_task���������׳̬
	memset(TaskInfo,0,10);						
	switch((int)TaskState)
	{
		case 0:
			sprintf(TaskInfo,"Running");
			break;
		case 1:
			sprintf(TaskInfo,"Ready");
			break;
		case 2:
			sprintf(TaskInfo,"Suspend");
			break;
		case 3:
			sprintf(TaskInfo,"Delete");
			break;
		case 4:
			sprintf(TaskInfo,"Invalid");
			break;
	}
	printf("����׳ֵ̬:%d,��Ӧ��׳̬Ϊ:%s\r\n",TaskState,TaskInfo);
	printf("/**************************����**************************/\r\n");
	//���Ĳ�������vTaskList()��ʹ��	
	printf("/*************������������vTaskList()��ʹ��*************/\r\n");
	vTaskList(InfoBuffer);							//��ȡ�����������Ϣ
	printf("%s\r\n",InfoBuffer);					//ͨ�����ڴ�ӡ�����������Ϣ
	printf("/**************************����**************************/\r\n");
	while(1){
	  vTaskDelay(1000);
		vTaskList(InfoBuffer);							//��ȡ�����������Ϣ
	  printf("%s\r\n",InfoBuffer);					//ͨ�����ڴ�ӡ�����������Ϣ 
	}
}

int idlehookcnt=0; 
void vApplicationIdleHook(void){
  idlehookcnt++; 
}

int tickhookcnt=0; 
void vApplicationTickHook(void){
  tickhookcnt++; 
}

void OneShotCallback(TimerHandle_t xTimer){ 
	BaseType_t xHigherPriorityTaskWoken, xResult;
	
} 
 
void UserGpio_Config(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC,ENABLE); 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIOB->BSRRL = GPIO_Pin_5;
 
	//2.5v and ref
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	//se1-se4
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
  