#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define  GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

MCUFRIEND_kbv tft;

int delayPeriod;

void  setup() {
  // put your setup code here, to run once:
  uint16_t ID=tft.readID();
  tft.begin(ID);
  
}

void loop() {
  // put your main code here,  to run repeatedly:
  tft.fillScreen(BLACK);
  tft.fillRect(11,11,298,48,RED);
  tft.drawRect(10,10,300,50,YELLOW);
  tft.setCursor(25,25);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Welcome to tutorial 2-");
  for(int  i=65;i<500;i+=7){
    tft.drawLine(0,240,100,i,CYAN);
    delay(100);
  }
  for(int i=60;i>5;i-=3){
    tft.drawCircle(210,140,i,GREEN);
    delay(30);  
  }
  for(int i=200;i>70;i-=7){
    tft.drawRect(120,220,i,i,YELLOW);
    delay(30);    
  }
  tft.setCursor(122.5,255);
  tft.setTextSize(2);
  tft.println("Enjoy!");

  delay(1500);

  tft.fillScreen(WHITE);

  tft.fillRect(10,10,300,60,YELLOW);
  tft.setCursor(40,30);
  tft.setTextSize(2);
  tft.setTextColor(MAGENTA);
  tft.println("Thanks for watching !");
  
  for(int i=0;i<80;i+=5){
      tft.drawTriangle(
        160,70+i*2,
        30+i,320-i,
        290-i,320-i,
        RED        
      );
      delay(200);
  }

  tft.fillRect(7,350,300,70,YELLOW);
  tft.setCursor(10,370);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("Repeating myself  in some  seconds");
  for(delayPeriod=4;delayPeriod>0;delayPeriod-=1){
    tft.print(".");
    delay(1000);
     
  }
          
}
