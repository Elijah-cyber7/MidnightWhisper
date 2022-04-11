// For I2C lcd (16,2)
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4); // set the lcd address to (0x27,...,...) for a 16 char and 2 line display (16,2)

//pressure sensor inits 
const int pressureInput = A0;
const int inputAtZero = 102.4;
const int inputAtMax = 921.6;
const int maxPressure = 500;
float currentPressure;
float sensorInput;

// solenoid relays 
const int Valve_1 = 12;
const int Valve_2 = 13;

//Inputs from user
const int trigger = 10;
int triggerState;
const int setButton = 11;
int setButtonState;
const int psiDial = A1;
int dialReading;

// extra variables displayed on screen
int currentShotCount = 0;
float pressureUsed = 0.0;

// pressure reading function 
float readPsi(float currentReading)
  {
  //converting analogInput to PSI
  float psi = 0;
  psi = float(((currentReading - inputAtZero)* 500)/(inputAtMax - inputAtZero));
  return psi;
  }
// shoot gate then trigger reset
void shoot()
  {
  //opening valves to shoot. Exhaust valve open and inlet closed
  digitalWrite(Valve_1, LOW);
  digitalWrite(Valve_2, HIGH);
  //displaying how much psi was in shot 
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("Firing");
  lcd.setCursor(3,0);
  lcd.print("PSI :"+ String(readPsi(sensorInput)));
  //delay to disapate gas
  delay(300);
  //reset valves and clear display
  lcd.clear();
  digitalWrite(Valve_2,LOW);
  digitalWrite(Valve_1, LOW);
  }
//set resivuor pressure function 
void setPressure(float cPressure, int TargetPressure)
    {
    while((currentPressure + 0.61) < float(TargetPressure))
     {
    digitalWrite(Valve_1, HIGH);
    digitalWrite(Valve_2, LOW);    
    currentPressure = analogRead(pressureInput);
     }
    digitalWrite(Valve_1, LOW);
    digitalWrite(Valve_2, LOW);
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("PSI set to:"+ TargetPressure);
    delay(500);
    }

// Displaying values on serial and lcd
void displayCurrent()
  {
  int psiSensor = readPsi(sensorInput);
  Serial.println("PSI: " + String(psiSensor));
  Serial.println("Set PSI" + String(dialReading));
  lcd.setCursor(0,0);
  lcd.print("PSI: " + String(psiSensor));
  lcd.setCursor(8,0);
  lcd.print("Set: " + String(dialReading));
  delay(200);
  }

// function for different button inputs
void readInputs(int trig, int set, int dial)
   {
   int trigpush = 0;
   int lastpush = 0;
   int setpush  = 0;
 
   if(trig == HIGH)
     {
      shoot();
     }
  else if(set == HIGH && (dial > currentPressure))
     {
     setPressure(sensorInput, dial); 
     }
  else
     {
     //displaying values if no actions need to be taken
     displayCurrent();
     Serial.println("Trigger = " + String(trig));
     Serial.println("Setbutton = " + String(set));
     }
  }

// Regular cringe arduino default functions
void setup() 
  {
  //lcd setup 
  lcd.init();
  lcd.backlight();
  //solenoid setup
  pinMode(Valve_1, OUTPUT);
  pinMode(Valve_2, OUTPUT);
  //button set up 
  pinMode(trigger, INPUT);
  pinMode(setButton, INPUT);
  //closing the valves 
  digitalWrite(Valve_1, LOW);
  digitalWrite(Valve_2, LOW);
  //starting the serial monitor
  Serial.begin(9600);
  }

void loop() {
  sensorInput = analogRead(pressureInput);
  dialReading = analogRead(psiDial);
  dialReading = map(dialReading, 0, 1023, 0, 500); 
  triggerState = digitalRead(trigger);
  setButtonState =  digitalRead(setButton);
  currentPressure = readPsi(sensorInput);
  readInputs(triggerState, setButtonState, dialReading);
  
}
