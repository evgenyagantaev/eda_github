// This class provides functionality of power control (power on and off)

class clsPowerController
{
//******************************************************************************   
private://                   PRIVATE ZONE
//******************************************************************************   

   //************************* constants ***************************************
   static const int POWERONINTERVAL = 2000; // miliseconds
   static const int POWEROFFINTERVAL = 2500; // miliseconds
   //*********************** end constants *************************************   


   //************************* variables ***************************************
   
   // flag indicates that power on proceduresuccessfully finished
   int powerOn;

   //*********************** end variables *************************************


   //********************* private functions ***********************************


   //******************** end private functions ********************************
   
      
//******************************************************************************   
public://                    PUBLIC ZONE
//******************************************************************************      

   //---------------------------------------------------------------------------
   // public constructor
   clsPowerController()
   {
      powerOn = 0;
   }//end clsPowerController
   //--------------------------end----------------------------------------------
   
   
   //************************ properties ***************************************
   int powerOnInterval()
   { return POWERONINTERVAL;}
   int powerOffInterval()
   { return POWERONINTERVAL;}
   //********************** end properties *************************************
   
   //********************* public functions ************************************
   
   //******************* end public functions **********************************
   
   
   //---------------------------------------------------------------------------
   // Function ...
   void someFunction(void)
   {
      
   }//end someFunction
   //--------------------------end----------------------------------------------
   
      
};