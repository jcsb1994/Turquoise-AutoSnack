
/**************************************************************************************************************************************************************/
////LIBRARIES
/**************************************************************************************************************************************************************/

#include <avr/sleep.h>
#include <avr/interrupt.h>



/**************************************************************************************************************************************************************/
////PINS
/**************************************************************************************************************************************************************/


#define switchPin  3
#define ledPin  2
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

/**************************************************************************************************************************************************************/
////OBJECTS
/**************************************************************************************************************************************************************/

/**************************************************************************************************************************************************************/
////SETUP
/**************************************************************************************************************************************************************/

void setup() {

  watchDog_counter = 0;


  //pins setup
  pinMode(switchPin, INPUT);
  digitalWrite(switchPin, HIGH);
  pinMode(ledPin, OUTPUT);



  //Watchdog timer setup
  //This order of commands is important and cannot be combined
  MCUSR &= ~(1 << WDRF); //Clear the watch dog reset
  WDTCR |= (1 << WDCE) | (1 << WDE); //Set WD_change enable, set WD enable
  WDTCR = 0B100001; //Set prescaler to 8 sec (see p.46 of datasheet to change prescaler), and OVERWRITE WDTCR value ( dont just use |= )
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int

}



/**************************************************************************************************************************************************************/
////LOOP
/**************************************************************************************************************************************************************/


void loop() {
  sleep();
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
}


/**************************************************************************************************************************************************************/
////FUNCTIONS
/**************************************************************************************************************************************************************/

/*Sleep function, activates external interrupts*/
void sleep() {
  //VOIR PCIE bit dans GIMSK REGISTER POUR ++ INTERRUPTS DE PCINT0 à 5, avec l'aide de PCMSK0
  //_BV(); vient de bit value. _BV(x) équivaut à 1 << x, donc on peut mettre une bit désirée
  //de la mm manière que |= ou &=
  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on

  sei();                                  // Enable interrupts
} // sleep



ISR(PCINT0_vect) {
  
  unsigned long interrupt_time = millis(); //saves time at which this interrupt starts
  if (interrupt_time - last_interrupt_time > 200)  //compares last time interrupt was finished to now. We give 200 millis to ignore rebounds
  {

    voltage = analogRead(voltagePin);
    buttonFlag = 1; //tell the arduino a button was pressed



    /**************************************************************************************************************************************************************/
    //UP BUTTON IS PRESSED
    if (minNb <= voltage && voltage <= maxNb) {

    }
    

    /**************************************************************************************************************************************************************/
    //DOWN BUTTON IS PRESSED
    if (minSize <= voltage && voltage <= maxSize) {

    }
    


    /**************************************************************************************************************************************************************/
    //SELECT BUTTON IS PRESSED
    if (minReset <= voltage && voltage <= maxReset) {

    }

    //SAVE WHEN THE INTERRUPT OCCURED TO PREVENT BOUNCES FROM TRIGGERING MULTIPLE INTERRUPTS
    last_interrupt_time = interrupt_time;
  }
}



ISR(WDT_vect) {
  watchDog_counter++;
}

