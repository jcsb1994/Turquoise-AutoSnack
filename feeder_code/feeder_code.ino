
/**************************************************************************************************************************************************************/
////LIBRARIES
/**************************************************************************************************************************************************************/

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <SoftwareServo.h>



/**************************************************************************************************************************************************************/
////PINS
/**************************************************************************************************************************************************************/

#define switchPin  2
#define voltagePin  A2

#define dataPin   3
#define clockPin  1
#define latchPin  0  //connecting latch and servo on the same pin seems to work for now

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
#define minNb  0 //175 to 185 based on a ___ resistor value for "Num of meals" button
#define maxNb  1023
#define minSize   //189 to 220 based on a ___ resistor value for "Size of meals" button
#define maxSize
#define minReset   //120 to 166 based on a ___ resistor value for SELECT button
#define maxReset


//Food Data variables

//amount of meals per day
int meal_nb = 1;
#define max_meal_nb 3

//amount of cylinder spins (meal size)
int meal_size = 1;
#define max_meal_size 5


//Servo variables
int pos = 0; //NEEDS TO BE AN INT, NOT A BYTE

//74hc595 variables
/*Q1  Q2, Q3 are 3 button LEDs: bit [4-6]
  Q0 is piezo buzzer: bit 7
  Q3 is transistor for meal LED bit 4
  Q4 is transistor for servo: bit 3
  Q7 is transistor for servo logic: bit 0
*/
byte leds = 0;
//bit constants
#define buzzer 7
#define nbLed 6
#define sizeLed 5
#define resetLed 4
#define servoVcc 3
#define signalLed 2
#define servoLogic 0

/**************************************************************************************************************************************************************/
////OBJECTS
/**************************************************************************************************************************************************************/
SoftwareServo feederServo;      // creates feederServo as object to control a servo


/**************************************************************************************************************************************************************/
////SETUP
/**************************************************************************************************************************************************************/

void setup() {

  // watchDog_counter = 0;

  //pins setup
  DDRB &= ~(1 << switchPin);
  PORTB |= (1 << switchPin);
  DDRB |= (5 << 1);     //set pins 1 and 3 as outputs


  //74hc9595 setup
  leds = 0;
  updateShiftRegister();


  //SoftwareServo setup
  feederServo.attach(0);                        // attaches the servo on pin 0 to the servo object


  //Pin Change interrupt setup
  //Enable interrupts
  GIMSK |= (1 << PCIE);
  PCMSK |= _BV(PCINT2);
  sei();


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


  //DEBUG-------------------------------------------
  leds = 1 << signalLed;  //turn on the big LED for telling fish food is coming
  updateShiftRegister();
  delay(100);
  leds = 0;
  updateShiftRegister();
  delay(100);
  //------------------------------------------------

  
  if (!(watchDog_counter % wd_target)) {
    feedTheFish();
  }

  if (buttonFlag) {
    buttonsAction();
    buttonFlag = 0;
  }


}


/**************************************************************************************************************************************************************/
////FUNCTIONS
/**************************************************************************************************************************************************************/


ISR(PCINT0_vect) { //NOT PCINT0_vect!
  if (!(PINB & (1 << switchPin))) buttonFlag = 1; //tell the arduino a button was pressed, not released
}


ISR(WDT_vect) {
  watchDog_counter++;
}


void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
}
