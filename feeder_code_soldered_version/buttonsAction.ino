void buttonsAction() {

  /*For now, monitoring how often interrupts occurs not needed because no bouncing, and caused problems in interrupts
    // unsigned long interrupt_time = millis(); //saves time at which this interrupt starts
    // if (interrupt_time - last_interrupt_time > 200)  //compares last time interrupt was finished to now. We give 200 millis to ignore rebounds
    // { */
  //delay(10);
  voltage = 0;
  for (int i = 0; i < 20; i ++) {
    voltage += analogRead(voltagePin);
    delay(5);
  }
  voltage /= 20;

  //DEBUG-------------------------------------------
  /*for (int i = 0; i < voltage; i += 10) {

    leds = 1 << signalLed;  //turn on the big LED for telling fish food is coming
    updateShiftRegister();
    delay(300);
    leds = 0;
    updateShiftRegister();
    delay(300);
    }*/
  //------------------------------------------------


  /**************************************************************************************************************************************************************/
  if (min4 <= voltage && voltage <= max4) {  //NB OF MEALS BUTTON IS PRESSED

    //1: button's action
    if (meal_nb < max_meal_nb) meal_nb++;   //add a meal during the day, up to 3 per day
    else meal_nb = 1;
    /* //1 meal per day: target = 10800
      //2 meals: trgt = 5400
      //3 meals: trgt = 3600
    */
    switch (meal_nb) {
      case 1:
        wd_target = 10800;
        break;
      case 2:
        wd_target = 5400;
        break;
      case 3:
        wd_target = 3600;
        break;
    }

    //2: cue the user button 4 was pressed
    leds = 9 << nbLed; //turn buzzer and "nb of meals" button led
    updateShiftRegister();
    delay(100);

    leds = 0;
    updateShiftRegister();
    delay(250);

    //3: inform the user of the variable's value
    for (int j = 0; j < (meal_nb - 1); j ++) {
      leds = 1 << nbLed; //turn "nb of meals" button led for the remaining nb of flashes it needs
      updateShiftRegister();
      delay(100);

      leds = 0;
      updateShiftRegister();
      delay(250);
    }
  }


  /**************************************************************************************************************************************************************/
  if (min3 <= voltage && voltage <= max3) {  //SIZE OF MEALS BUTTON IS PRESSED

    //1: button's action
    if (meal_size < max_meal_size) meal_size++;   //increase size of each meal
    else meal_size = 1;

    //2: cue the user button 3 was pressed

    leds = 5 << sizeLed; //turn buzzer and "size of meals" button led
    updateShiftRegister();
    delay(80);

    leds = 0;
    updateShiftRegister();
    delay(200);

    //3: inform the user of the variable's value
    for (int j = 0; j < (meal_size - 1); j ++) {
      leds = 1 << sizeLed; //turn "size of meals" button led for the remaining nb of flashes it needs
      updateShiftRegister();
      delay(80);

      leds = 0;
      updateShiftRegister();
      delay(200);
    }

  }

  /**************************************************************************************************************************************************************/
  if (min1 <= voltage && voltage <= max1) {  //TOGGLE LED BUTTON IS PRESSED

    //1: button's action
    watchDog_counter = wd_target;

    //2: cue the user button 1 was pressed
    leds = 7 << nbLed; //all 3 leds
    updateShiftRegister();
    delay(100);

    leds = 0;
    updateShiftRegister();


  }

  /**************************************************************************************************************************************************************/
  if (min2 <= voltage && voltage <= max2) {  //RESET BUTTON IS PRESSED

    //1: button's action
    food_signal_flag = ~food_signal_flag;



    //2: cue the user button 2 was pressed
    
    if (food_signal_flag) {
      leds = 3 << resetLed; //turn buzzer and "reset" button led
      updateShiftRegister();
      delay(100);

      leds = 0;
      updateShiftRegister();
    }


  }

  //For now, monitoring how often interrupts occurs not needed because no bouncing, and caused problems in interrupts
  //SAVE WHEN THE INTERRUPT OCCURED TO PREVENT BOUNCES FROM TRIGGERING MULTIPLE INTERRUPTS
  //    last_interrupt_time = interrupt_time;
  // }
}
