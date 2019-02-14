void feedTheFish() {

  //0: Shake the food supplies to make it fall in


  //1: mouvement dans sens horaire vers la sortie
  for (int s = 0; s < 1; s++) {
    
    for (pos = 0; pos <= 180; pos ++)       // goes from 0 degrees to 180 degrees
    { // in steps of 1 degree
      feederServo.write(pos);                     // tell servo to go to position in variable 'pos'
      delay(15);                              // waits 15 milliseconds for the servo to reach the position
      SoftwareServo::refresh();               // required by SoftwareServo Library to sweep correctly
    }
    delay (200);                               // required by SoftwareServo Library to sweep correctly
    


      //2: mouvement dans sens antihoraire vers le réservoir
      for (pos = 180; pos >= 0; pos --)     {  // goes from 180 degrees to 0 degrees 
        feederServo.write(pos);                     // tell servo to go to position in variable 'pos'
        delay(15);                              // waits 15 milliseconds for the servo to reach the position
        SoftwareServo::refresh();               // required by SoftwareServo Library to sweep correctly
      }
      delay(200);                                // required by SoftwareServo Library to sweep correctly

      
      for (int i = 0; i < 10; i++) {
        PORTB ^= (1 << ledPin);
        delay(100);
      }

    }
  }
