//토글 LED 버튼, 미세먼지, 습도, AWS와 통신
//+ 웹에서 LED ON OFF 할 수 있게 하기.
#include <DHT.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

#define DHT_PIN 2
#define DHT_TYPE DHT11

#define SS_PIN 9
#define RST_PIN 8

//LED, 버튼
#define redLED_PIN 4
#define greenLED_PIN 5
#define buttonPin 11
#define LED_BUILTIN 7

//미세먼지 센서
#define DUST_PIN A0
#define DUST_LED_PIN 12
#define DUST_SAMPLING 280
#define DUST_WAITING 40
#define DUST_STOPTIME 9680
long dustTimer = 0;

// float dustDensityung = 0;

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

DHT dht(DHT_PIN, DHT_TYPE);
long dhtTimer = 0;
//-60000

char serialBuffer[128];
int serialBufferIndex = 0;

bool ledStatus = false;
long ledTimer = 0;


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
bool dehumidifier = false;
bool doorstepLed = false;
bool livingroomLed = false;
bool window = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Setup Start");
  while (!Serial) {
    ;
  }


  pinMode(buttonPin, INPUT_PULLUP);  //버튼 레디.
  pinMode(redLED_PIN, OUTPUT);       //빨간 LED 레디.
  pinMode(greenLED_PIN, OUTPUT);     //녹색 LED 레디.
  pinMode(DUST_LED_PIN, OUTPUT);     // 먼지센서 레디.
  pinMode(LED_BUILTIN, OUTPUT);      // Start Init LED PIN
  // digitalWrite(LED_BUILTIN, HIGH);

  // Start Init Serial Buffer
  memset(serialBuffer, 0, 128);

  // Start Init DHT
  dht.begin();


  // Start Init RFID Module
  SPI.begin();
  rfid.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] == 0xFF;
  }
}

void loop() {

  checkSerial();
  switchLightByToggle(digitalRead(buttonPin));
  
  checkDHT();
  checkRFID();
  checkLEDStatus();
}

void checkSerial() {
  if (Serial.available()) {
    serialBuffer[serialBufferIndex] = (char)Serial.read();

    if (serialBuffer[serialBufferIndex] == '\n') {
      handleSerialData();
      serialBufferIndex = 0;
    } else {
      serialBufferIndex++;
    }
  }
}

void handleSerialData() {

  DynamicJsonDocument doc(128);
  deserializeJson(doc, serialBuffer);

  if (doc["type"] == "rfid") {
    if (doc["result"] == 1) {
      digitalWrite(LED_BUILTIN, HIGH);
      ledStatus = true;
      ledTimer = millis();
    } else {
    }
  }

  if (doc["type"] == "devices") {
    if (doc["livingroomLed"] != livingroomLed) {
      switchLightByToggle(doc["livingroomLed"]);
    }
  }
}

void checkLEDStatus() {
  if (ledStatus == true) {
    if (millis() > ledTimer + 5000) {
      digitalWrite(LED_BUILTIN, LOW);
      ledStatus = false;
      ledTimer = 0;
    }
  }
}

//습도&미세먼지 센서
void checkDHT() {
  if (millis() > dhtTimer + 6000) {
    dhtTimer = millis();
    float humidity = dht.readHumidity();  //습도센서로부터 값 저장.

    //미세먼지
    digitalWrite(DUST_LED_PIN, LOW);         //먼저 센서 LED ON
    delayMicroseconds(DUST_SAMPLING);        //SAMPLING
    float dustValue = analogRead(DUST_PIN);  //센서로부터 데이터 리드
    delayMicroseconds(DUST_WAITING);
    digitalWrite(DUST_LED_PIN, HIGH);
    delayMicroseconds(DUST_STOPTIME);

    float dustDensityung = (0.17 * (dustValue * (5.0 / 1024)) - 0.1) * 1000;
    Serial.println("Dust[ug/m3]" + String(dustDensityung));

    //데이터 객체화
    DynamicJsonDocument doc(256);
    doc["messageType"] = "readHumidity";
    doc["humidity"] = humidity;
    doc["dust"] = dustDensityung;
    serializeJson(doc, Serial);
    Serial.println();
    delay(1);
  }
}

void checkRFID() {
  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (!rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }
  Serial.println("########card check#########");
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
    Serial.println();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1);
}

void switchLightByToggle(bool switching) {
  reading = switching;  // 버튼 상태 읽음

  //버튼이 눌려졌고 스위치 토글 눌림 경과시간이 Debounce 시간보다 크면 실행
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH)  // LED 가 HIGH 면 LOW 로 바꿔준다.
      state = LOW;
    else  // LED 가 LOW 면 HIGH 로 바꿔준다.
      state = HIGH;


    //LED 상태 객체화 전송.
    DynamicJsonDocument doc(256); 
    doc["messageType"] = "readDeviceStatus";
    doc["livingroomLed"] = state;
    serializeJson(doc, Serial);
    Serial.println();

    time = millis();
  }


  digitalWrite(redLED_PIN, state);
  previous = reading;
}