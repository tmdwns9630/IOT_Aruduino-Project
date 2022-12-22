//22-10-18 Tues, Mini Project : 출입관리 시스템 만들기.
//서버 : DC80A559
//→ 아두이노 파워 문제로 그냥 RFID 빼고 서버에 전송했을 때 성공 나오면 초록색, 실패 나오면 빨간색 점멸시킨다.
#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <ArduinoHttpClient.h>

#define SS_PIN 9
#define RST_PIN 8

MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class
MFRC522::MIFARE_Key key;
byte nuidPICC[4]; // 이전 ID와 비교하기위한 변수

byte mac[] = {
  0x74, 0x69, 0x69, 0x2D, 0x30, 0x0C
};

byte ip[] ={
 192,168,0,112
};

const char servername[]="61.103.243.247";
const int port = 8000;
EthernetClient ethernet;

HttpClient client = HttpClient(ethernet, servername, port);
const char path = "/";


void setup() {
  Serial.begin(9600);  
  pinMode(2, OUTPUT);//LED 빨강
  pinMode(3, OUTPUT);//LED 초록

  // digitalWrite(2, HIGH);
  // digitalWrite(3, HIGH);

  SPI.begin();      // SPI 시작
  rfid.PCD_Init();  // RFID 시작
  //초기 키 ID 초기화
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println(F("Fusion of today's lesson"));
  Serial.print(F("Using the following key:"));
 // printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  Serial.println(("Start Ether connection"));
  Ethernet.begin(mac,ip);
}


void loop() {
   client.get("/checkID?id=DC80A550");
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.println(statusCode);
  Serial.println(response);

  //response가 {"success":true} 문자열일 때 퍼펙트 출력.
  if(response == "{\"success\":\"true\"}"){
    Serial.println("ID is OK!");
    greenLEDtwingkle();
  }
  else{
     Serial.println("ID is Wrong");
     redLEDtwingkle();
  }

  delay(10000);
 
}


void redLEDtwingkle(){
  digitalWrite(2, HIGH);
  delay(5000);
  digitalWrite(2, LOW);
}

void greenLEDtwingkle(){
  digitalWrite(3, HIGH);
  delay(5000);
  digitalWrite(3, LOW);
}

//미완
/*
void cardcheck({
   // 카드가 인식되었다면 다음으로 넘어가고 아니면 더이상
  // 실행 안하고 리턴
  if (!rfid.PICC_IsNewCardPresent())
    return;
  // ID가 읽혀졌다면 다음으로 넘어가고 아니면 더이상
  // 실행 안하고 리턴
  if (!rfid.PICC_ReadCardSerial())
    return;
  Serial.print(F("PICC type: "));
  // 카드의 타입을 읽어온다.
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // 모니터에 출력
  Serial.println(rfid.PICC_GetTypeName(piccType));
  // MIFARE 방식인지 확인하고 아니면 리턴
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    redLEDtwingkle();
    return;
  }

  // 만약 바로 전에 인식한 RF 카드와 다르다면..
  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3]) {
    Serial.println(F("A new card has been detected."));
    // ID를 저장해둔다.
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    // 모니터 출력
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    // 16진수로 변환해서 출력
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    // 10진수로 출력
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  } else Serial.println(F("Card read previously."));  //바로 전에 인식한 것과 동일하다

  // PICC 종료
  rfid.PICC_HaltA();
  // 암호화 종료(?)
  rfid.PCD_StopCrypto1();
  // 다시 처음으로 돌아감.
})
*/

void printIPAddress() {
  Serial.print(F("My IP Address: "));
  for(byte thisByte =0; thisByte < 4; thisByte++){
    Serial.print(Ethernet.localIP()[thisByte],DEC);
    if(thisByte < 3)Serial.print(F("."));
   
  }
   Serial.println();
}

//16진수
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

// 10진수로 변환하는 함수
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}