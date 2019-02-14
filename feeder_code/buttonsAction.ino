void buttonsAction() {
  unsigned long interrupt_time = millis(); //saves time at which this interrupt starts
  if (interrupt_time - last_interrupt_time > 200)  //compares last time interrupt was finished to now. We give 200 millis to ignore rebounds
  {

    voltage = analogRead(voltagePin);




    /**************************************************************************************************************************************************************/
    if (minNb <= voltage && voltage <= maxNb) {  //NB OF MEALS BUTTON IS PRESSED

      leds = 3 << nbLed; //turn buzzer and "nb of meals" button led
      updateShiftRegister();
      delay(100);
      leds = 0;
      updateShiftRegister();


      if (meal_nb < max_meal_nb) meal_nb++;
      else meal_nb = 1;
    }


    /**************************************************************************************************************************************************************/
    /*  if (minSize <= voltage && voltage <= maxSize) {  //SIZE OF MEALS BUTTON IS PRESSED

        if (meal_size < max_meal_size) meal_size++;
        else meal_size = 1;
      } */



    /**************************************************************************************************************************************************************/
    //if (minReset <= voltage && voltage <= maxReset) {  //RESET BUTTON IS PRESSED


    // }

    //SAVE WHEN THE INTERRUPT OCCURED TO PREVENT BOUNCES FROM TRIGGERING MULTIPLE INTERRUPTS
    last_interrupt_time = interrupt_time;

  }
}
