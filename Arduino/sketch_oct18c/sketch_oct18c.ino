//ehternet+https
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <ArduinoHttpClient.h>

byte mac[] = {
  0x74, 0x69, 0x69, 0x2D, 0x30, 0x0C
};

byte ip[] ={
 192,168,0,112
};

byte dns[]={
  188,126,63,1
};

byte gw[]={
  192, 168,0,1
};

const char servername[]="61.103.243.247";
const int port = 8000;
EthernetClient ethernet;

HttpClient client = HttpClient(ethernet, servername, port);
const char path = "/";

void setup() {
  Serial.begin(9600);
  Serial.println();

  Serial.println(F("Start Ether connection"));
 // Ethernet.begin(mac,ip);
  Ethernet.begin(mac,ip,dns,gw);
  // if(Ethernet.begin(mac) == 0){
  //   Serial.println("Failded to configure Ehternet using DHCP");
  //   for(;;);
  // }

  printIPAddress();
}

void loop() {
  client.get("/");
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.println(statusCode);
  Serial.println(response);

  //response가 {"success":true} 문자열일 때 퍼펙트 출력.
  if(response == "{\"success\":true}"){
    Serial.println("Perfect");
  }

  delay(1000);
}

void printIPAddress() {
  Serial.print(F("My IP Address: "));
  for(byte thisByte =0; thisByte < 4; thisByte++){
    Serial.print(Ethernet.localIP()[thisByte],DEC);
    if(thisByte < 3)Serial.print(F("."));
   
  }
   Serial.println();
}