  void shakeTheFood() {  //shake the container 3 times to settle 
    for (int i = 0; i < 3; i++) {
      for (pos = 0; pos <= 20; pos ++)       // goes from 0 degrees to 180 degrees
      { // in steps of 1 degree
        feederServo.write(pos);                     // tell servo to go to position in variable 'pos'
        delay(15);                              // waits 15 milliseconds for the servo to reach the position
        SoftwareServo::refresh();               // required by SoftwareServo Library to sweep correctly
      }
      delay (20);

      for (pos = 20; pos >= 10; pos --)       // goes from 0 degrees to 180 degrees
      { // in steps of 1 degree
        feederServo.write(pos);                     // tell servo to go to position in variable 'pos'
        delay(15);                              // waits 15 milliseconds for the servo to reach the position
        SoftwareServo::refresh();               // required by SoftwareServo Library to sweep correctly
      }
      delay (20);
    }
  }
