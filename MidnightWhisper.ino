

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define IRpin 4
LiquidCrystal_I2C lcd(0x27,20,4); // set the lcd address to (0x27,...,...) for a 16 char and 2 line display (16,2)



//pressure sensor inits 

const int inputAtZero = 101.0;
const int tinputAtZero = 108.3;
const int inputAtMax = 921.6;  
const int tinputAtMax = 914.7;
const int tankPressurePin = A1;
const int pressureInput = A3;
const int maxPressure = 500;
const int tmaxPressure = 1000;

int sensorInput;
int tankPressure;
int presscount = 0;
int triggerState; 
 
float input;
const int Valve_1 = 3;
const int Valve_2 = 2;

const int trigger = 7;
//const int psiDial = A2;
int dialReading;

long velocity = -1;
unsigned long timeTracker;
unsigned long initial ;
unsigned long exiting ;
float timeTot ;
const int xx = 1000000;
unsigned long yy;

float readPsi(float currentReading)
{
 float psi = 0;
 psi = float(((currentReading - inputAtZero)* maxPressure)/(inputAtMax - inputAtZero));
 return psi;
}

float readtPsi(float currentReading)
{
 float psi = 0;
 psi = float(((currentReading - tinputAtZero)* tmaxPressure)/(tinputAtMax - tinputAtZero));
 return psi;
}


void setPressure(float tPressure){

timeTracker = millis(); 
while(((readPsi(analogRead(pressureInput)) + 0.61) < tPressure) && ((millis() - timeTracker) < 2000)){
  lcd.setCursor(4,0);
  lcd.print("Filling.");
  digitalWrite(Valve_1, HIGH);
  digitalWrite(Valve_2, LOW);
}
digitalWrite(Valve_1, LOW);
digitalWrite(Valve_2, LOW);
  lcd.clear();
}



void shoot(){
  digitalWrite(Valve_1, LOW);
  digitalWrite(Valve_2, LOW);
  digitalWrite(Valve_2, HIGH);
  lcd.setCursor(4,0);
  lcd.print("Shooting"); 
  initial = micros();
  lcd.clear();
  lcd.print("Done");
  delay(1000);
  lcd.clear();
  digitalWrite(Valve_2, LOW);
}

void mainFunc(){
     tankPressure = int(readtPsi(analogRead(tankPressurePin)));
     sensorInput = int(readPsi(analogRead(pressureInput)));
     Serial.println(analogRead(pressureInput));
     
     dialReading = 300;//map((analogRead(psiDial)), 0, 1023, 0, 500);  
     triggerState = digitalRead(trigger);
     
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("PSI: " + String(sensorInput));
     lcd.setCursor(0,1);
     lcd.print("Tank: " + String(tankPressure));
     lcd.setCursor(11,1);
     delay(200);
     lcd.clear();
     if(triggerState){
        presscount++;
        if(((presscount % 2) == 1) && dialReading > sensorInput){
           setPressure(dialReading);
           delay(200);
        }
        else if((presscount % 2) == 1){
          shoot();
          delay(200);
        }
        else if ((presscount %2) == 0){
          shoot();
          delay(200);   
     }
}
}
void setup(){
  lcd.init();
  lcd.backlight();
 pinMode(Valve_1, OUTPUT);
 pinMode(Valve_2, OUTPUT);
 pinMode(trigger, INPUT);
 Serial.begin(9600);
 digitalWrite(Valve_1, LOW);
 digitalWrite(Valve_2, LOW);


}

void loop(){
  mainFunc();
  //Serial.println("Sensor Input: " + String(digitalRead(IRpin)));
}
