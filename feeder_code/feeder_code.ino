
/**************************************************************************************************************************************************************/
////LIBRARIES
/**************************************************************************************************************************************************************/

#include <avr/sleep.h>
#include <avr/interrupt.h>



/**************************************************************************************************************************************************************/
////PINS
/**************************************************************************************************************************************************************/


#define switchPin  2
#define ledPin  1
#define voltagePin  4


/**************************************************************************************************************************************************************/
////VARIABLES
/**************************************************************************************************************************************************************/

//WDT variables
volatile int watchDog_counter;

//External interrupt variables
volatile int buttonFlag;
unsigned long last_interrupt_time;
int voltage;

// values are set depending on resistor values. Common resistor is 10k
#define minNb  190 //175 to 185 based on a 4.7k resistor value for UP arrow button
#define maxNb  205
#define minSize  210 //189 to 220 based on a 1k resistor value for DOWN arrow button
#define maxSize  220
#define minReset  175 //120 to 166 based on a 10k resistor value for SELECT button
#define maxReset  189


//Food Data variables

//amount of meals per day
byte meal_nb;
#define max_meal_nb 3

//amount of cylinder spins (meal size)
byte meal_size;
#define max_meal_size 5

/**************************************************************************************************************************************************************/
////OBJECTS
/**************************************************************************************************************************************************************/

/**************************************************************************************************************************************************************/
////SETUP
/**************************************************************************************************************************************************************/

void setup() {

  watchDog_counter = 0;


  //pins setup
  DDRB &= ~(1 << switchPin);
  PORTB |= (1 << switchPin);
  DDRB |= (1 << ledPin);




  //Watchdog timer setup
  //This order of commands is important and cannot be combined
  MCUSR &= ~(1 << WDRF); //Clear the watch dog reset
  WDTCR |= (1 << WDCE) | (1 << WDE); //Set WD_change enable, set WD enable
  WDTCR = 0B100001; //Set prescaler to 8 sec (see p.46 of datasheet to change prescaler), and OVERWRITE WDTCR value ( dont just use |= )
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int



  //External interrupt setup
  //Enable interrupts
  GIMSK |= (1 << INT0);
  sei();
  //set low lvl trigger (keeps triggering while button not released)
  MCUCR &= ~(1 << ISC01);
  //TRYING TO PUT THIS INSTEAD!! (FOR FALLING EDGE):  MCUCR |= (1 << ISC01);
  MCUCR &= ~(1 << ISC00);


  ADCSRA &= ~_BV(ADEN);      //Turn ADC off, saves ~230uA           
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}



/**************************************************************************************************************************************************************/
////LOOP
/**************************************************************************************************************************************************************/


void loop() {
  sleep_cpu();
  //ADCSRA |= _BV(ADEN);        //will ADC need to be turned on after sleep?
  
  PORTB |= (1 << ledPin);
  delay(100);
  PORTB ^= (1 << ledPin);
  delay(500);

  if (buttonFlag) {
    buttonFlag = 0;
  }
}


/**************************************************************************************************************************************************************/
////FUNCTIONS
/**************************************************************************************************************************************************************/

/*Sleep function, activates external interrupts*/
/*void sleep() {

  GIMSK |= _BV(INT0);
   sei();                                  // Enable interrupts
    MCUCR |= 1<<ISC01; // Falling EDGE




  //PCMSK |= _BV(PCINT2);                   // Use PB2 as interrupt pin (PCINT3 would be 3, etc.)
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  // sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  //PCMSK &= ~_BV(PCINT2);                  // Turn off PB2 as interrupt pin
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on

  sei();                                  // Enable interrupts
  } // sleep*/



ISR(INT0_vect) { //NOT PCINT0_vect!

  unsigned long interrupt_time = millis(); //saves time at which this interrupt starts
  if (interrupt_time - last_interrupt_time > 200)  //compares last time interrupt was finished to now. We give 200 millis to ignore rebounds
  {

    voltage = analogRead(voltagePin);
    buttonFlag = 1; //tell the arduino a button was pressed



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



ISR(WDT_vect) {
  watchDog_counter++;
}

