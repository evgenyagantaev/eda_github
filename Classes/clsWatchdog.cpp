//This class provides independed watchdog (IWDG) /w 1s counter
class clsWatchdog
{
    public:
    clsWatchdog()
    {
        /* IWDG timeout equal to 250ms.
        The timeout may varies due to LSI frequency dispersion, the 
        LSE value is centred around 32 KHz */
        /* Enable write access to IWDG_PR and IWDG_RLR registers */
        IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
        /* IWDG counter clock: LSI/32 */
        IWDG_SetPrescaler(IWDG_Prescaler_32);
        /* Set counter reload value to obtain 250ms IWDG TimeOut.
        Counter Reload Value = 250ms/IWDG counter clock period
        = 250ms / (LSI/32)
        = 0.25s / (32 KHz /32)
        = 250
        */
        IWDG_SetReload(1000);
        /* Reload IWDG counter */
        IWDG_ReloadCounter();

    }
  
  void start()
  {
    IWDG_Enable();
  }
 
  void reload()
  {
    IWDG_ReloadCounter();
  }
  

};