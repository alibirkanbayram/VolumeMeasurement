//━━━━━━━━━━━━━━━━━━━━━━━━━━━ Library ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#include <Stepper.h> // Stepper motor Library
#include <FirebaseESP32.h> // Firebase Library
#include <WiFiManager.h> // Wifi Manager Library
#include <Preferences.h> // Wifi Manager Library

//━━━━━━━━━━━━━━━━━━━━━━━━━━━ Definitions ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
const int motorStep = 200; // Motor step
const int laps = 21.5; // Motor number of laps
const int motorRPM = 240; // Motorun rpm (step/second)
//Stepper stepperMotor(motorStep, D8,D7,D6,D5); // Step motor definition (nodemcu)
Stepper stepperMotor(motorStep, 15,21,4,5); // Step motor definition (esp32)
// const int relay=13 ; // Motor relay
const String machineID=""; // Machine ID
const int backSwitch=34; // Back limit switch pin
const int frontSwitch=35; // Front limit switch pin
const int backIR=36; // Back IR sensor
const int frontIR=39; // Front IR sensor
int trigPin1=32,trigPin2=25,trigPin3=27;
int echoPin1=33,echoPin2=26,echoPin3=14;

bool coverStatus=0;

Preferences preferences_nesnesi; // Verileri saklamak için bir Preferences nesnesi oluşturuyoruz.

//━━━━━━━━━━━━━━━━━━━━━━━━━━━ Firebase Definitions ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Database is firebase
#define DATABASE_URL "url"
#define DATABASE_SECRET "token"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String path = "path"; // Database path (Database için gerekli yol)
bool res;
WiFiManager wm; // Wifi managerden bir nesne ürettik.

void goBack(){ // Go to back
  digitalWrite(role,LOW); // Relay on
  delay(2); // 2ms wait
  while(!digitalRead(backSwitch)){
    stepperMotor.step(-(motorAdim)); // Motor adımı ile tur sayısı çarpılır ve bu kadar adım geri gider.
    // delay(1); // Delay function (1-10ms)
  }
  delay(2);
  digitalWrite(role,HIGH);
  // Firebase.setBool(fbdo, path + "/COVER_STATUS", true);
}
void goOn(){ // Go to front
  digitalWrite(role,LOW); // Relay on
  delay(2); // 2ms wait
  while(!digitalRead(frontSwitch)){
    stepperMotor.step(motorAdim); // Motor adımı ile tur sayısı çarpılır ve bu kadar adım geri gider.
    // delay(1); // Delay function (1-10ms)
  }
  delay(2);
  digitalWrite(role,HIGH);
  // Firebase.setBool(fbdo, path + "/COVER_STATUS", true);
}
void findLocation(){ // Location finding
  if(!digitalRead(frontSwitch))
    goBack();
  coverStatus=0;
}

void isObject(){ //
  if (!digitalRead(backSwitch))
    goBack();
  if(digitalRead(backIR)){
    goOn();
    volumeMeasurement();
    goBack();
  }
}

void distanceMeasurement(int trigPin,int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  time = pulseIn(echoPin, HIGH);
  distance= time /29.1/2;
  returun distance;
}

void volumeMeasurement(){ // Volume measurement
  // pass
}

void setup(){ // Setup function
  Serial.begin(115200); // Serial screen buand
  stepperMotor.setSpeed(motorRPM); // Step motor speed definition
  // wm.resetSettings(); // Wifi data clear
  WiFi.mode(WIFI_STA);  // Wifi mode
  res=wm.autoConnect("Volume","12345678"); // Wifi name and password

 if(!res)
   Serial.println("Wifi bağlantısı kurulamadı!");
 else
   Serial.println("Wifi bağlantısı kuruldu.");

  WiFi.localIP();
  Serial.println(WiFi.localIP());

  // Firebase kurulumları
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);
  pinMode(backSwitch,INPUT); pinMode(frontSwitch,INPUT);
  pinMode(backIR,INPUT); pinMode(frontIR,INPUT);

  findLocation(); // Finding Location
}

void loop(){ // Loop function
  isObject();
}
