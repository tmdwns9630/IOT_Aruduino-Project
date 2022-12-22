#include <DHT.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <ArduinoHttpClient.h>

// #include <ArduinoJson.h>

//온도 센서
#define DHT_PIN 4
#define DHT_TYPE DHT11

//RFID
#define SS_PIN 53
#define RST_PIN 5

DHT dht(DHT_PIN, DHT_TYPE);

MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class
MFRC522::MIFARE_Key key;
byte nuidPICC[4];  // 이전 ID와 비교하기위한 변수
byte checkPICC[4];
byte cardPC[4] = { 108, 18, 133, 89 };

//Mac : 60-A5-E2-B2-76-45
//ip4 : 61.103.243.205

// byte mac[] = {
//   0x74, 0x69, 0x69, 0x2D, 0x30, 0x0C
// };

// byte ip[] = {
//   192, 168, 0, 112
// };

//const char servername[] = "61.103.243.247";

//와이파이 ip, mac
byte mac[] = {
  0x60, 0xA5, 0xE2, 0xB2, 0x76, 0x45
};

const char ip[] = "61.103.243.205";
const int port = 80;
EthernetClient ethernet;
HttpClient client = HttpClient(ethernet, ip, port);


const char path = "/";
const String cardCode = "DC80A559";


void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);  //LED 빨강
  pinMode(3, OUTPUT);  //LED 초록

  // digitalWrite(2, HIGH);
  // digitalWrite(3, HIGH);

  SPI.begin();      // SPI 시작
  rfid.PCD_Init();  // RFID 시작
  //초기 키 ID 초기화
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println("Fusion of today's lesson");
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  Serial.println();
  Serial.println(("Start Ether connection"));
  Ethernet.begin(mac, ip);
}


void loop() {


  //온도
  float temperature = readDHT11();
  delay(2000);

  // //RFID--------------------------------------------------
  // String code = "";

  // // 카드가 인식되었다면 다음으로 넘어가고 아니면 더이상
  // // 실행 안하고 리턴
  // if (!rfid.PICC_IsNewCardPresent())
  //   return;
  // // ID가 읽혀졌다면 다음으로 넘어가고 아니면 더이상
  // // 실행 안하고 리턴
  // if (!rfid.PICC_ReadCardSerial())
  //   return;
  // code = RfidCheck();
  // //하얀 카드가 맞으면 String Code에 서버 value 리턴.

  // Serial.println(code);
  // // PICC 종료
  // rfid.PICC_HaltA();
  // // 암호화 종료(?)
  // rfid.PCD_StopCrypto1();
  // // 다시 처음으로 돌아감
  // //RFID end--------------------------------------------------
  temperature =99.9;
  float humidity = 55.5;
  while(1){
    Serial.println("making POST request");
    String contentType = "application/x-www-form-urlencoded";
    //String contentType = "api/sensor_log";
    //String postData = "name=Alice&age=12";
    String postData = "temp=" +String(temperature)+"&humidity="+String(humidity);
   
    Serial.println(postData);
    client.post("/api/sensor_log", contentType, postData);

    int statusCode = client.responseStatusCode();
    String response = client.responseBody();


    Serial.println(statusCode);
    Serial.println(response);

    // //response가 {"success":true} 문자열일 때 OK 출력.
    // if (response == "{\"success\":\"true\"}") {
    //   Serial.println("ID is OK!");
    //   greenLEDtwingkle();
    // } else {
    //   Serial.println("ID is Wrong");
    //   redLEDtwingkle();
    // }

    delay(1000);
  }
}

void sendDHTData(float temp, float humidity){
  char tempStr[5];
  memset(tempStr,0,5);
  dtostrf(temp,5,2,tempStr);
  
  char humidityStr[5];
  memset(humidityStr,0,5);
  dtostrf(humidity,5,2,humidityStr);
  
  char url[50];
  memset(url,0,50);
  sprintf(url,"")

}

//온도센서
float readDHT11() {
  float temperature = dht.readTemperature();

  Serial.print(F("Temperature: "));
  Serial.println(temperature);
  return temperature;
}

void redLEDtwingkle() {
  digitalWrite(2, HIGH);
  delay(5000);
  digitalWrite(2, LOW);
}

void greenLEDtwingkle() {
  digitalWrite(3, HIGH);
  delay(5000);
  digitalWrite(3, LOW);
}

//RFID 체크 함수
String RfidCheck() {
  Serial.print(F("PICC type: "));
  // 카드의 타입을 읽어온다.
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // 모니터에 출력
  Serial.println(rfid.PICC_GetTypeName(piccType));
  // MIFARE 방식인지 확인하고 아니면 리턴
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    redLEDtwingkle();  //빨간색 점멸
    return;
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
  Serial.println(" ");
  //RFID가 하얀 카드가 맞는지 확인.
  if (rfid.uid.uidByte[0] == cardPC[0] && rfid.uid.uidByte[1] == cardPC[1] && rfid.uid.uidByte[2] == cardPC[2] && rfid.uid.uidByte[3] == cardPC[3]) {
    Serial.println(F("This is Right Card Yeah, Take this num!"));
    //맞는 카드면 cardCode값 리턴.
    return cardCode;
  } else {
    Serial.println("not this card");
    //다른 카드면 그냥 인식값 그대로.
    return rfid.uid.uidByte;
  }
}

//RFID--------------------------------------------------

void printIPAddress() {
  Serial.print(F("My IP Address: "));
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    if (thisByte < 3) Serial.print(F("."));
  }
  Serial.println();
}

//16진수
void printHex(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

// 10진수로 변환하는 함수
void printDec(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}