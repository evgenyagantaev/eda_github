extern void readAds8320(void);

void ads8320Task(void *parameters)
{
   
 
   char outByteBuffer[3];
   char txt[64];
   
   
   

   outByteBuffer[2] = 0;
  
   //watchdog.start();          // watchdog.start watchdog.start watchdog.start
   common.samplesCounter = 0;
   
   while(1)
   {
      //watchdog.reload();
      
      xSemaphoreTake(newSampleTimerSemaphore, portMAX_DELAY);
      //timer2.stopTimer();
      
      if(common.pulseOn)
      {
         if(common.samplesCounter == common.numberOfSamples)
         {
            common.samplesCounter = 0;
            common.pulseDownCounter--;
            timer2.stopTimer();
            // give semaphore to adc1 task
            xSemaphoreGive(u_inMeasureTimerSemaphore);
            taskYIELD();
         }
         else
         {
            readAds8320();
            // recalculate in mv
#ifdef ACC2V
            double ads8320_mv = ((double)common.ads8320Data - (double)common.last_dU)/25.0;
#else
            double ads8320_mv = ((double)common.ads8320Data - (double)common.last_dU)/38.6;
#endif
            sprintf(txt, "%.2f\r\n", ads8320_mv);
            uart.transmitMessage(txt);
         
            common.samplesCounter++;
         }
         
         //timer2.startTimer();
      }

      //taskYIELD();
      
      //vTaskDelay(delay5);
      
   }//end while(1) (superloop)
}


void readAds8320(void)
{
   uint32_t ads8320Data = 0;
   uint8_t byteBuffer;
   
   GPIOA->BSRRH=GPIO_Pin_8;  //CS pin low (start spi communication)
   
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   SPI1->DR=0x55;
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
   byteBuffer=SPI1->DR;
   // apply 00000011 mask (0x03); 5 clocks and first zero dout
   byteBuffer &= 0x03;
   ads8320Data |= (uint32_t)byteBuffer;
   ads8320Data <<= 8;
   
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   SPI1->DR=0x55;
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
   byteBuffer=SPI1->DR;
   ads8320Data |= (uint32_t)byteBuffer;
   ads8320Data <<= 8;
   
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   SPI1->DR=0x55;
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
   byteBuffer=SPI1->DR;
   ads8320Data |= (uint32_t)byteBuffer;
   ads8320Data >>= 2;  // 2 "extra" clocks
   
   common.ads8320Data = ads8320Data;
   
   GPIOA->BSRRL=GPIO_Pin_8;  //CS pin high (stop spi communication)
}



