// This task performs 50 Hz noice filtration

void filter50HzTask(void *pvParameters)
{
   uint16_t filterBufferTail = 0;
   
   uint16_t  filteredSample;
   
   //span of noice amplitude
   uint16_t noiceSpan;
   
   // time interval between local max and local min
   portTickType timeSpan;
   
   // debug
   // text buffer for output via uart
   char sampleTxtBuffer[24];
   //clear semaphore before the forever loop
   xSemaphoreTake(newSampleSemaphore, portMAX_DELAY);
   // super loop
   //watchdog.start();
   while(1)
   {
      //watchdog.reload();
      // try to take a semaphore
      // if not a new sample is written in a cyclic buffer yet, task will be blocked
      xSemaphoreTake(newSampleSemaphore, portMAX_DELAY);
      
      // new sample is written in buffer since we had read last time
      if(filterBufferHead != filterBufferTail) 
      {
         // cyclic increment of index
         filterBufferTail = (filterBufferTail + 1)%ADC1BUFFERLEN;
         
         //filter50Hz.updateSample(filterBuffer[filterBufferTail]);
         //filter50Hz.findMaxMinInflection();
         //filter50Hz.filtration();
         
         //--------------------------------------
         //debug
         // read sample from filter buffer and copy it in sample buffer for 
         // analysis
         filteredSample = filterBuffer[filterBufferTail]; // fake filtration
         
         //filteredSample = filter50Hz.getFilteredSample();
         //noiceSpan = filter50Hz.getNoiceSpan();
         //timeSpan = filter50Hz.getTimeSpan();
         
         //debug
         //sprintf(sampleTxtBuffer, "%dI%d\r\n", filteredSample, filteredSample);
         //xSemaphoreTake(uartMutex, portMAX_DELAY);
         //uart.sendMessage(sampleTxtBuffer);
         //xSemaphoreGive(uartMutex);
         
         
         adc1BufferHead++;
         if(adc1BufferHead==ADC1BUFFERLEN) adc1BufferHead=0;
         adc1Buffer[adc1BufferHead] = filteredSample;
         
         
         // give semaphore to the task which analyses ecg
         xSemaphoreGive(newFilteredSemaphore);
         
         //debug 
         //sprintf(sampleTxtBuffer, "%dI%d\r\n", 
                 //timeSpan, 
                 //timeSpan);
         //sprintf(sampleTxtBuffer, "%dI%d\r\n", 
                 //noiceSpan, 
                 //noiceSpan);
         
#ifdef RAW   // output RAW signal
         //sprintf(sampleTxtBuffer, "%dI%d\r\n", 
                 //common.filterBuffer[filterBufferTail], 
                 //common.filterBuffer[filterBufferTail]);
#endif
#ifdef FILTER   // output filtered signal         
         //sprintf(sampleTxtBuffer, "%dI%d\r\n", 
                 //common.adc1Buffer[common.adc1BufferHead], 
                 //common.adc1Buffer[common.adc1BufferHead]);
         //sprintf(sampleTxtBuffer, "%dI%d\r\n",filteredSample,filteredSample);
#endif         
         
         // output a string via uart
         //xSemaphoreTake(uartMutex, portMAX_DELAY);
         //uart.sendMessage(sampleTxtBuffer);
         //xSemaphoreGive(uartMutex);
         //--------------------------------------
      }
      
      // yield control to scheduler
      //taskYIELD();
      
   }// end while super loop
   
}// end filter50HzTask