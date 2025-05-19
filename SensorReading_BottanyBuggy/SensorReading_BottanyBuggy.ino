//COBA COBA SEND SOIL MOISTURE SENSOR AND DHT11 KE FIREBASE
//BISA SAVE DATA JUGA
//SAVE DATA UDAH GW HAPUS
//FILE INI UNTUK PAKE FOR LOOP BUAT DHT AND SOIL MOISTURE

/*
 *  Reference : https://randomnerdtutorials.com/esp32-firebase-realtime-database/
 */

//======================================== Including the libraries.
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "DHT.h"
// 
//======================================== 

//======================================== Insert your network credentials.
#define WIFI_SSID "HUAWEI-t99T"
#define WIFI_PASSWORD "Mm77BR6z"
//======================================== 

//======================================== Defines the pin of soile moisture sensor
#define moistureSensorPin1 34  //D34 
#define moistureSensorPin2 35  //D34 
#define moistureSensorPin3 32  //D34 
#define moistureSensorPin4 33  //D34 
// #define moistureSensorPin5 26  //D34 
// #define moistureSensorPin6 26  //D34 


#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht11(DHTPIN, DHTTYPE);
//======================================== 

// Defines the Digital Pin of the "On Board LED".
#define On_Board_LED 2

// Defines the Digital Pin of the LED.
// #define LED_01_PIN 13

//Provide the token generation process info.
#include "addons/TokenHelper.h"

//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAOGZa7-TjZw2SxkPHC2vvBhEg0RkChWxU"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://botanicalbuggy-default-rtdb.firebaseio.com/"

// Define Firebase Data object.
FirebaseData fbdo;

// Define firebase authentication.
FirebaseAuth auth;

// Definee firebase configuration.
FirebaseConfig config;

//======================================== Millis variable to send/store data to firebase database.
unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 3000; //--> Sends/stores data to firebase database every 10 seconds.
//======================================== 

// Boolean variable for sign in status.
bool signupOK = false;

float Temp_Val;
int Humd_Val;
// int LED_01_State;

int sensorVal1;
int sensorVal2;
int sensorVal3;
int sensorVal4;
// int sensorVal5;
// int sensorVal6;


//________________________________________________________________________________ Get temperature and humidity values from the DHT11 sensor.
void read_DHT11() {
  Temp_Val = dht11.readTemperature();
  Humd_Val = dht11.readHumidity();

  //---------------------------------------- Check if any reads failed.
  if (isnan(Temp_Val) || isnan(Humd_Val)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    Temp_Val = 0.00;
    Humd_Val = 0;
  }
  //---------------------------------------- 

  Serial.println("---------------Read_DHT11");
  Serial.print(F("Humidity   : "));
  Serial.print(Humd_Val);
  Serial.println("%");
  Serial.print(F("Temperature: "));
  Serial.print(Temp_Val);
  Serial.println("°C");
  Serial.println("---------------");
}

void read_moistureSensorPin(){
  sensorVal1 = analogRead(moistureSensorPin1);
  sensorVal2 = analogRead(moistureSensorPin2);
  sensorVal3 = analogRead(moistureSensorPin3);
  sensorVal4 = analogRead(moistureSensorPin4);
  // sensorVal5 = analogRead(moistureSensorPin5);
  // sensorVal6 = analogRead(moistureSensorPin6);

  Serial.print("Moisture1 Value: ");
  Serial.println(sensorVal1);

  Serial.print("Moisture2 Value: ");
  Serial.println(sensorVal2);

  Serial.print("Moisture3 Value: ");
  Serial.println(sensorVal3);

  Serial.print("Moisture4 Value: ");
  Serial.println(sensorVal4);
  
  // Serial.print("Moisture5 Value: ");
  // Serial.println(sensorVal5);

  // Serial.print("Moisture6 Value: ");
  // Serial.println(sensorVal6);
  

  if (isnan(sensorVal1)) {
  Serial.println(F("Failed to read from Soil moisture sensor 1!"));
  sensorVal1 = 0;
  }

  if (isnan(sensorVal2)) {
  Serial.println(F("Failed to read from Soil moisture sensor 2 !"));
  sensorVal2 = 0;
  }

  if (isnan(sensorVal3)) {
  Serial.println(F("Failed to read from Soil moisture sensor 3!"));
  sensorVal3 = 0;
  }
  if (isnan(sensorVal4)) {
  Serial.println(F("Failed to read from Soil moisture sensor 4!"));
  sensorVal4 = 0;
  }
  // if (isnan(sensorVal5)) {
  // Serial.println(F("Failed to read from Soil moisture sensor 5 !"));
  // sensorVal5 = 0;
  // }

  //   if (isnan(sensorVal6)) {
  // Serial.println(F("Failed to read from Soil moisture sensor 6 !"));
  // sensorVal6 = 0;
  // }
  }

//________________________________________________________________________________ 

//________________________________________________________________________________ Store data to firebase database.
void store_and_save_data_to_firebase_database() {
  Serial.println();
  Serial.println("---------------Store Data");
  digitalWrite(On_Board_LED, HIGH);

  for (int i = 1; i <= 5; i++) {
    // Create a string that includes the value of i
    String variableString = String(i);
    String fullPath1 = "soilMoisture1_Data/" + variableString + "/Moisture";
    String fullPath2 = "soilMoisture2_Data/" + variableString + "/Moisture";
    String fullPath3 = "soilMoisture3_Data/" + variableString + "/Moisture";
    
    // String fullPath5 = "soilMoisture5_Data/" + variableString + "/Moisture";
    String fullPathDHT1 = "DHT11_Data/" + variableString + "/Humidity";
    String fullPathDHT2 = "DHT11_Data/" + variableString + "/Temperature";
    read_moistureSensorPin(), read_DHT11();
      // Write an Int number on the database path soilMoisture_Data/Moisture
      if (Firebase.RTDB.setInt(&fbdo, fullPath1, sensorVal1)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
      if (Firebase.RTDB.setInt(&fbdo, fullPath2, sensorVal2)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      if (Firebase.RTDB.setInt(&fbdo, fullPath3, sensorVal3)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      
      // if (Firebase.RTDB.setInt(&fbdo, fullPath5, sensorVal5)) {
      //   Serial.println("PASSED");
      //   Serial.println("PATH: " + fbdo.dataPath());
      //   Serial.println("TYPE: " + fbdo.dataType());
      // }
      if (Firebase.RTDB.setInt(&fbdo, fullPathDHT1, Humd_Val)) {
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
      if (Firebase.RTDB.setFloat(&fbdo, fullPathDHT2, Temp_Val)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      }

      //ROBOT PATHWAY

      if (Firebase.RTDB.setFloat(&fbdo, "robot/hum", Humd_Val)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      }

      if (Firebase.RTDB.setFloat(&fbdo, "robot/sm1", sensorVal1)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      }
          
      if (Firebase.RTDB.setFloat(&fbdo, "robot/sm2", sensorVal2)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      }

      if (Firebase.RTDB.setFloat(&fbdo, "robot/sm3", sensorVal3)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      }
      
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
      // delay(2000);
      }
      Serial.println(fullPath1);
      Serial.println(fullPath2);
      Serial.println(fullPath3);
     
      // Serial.println(fullPath5);
      // Serial.println(fullPath6);
      Serial.print(fullPathDHT1);
      Serial.print(fullPathDHT2); // Print the full path to the serial monitor

      digitalWrite(On_Board_LED, LOW);
      Serial.println("---------------");

      delay(2000);
  }
}
/////////////////////////////////////////////////////////////////////////////////
//for loop untuk dht11
  // for (int j = 1; j <= 5; j++) {
  //   // Create a string that includes the value of i
  //   String variableString = String(j);
  //   String fullPathDHT1 = "DHT11_Data/" + variableString + "/Humidity";
  //   String fullPathDHT2 = "DHT11_Data/" + variableString + "/Temperature";
  //   read_DHT11();
  //     // Write an Int number on the database path soilMoisture_Data/Moisture
  //     if (Firebase.RTDB.setInt(&fbdo, fullPathDHT1, Humd_Val)) {
  //     Serial.println("PASSED");
  //     Serial.println("PATH: " + fbdo.dataPath());
  //     Serial.println("TYPE: " + fbdo.dataType());
  //   }
  //     if (Firebase.RTDB.setFloat(&fbdo, fullPathDHT2, Temp_Val)) {
  //     Serial.println("PASSED");
  //     Serial.println("PATH: " + fbdo.dataPath());
  //     Serial.println("TYPE: " + fbdo.dataType());
  // }
  //     else {
  //     Serial.println("FAILED");
  //     Serial.println("REASON: " + fbdo.errorReason());
  //     // delay(2000);
  // }

  // Serial.print(fullPath);
  // Serial.print(fullPathDHT1);
  // Serial.print(fullPathDHT2);

  // digitalWrite(On_Board_LED, LOW);
  // Serial.println("---------------");

  // delay(5000);
  // }

  // Write an Int number on the database path soilMoisture_Data/Moisture

  // if (Firebase.RTDB.pushInt(&fbdo, "History_soilMoisture_Data/Moisture", sensorVal)) {
  //   Serial.println("SAVED");
  //   Serial.println("PATH: " + fbdo.dataPath());
  //   Serial.println("TYPE: " + fbdo.dataType());
  // }
  // else {
  //   Serial.println("FAILED");
  //   Serial.println("REASON: " + fbdo.errorReason());
  // }


  // if (Firebase.RTDB.pushInt(&fbdo, "History_DHT11_Data/Humidity", Humd_Val)) {
  //   Serial.println("SAVED");
  //   Serial.println("PATH: " + fbdo.dataPath());
  //   Serial.println("TYPE: " + fbdo.dataType());
  // }
  


//________________________________________________________________________________ 


//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println();

  pinMode(On_Board_LED, OUTPUT);
  pinMode(moistureSensorPin1, INPUT);
  pinMode(moistureSensorPin2, INPUT);
  pinMode(moistureSensorPin3, INPUT);
  pinMode(moistureSensorPin4, INPUT);
  // pinMode(moistureSensorPin5, INPUT);
  // pinMode(moistureSensorPin6, INPUT);
  // pinMode(LED_01_PIN, OUTPUT);

  //---------------------------------------- The process of connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("---------------Connection");
  Serial.print("Connecting to : ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");

    digitalWrite(On_Board_LED, HIGH);
    delay(250);
    digitalWrite(On_Board_LED, LOW);
    delay(250);
  }
  digitalWrite(On_Board_LED, LOW);
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(WIFI_SSID);
  //Serial.print("IP : ");
  //Serial.println(WiFi.localIP());
  Serial.println("---------------");
  //---------------------------------------- 

  // Assign the api key (required).
  config.api_key = API_KEY;

  // Assign the RTDB URL (required).
  config.database_url = DATABASE_URL;

  // Sign up.
  Serial.println();
  Serial.println("---------------Sign up");
  Serial.print("Sign up new user... ");
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Serial.println("---------------");
  
  // Assign the callback function for the long running token generation task.
  config.token_status_callback = tokenStatusCallback; //--> see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  dht11.begin();

  delay(1000);
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:
  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    // read_moistureSensorPin();
    // read_DHT11();
    store_and_save_data_to_firebase_database();
    //read_data_from_firebase_database();

    // Serial.print("Moisture Value: ");
    // Serial.println(sensorVal);



  }
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

