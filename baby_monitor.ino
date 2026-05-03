#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);


#define DHT22_PIN     7
#define DHTTYPE       DHT22
#define MQ135_PIN     A0
#define SOUND_PIN     A1
#define BUZZER_PIN    8


// --- Air Quality Thresholds ---
#define AQ_GOOD       100
#define AQ_MODERATE   200
#define AQ_POOR       300


// --- Temperature Thresholds (°C) ---
#define TEMP_LOW      18.0
#define TEMP_HIGH     22.0


// --- Humidity Thresholds ---
#define HUM_LOW       40.0
#define HUM_HIGH      55.0


// --- Sound Threshold ---
#define SOUND_THRESH  600


DHT dht(DHT22_PIN, DHTTYPE);


void setup() {
 lcd.init();
 lcd.backlight();
 dht.begin();
 pinMode(BUZZER_PIN, OUTPUT);
 digitalWrite(BUZZER_PIN, LOW);
}


String airQualityLabel(int val) {
 if (val < AQ_GOOD)     return "Good       ";
 if (val < AQ_MODERATE) return "Moderate   ";
 if (val < AQ_POOR)     return "Poor       ";
 return                        "Hazardous  ";
}


void alertBuzzer(int beeps) {
 for (int i = 0; i < beeps; i++) {
   digitalWrite(BUZZER_PIN, HIGH);
   delay(200);
   digitalWrite(BUZZER_PIN, LOW);
   delay(200);
 }
}


void loop() {
 float temperature = dht.readTemperature();
 float humidity    = dht.readHumidity();
 int   airQuality  = analogRead(MQ135_PIN);
 int   soundLevel  = analogRead(SOUND_PIN);


 bool tempAlert  = false;
 bool humAlert   = false;
 bool aqAlert    = false;
 bool soundAlert = false;


 // --- DHT Error Check ---
 if (isnan(temperature) || isnan(humidity)) {
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Sensor Error!   ");
   lcd.setCursor(0, 1);
   lcd.print("Check DHT22     ");
   delay(2000);
   return;
 }


 // --- Check Thresholds ---
 if (temperature < TEMP_LOW || temperature > TEMP_HIGH) tempAlert  = true;
 if (humidity    < HUM_LOW  || humidity    > HUM_HIGH)  humAlert   = true;
 if (airQuality  > AQ_POOR)                             aqAlert    = true;
 if (soundLevel  > SOUND_THRESH)                        soundAlert = true;


 // --- Screen 1: Temperature ---
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Temp: ");
 lcd.print(temperature, 1);
 lcd.print((char)223);
 lcd.print("C");
 lcd.setCursor(0, 1);
 if (tempAlert) {
   lcd.print("OUT OF RANGE! ");
   alertBuzzer(2);
 } else {
   lcd.print("NORMAL!");
 }
 delay(2000);


 // --- Screen 2: Humidity ---
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Humidity: ");
 lcd.print(humidity, 1);
 lcd.print("%");
 lcd.setCursor(0, 1);
 if (humAlert) {
   lcd.print("OUT OF RANGE !");
   alertBuzzer(2);
 } else {
   lcd.print("NORMAL!");
 }
 delay(2000);


 // --- Screen 3: Air Quality ---
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Air: ");
 lcd.print(airQuality);
 lcd.print(" ppm     ");
 lcd.setCursor(0, 1);
 if (aqAlert) {
   lcd.print("HAZARDOUS!");
   alertBuzzer(3);
 } else {
   lcd.print(airQualityLabel(airQuality));
   lcd.print("        ");
 }
 delay(2000);


 // --- Screen 4: Sound ---
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Sound: ");
 lcd.print(soundLevel);
 lcd.setCursor(0, 1);
 if (soundAlert) {
   lcd.print("LOUD/CRY!");
   alertBuzzer(1);
 } else {
   lcd.print("NORMAL!");
 }
 delay(2000);
}
