//━━━━━━━━━━━━━━━━━━━━━━━━━━━ Library ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#include <FirebaseESP32.h> // Firebase Library
#include <WiFiManager.h> // Wifi Manager Library
//━━━━━━━━━━━━━━━━━━━━━━━━━━━ Definitions ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
const int dirPin = 2;
const int stepPin = 3;
const int stepsPerRevolution = 200;
const int stepMotorDelay=50;
const int relay=13 ; // Motor relay
const String machineID=""; // Machine ID
const int backSwitch=34; // Back limit switch pin
const int frontSwitch=35; // Front limit switch pin
const int backIR=36; // Back IR sensor
const int frontIR=39; // Front IR sensor
int trigPin1=32,trigPin2=25,trigPin3=27;
int echoPin1=33,echoPin2=26,echoPin3=14;
double v=0; // Volume
bool coverStatus=false;
//━━━━━━━━━━━━━━━━━━━━━━━━━━━ Firebase Definitions ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Database is firebase
#define DATABASE_URL "url"
#define DATABASE_SECRET "token"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String path = "path"; // Database path (Database için gerekli yol)
bool res;
WiFiManager wm; // Wifi manager object
void motorWork(){
  for(int x = 0; x < stepsPerRevolution; x++){
		digitalWrite(stepPin, HIGH); delayMicroseconds(stepMotorDelay);
		digitalWrite(stepPin, LOW); delayMicroseconds(stepMotorDelay);
	}
}
void goBack(){ // Go to back
  digitalWrite(role,LOW); // Relay on
  delayMicroseconds(5);
  while(!digitalRead(backSwitch)){
    digitalWrite(dirPin, LOW);
    motorWork(); // The motor step is multiplied by the number of revolutions and it goes back that many steps.
    // delay(1); // Delay function (1-10ms)
  }
  delayMicroseconds(5);
  digitalWrite(role,HIGH);
  // Firebase.setBool(fbdo, path + "/COVER_STATUS", false);
}
void goOn(){ // Go to front
  digitalWrite(role,LOW); // Relay on
  delayMicroseconds(5);
  while(!digitalRead(frontSwitch)){
    digitalWrite(dirPin, HIGH);
    motorWork();// The motor step is multiplied by the number of revolutions and it goes back that many steps.
    // delay(1); // Delay function (1-10ms)
  }
  delayMicroseconds(5);
  digitalWrite(role,HIGH);
  // Firebase.setBool(fbdo, path + "/COVER_STATUS", true);
}
void findLocation(){ // Location finding
  if(!digitalRead(frontSwitch))
    goBack();
  coverStatus=false;
}
void isObject(){
  if (!digitalRead(backSwitch))
    findLocation();
  if(digitalRead(backIR)){
    digitalWrite(role,LOW); // Relay on
    delayMicroseconds(5);
    digitalWrite(dirPin, LOW);
    while(!digitalRead(frontIR))      
      motorWork();
    while (digitalRead(frontSwitch)){
      stepsPerRevolution=100;
      motorWork(); // The motor step is multiplied by the number of revolutions and it goes back that many steps.
      stepsPerRevolution=200;
      v+=volumeMeasurement()*0.1*; // Sensibility 0.01 and cm->mm == 0.1
    }
    Serial.print("Volume measurement : ");
    Serial.println(v);
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
  y1=distanceMeasurement(trigPin1,echoPin1);
  y2=distanceMeasurement(trigPin2,echoPin2);
  y=y1+y2; y= 15-y;
  h=distanceMeasurement(trigPin3,echoPin3);
  if(h<=15)
    h=15-h;
  return x*h;
}
void setup(){ // Setup function
  Serial.begin(115200); // Serial screen buand
  // wm.resetSettings(); // Wifi data clear
  WiFi.mode(WIFI_STA);  // Wifi mode
  res=wm.autoConnect("Volume","12345678"); // Wifi name and password
 if(!res)
   Serial.println("Wifi connection failed!");
 else
   Serial.println("Wifi connected.");

  WiFi.localIP();
  Serial.println(WiFi.localIP());

  // Firebase definition
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);
  pinMode(stepPin, OUTPUT); // Step motor definition
	pinMode(dirPin, OUTPUT); // Step motor definition
  pinMode(backSwitch,INPUT); pinMode(frontSwitch,INPUT);
  pinMode(backIR,INPUT); pinMode(frontIR,INPUT);
  findLocation(); // Finding Location
}
void loop(){ // Loop function
  isObject();
}
