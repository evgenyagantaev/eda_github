void adc1Task(void *parameters)
{
   
   char U_in_string[80];
   int odd_even = 0;
   
   uint16_t dacCurrent = 1;
   
   uint32_t adc1Data, adc2Data;
   
   //dacPower.setDacValue(0x0003);
   dacPower.offCurrent();
   
   // start timer
   timer3.startTimer();
   //timer2.startTimer();
   
   while(1)
   {
      // waiting for a semaphore
      // semaphore will be given from timer 3 isr or from ads8320task (on every 50-th iteration)
      xSemaphoreTake(u_inMeasureTimerSemaphore, portMAX_DELAY);
      //*
      adc1Data = 0;
      for(int i=0; i<11; i++)
      {
         ADC_SoftwareStartConv(ADC1);
         while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
            taskYIELD();
         //adc1Data += ADC_GetConversionValue(ADC1);
         adc1Data += (uint32_t)((double)ADC_GetConversionValue(ADC1) * 4.909);
      }
      adc1Data /= 11;
      //*/
      adc2Data = 0;
      for(int i=0; i<3; i++)
      {
         ADC_SoftwareStartConv(ADC2);
         while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC))
            taskYIELD();
         adc2Data += ADC_GetConversionValue(ADC2);
      }
      adc2Data /= 3;
      
      readAds8320();
      
      if(common.accumConnected && (adc1Data < 1000))
      {
         common.accumConnected = 0;
         dac1.setData(0);
         
         dacPower.stopPulse();
      }
      
      
      if(!common.accumConnected)
      {
         if(adc1Data < 1000)
         {
            sprintf(U_in_string, "u_in = %d, T = %.4f \r\nAccum isn't connected\r\n", 
                    adc1Data, thermometer.readThemperature());
            uart.transmitMessage(U_in_string);
         }
         else
         {
            sprintf(U_in_string, "u_in = %d, T = %.4f \r\n", 
                    adc1Data, thermometer.readThemperature());
            uart.transmitMessage(U_in_string);
            common.accumConnected = 1;
            dac1.setData(adc1Data/2);
            dac1.tuneDac();
         }
         
      }
      
      
      
      if(!common.pulseOn && (common.pulseDownCounter > 0) && common.accumConnected)
      {
       
         //dac1.setData(common.adc1Buffer[0]/2);
         dac1.tuneDac();
         common.pulseOn = 1;
         odd_even = 0;
         timer3.stopTimer();
         
         
         // remember the last value of dU
         common.last_dU = common.ads8320Data;
         
         sprintf(U_in_string, "time=%d, freq=%f, current=%d, number of samples = %d;\r\n", 
                 common.length, common.frequency, common.current, common.numberOfSamples);
         uart.transmitMessage(U_in_string);
         //timer2.startTimer();
      }
      else if(!common.pulseOn && !(common.pulseDownCounter > 0) && common.accumConnected)
      {
         // recalculate in mv
#ifdef ACC2V
         int adc1 = (int)((double)adc1Data * 1.073);
         
#else
         //int adc1 = (int)((double)adc1Data * 4.909); // умножение на этот коэффициент перекочевало выше в блок измерения и усреднения данных ацп
         int adc1 = adc1Data;
#endif
         
         sprintf(U_in_string, "u_in = %d\, dU = %d, T = %.4f\r\n", adc1, 
                 common.ads8320Data, thermometer.readThemperature());
         uart.transmitMessage(U_in_string);
         // tuning of du value
         if(common.ads8320Data < 1000 || common.ads8320Data > 5000)
           dac1.tuneDac();
      }
      
      
      if(common.pulseOn && common.accumConnected)
      {
          if(!(common.pulseDownCounter > 0))
          {
            dacPower.stopPulse();
            // print finish pulse mark
            sprintf(U_in_string, "*** complete ***\r\n");
            uart.transmitMessage(U_in_string);
          }
          else
          {
         
            if(odd_even == 0) 
            {
               dacCurrent = common.current;
               dacPower.setDacValue(dacCurrent);
               dacPower.onCurrent();
               odd_even = 1;
               common.samplesCounter = 0;
            }    
            else
            {
               dacPower.offCurrent();
               odd_even = 0;
            }
            
            //timer2.stopTimer();
            
            int I;
#ifdef ACC2V
            adc2Data *=5;
#endif
            
#ifdef ADC2_2V_4A
            adc2Data = (uint16_t)((double)adc2Data / (4096.0/3.3*2.0/4000.0));
#endif

            
            if(odd_even == 1)
            {
               // remember the last value of I
               common.last_I = adc2Data;
               I = 0;
            }
            else
            {
               I = adc2Data - common.last_I;
            }
            
            sprintf(U_in_string, "I=%d **************************************** %d\r\n", I, common.pulseDownCounter);
            uart.transmitMessage(U_in_string);
            timer2.startTimer();
          }
      }
      else if(!(common.pulseDownCounter > 0))
         dacPower.offCurrent();
      
      // yield control to scheduler
      taskYIELD();
      //vTaskDelay(delay1000);
   }
}