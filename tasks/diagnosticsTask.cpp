# define STATUSSTRINGLENGTH 32

// This task performs diagnostics
void diagnosticsTask(void *parameters)
{
   
   char statusString[STATUSSTRINGLENGTH];
   char debugstatus1[STATUSSTRINGLENGTH];
   char debugstatus2[STATUSSTRINGLENGTH];
   
   // debug ***
   char rrvectorString[600];
   char rrString[16];
   // debug ***
   
   
   while(1)
   {
      if(common.heartRate != 444)
         diagnost.makeDiagnosis(common.heartRate, 
            movementDetector.getWalkingStatus(), movementDetector.getRunningStatus(),
            movementDetector.getPosition());
      
      int motion = movementDetector.getPosition();
      int battery = common.batteryCharge;
      
      // send status string via uart
      
      if(adcFault)
      {
         // no correct data from ADC
         sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", 0, 
                 777, motion, battery);
      }
      else if(adcAbnormal)
      {
         adcAbnormal = 0;
         // no correct data from ADC
         sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", 0, 
                 555, motion, battery);
      }
      else if(diagnost.badAdcRange)
      {
         
         //*
         if(diagnost.getBadAdcRangeFlag() == 0) // 444 appeared right now
         {
            diagnost.setBadAdcRangeFlag();
            diagnost.setBadAdcRangeMarker(common.secondsTimer);
         
            sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", diagnost.getStatus(), 
            diagnost.getLastDisplayedPulse(), movementDetector.getPosition(), 
            common.batteryCharge);
         }
         else // 444 appeared in some moment in the past
         {
            if((common.secondsTimer - diagnost.getBadAdcRangeMarker()) > 10 ) // 444 more then 10 seconds
            {
               // adc input is out of range
               sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", 0, 
                       444, motion, battery);
               common.heartRate = 444;
               
               //sprintf(debugstatus1, "Iso = %d\r\n", diagnost.badIsoline);
               //xSemaphoreTake(uartMutex, portMAX_DELAY);
               //uart.sendMessage(debugstatus1);
               //xSemaphoreGive(uartMutex);
               //vTaskDelay(delay10);
            }
            else // 444 less then 10 seconds
            {
               sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", diagnost.getStatus(), 
               diagnost.getLastDisplayedPulse(), movementDetector.getPosition(), 
               common.batteryCharge);
            }
         }
         //*/
         
         // adc input is out of range
         //sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", 0, 
                 //444, motion, battery);
         
         //sprintf(debugstatus1, "Iso = %d\r\n", diagnost.badIsoline);
         //xSemaphoreTake(uartMutex, portMAX_DELAY);
         //uart.sendMessage(debugstatus1);
         //xSemaphoreGive(uartMutex);
         //vTaskDelay(delay10);
         
         
      }
      else if(ecgAnalizer.getHeartRate() > 205)
      {
         // high noice in data
         if(diagnost.getLastDisplayedPulse() == 444) // no normal pulse yet
         {
            sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", 0, 
                    444, motion, battery);
         }
         else
         {
            sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", 0, 
                    333, motion, battery);
         }
      }
      else
      {
         // everything ok
         diagnost.resetBadAdcRangeFlag();
         
         if(common.heartRate != 444)
         {
            diagnost.setLastDisplayedPulse(common.heartRate);
            // print classic form of status string
            //sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", diagnost.getStatus(), 
               //common.heartRate, movementDetector.getPosition(), 
               //common.batteryCharge);
            // print form of status string with Bayevsky tension index
            sprintf(statusString, "c%dp%03dm%dv%03db%dG\r\n", diagnost.getStatus(), 
               common.heartRate, movementDetector.getPosition(), 
               common.batteryCharge, (int)(rrHistogramm.getTensionIndex()));
         }
         else
         {
            sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", 0, 
               444, movementDetector.getPosition(), 
               common.batteryCharge);
         }
         //sprintf(statusString, "c%dp%03dm%dv%03db%dG\r\n", diagnost.getStatus(), 
            //common.heartRate, movementDetector.getPosition(), 
            //common.batteryCharge, (int)(rrHistogramm.getTensionIndex()));
         
         // debug recreation curve
         //sprintf(statusString, "c%dp%03dm%dv%03dG\r\n", diagnost.getStatus(), 
            //ecgAnalizer.getHeartRate(), movementDetector.getPosition(), 
            //diagnost.getTachiThreshold());
      }
      
      //output diagnosys
      long interval = diagnost.getInterval();
      if((common.secondsTimer%interval) == 0)
      {
         xSemaphoreTake(sdioMutex, portMAX_DELAY);
         if(!common.commandMode) sdio.writeStringToBuffer(statusString);
         xSemaphoreGive(sdioMutex);
           
         if(common.sendStatus)
         {
            xSemaphoreTake(uartMutex, portMAX_DELAY);
            uart.sendMessage(statusString);
            //debug ***
            //sprintf(debugstatus1, "^^^^^^^^^%ld^^^^%ld\r\n", common.secondsTimer, interval);
            //uart.sendMessage(debugstatus1);
            //debug ***
            //debug ***
            /*
            rrvectorString[0] = 0;
            for(int i=128; i >= 1; i--)
            {
               sprintf(rrString, "%d ", bayevsky.getRRbyIndex(600 - i));
               strcat(rrvectorString, rrString);
            }
            sprintf(rrString, "\r\n");
            strcat(rrvectorString, rrString);
            
            uart.sendMessage(rrvectorString);
            //*/
            //debug ***
            xSemaphoreGive(uartMutex);
         }
      }
      //debug************
      /*
      else
      {
         sprintf(debugstatus2, "**********%ld****%ld\r\n", common.secondsTimer, interval);
         xSemaphoreTake(uartMutex, portMAX_DELAY);
         uart.sendMessage(debugstatus2);
         xSemaphoreGive(uartMutex);
      }
      //*/
      
      //sleep for a known interval
      //vTaskDelay(diagnost.getInterval() * configTICK_RATE_HZ);
      vTaskDelay(delay1000);
   }
}