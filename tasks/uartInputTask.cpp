//this task provides command analyze from uart input

uint16_t min(int a, int b)
{
   if(a<b)
      return a;
   else
      return b;
}

float min(float a, float b)
{
   if(a<b)
      return a;
   else
      return b;
}

void uartInputTask(void *pvParameters)
{
   int length = 0;
   float frequency = 0;
   uint16_t current = 0;
   int period = 1000;
   
   char txtOut[64];
   
   
   int i = 0;
   while(common.uartRecieveBuffer[i] && i<common.INBUFFERLENGTH)
   {
      common.uartRecieveBuffer[i] = 0;
      i++;
   }
   
   vTaskDelay(delay1000);
   
   //read garbage
   if (USART_GetITStatus(USART1, USART_IT_RXNE)==SET)
   {
      char r= (char)(USART1->DR & (uint16_t)0x01FF);
   }
   uart.initUartRecieve();
   
  while (1)
  {
    xSemaphoreTake(uartInputSemaphore, portMAX_DELAY);
    
    /*
    i = 0;
    sprintf(txtOut, "\r\n");
    uart.transmitMessage(txtOut);
    while(i<common.INBUFFERLENGTH)
    {
      sprintf(txtOut, "%d ",common.uartRecieveBuffer[i]);
      uart.transmitMessage(txtOut);
      i++;
    }
    sprintf(txtOut, "\r\n");
    uart.transmitMessage(txtOut);
    //*/
    
    
    
    char *pointer;
    pointer = strstr(common.uartRecieveBuffer, "ime=");
    if(pointer == NULL)
    {
       sprintf(txtOut, "Wrong data format\r\n");
       uart.transmitMessage(txtOut);
       sprintf(txtOut, "Data format is: \"time=%%d, freq=%%f, current=%%d;\"\r\n");
       uart.transmitMessage(txtOut);
       sprintf(txtOut, "Real data string is ->\r\n");
       uart.transmitMessage(txtOut);
       uart.transmitMessage(common.uartRecieveBuffer);
       
       //*
       i = 0;
       sprintf(txtOut, "\r\n");
       uart.transmitMessage(txtOut);
       while(i<common.INBUFFERLENGTH)
       {
         sprintf(txtOut, "%d ",common.uartRecieveBuffer[i]);
         uart.transmitMessage(txtOut);
         i++;
       }
       sprintf(txtOut, "\r\n");
       uart.transmitMessage(txtOut);
       //*/
       
       
    }
    //else if((pointer - common.uartRecieveBuffer) > 0 ) // some garbage before a data string
    //{
       // push garbage out
    //   for(int i=0; i < (pointer - common.uartRecieveBuffer); i++)
    //   {
    //      for(int j=0; j < (strlen(common.uartRecieveBuffer)); j++)
    //         common.uartRecieveBuffer[j] = common.uartRecieveBuffer[j+1];
    //   }
    //}
    else 
    {
       //int result = sscanf(common.uartRecieveBuffer, "time=%d, freq=%f, current=%d, period=%d;", 
                           //&length, &frequency, &current, &period);
       uart.transmitMessage(pointer);
       
       int result = sscanf(pointer, "ime=%d, freq=%f, current=%d;", 
                           &length, &frequency, &current);
       
       if(result != 3)
       {
          sprintf(txtOut, "Wrong data format\r\n");
          uart.transmitMessage(txtOut);
          sprintf(txtOut, "Data format is: \"time=%%d, freq=%%f, current=%%d;\"\r\n");
          uart.transmitMessage(txtOut);
       }
       else // everything ok
       {
          if(length > 0)
             common.length = min(length, 300);
          else
             common.length = 0;
          if(frequency >= 0.1)
             common.frequency = min(frequency, 100.0);
          else
             common.frequency = 0.1;
          if(current > 0)
             common.current = min(current, 255);
          else
             common.current = 0;
          
          //debug
          common.pulseTim2Period = period;
          
          i = 0;
          while(common.uartRecieveBuffer[i] && i<common.INBUFFERLENGTH)
          {
            common.uartRecieveBuffer[i] = 0;
            i++;
          }
          
          if(common.accumConnected)
            dacPower.startPulse();
          
          //********************************************************************
          //********************************************************************
          //debug
          /*
          
          timer3.stopTimer();
          
          common.pulseTim2Period = 1000;
          timer2.setPeriod(common.pulseTim2Period);
          timer2.startTimer();
          
          int numberOfIterations = common.length*1000;
          
          
          
          for(int i=1; i<=numberOfIterations; i++)
          {
             //wait for timer2
             while(!common.timer2Tick);
             
             common.timer2Tick = 0;
             
             sprintf(txtOut, "%d %d %d\r\n", i,i,i);
             uart.transmitMessage(txtOut);
             
             if(i%50 == 0)
             {
                sprintf(txtOut, "****************************************\r\n");
                uart.transmitMessage(txtOut);
             }
             
          }
          
          timer2.stopTimer();
          timer3.startTimer();
          
          //*/
          //********************************************************************
          //********************************************************************
          
       }
    }
    
    //vTaskDelay(delay200);
    
    taskYIELD();
    
  } //end while(1)
}