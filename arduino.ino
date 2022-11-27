#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <IRremote.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTTYPE DHT11  // DHT 11
#define dht_dpin 12
DHT dht(dht_dpin, DHTTYPE);

int PULT = 13;
IRrecv irrecv(PULT);

decode_results results;

#define FIREBASE_HOST "https://arduinoapp-7a227-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "lSAkdSysFr2UDBGVa3jbAgAcf13Eg4tKFnmhoKij"

#define WIFI_SSID ":)"
#define WIFI_PASSWORD "azamat2018$"

FirebaseData firebaseData;

int analogPin = A0;
int analogValue = 0;
int w = 0;

void setup() {

  pinMode(16, OUTPUT);
  pinMode(14, OUTPUT);
  digitalWrite(16, HIGH);
  digitalWrite(14, HIGH);

  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to ....");
  lcd.setCursor(5, 1);
  lcd.print("WI-FI");
  irrecv.enableIRIn();
  dht.begin();
  Serial.println("Serial communication started\n\n");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.reconnectWiFi(true);
  delay(1000);
  lcd.clear();
}

void loop() {
  if (irrecv.decode(&results)) {
    if (results.value == 0xFFA25D) {
      Serial.println("Button 1");
      if (Firebase.getInt(firebaseData, "data/on")) {
        if (firebaseData.intData() == 1) {
          Firebase.setInt(firebaseData, "data/on", 0);
        } else {
          Firebase.setInt(firebaseData, "data/on", 1);
        }
      }
    }
    if (results.value == 0xFF22DD) {
      //Serial.println("Button 4");
      Firebase.setString(firebaseData, "phone/rele1", "auto");
    }
    if (results.value == 0xFFC23D) {
      //Serial.println("Button 6");
      Firebase.setString(firebaseData, "phone/rele2", "auto");
    }

    if (results.value == 0xFFE01F) {
      //Serial.println("Button 7");
      digitalWrite(14, LOW);
      Firebase.setString(firebaseData, "phone/rele1", "on");
    }
    if (results.value == 0xFF906F) {
      //Serial.println("Button 9");
      digitalWrite(16, LOW);
      Firebase.setString(firebaseData, "phone/rele2", "on");
    }

    if (results.value == 0xFF6897) {
      //Serial.println("Button *");
      digitalWrite(14, HIGH);
      Firebase.setString(firebaseData, "phone/rele1", "off");
    }
    if (results.value == 0xFFB04F) {
      //Serial.println("Button #");
      digitalWrite(16, HIGH);
      Firebase.setString(firebaseData, "phone/rele2", "off");
    }
    //Serial.println(results.value, HEX);
    irrecv.resume();
  }

  if (Firebase.getInt(firebaseData, "data/on")) {
    if (firebaseData.intData() == 1) {
      Serial.println("LED is ON");
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      analogValue = analogRead(analogPin);
      Serial.println(analogValue);
      if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        lcd.setCursor(0, 0);
        lcd.print("Chesk sensors...");
        return;
      }
      Firebase.setInt(firebaseData, "data/soilmoisture", analogValue);
      Firebase.setFloat(firebaseData, "data/temperature", t);
      Firebase.setFloat(firebaseData, "data/humidity", h);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("HUM:");
      lcd.setCursor(5, 0);
      lcd.print(h);
      lcd.setCursor(8, 0);
      lcd.print("TEMP:");
      lcd.setCursor(14, 0);
      lcd.print(t);
      lcd.setCursor(0, 1);
      lcd.print("plant:");

      lcd.setCursor(6, 1);
      lcd.print(analogValue);

      if (Firebase.getInt(firebaseData, "phone/humidity")) {
        w = firebaseData.intData();
        if (Firebase.getString(firebaseData, "phone/rele2")) {
          if (firebaseData.stringData() == "auto") {
            if (h < w) {
              digitalWrite(16, LOW);
            } else {
              digitalWrite(16, HIGH);
            }
          }

          if (firebaseData.stringData() == "on") {
            digitalWrite(16, LOW);
          }
          if (firebaseData.stringData() == "off") {
            digitalWrite(16, HIGH);
          }
        }
        if (Firebase.getInt(firebaseData, "phone/soilmoisture")) {
          w = firebaseData.intData();
          if (Firebase.getString(firebaseData, "phone/rele1")) {
            if (firebaseData.stringData() == "auto") {
              if (analogValue < w) {
                digitalWrite(14, LOW);
              } else {
                digitalWrite(14, HIGH);
              }
            }
            if (firebaseData.stringData() == "on") {
              digitalWrite(14, LOW);
            }
            if (firebaseData.stringData() == "off") {
              digitalWrite(14, HIGH);
            }
          }
        }
        if (Firebase.getInt(firebaseData, "phone/temperature")) {
          w = firebaseData.intData();
          if (Firebase.getString(firebaseData, "phone/rele1")) {
            if (firebaseData.stringData() == "auto") {
              if (t > w) {
                digitalWrite(14, LOW);
              } else {
                digitalWrite(14, HIGH);
              }
            }
            if (firebaseData.stringData() == "on") {
              digitalWrite(14, LOW);
            }
            if (firebaseData.stringData() == "off") {
              digitalWrite(14, HIGH);
            }
            
          }
        }
      }

    } else {
      lcd.clear();
      Serial.println("Program is OFF");
      lcd.setCursor(2, 0);
      lcd.print("DEVICE IS OFF");
    }
  } else {
    lcd.clear();
    Serial.println("Not Connection wifi");
    lcd.setCursor(2, 0);
    lcd.print("DEVICE IS OFF");
  }
}