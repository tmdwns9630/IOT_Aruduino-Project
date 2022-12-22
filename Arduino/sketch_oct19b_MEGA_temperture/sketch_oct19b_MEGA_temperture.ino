#include <DHT.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//온도 센서
#define DHT_PIN 4 //번호를 맞췄어야지
#define DHT_TYPE DHT11

//먼지
#define DUST_PIN A0
#define DUST_LED_PIN 6
#define DUST_SAMPLING 280
#define DUST_WAITING 40
#define DUST_STOPTIME 9680

float dustValue = 0;
float dustDensityung =0;

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(F("DHT11 Init"));
}

void loop() {
  readDHT11();
   readDustSensor();
  delay(2000);
}

//온도센서
void readDHT11(){
  float temperature = dht.readTemperature();

  Serial.print(F(",Temperature: "));
  Serial.println(temperature);
}

//먼지 센서
void readDustSensor(){
  //먼저 센서 LED ON
  digitalWrite(DUST_LED_PIN, LOW);
  //SAMPLING
  delayMicroseconds(DUST_SAMPLING);
  //센서로부터 데이터 리드
  dustValue = analogRead(DUST_PIN);

  //waiting for read
  delayMicroseconds(DUST_WAITING);
  digitalWrite(DUST_LED_PIN, HIGH);
  delayMicroseconds(DUST_STOPTIME);
  dustDensityung = (0.17 * (dustValue * (5.0 / 1024))-0.1)*1000;
  Serial.println("Dust Density [ug/m3]"+String(dustDensityung));
  // lcd.setCursor(0, 0);
  // lcd.println("Dust Density [ug/m3]");
  // lcd.setCursor(0, 1);
  // lcd.println(String(dustDensityung));
}
