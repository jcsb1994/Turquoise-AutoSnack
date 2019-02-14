
/**************************************************************************************************************************************************************/
////LIBRARIES
/**************************************************************************************************************************************************************/

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <SoftwareServo.h>



/**************************************************************************************************************************************************************/
////PINS
/**************************************************************************************************************************************************************/

//#define voltagePin  2
#define ledPin  1
#define voltagePin  A2


/**************************************************************************************************************************************************************/
////VARIABLES
/**************************************************************************************************************************************************************/

//WDT variables
volatile int watchDog_counter;
int wd_target = 2; //target for wd counter

//PC interrupt variables
volatile int buttonFlag;
unsigned long last_interrupt_time;
int voltage;

// values are set depending on resistor values.
//3 resistors used (1 per button): 1.5k, 5.6k, 7.5k
#define minNb  190 //175 to 185 based on a ___ resistor value for "Num of meals" button
#define maxNb  205
#define minSize  210 //189 to 220 based on a ___ resistor value for "Size of meals" button
#define maxSize  1023
#define minReset  0 //120 to 166 based on a ___ resistor value for SELECT button
#define maxReset  189


//Food Data variables

//amount of meals per day
int meal_nb = 1;
#define max_meal_nb 3

//amount of cylinder spins (meal size)
int meal_size = 1;
#define max_meal_size 5


//Servo variables
int pos = 0; //NEEDS TO BE AN INT, NOT A BYTE


/**************************************************************************************************************************************************************/
////OBJECTS
/**************************************************************************************************************************************************************/
SoftwareServo feederServo;      // creates feederServo as object to control a servo


/**************************************************************************************************************************************************************/
////SETUP
/**************************************************************************************************************************************************************/

void setup() {

  // watchDog_counter = 0;

  //SoftwareServo setup
  feederServo.attach(0);                        // attaches the servo on pin 0 to the servo object


  //pins setup
  DDRB &= ~(1 << voltagePin);
  PORTB |= (1 << voltagePin);
  DDRB |= (1 << ledPin);



  //Pin Change interrupt setup
  //Enable interrupts
  GIMSK |= (1 << PCIE);
  PCMSK |= _BV(PCINT4);
  sei();


  //EXT INTERRUPT STUFF
  //IMSK |= (1 << PCIE);
  //set low lvl trigger (keeps triggering while button not released)
  //MCUCR &= ~(1 << ISC01);
  //TRYING TO PUT THIS INSTEAD!! (FOR FALLING EDGE):  MCUCR |= (1 << ISC01);
  //MCUCR |= (1 << ISC00);


  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}



/**************************************************************************************************************************************************************/
////LOOP
/**************************************************************************************************************************************************************/


void loop() {
  //Watchdog timer setup  //Putting this in setup causes a problem when servo writing
  //This order of commands is important and cannot be combined
  MCUSR &= ~(1 << WDRF); //Clear the watch dog reset
  WDTCR |= (1 << WDCE) | (1 << WDE); //Set WD_change enable, set WD enable
  WDTCR = 0B100001; //Set prescaler to 8 sec (see p.46 of datasheet to change prescaler), and OVERWRITE WDTCR value ( dont just use |= )
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int



  ADCSRA &= ~_BV(ADEN);      //Turn ADC off, saves ~230uA
  sleep_cpu();
  ADCSRA |= _BV(ADEN);        //will ADC need to be turned on after sleep?


  delay(1000);
  PORTB &= ~(1 << ledPin);



  if (!(watchDog_counter % wd_target)) {
    //  feedTheFish();
  }


  if (buttonFlag) {
    // buttonsAction(); //reads voltage and makes a different action depending on button that was pressed
    buttonFlag = 0;
  }


}


/**************************************************************************************************************************************************************/
////FUNCTIONS
/**************************************************************************************************************************************************************/




ISR(PCINT0_vect) { //NOT PCINT0_vect!
  if (!(PINB & (1 << PB4))) buttonFlag = 1; //tell the arduino a button was pressed, not released
  PORTB |= (1 << ledPin);
}




ISR(WDT_vect) {
  watchDog_counter++;
  PORTB |= (1 << ledPin);
}
