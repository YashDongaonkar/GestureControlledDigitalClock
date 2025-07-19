#include <Wire.h>              // For I2C communication 
#include <U8g2lib.h>           // For OLED display control 
#include <RTClib.h>            // For DS3231 RTC module 
#include <DHT.h>               // For DHT22 temperature & humidity sensor 
#include <SparkFun_APDS9960.h> // For APDS9960 gesture & proximity sensor 

#define TRIG_PIN 9 
#define ECHO_PIN 10 
#define BUZZER_PIN 8 
#define DHT_PIN 2 

#define DISTANCE_THRESHOLD_MIN 5 
#define DISTANCE_THRESHOLD_MAX 30 

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE); 
RTC_DS3231 rtc;                               
DHT dht(DHT_PIN, DHT22);                      
SparkFun_APDS9960 apds = SparkFun_APDS9960(); 

int currentView = 1; 
int counter = 0; 
uint8_t proximity_data = 0; 

bool thirdViewWelcomeScreen = false; 
static int alarmHour = 0; 
static int alarmMinute = 0; 
int alarmRepeatCount = 10; 

bool setAlarm = false; 
bool setMin = false; 
bool setH = false; 

void config() {
    Wire.begin();                  
    pinMode(TRIG_PIN, OUTPUT);     
    pinMode(ECHO_PIN, INPUT);      
    pinMode(BUZZER_PIN, OUTPUT);   
    digitalWrite(BUZZER_PIN, LOW); 

    u8g2.begin(); 
    dht.begin();  

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC"); 
        while (1);
    } 
    if (rtc.lostPower()) {
        rtc.adjust(DateTime(__DATE__, __TIME__)); 
    } 

    if (!apds.init()) {
        Serial.println(F("APDS-9960 init failed!")); 
    } 
    if (!apds.setProximityGain(PGAIN_2X)) {
        Serial.println(F("Failed to set PGAIN")); 
    } 
    if (!apds.enableProximitySensor(false)) {
        Serial.println(F("Proximity sensor enable failed!")); 
    } 
} 

void setup() {
    Serial.begin(9600); 
    config();           
} 

long getDistance() {
    digitalWrite(TRIG_PIN, LOW); 
    delayMicroseconds(2); 
    digitalWrite(TRIG_PIN, HIGH); 
    delayMicroseconds(10); 
    digitalWrite(TRIG_PIN, LOW); 
    long duration = pulseIn(ECHO_PIN, HIGH); 
    return duration * 0.034 / 2; 
} 

void loop() {
    DateTime now = rtc.now();      
    long distance = getDistance(); 

    if (distance < DISTANCE_THRESHOLD_MAX && distance > DISTANCE_THRESHOLD_MIN) {
        delay(1500); 
        if (currentView == 2) {
            thirdViewWelcomeScreen = true; 
        } 
        currentView = (currentView % 3) + 1; 
    } 

    if (now.hour() == alarmHour && now.minute() == alarmMinute && now.second() < 5) {
        u8g2.clearBuffer(); 
        u8g2.setFont(u8g2_font_6x12_tf); 
        u8g2.setCursor(10, 15); 
        u8g2.print("Ringing..."); 
        u8g2.sendBuffer(); 

        for (int i = 0; i < alarmRepeatCount; i++) {
            digitalWrite(BUZZER_PIN, HIGH); delay(100); 
            digitalWrite(BUZZER_PIN, LOW); delay(100); 
        } 
    } else {
        if (currentView == 1) {
            u8g2.clearBuffer(); 
            u8g2.setFont(u8g2_font_6x12_tf); 
            u8g2.setCursor(10, 15); 
            u8g2.print("Time: "); u8g2.print(now.hour(), DEC); u8g2.print(":"); 
            if (now.minute() < 10) u8g2.print("0"); u8g2.print(now.minute(), DEC); 
            u8g2.print(":"); if (now.second() < 10) u8g2.print("0"); 
            u8g2.print(now.second(), DEC); 
            u8g2.setCursor(10, 30); 
            u8g2.print("Date: "); u8g2.print(now.day(), DEC); u8g2.print("/"); 
            u8g2.print(now.month(), DEC); u8g2.print("/"); u8g2.print(now.year(), DEC); 
            u8g2.sendBuffer(); 
        } else if (currentView == 2) {
            float temperature = dht.readTemperature(); 
            float humidity = dht.readHumidity(); 

            u8g2.clearBuffer(); 
            u8g2.setFont(u8g2_font_6x12_tf); 
            u8g2.setCursor(10, 15); 
            if (isnan(temperature) || isnan(humidity)) {
                u8g2.print("Failed to read DHT!"); 
            } else {
                u8g2.print("Temp: "); u8g2.print(temperature); u8g2.print("C"); 
                u8g2.setCursor(10, 30); u8g2.print("Humidity: "); u8g2.print(humidity); u8g2.print("%"); 
            } 
            u8g2.sendBuffer(); 
        } else if (currentView == 3) {
            apds.readProximity(proximity_data); 

            if (thirdViewWelcomeScreen) {
                u8g2.clearBuffer(); u8g2.setFont(u8g2_font_6x12_tf); u8g2.setCursor(20, 20); 
                u8g2.print("Set Alarm"); u8g2.sendBuffer(); 
                thirdViewWelcomeScreen = false; setAlarm = true; setMin = true; 
                digitalWrite(BUZZER_PIN, HIGH); delay(100); 
                digitalWrite(BUZZER_PIN, LOW); delay(100); delay(3000); 
            } else if (setAlarm) {
                if (proximity_data > 10) { counter++; if (counter == 60) counter = 0; delay(100); }
                if (proximity_data < 10) {
                    delay(500); if (proximity_data < 10) { delay(500); if (proximity_data < 10) {
                        if (setMin) {
                            setMin = false; alarmMinute = counter; counter = 0; setH = true;
                            digitalWrite(BUZZER_PIN, HIGH); delay(100); digitalWrite(BUZZER_PIN, LOW); delay(600);
                        } else if (setH) {
                            setH = false; alarmHour = counter; counter = 0; setAlarm = false;
                            digitalWrite(BUZZER_PIN, HIGH); delay(100); digitalWrite(BUZZER_PIN, LOW); delay(600);
                            u8g2.clearBuffer(); u8g2.setFont(u8g2_font_6x12_tf); u8g2.setCursor(20, 10); 
                            u8g2.print("Alarm set to : "); u8g2.setCursor(20, 20); 
                            if (alarmHour < 10) u8g2.print("0"); u8g2.print(alarmHour); 
                            u8g2.print(":"); if (alarmMinute < 10) u8g2.print("0"); 
                            u8g2.print(alarmMinute); u8g2.sendBuffer(); delay(1000); 
                            currentView = 1;
                        }
                    }} 
                }
                u8g2.clearBuffer(); u8g2.setFont(u8g2_font_6x12_tf); u8g2.setCursor(20, 20); 
                if (setMin) { u8g2.print("Set Minutes : "); if (counter < 10) u8g2.print("0"); u8g2.print(counter); } 
                else if (setH) { u8g2.print("Set Hour : "); if (counter < 10) u8g2.print("0"); u8g2.print(counter); } 
                u8g2.sendBuffer(); 
            }
        }
    }
    delay(100); 
}