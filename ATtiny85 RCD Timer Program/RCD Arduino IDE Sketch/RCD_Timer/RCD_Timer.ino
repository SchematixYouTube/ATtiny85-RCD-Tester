/* ATtiny85 as an I2C Master   Ex2        BroHogan                           1/21/11
   Modified for Digistump - Digispark LCD Shield by Erik Kettenburg 11/2012
   SETUP:
   ATtiny Pin 1 = (RESET) N/U                      ATtiny Pin 2 = (D3) N/U
   ATtiny Pin 3 = (D4) to LED1                     ATtiny Pin 4 = GND
   ATtiny Pin 5 = SDA on DS1621  & GPIO            ATtiny Pin 6 = (D1) to LED2
   ATtiny Pin 7 = SCK on DS1621  & GPIO            ATtiny Pin 8 = VCC (2.7-5.5V)
   NOTE! - It's very important to use pullups on the SDA & SCL lines!
   PCA8574A GPIO was used wired per instructions in "info" folder in the LiquidCrystal_I2C lib.
   This ex assumes A0-A2 are set HIGH for an addeess of 0x3F
   LiquidCrystal_I2C lib was modified for ATtiny - on Playground with TinyWireM lib.
   TinyWireM USAGE & CREDITS: - see TinyWireM.h
*/

//#define DEBUG
#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI - comment this out to use with standard arduinos
#include <LiquidCrystal_I2C.h>          // for LCD w/ GPIO MODIFIED for the ATtiny85

#define GPIO_ADDR     0x27             // (PCA8574A A0-A2 @5V) typ. A0-A3 Gnd 0x20 / 0x38 for A - 0x27 is the address of the Digispark LCD modules.

#define SENSE_PIN 1
#define RESET_PIN 3

LiquidCrystal_I2C lcd(GPIO_ADDR, 16, 2); // set address & 16 chars / 2 lines

#define TIME_BIAS 0L

typedef enum MACHINE_STATE {
  ST_INITIAL,
  ST_READY,
  ST_TIMING,
  ST_DISPLAY_PENDING,
  ST_DISPLAY,
} MACHINE_STATE;

MACHINE_STATE machineState;

bool listening = true;

void setup() {
  machineState = ST_INITIAL;
  // normally high inputs
  pinMode(SENSE_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT_PULLUP);
  TinyWireM.begin();                    // initialize I2C lib - comment this out to use with standard arduinos
  lcd.init();                           // initialize the lcd
  lcd.backlight();                      // Print a message to the LCD.
  lcd.print(".. GFCI");
}

unsigned long sense_start = 0L;
unsigned long sense_interval = 0L;

void loop() {
  bool sensePin = digitalRead(SENSE_PIN);
  bool resetPin = digitalRead(RESET_PIN);

  switch ( machineState ) {
    case ST_INITIAL:
      new_state(ST_READY);
      break;
    case ST_READY:
      if ( ! sensePin ) {
        new_state(ST_TIMING);
      }
      break;
    case ST_TIMING:
      if ( sensePin ) {
        new_state(ST_DISPLAY_PENDING);
      }
      break;
    case ST_DISPLAY_PENDING:
      new_state(ST_DISPLAY);
      break;
    case ST_DISPLAY:
      if ( ! resetPin ) {
        new_state(ST_READY);
      }
      break;
  }
}

unsigned long timer_start;
unsigned long timer_display;

void new_state(const MACHINE_STATE newState) {
  machineState = newState;
  switch ( newState ) {
    case ST_READY:
      lcd.clear();
      lcd.print("READY");
      break;
    case ST_TIMING:
      timer_start = millis();
      lcd.clear();
      lcd.print("TESTING");
      break;
    case ST_DISPLAY_PENDING:
      {
        unsigned long now = millis();
        timer_display = TIME_BIAS + ( now - timer_start );
        lcd.clear();
        lcd.print(timer_display);
      }
      break;
    case ST_DISPLAY:
      // Just waiting for reset
      break;
  }
}
