// For I2C lcd (16,2)
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4); // set the lcd address to (0x27,...,...) for a 16 char and 2 line display (16,2)



//pressure sensor inits 

const int pressureInput = A0;
const int inputAtZero = 102.4;
const int inputAtMax = 921.6;
const int maxPressure = 500;
float sensorInput;

// solenoid relays 
float input;
const int Valve_1 = 12;
const int Valve_2 =13 ;

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

//RangeFinder variables and set up 
const int trigPin = 9;
const int echoPin = 8;

// pressure reading function 
float readPsi(float currentReading)
{
 float psi = 0;
 psi = float(((currentReading - inputAtZero)* 500)/(inputAtMax - inputAtZero));
 return psi;
}

// shoot gate then trigger reset
void shoot(){
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
  //clear display, then display what is left
  
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("Shot-" + String(currentShotCount));
  //reset valves
  lcd.clear();
  digitalWrite(Valve_2,LOW);
  digitalWrite(Valve_1, LOW);

  
}
//set resivuor pressure function 


void setPressure(float cPressure, int TargetPressure){
  Serial.println("PSI: " + String(readPsi(cPressure)));
  Serial.println(String(TargetPressure));
  float currentPressure = 0;
  currentPressure = readPsi(cPressure);
  while((currentPressure + 0.61) < float(TargetPressure)){
    digitalWrite(Valve_1, HIGH);
    digitalWrite(Valve_2, LOW);
    //lcd printing 
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Setting PSI...");
    lcd.setCursor(2,1);
    lcd.print("To: "+  String(TargetPressure));
    
    
    // for serial reading uncomment Serial.println("Setting pressure: " + String(currentPressure) + " to " + String(TargetPressure));
      currentPressure = analogRead(pressureInput);
      delay(50);
    
  }
  digitalWrite(Valve_1, LOW);
  digitalWrite(Valve_2, LOW);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("PSI set to:"+ TargetPressure);
  delay(500);
  lcd.clear();
  
}



// Displaying values on serial and lcd

void displayCurrent(){
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
void readInputs(int trig, int set, int dial){
 int trigpush = 0;
 int lastpush = 0;
 int setpush  = 0;

 
 if(trig == HIGH){
 shoot();
 delay(1000);
 trigpush = 0;
 }
 if(set == HIGH){
  setpush = 1;
 }
  if((dial > readPsi(sensorInput)) && setpush == 1){
    setPressure(sensorInput, dial);
    setpush = 0;
    
  }
 else{
  
  Serial.println(dialReading);
  Serial.println("Trigger = " + String(trig));
  Serial.println("Setbutton = " + String(set));
 }
}

// Function for reading distance and displaying to lcd
void findRange(){
  long duration, cm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  int distance;
  // converting the time it takes to hit an object and get back to centimeters
 // then converting into feet, i was too lazy to do the ft per microsecond conversion lol
  distance = (duration/29/2) * .0328;
  // displaying it on screen
  lcd.setCursor(0,1);
  lcd.print("Range: " + String(distance));
}


// Regular cringe arduino default functions
void setup() 
{
  //lcd setup 
  lcd.init();
  lcd.backlight();
  
 pinMode(Valve_1, OUTPUT);
 pinMode(Valve_2, OUTPUT);
 pinMode(trigger, INPUT);
 pinMode(setButton, INPUT);
 Serial.begin(9600);
 digitalWrite(Valve_1, LOW);
 digitalWrite(Valve_2, LOW);
}

void loop() {
  sensorInput = analogRead(pressureInput);
  dialReading = analogRead(psiDial);
  dialReading = map(dialReading, 0, 1023, 0, 500); 
  triggerState = digitalRead(trigger);
  setButtonState =  digitalRead(setButton);
  Serial.println(dialReading);
  displayCurrent();
  readInputs(triggerState, setButtonState, dialReading);
  //input = Serial.parseFloat();
  
}
