// This task reads data from ADS8320 via SPI interface

void ads8320SpiReadDataTask(void *pvParameters)
{
   
   uint16_t ads8320Data;
   char txt[32];
   
   //watchdog.start();          // watchdog.start watchdog.start watchdog.start
   while(1)
   {
      //watchdog.reload();
      //*
      GPIOB->BSRRH=GPIO_Pin_8;  //CS pin low (start spi communication)
      
      // pa6 - miso
      // pa6 - ad7792's drdy/dout
      //while((GPIOA->IDR & GPIO_Pin_6) != (uint32_t)Bit_RESET);
      
      while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
      GPIOB->BSRRL=GPIO_Pin_8;  //CS pin high (stop spi communication)
      ads8320Data = SPI1->DR;
      
      sprintf(txt, "DU%04\r\n", ads8320Data);
      uart.transmitMessage(txt);
      //*/
      
      // yield control to scheduler
      //taskYIELD();
      
      printf("task\r\n");
      
      vTaskDelay(delay500);
      
   }//end while(1) (superloop)
   
}// end ads8320SpiReadDataTask





