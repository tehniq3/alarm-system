/*
Zobacz opis alarmu na http://majsterkowo.pl/zabezpieczamy-dom-czyli-jak-zrobic-alarm-z-odswiezacza-powietrza/
Autor programu: Łukasz Więcek
small changes by Nicu FLORICA (aka niq_ro) from http://www.arduinotehniq.com/
*/

#include <Keypad.h>                // library for keyboard
#include <Password.h>              // library for password
Password password = Password( "1234" );  // pasword

const byte rows = 4;                     // four rows       
const byte cols = 3;                     // three columns
char keys[rows][cols] = {                // keys on keypad
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};

byte rowPins[rows] = {2, 7, 6, 4};
byte colPins[cols] = {3, 1, 5};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);



#define senzor 14      // D14 is A0, pin for PIR sensor data
//#define contact 15     // D2, pin for on/off alarm
#define syrena 13      // D13, pin for syren, buzzer
#define rosu 11        // D13, pin for red led
#define verde 9        // D9, pin for green led
#define albastru 10    // D10, pin for blue led

int senzorData;
unsigned long ceas, timpmemorat;

int intarziereactivare = 20;    // delay for standby to armed
int intarzieredezactivare = 10; // delay for triggered to syren activated
int timpurlat = 10;             // time to syren is on

// variable for states "0"
char caz = 0;

int sistem = 0;      // sistem is 0 for off and 1 for on


/*
States for :
  0 - off
  1 - stand-by
  2 - waitting
  3 - countdown
  4 - alarm
*/

void setup()
  {
  keypad.addEventListener(keypadEvent); // se creaza un obiect pentru urmarire apasare taste
  
//  Serial.begin(9600);  
//  Serial.println("Alarm system by Łukasz Więcek from http://majsterkowo.pl");
//  Serial.println("Small changed by niq_ro from http://arduinotehniq.com");
  pinMode(syrena, OUTPUT);
  pinMode(senzor, INPUT);
//  pinMode(contact, INPUT);
  pinMode(rosu, OUTPUT);
  pinMode(albastru, OUTPUT);
  pinMode(verde, OUTPUT);

//  pinMode(contact, INPUT_PULLUP);
  }

void loop()
  {
  ceas = millis();    // read the internal clock

 keypad.getKey();
 
//  if(digitalRead(contact) == HIGH)      
    if (sistem%2 == 0)
    {
    // alarm is off
    analogWrite(verde, 0);
    analogWrite(rosu, 0);
    analogWrite(albastru, 240);

    digitalWrite(syrena, LOW);
    caz = 0;
 //   Serial.println("System is OFF !");
    }

  else
    {
    // alarm is on
    if(caz == 0) 
     {
     caz = 1;
     timpmemorat = ceas;
     analogWrite(albastru, 0);
     }

    if(caz == 1)              // system waitting
      {
 /*     analogWrite(verde, 240);
      delay(100);
 keypad.getKey();     
      analogWrite(verde, 0);
      delay(100);
*/
      if ((ceas%1000)<500) analogWrite(verde, 240);
      else analogWrite(verde, 0);
 keypad.getKey();
      if(ceas >= timpmemorat + intarziereactivare * 1000) {caz = 2;}
//      Serial.println("System is waitting !");
      }
      
    if(caz == 2)              // system is armed
      {
      analogWrite(verde, 240); 
 keypad.getKey();
   //   senzorData = analogRead(senzor);
      senzorData = digitalRead(senzor);  
//      Serial.print("senzordData = ");
//      Serial.println(senzorData);
   
  //    if(senzorData > 600) {caz = 3; timpmemorat = ceas;}
     if(senzorData == HIGH)
       {
       caz = 3;
       timpmemorat = ceas;
       analogWrite(verde, 0);
       }
 //     Serial.println("System is armed !");
      }

    if(caz == 3)              // system is triggered and countdown
      {             
 /*     analogWrite(rosu, 240);
      delay(100);
 keypad.getKey();
      analogWrite(rosu, 0);
      delay(100);
*/
      if ((ceas%500)<100) analogWrite(rosu, 240);
      else analogWrite(rosu, 0);
 keypad.getKey();
      if(ceas >= timpmemorat + intarzieredezactivare * 1000) {caz = 4; timpmemorat = ceas;}
 //     Serial.println("System is triggered and countdown !");
      }

    if(caz == 4)              // syren (buzzer) is active
      {
      digitalWrite(syrena, HIGH);
      analogWrite(rosu, 240);
 //     Serial.println("Syren is active !");
 keypad.getKey();      
      if(ceas >= timpmemorat + timpurlat * 1000) {caz = 2; analogWrite(rosu, 0);}
      }
    }
  }
  
  //take care of some special events
void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
    case PRESSED:
	Serial.print("Pressed: ");
	Serial.println(eKey);
	switch (eKey){
	  case '*': checkPassword(); break;
	  case '#': password.reset(); break;
	  default: password.append(eKey);
     }
  }
}

  
  void checkPassword(){
  if (password.evaluate()){
//    Serial.println("Success");
  sistem++;
    //Add code to run if it works
  }else{
//    Serial.println("Wrong");
    //add code to run if it did not work
  }
}
