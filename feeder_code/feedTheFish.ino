void feedTheFish() {

  //1: Signal that there's an incoming meal--------------------------------------------------
  //This feature can be toggled On or OFF by the user (with the ON-OFF switch)
  for (int i = 0; i < 6; i++) {
    leds ^= (1 << signalLed);  //turn on the big LED for telling fish food is coming
    updateShiftRegister();
    delay(100);
  }

  //2: activate servo transistors to power it up----------------------------------------------
  leds = 0B1001; //turn bits 3 and 0 ON, which are the 2 servo transistors
  updateShiftRegister();
  delay(100);

  
  //3: Shake the food supplies to make it fall in---------------------------------------------


  //4: feed based on size of meals------------------------------------------------------------
  for (int s = 0; s < 1; s++) {

    for (pos = 0; pos <= 180; pos ++)       // goes from 0 degrees to 180 degrees
    { // in steps of 1 degree
      feederServo.write(pos);                     // tell servo to go to position in variable 'pos'
      delay(15);                              // waits 15 milliseconds for the servo to reach the position
      SoftwareServo::refresh();               // required by SoftwareServo Library to sweep correctly
    }
    delay (200);                               // required by SoftwareServo Library to sweep correctly

    for (pos = 180; pos >= 0; pos --)     {  // goes from 180 degrees to 0 degrees
      feederServo.write(pos);                     // tell servo to go to position in variable 'pos'
      delay(15);                              // waits 15 milliseconds for the servo to reach the position
      SoftwareServo::refresh();               // required by SoftwareServo Library to sweep correctly
    }
    delay(200);                                // required by SoftwareServo Library to sweep correctly

    leds = 0;
    updateShiftRegister();
    delay(100);
  }

}
