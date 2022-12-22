//mega에 rfid + 온도, 이더넷 없이
//강사님 파일 복사본
#include <DHT.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

#define DHT_PIN 2
#define DHT_TYPE DHT11

#define SS_PIN 9
#define RST_PIN 8

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

DHT dht(DHT_PIN, DHT_TYPE);
unsigned long dhtTimer = 0;

char serialBuffer[128];




void setup() {
  Serial.begin(9600);
  Serial.println();
  while (!Serial) {
    ;
  }

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
  Serial.println("DHT!");
  checkDHT();
  delay(1000);
  Serial.println("");


  Serial.println("RFID!");
  checkRFID();
  delay(1000);
}


void checkDHT() {
  if (millis() > dhtTimer + 2000) {
    dhtTimer = millis();
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    DynamicJsonDocument doc(256);
    doc["messageType"] = "readHumidity";
    doc["temp"] = temperature;
    doc["humidity"] = humidity;

    serializeJson(doc, Serial);
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

//온도센서
float readDHT11() {
  float temperature = dht.readTemperature();

  Serial.print(F("Temperature: "));
  Serial.println(temperature);
  return temperature;
}