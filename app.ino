#include <Arduino.h>
#include <SPIFFS.h>
#include<M5ModuleDisplay.h>
#include <M5Unified.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_SHT4x.h>
#include <Adafruit_BMP280.h>
#include <Ambient.h>
#include <WiFi.h>

#define LIGHT_SENSOR_PIN 36 
Servo myservo;
int servoPin = 32;
#include <SD.h>
int count = 0;
int fillscreenflag1=0;
int fillscreenflag2=0;
int votescreenflag=1;
int moniter_time=0;
int option_flag1=0;
int option_count1=0;
int option_flag2=0;
int option_count2=0;
int displayWidth;
int displayHeight;
int timeout = 0;
int menu_flg =0;
int select_number = 0;
// PIRセンサーのピン番号
const int PIR_PIN = 33;  // PIRセンサーのデータピンをGPIO21に接続
int brightness = 128;    // 初期の明るさ（0～255）
unsigned long lastMotionTime = 0;  // 最後に動きを検知した時間
const unsigned long idleTime = 10000;  // 明るさを下げるまでの待機時間（ミリ秒）
int lightValue;
int lightFlag=0;
int f1=0;
// --- WiFi情報 ---
const char* ssid = "your_Wifi_ssid";
const char* password = "your_Wifi_password";

// --- Ambient設定 ---
unsigned int channelId = 012345;         // AmbientチャネルID
const char* writeKey = "Your_Write_Key"; // AmbientのWrite Key

WiFiClient client;
Ambient ambient;

// --- センサーオブジェクト ---
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
Adafruit_BMP280 bmp280; // I2C



void setup() {
    auto cfg = M5.config();
    cfg.external_display.module_display = true;
    cfg.module_display.logical_width = 1280;
    cfg.module_display.logical_height = 720;
    M5.begin(cfg);
    //
    int display_count = M5.getDisplayCount();
    
    M5.Lcd.setTextFont(&fonts::efontJA_16);
    M5.setPrimaryDisplayType({
      m5::board_t::board_M5ModuleDisplay,
    });
    int index_module_display = M5.getDisplayIndex(m5::board_t::board_M5ModuleDisplay);
    
    // PIRセンサーのピンを入力モードに設定
    pinMode(PIR_PIN, INPUT);
    myservo.setPeriodHertz(50);
    myservo.attach(servoPin, 400, 2400);
    pinMode(27, OUTPUT);  // LED (GPIO10) を出力モードに設定
    digitalWrite(27, LOW); 


    // --- WiFi接続 ---
    WiFi.begin(ssid, password);
    
    

    // --- Ambient初期化 ---
    ambient.begin(channelId, writeKey, &client);

    

  
    
    


}

void loop() {
    // M5の状態を更新
    M5.update();
    if (M5.BtnB.wasPressed() || menu_flg==1) {
      if (M5.BtnB.wasPressed()) {
        M5.Displays(0).fillScreen(BLACK); 
        timeout=5000;
        menu_flg=1;
        
      }
      menu_mode();

    }
    else{
      devote_mode();
    }

    int motionDetected = digitalRead(PIR_PIN);

    if (motionDetected == HIGH) {
        // 動きを検知した場合
        brightness = 255;  // 明るさを最大に設定
        lastMotionTime = millis();  // 最後の動き検知時間を更新
        M5.Displays(0).setBrightness(brightness);
        lightFlag=1;
        ambient.set(4, lightFlag);
        ambient.send();
        f1=1;
        
        
    }else{
      lightFlag=0;
      if(f1==1){
        ambient.set(4, lightFlag);
        ambient.send();
      }
      f1=0;
        
      // 動きが一定時間検知されなかった場合
      brightness = 0;
          // 明るさを最低に設定
      M5.Displays(0).setBrightness(brightness);
      
        
        
          
        

    }
  
    
}

void menu_mode(){
  timeout-=1;
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Displays(0).setCursor(0,0);
  M5.Displays(0).setTextSize(2);
  M5.Displays(0).print(String(timeout));
  lightValue = analogRead(LIGHT_SENSOR_PIN);  // 光センサーの値を取得
  M5.Displays(0).fillRect(10, 40, 200, 20, TFT_BLACK);   // 画面をクリア
  M5.Displays(0).setCursor(10, 40);
  M5.Displays(0).print(String(lightValue));  // 値を表示
   // 0.5秒ごとに更新
  if(timeout<0){
    select_number=0;
    menu_flg=0;
    M5.Displays(0).fillScreen(BLACK); 
    fillscreenflag2=0;
  }
  M5.Displays(0).setCursor(50,50);
  M5.Displays(0).setTextSize(2);
  if(select_number==1){
    M5.Displays(0).setTextColor(TFT_BLACK,TFT_WHITE);
  }else{
    M5.Displays(0).setTextColor(TFT_WHITE, TFT_BLACK);
  }

  M5.Displays(0).print(String(option_count1));
  M5.Displays(0).setCursor(100,50);
  M5.Displays(0).setTextSize(2);
  if(select_number==2){
    M5.Displays(0).setTextColor(TFT_BLACK,TFT_WHITE);
  }else{
    M5.Displays(0).setTextColor(TFT_WHITE, TFT_BLACK);
  }
  M5.Displays(0).print(String(option_count2));
  M5.Displays(0).setCursor(150,50);
  M5.Displays(0).setTextSize(2);
  if(select_number==3){
    M5.Displays(0).setTextColor(TFT_BLACK,TFT_WHITE);
  }else{
    M5.Displays(0).setTextColor(TFT_WHITE, TFT_BLACK);
  }
  M5.Displays(0).print(String(count));
  if (M5.BtnA.wasPressed()){
    select_number-=1;
  }
  if (M5.BtnC.wasPressed()) {
    select_number+=1;
  }
  if (M5.BtnB.wasPressed()){
    if(select_number==1){
      option_count1+=1;
      count+=1;
    }
    if(select_number==2){
      option_count2+=1;
      count+=1;
    }
    if(select_number==3){
      count+=1;
    }
  }

}

void devote_mode(){

  // タッチポイントのチェック
  if (M5.Touch.getCount() > 0) {  // タッチポイントが存在する場合
    auto touch = M5.Touch.getDetail(0);
    if(touch.y<140 && touch.x<300){ 
      digitalWrite(27, HIGH);
      delay(1000);
      lightValue = analogRead(LIGHT_SENSOR_PIN);
      digitalWrite(27, LOW);
      if(lightValue>=4000){
        M5.Displays(0).setCursor(10, 220);
        if(fillscreenflag1==0){
          M5.Displays(0).fillRect(10, 10, 300, 130,TFT_BLUE);
          fillscreenflag1=1;
          M5.Displays(0).setCursor(20,40);
          M5.Displays(0).setTextColor(TFT_WHITE, TFT_BLUE);
          M5.Displays(0).setTextSize(2);
          M5.Displays(0).printf("ご協力ありがとう");
          M5.Displays(0).setCursor(20,75);
          M5.Displays(0).printf("ございます");
          M5.Displays(0).print(String(lightValue));

              
          if(option_flag1==1){
            option_count1+=1;
          }else{
            option_count2+=1;
          }
          count+=1;
          // Ambientにデータ送信
          ambient.set(2, count);
          ambient.set(3, option_count1);
          ambient.set(4, option_count2);
          ambient.send();
          myservo.write(135); delay(700);
          myservo.write(0); 
        }  
      }else{
        if(fillscreenflag1==0){
          M5.Displays(0).fillRect(10, 10, 300, 130,TFT_YELLOW);
          fillscreenflag1=1;
          M5.Displays(0).setCursor(20,40);
          M5.Displays(0).setTextColor(TFT_BLACK, TFT_YELLOW);
          M5.Displays(0).setTextSize(2);
          M5.Displays(0).printf("キャップを");
          M5.Displays(0).setCursor(20,75);
          M5.Displays(0).printf("入れてください");
          M5.Displays(0).print(String(lightValue));
          delay(100);
        }
      }
      fillscreenflag2=0;
    } 
    if(touch.y>140 && touch.x<150){
      option_flag1=1;
      option_flag2=0;
      votescreenflag=1;  
    }
    if(touch.y>140 && touch.x>150){
      option_flag1=0;
      option_flag2=1;
      votescreenflag=1;       
    }
  }else{
    // タッチがない場合 
    M5.Displays(0).setCursor(10, 220);
    if(votescreenflag==1){
      if(option_flag1==0){
        M5.Displays(0).fillRect(10, 140, 150, 220,  0xEEC3);
        M5.Displays(0).setCursor(70, 175);
        M5.Displays(0).setTextSize(1);
        M5.Displays(0).setTextColor(TFT_BLACK,  0xEEC3);
        M5.Displays(0).printf("選択肢1");
        M5.Displays(0).setCursor(70, 195);
        M5.Displays(0).printf("");
      }else{
        M5.Displays(0).fillRect(10, 140, 150, 220, 0xEEC3);
        M5.Displays(0).setCursor(70, 175);
        M5.Displays(0).setTextSize(1);
        M5.Displays(0).setTextColor(TFT_BLACK,  0xEEC3);
        M5.Displays(0).printf("選択肢1");
        M5.Displays(0).setCursor(70, 195);
        M5.Displays(0).printf("");
        M5.Displays(0).setCursor(70, 215);
        M5.Displays(0).printf("投票選択中");
      }
      if(option_flag2==0){
        M5.Displays(0).fillRect(160, 140, 150, 220, 0xAFE0);
        M5.Displays(0).setCursor( 210, 175);
        M5.Displays(0).setTextSize(1);
        M5.Displays(0).setTextColor(TFT_BLACK, 0xAFE0);
        M5.Displays(0).printf("選択肢2");
        M5.Displays(0).setCursor(210, 195);
        M5.Displays(0).printf("");
      }else{
        M5.Displays(0).fillRect(160, 140, 150, 220, 0xAFE0);
        M5.Displays(0).setCursor( 210, 175);
        M5.Displays(0).setTextSize(1);
        M5.Displays(0).setTextColor(TFT_BLACK, 0xAFE0);
        M5.Displays(0).printf("選択肢2");
        M5.Displays(0).setCursor(210, 195);
        M5.Displays(0).printf("");
        M5.Displays(0).setCursor(210, 215);
        M5.Displays(0).printf("投票選択中");
      }
      votescreenflag=0;
    }
        
    if(fillscreenflag2==0){ 
      M5.Displays(0).fillRect(10, 10, 300, 130,TFT_RED);
      fillscreenflag2=1;
      M5.Displays(0).setCursor(20,40);
      M5.Displays(0).setTextColor(TFT_WHITE, TFT_RED);
      M5.Displays(0).setTextSize(2);
      M5.Displays(0).printf("キャップを入れて");
      M5.Displays(0).setCursor(20,75);
      M5.Displays(0).printf("押してね");
      M5.Displays(1).setTextFont(&fonts::efontJA_16);
      displayWidth = M5.Displays(1).width();
      displayHeight = M5.Displays(1).height();
      M5.Displays(1).setCursor(0, 0);
      M5.Displays(1).fillRect(0, 0, displayWidth/2, displayHeight*3/4,0xEEC3);
      M5.Displays(1).fillRect(displayWidth/2, 0, displayWidth/2, displayHeight*3/4,0xAFE0);
      M5.Displays(1).setTextSize(5);
      M5.Displays(1).setCursor(displayWidth/8-50, 140);
      M5.Displays(1).setTextColor(TFT_BLACK,  0xEEC3);
      M5.Displays(1).print("選択肢1");
      M5.Displays(1).setCursor(displayWidth/8-50, 220);
      M5.Displays(1).print("");
      M5.Displays(1).setTextSize(5);
      M5.Displays(1).setCursor(displayWidth/8+100, 330);
      M5.Displays(1).setTextSize(10);
      M5.Displays(1).print(String(option_count1));
      M5.Displays(1).setCursor(displayWidth*5/8-50, 140);
      M5.Displays(1).setTextColor(TFT_BLACK, 0xAFE0);
      M5.Displays(1).setTextSize(5);
      M5.Displays(1).print("選択肢2");
      M5.Displays(1).setCursor(displayWidth*5/8-50, 220);
      M5.Displays(1).print("");
      M5.Displays(1).setCursor(displayWidth*5/8+100, 330);
      M5.Displays(1).setTextSize(10);
      M5.Displays(1).print(String(option_count2));
      M5.Displays(1).setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Displays(1).setBrightness(brightness);
      M5.Displays(1).setTextSize(10);
      M5.Displays(1).setCursor(displayWidth/4, displayHeight*4/5);
      M5.Displays(1).print("総数："+String(count));
    }
    fillscreenflag1=0;      
  }
}