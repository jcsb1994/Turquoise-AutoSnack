void buttonsAction() {
  unsigned long interrupt_time = millis(); //saves time at which this interrupt starts
  if (interrupt_time - last_interrupt_time > 200)  //compares last time interrupt was finished to now. We give 200 millis to ignore rebounds
  {

    voltage = analogRead(voltagePin);


    for (int i = 0; i < 10; i++) {
      PORTB ^= (1 << ledPin);
      delay(100);
    }

    /**************************************************************************************************************************************************************/
    //NB OF MEALS BUTTON IS PRESSED
    if (minNb <= voltage && voltage <= maxNb) {
      if (meal_nb < max_meal_nb) meal_nb++;
      else meal_nb = 1;
    }


    /**************************************************************************************************************************************************************/
    //SIZE OF MEALS BUTTON IS PRESSED
    if (minSize <= voltage && voltage <= maxSize) {
      if (meal_size < max_meal_size) meal_size++;
      else meal_size = 1;
    }



    /**************************************************************************************************************************************************************/
    //RESET BUTTON IS PRESSED
    if (minReset <= voltage && voltage <= maxReset) {

    }

    //SAVE WHEN THE INTERRUPT OCCURED TO PREVENT BOUNCES FROM TRIGGERING MULTIPLE INTERRUPTS
    last_interrupt_time = interrupt_time;

  }
}

