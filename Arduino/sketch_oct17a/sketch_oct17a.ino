#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int nTones[] = {262, 294, 330, 349, 392, 440, 494, 523};  //도레미파솔라시도
int nMax = 8;
int count = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("\nInit LCD");
  lcd.init();
  lcd.backlight();
  // lcd.setCursor(0, 0);
  // lcd.print("Hello worlde");
  // lcd.setCursor(0,1);
  // lcd.print(2016244084);
  pinMode(2, OUTPUT);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, OUTPUT);

       

}



void loop() {
  //3번
  if(digitalRead(3)==0){
    
    digitalWrite(2, HIGH);
       for (int i = 0; i < nMax; i++)
        {
          tone(4, nTones[i],400);         //0부터 순서대로 들어간다.
          delay(200);               //0.2초의 지연시간을 준다.
          // if(i == nMax-1){
          //   count++;
          // }
        }
       count++;
       lcd.setCursor(0, 0);
       lcd.print(count);
        Serial.println(count);
        noTone(4);
        
  }
   else {
    // lcd.setCursor(0, 0);
    // lcd.print("      ");
        digitalWrite(2, LOW);
        lcd.setCursor(0, 0);
        lcd.print(count);
      }

  
}