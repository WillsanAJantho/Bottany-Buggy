#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "X"
#define WIFI_PASSWORD "Password"
#define On_Board_LED 2
#define API_KEY "AIzaSyAOGZa7-TjZw2SxkPHC2vvBhEg0RkChWxU"
#define DATABASE_URL "https://botanicalbuggy-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
const long intervalMillis = 7000;

bool signupOK = false;

float read_Temperature;
int read_Humidity;
int read_soilMoisture1;
int read_soilMoisture2;
int read_soilMoisture3;

#define SERVO_PIN 22
#define SERVOHUM 27
#define outPin 17
#define s0 18
#define s1 19
#define s2 23
#define s3 13

boolean DEBUG = true;
int red, grn, blu;
String color = "";
long startTiming = 0;
long elapsedTime = 0;

#define Motor 14
#define Pump 21

const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int pwmFrequency = 50; // 50 Hz for servo
const int pwmResolution = 16; // 16-bit resolution

unsigned long lastActionTime = 0;
bool firstMoveDone = false;

void connectToWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("---------------Connection");
    Serial.print("Connecting to: ");
    Serial.println(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        digitalWrite(On_Board_LED, HIGH);
        delay(250);
        digitalWrite(On_Board_LED, LOW);
        delay(250);
    }
    digitalWrite(On_Board_LED, LOW);
    Serial.println();
    Serial.print("Successfully connected to: ");
    Serial.println(WIFI_SSID);
    Serial.println("---------------");

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    Serial.println();
    Serial.println("---------------Sign up");
    Serial.print("Sign up new user... ");
    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("ok");
        signupOK = true;
    } else {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    pinMode(On_Board_LED, OUTPUT);

    connectToWiFi();

    Serial.println("---------------");

    config.token_status_callback = tokenStatusCallback;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // Setup PWM for servos
    ledcSetup(pwmChannel1, pwmFrequency, pwmResolution);
    ledcSetup(pwmChannel2, pwmFrequency, pwmResolution);
    ledcAttachPin(SERVO_PIN, pwmChannel1);
    ledcAttachPin(SERVOHUM, pwmChannel2);

    pinMode(s0, OUTPUT);
    pinMode(Motor, OUTPUT);
    pinMode(Pump, OUTPUT);
    pinMode(s1, OUTPUT);
    pinMode(s2, OUTPUT);
    pinMode(s3, OUTPUT);
    pinMode(outPin, INPUT);

    digitalWrite(s0, HIGH);
    digitalWrite(s1, HIGH);
}

void loop() {
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > intervalMillis || sendDataPrevMillis == 0)) {
        sendDataPrevMillis = millis();

        Serial.println();
        Serial.println("---------------Get Data");
        digitalWrite(On_Board_LED, HIGH);

        unsigned long startTime = millis();

        if (Firebase.RTDB.getJSON(&fbdo, "/robot")) {
            FirebaseJson &json = fbdo.jsonObject();
            String jsonStr;
            json.toString(jsonStr, true);
            Serial.println(jsonStr);

            FirebaseJsonData jsonData;

            if (json.get(jsonData, "hum") && jsonData.typeNum == FirebaseJson::JSON_INT) {
                read_Humidity = jsonData.intValue;
                Serial.print("HUMIDITY: ");
                Serial.println(read_Humidity);
            }

            if (json.get(jsonData, "sm1") && jsonData.typeNum == FirebaseJson::JSON_INT) {
                read_soilMoisture1 = jsonData.intValue;
                Serial.print("SM1: ");
                Serial.println(read_soilMoisture1);
            }

            if (json.get(jsonData, "sm2") && jsonData.typeNum == FirebaseJson::JSON_INT) {
                read_soilMoisture2 = jsonData.intValue;
                Serial.print("SM2: ");
                Serial.println(read_soilMoisture2);
            }

            if (json.get(jsonData, "sm3") && jsonData.typeNum == FirebaseJson::JSON_INT) {
                read_soilMoisture3 = jsonData.intValue;
                Serial.print("SM3: ");
                Serial.println(read_soilMoisture3);
            }
        } else {
            Serial.println(fbdo.errorReason());
        }

        unsigned long endTime = millis();
        Serial.print("Time taken for Firebase operations: ");
        Serial.print(endTime - startTime);
        Serial.println(" ms");

        Serial.println();
        digitalWrite(On_Board_LED, LOW);
        Serial.println("---------------");
    }

    if (read_Humidity < 90) {
    if (!firstMoveDone || (millis() - lastActionTime >= 10000)) {
        // Perform the servo rotation sequence
        rotateServoHUM(90);  // First rotation
        delay(500);
        rotateServoHUM(0);
        delay(500);  // Small delay after returning to initial position
        

        // Delay for 10 seconds after the first move
        delay(10000);

        rotateServoHUM(90);  // Second rotation
        delay(500);
        rotateServoHUM(0);
        delay(500);  // Small delay after returning to initial position
        

        rotateServoHUM(90);  // Third rotation
        delay(500);
        rotateServoHUM(0);
        delay(500);  // Small delay after returning to initial position
        

        // Update the last action time to track the completion of this sequence
        lastActionTime = millis();
        firstMoveDone = true;
    }
} else {
    firstMoveDone = false;  // Reset the flag if humidity condition is not met
}

    getColor();
    printData();

    if (read_soilMoisture1 > 4000 && color == "BLUE") {
        digitalWrite(Motor, HIGH);
        Serial.println("Motor Started for SM1");
        delay(5000);
        rotateServo(180);
        delay(1000);
        digitalWrite(Pump, HIGH);
        Serial.println("Pump Started for SM1");
        delay(5000);
        digitalWrite(Pump, LOW);
        Serial.println("Pump Stopped for SM1");
        delay(2000);    
        rotateServo(90);
        delay(1000);
        digitalWrite(Motor, LOW);
        Serial.println("Motor Stopped for SM1");
        delay(3000);
    } else if (read_soilMoisture2 > 4000 && color == "BROWN") {
        digitalWrite(Motor, HIGH);
        Serial.println("Motor Started for SM2");
        delay(5000);
        rotateServo(180);
        delay(1000);
        digitalWrite(Pump, HIGH);
        Serial.println("Pump Started for SM2");
        delay(5000);
        digitalWrite(Pump, LOW);
        Serial.println("Pump Stopped for SM2");
        delay(2000);    
        rotateServo(90);
        delay(1000);
        digitalWrite(Motor, LOW);
        Serial.println("Motor Stopped for SM2");
        delay(1700);
    } else if (read_soilMoisture3 > 4000 && color == "GREEN") {
        digitalWrite(Motor, HIGH);
        Serial.println("Motor Started for SM3");
        delay(5000);
        rotateServo(180);
        delay(1000);
        digitalWrite(Pump, HIGH);
        Serial.println("Pump Started for SM3");
        delay(5000);
        digitalWrite(Pump, LOW);
        Serial.println("Pump Stopped for SM3");
        delay(2000);    
        rotateServo(90);
        delay(1000);
        digitalWrite(Motor, LOW);
        Serial.println("Motor Stopped for SM3");
        delay(3000);
    } else if (color == "RED" && read_soilMoisture1 <= 4000 && read_soilMoisture2 <= 4000 && read_soilMoisture3 <= 4000) {
        digitalWrite(Motor, HIGH);
        Serial.println("Motor Started for RED");
        digitalWrite(Pump, LOW);
        rotateServo(90);
        Serial.println("Servo Rotated for RED");
    } else {
        digitalWrite(Motor, LOW);
        Serial.println("Motor Stopped");
    }

    getColor();
    printData();
}

void rotateServo(int angle) {
    int dutyCycle = map(angle, 0, 180, 1638, 8192); // Map angle to duty cycle (0.5ms to 2.5ms)
    ledcWrite(pwmChannel1, dutyCycle);
    Serial.println("Servo Rotated");
}

void rotateServoHUM(int angle) {
    int dutyCycle = map(angle, 0, 180, 1638, 8192); // Map angle to duty cycle (0.5ms to 2.5ms)
    ledcWrite(pwmChannel2, dutyCycle);
    Serial.println("Humidity Servo Rotated");
}

void readRGB() {
    red = 0, grn = 0, blu = 0;
    int n = 10;
    for (int i = 0; i < n; ++i) {
        digitalWrite(s2, LOW);
        digitalWrite(s3, LOW);
        red = red + pulseIn(outPin, LOW);

        digitalWrite(s2, HIGH);
        digitalWrite(s3, HIGH);
        grn = grn + pulseIn(outPin, LOW);

        digitalWrite(s2, LOW);
        digitalWrite(s3, HIGH);
        blu = blu + pulseIn(outPin, LOW);
    }
    red = red / n;
    grn = grn / n;
    blu = blu / n;
}

void printData() {
    Serial.print("red= ");
    Serial.print(red);
    Serial.print("   green= ");
    Serial.print(grn);
    Serial.print("   blue= ");
    Serial.print(blu);
    Serial.print(" - ");
    Serial.print(color);
    Serial.println(" detected!");
}

void getColor() {
    readRGB();
    if (red > 17 && red < 28 && grn > 65 && grn < 80 && blu > 55 && blu < 63) color = "RED";
    else if (red > 25 && red < 47 && grn > 25 && grn < 44 && blu > 48 && blu < 62) color = "GREEN";
    else if (red > 90 && red < 115 && grn > 45 && grn < 65 && blu > 15 && blu < 35) color = "BLUE";
    else if (red > 70 && red < 90 && grn > 105 && grn < 130 && blu > 95 && blu < 125) color = "BROWN";
    else color = "NONE";
}