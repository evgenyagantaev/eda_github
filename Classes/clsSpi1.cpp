class clsSpi1 
{
public:
      clsSpi1()
      {
        GPIO_InitTypeDef GPIO_InitStructure;
        SPI_InitTypeDef SPI_InitStructure;
        
        /* Enable the SPI clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
        /* Enable the GPIOA clock (SCK,MISO,MOSI) */
        //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        /* Enable the GPIOB clock (CS) */
        //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        /* Connect PA5 to SPI1_SCK */  
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
        /* Connect PA6 to SPI1_MISO */
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
        /* Connect PA7 to SPI1_MOSI */
        //GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); MOSI nam zdes' ne nuzhen
        // budem konfigurirovat' PA7 kak gpio out
        /* SDI pin of AD7691 configuration */ 
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_SetBits(GPIOA, GPIO_Pin_7); // set pa7 (SDI of AD7691) high
       
        /* Configure SPI1 pins as alternate function (No need to configure PA4 since NSS will be managed by software) */ 
        //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        /* CS pin configuration */ 
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        GPIO_ResetBits(GPIOA, GPIO_Pin_8); //Set CS pin to low
        
        /* SPI configuration *****************************************/ 
        SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
        SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
        // 60 MHz / 32 = 1.875 MHZ -> clock frequency of SPI1 (ads8320)
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
        SPI_InitStructure.SPI_CRCPolynomial = 7;
        SPI_Init(SPI1, &SPI_InitStructure);
        SPI_Cmd(SPI1, ENABLE);
      }
      
             
      
private:
  
};