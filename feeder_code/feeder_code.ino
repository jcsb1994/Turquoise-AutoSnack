
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
volatile int watchDog_counter = 0;
//wd counter must reach a target, different depending on nb of meals per day
//1 meal per day: target = 10800
//2 meals: trgt = 5400
//3 meals: trgt = 3600
int wd_target = 2

; //target for wd counter, changes depending on user settings for number of meals per day

//PC interrupt variables
volatile int buttonFlag;
unsigned long last_interrupt_time;
int voltage;

//Voltage divider button values (they are the same no matter the voltage)  5 X 1,5K OHMS  
//button 1 is "Nb of meals"
//calculated value on 1023 between 800ohms and 60 ohms is 71
#define min1  140
#define max1  159

//button 2 is "Size of meals"
//calculated value on 1023 between 800ohms and 45 ohms is 54
#define min2  109
#define max2  139

//button 3 is "Reset"
//calculated value on 1023 between 800ohms and 30 ohms is 37
#define min3  60
#define max3  90

//button 4 is "toggle meal led"
//calculated value on 1023 between 800ohms and 15 ohms is 19
#define min4  20
#define max4  49


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
//Byte = 0B buzzer resetLed ... servoLogic
#define buzzer 7
#define resetLed 6
#define sizeLed 5
#define nbLed 4
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
  DDRB &= ~(1 << voltagePin);
  PORTB |= (1 << voltagePin);
  DDRB |= (5 << 1);     //set pins 1 and 3 as outputs
//pin 0 should be output too. why works? looks like latchpin doesnt need to be output..



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
  cli();
  //This order of commands is important and cannot be combined
  MCUSR &= ~(1 << WDRF); //Clear the watch dog reset
  WDTCR |= (1 << WDCE) | (1 << WDE); //Set WD_change enable, set WD enable
  WDTCR = 0B100001; //Set prescaler to 8 sec (see p.46 of datasheet to change prescaler), and OVERWRITE WDTCR value ( dont just use |= )
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int
  sei();

  //74hc9595 setup (doesnt work in setup function)
  leds = 0;
  updateShiftRegister();

  ADCSRA &= ~_BV(ADEN);      //Turn ADC off, saves ~230uA
  sleep_cpu();
  ADCSRA |= _BV(ADEN);        //will ADC need to be turned on after sleep?


/*sketch works without the LOW button check (only 1 interrupt fires anyway, probably because voltage goes out of acceptable range 
 * for button activation on the rising interrupt, so buttonsAction() doesn<t do anything when button is released*/   
  if (!(PINB & (1 << switchPin)))  buttonFlag = 1; //tell the arduino a button was pressed, not released   
  

  //DEBUG-------------------------------------------
  leds = 1 << signalLed;  //turn on the big LED for telling fish food is coming (in this case just for debugging)
  updateShiftRegister();
  delay(10);
  leds = 0;
  updateShiftRegister();
  //delay(10);*/
  //------------------------------------------------

  //check if time has come to feed
  if (watchDog_counter >= wd_target) {
    feedTheFish();
    watchDog_counter = 0;
  }

  //check if a button was pressed
 if (buttonFlag) {
    buttonFlag = 0;
    buttonsAction();
    
 }


}


/**************************************************************************************************************************************************************/
////FUNCTIONS
/**************************************************************************************************************************************************************/


ISR(PCINT0_vect) { //NOT PCINT0_vect!

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
