
#include "stm32f1xx_hal.h"
#include "usart/bsp_debug_usart.h"
#include "ESP8266/bsp_esp8266.h"
#include "beep/bsp_beep.h"
#include "led/bsp_led.h"
#include "stdlib.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
#define User_ESP8266_ApSsid                       "HUAWEI"              //Ҫ���ӵ��ȵ������
#define User_ESP8266_ApPwd                        "huaweiapppp"           //Ҫ���ӵ��ȵ����Կ

#define User_ESP8266_TcpServer_IP                 "106.14.8.5"       //Ҫ���ӵķ������� IP
#define User_ESP8266_TcpServer_Port               "1234"                 //Ҫ���ӵķ������Ķ˿�

/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
extern __IO uint8_t ucTcpClosedFlag;

/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ϵͳʱ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;  // �ⲿ����8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;  // 9��Ƶ���õ�72MHz��ʱ��
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // ϵͳʱ�ӣ�72MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHBʱ�ӣ�72MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;               // APB1ʱ�ӣ�36MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;               // APB2ʱ�ӣ�72MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

 	// HAL_RCC_GetHCLKFreq()/1000    1ms�ж�һ��
	// HAL_RCC_GetHCLKFreq()/100000	 10us�ж�һ��
	// HAL_RCC_GetHCLKFreq()/1000000 1us�ж�һ��
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);  // ���ò�����ϵͳ�δ�ʱ��
  /* ϵͳ�δ�ʱ��ʱ��Դ */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  /* ϵͳ�δ�ʱ���ж����ȼ����� */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * ��������: ������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
int main(void)
{  
  uint8_t ucStatus;  
  uint8_t pCH;
  
  /* ��λ�������裬��ʼ��Flash�ӿں�ϵͳ�δ�ʱ�� */
  HAL_Init();
  /* ����ϵͳʱ�� */
  SystemClock_Config();

  LED_GPIO_Init();
  BEEP_GPIO_Init();
  
  /* ��ʼ�����ڲ����ô����ж����ȼ� */
  MX_DEBUG_USART_Init();
  
  ESP8266_Init();
  
  ESP8266_Init();
  printf("�������� ESP8266 ......\n" );
  
  if(ESP8266_AT_Test())
  {
    printf("AT test OK\n");
  }
  printf("\n< 1 >\n");
	if(ESP8266_Net_Mode_Choose(STA))
  {
    printf("ESP8266_Net_Mode_Choose OK\n");
  }  
  printf("\n< 2 >\n");
  while(!ESP8266_JoinAP(User_ESP8266_ApSsid,User_ESP8266_ApPwd));		
	printf("\n< 3 >\n");
  ESP8266_Enable_MultipleId(DISABLE);	
	while(!ESP8266_Link_Server(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0));	
	printf("\n< 4 >\n");
  while(!ESP8266_UnvarnishSend());	
	printf("���� ESP8266 ���\n");
	BEEP_ON;
	HAL_Delay(300);
	BEEP_OFF;

  __HAL_UART_ENABLE_IT(&husartx_esp8266,UART_IT_IDLE);

  /* ����ѭ�� */
  while (1)
  {
    ESP8266_ReceiveString(ENABLE);
    if(strEsp8266_Fram_Record .InfBit .FramFinishFlag )
		{
      HAL_Delay(10); 
    	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
      printf ( "\r\n%s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );
      /*�����յ����ַ���ת��������*/
      pCH=atoi(strEsp8266_Fram_Record .Data_RX_BUF);
       switch(pCH)
       {
         case 0:
           LED1_OFF;
         break;
         case 1:
           LED1_ON;
         break;
         case 2:
           LED2_ON;
         break;
         case 3:
           LED2_OFF;
         break;
         case 4:
           LED3_ON;
         break;
         case 5:
           LED3_OFF;
         break;
         case 6:
           BEEP_ON;
         break;
         case 7:
           BEEP_OFF;
         break;
       }         
    }
    if(ucTcpClosedFlag)                                             //����Ƿ�ʧȥ����
		{
			ESP8266_ExitUnvarnishSend();                                    //�˳�͸��ģʽ			
			do ucStatus = ESP8266_Get_LinkStatus();                         //��ȡ����״̬
			while(!ucStatus);			
			if(ucStatus==4)                                             //ȷ��ʧȥ���Ӻ�����
			{
				printf("���������ȵ�ͷ����� ......\n");				
				while(!ESP8266_JoinAP(User_ESP8266_ApSsid,User_ESP8266_ApPwd));				
				while(!ESP8266_Link_Server(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0));				
				printf("�����ȵ�ͷ������ɹ�!!!\n");
			}			
			while(!ESP8266_UnvarnishSend());					
		}
  }
}

/**
  * ��������: ���ڽ�����ɻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  if(strEsp8266_Fram_Record.InfBit.FramLength<(RX_BUF_MAX_LEN-1))                       //Ԥ��1���ֽ�д������
  {
    strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength++]=esp8266_rxdata;    
  }     
  HAL_UART_Receive_IT(&husartx_esp8266,&esp8266_rxdata,1);
}