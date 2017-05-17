void dac1Task(void *parameters)
{
   
   uint32_t ads8320Data = 0;
   uint8_t byteBuffer;
   char txt[64];

   uint16_t dacData;
   
   //watchdog.start();          // watchdog.start watchdog.start watchdog.start
   while(1)
   {
      if(abs(dacData - common.adc1Buffer[0]) > 1000)
         dacData = common.adc1Buffer[0];
      
      if(ads8320Data < 1000)
      {
         dac1.setData(dacData);
         dacData += 1;
      }
      else if(ads8320Data > 2000)
      {
         dac1.setData(dacData);
         dacData -= 1;
      }
      
      sprintf(txt, "dacData = %d\r\n", dacData);
      //uart.transmitMessage(txt);
   
      //watchdog.reload();
      
      //*
      ads8320Data = 0;
      
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
      
      GPIOA->BSRRL=GPIO_Pin_8;  //CS pin high (stop spi communication)
      
      //sprintf(txt, "DU%05d\r\n", ads8320Data);
      sprintf(txt, "%dI%d\r\n", ads8320Data, ads8320Data);
      uart.transmitMessage(txt);
      //*/
      
      // yield control to scheduler
      //taskYIELD();
      
      //printf("task\r\n");
      
      vTaskDelay(delay4);
      
   }//end while(1) (superloop)
}