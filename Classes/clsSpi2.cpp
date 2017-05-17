class clsSpi2 //smb380 accelerometer class via spi2 @PB.10, PC.2, PC.3, PB.13 pins
{
public:
  
  clsSpi2()
  {
    
        GPIO_InitTypeDef GPIO_InitStructure;
        SPI_InitTypeDef SPI_InitStructure;
        /* Enable the SPI clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
        /* Enable the GPIOB&GPIOC clock */
        //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
        
        
        /* Connect PB10 to SPI2_SCK */
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
        /* Connect PC2 to SPI2_MISO */
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_SPI2);
        /* Connect PC3 to SPI2_MOSI */
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);
        
        /* Configure SPI2 pins as alternate function */ 
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3; //MISO, MOSI pins
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //SCK pin
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //CS pin
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
       
        /* SPI configuration *****************************************/ 
        SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
        SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //PCLK1/16 = 30MHz/16
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
        SPI_InitStructure.SPI_CRCPolynomial = 7;
        SPI_Init(SPI2, &SPI_InitStructure);
        SPI_Cmd(SPI2, ENABLE);
        

  }
  
  
  
  
    
  
//private zone-----------------------------------------------------------------  
  
private:
  
  
  
  
};