#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include "DHT.h"
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 12
DHT dht(dht_dpin, DHTTYPE); 

#define FIREBASE_HOST "https://arduinoapp-7a227-default-rtdb.asia-southeast1.firebasedatabase.app" //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "lSAkdSysFr2UDBGVa3jbAgAcf13Eg4tKFnmhoKij" //Your Firebase Database Secret goes here

#define WIFI_SSID ":)"                                               //WiFi SSID to which you want NodeMCU to connect
#define WIFI_PASSWORD "azamat2018$"                                      //Password of your wifi network 
 

FirebaseData firebaseData;
Servo myservo; 

int analogPin = A0;
int analogValue = 0;

void setup() {
  lcd.begin(16,2);
  lcd.init();// lcd.clear(); // Clears the display 
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to ....");
  lcd.setCursor(5, 1);
  lcd.print("WI-FI");
  Serial.begin(115200);                                   // Select the same baud rate if you want to see the datas on Serial Monitor
  dht.begin();
  myservo.attach(14,600,2300);  // (pin, min, max)
  Serial.println("Serial communication started\n\n");  
           
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
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
  Serial.println(WiFi.localIP());                                            //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   // connect to firebase

  Firebase.reconnectWiFi(true);
  delay(1000);
  lcd.clear(); 
}

void loop() { 
    float h = dht.readHumidity();
    float t = dht.readTemperature(); 
    analogValue = analogRead(analogPin);
    Serial.println(analogValue);
    // if humidity or temperature read failed, don't change the values
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      lcd.setCursor(0, 0);
      lcd.print("error tempratura");
      return;
    }
    //if humidity > 30% myservo write 90
    if (h < 30) {
      myservo.write(90);
    }else{
       myservo.write(0);
    }

    Firebase.setInt(firebaseData, "data/soilmoisture", analogValue);
    Firebase.setFloat(firebaseData, "data/temperature", t);
    Firebase.setFloat(firebaseData, "data/humidity", h);
    delay(1000);

   //..5555555555555555
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
}
