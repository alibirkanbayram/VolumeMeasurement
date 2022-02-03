//━━━━━━━━━━━━━━━━━━━━━━━━━━━ Library ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#include <Stepper.h> // Stepper motor Library
#include <FirebaseESP32.h> // Firebase Library
#include <WiFiManager.h> // Wifi Manager Library

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

TaskHandle_t Task1;
TaskHandle_t Task2;



void goBack(){ // Go to back
  digitalWrite(role,LOW); // Relay on
  delay(2); // 2ms wait
  while(!digitalRead(backSwitch)){
    stepperMotor.step(-(motorStep)); // The motor step is multiplied by the number of revolutions and it goes back that many steps.
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
    stepperMotor.step(motorStep; // The motor step is multiplied by the number of revolutions and it goes back that many steps.
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

void isObject(){
  if (!digitalRead(backSwitch))
    goBack();
  if(digitalRead(backIR)){
    while (digitalRead(frontSwitch)){
      goOn();
      volumeMeasurement();
    }
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
  x1=distanceMeasurement();
  x2=distanceMeasurement();
  x=x1+x2; x= 15-x;
  h=distanceMeasurement();
  if(h<=15)
    h=15-h;
  return x*h;
}

void setup(){ // Setup function
  Serial.begin(115200); // Serial screen buand
  stepperMotor.setSpeed(motorRPM); // Step motor speed definition
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
  pinMode(backSwitch,INPUT); pinMode(frontSwitch,INPUT);
  pinMode(backIR,INPUT); pinMode(frontIR,INPUT);

  findLocation(); // Finding Location

  xTaskCreatePinnedToCore( // Multitasking CORE0
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore( // Multitasking CORE1
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
}

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led1, HIGH);
    delay(1000);
    digitalWrite(led1, LOW);
    delay(1000);
  } 
}

//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led2, HIGH);
    delay(700);
    digitalWrite(led2, LOW);
    delay(700);
  }
}

void loop(){ // Loop function
}
