//iot 최종 프로젝트
//각종 센서 테스트
#include <DHT.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

//LED, 버튼
#define redLED_PIN 4
#define greenLED_PIN 5
#define buttonPin 13

//미세먼지 센서
#define DUST_PIN A0
#define DUST_LED_PIN 12
#define DUST_SAMPLING 280
#define DUST_WAITING 40
#define DUST_STOPTIME 9680

float dustValue = 0;
float dustDensityung = 0;

//온습도 센서
#define DHT_PIN 2
#define DHT_TYPE DHT11

//RFID 센서
#define SS_PIN 9
#define RST_PIN 8
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

DHT dht(DHT_PIN, DHT_TYPE);
unsigned long dhtTimer = 0;
//delay 없이 지연 반복
unsigned long pre_timer = 0;
int repeat_time = 2000;

char serialBuffer[128];

//버튼 설정--------------------
int buttonState = 0;
int sw = 10;  // 스위치(SW) 핀 설정
int led = 6;  // LED 핀 설정

int state = LOW;     // LED 상태
int reading;         // SW 상태
int previous = LOW;  // SW 이전 상태

long time = 0;        // LED가 ON/OFF 토글된 마지막 시간
long debounce = 100;  // Debounce 타임 설정
//-------------------------------


void setup() {
  Serial.begin(9600);
  Serial.println("start");
  while (!Serial) {
    ;
  }
  pinMode(buttonPin, INPUT_PULLUP);  //버튼 레디.
  pinMode(redLED_PIN, OUTPUT);       //빨간 LED 레디.
  pinMode(greenLED_PIN, OUTPUT);     //녹색 LED 레디.
  pinMode(DUST_LED_PIN, OUTPUT);     // 먼지센서 레디.
  dht.begin();                       //DHT 시작

  //RFID Set
  memset(serialBuffer, 0, 128);  //Start Init Serial Buffer
  // Start Init RFID Module
  SPI.begin();
  rfid.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] == 0xFF;
  }
}


void loop() {
  switchLightByToggle();

  //5초 딜레이로 실행되는 함수는 여기 넣는다.
  repeat_time = 5000;
  if (millis() > pre_timer + repeat_time) {

    pre_timer = millis();

    //5초마다 실행되는 명령
    checkDHT();
    readDustSensor();
  }
}

//미세먼지 측정


//온습도 센서
void checkDHT() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    Serial.print(F("Temperature: "));
    Serial.println(temperature);
    Serial.print(F("humidity: "));
    Serial.println(humidity);

    DynamicJsonDocument doc(256);
    doc["messageType"] = "readHumidity";
    doc["temp"] = temperature;
    doc["humidity"] = humidity;
    serializeJson(doc, Serial);
    delay(1);
  
}

//RFID 리더기
void checkRFID() {
  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (!rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }
  Serial.print("sak value : ");
  Serial.println(rfid.uid.sak);

  // 인식된 RFID 카드 Byte rfid.uid.uidByte
  // 인식된 RFID 카드 Byte 길이 rfid.uid.size
  char hexPtr[8];
  memset(hexPtr, 0, 8);
  if (rfid.uid.size == 4) {
    sprintf(hexPtr, "%02X%02X%02X%02X", rfid.uid.uidByte[0], rfid.uid.uidByte[1], rfid.uid.uidByte[2], rfid.uid.uidByte[3]);
    Serial.print("hexPtr : ");
    Serial.println(hexPtr);

    DynamicJsonDocument doc(256);
    doc["messageType"] = "readRfid";
    doc["rfidTag"] = hexPtr;
    serializeJson(doc, Serial);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}


//버튼 누르면 빨간불, 떼면 파란불
void switchLightON() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(greenLED_PIN, HIGH);
    digitalWrite(redLED_PIN, LOW);
  } else {
    // turn LED off:
    digitalWrite(greenLED_PIN, LOW);
    digitalWrite(redLED_PIN, HIGH);
  }
}

//토글버튼
void switchLightByToggle() {
  reading = digitalRead(buttonPin);  // SW 상태 읽음

  //SW 가 눌려졌고 스위치 토글 눌림 경과시간이 Debounce 시간보다 크면 실행
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH)  // LED 가 HIGH 면 LOW 로 바꿔준다.
      state = LOW;
    else  // LED 가 LOW 면 HIGH 로 바꿔준다.
      state = HIGH;

    time = millis();
  }

  digitalWrite(redLED_PIN, state);

  previous = reading;
}
//먼지센서
 void readDustSensor() {
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
  dustDensityung = (0.17 * (dustValue * (5.0 / 1024)) - 0.1) * 1000;
  Serial.println("Dust Density [ug/m3]" + String(dustDensityung));
  delay(1);
}