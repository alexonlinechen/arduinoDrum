
#include <SD.h>
#include <TMRpcm.h>
#include <SPI.h>
#include "tonewav.h"

TMRpcm tmrpcm;

#define SD_ChipSelectPin 5
#define BUTTON1_PIN  2  // 按鍵的接腳D2
#define BUTTON2_PIN  3
#define BUTTON3_PIN  4
#define pinReset 6   //重啟ping
#define DELAYTIME 1000 //定義按鈕延遲時間

unsigned long timePrevious, timeCurrent;  

int button1State ;   // 按鈕的狀態
int button2State ;   
int button3State ;  


int password;
int guess ;
int min_no = 0 ;
int max_no = 99 ;
int L_no = 0;
int R_no = 0 ;
int P_L_no ;
int P_R_no ;

int bomb ;
int bomb_no ;

// 骰子
int dice;

//賓果
int n[16]={};
int bingo_count = 0 ;
int bingo_no;

//analog Arduino's inputs
const int Pad[8]={0,1,2,3,4,5,6,7}; 

static const char wav_0[] PROGMEM = "D01.wav";
static const char wav_1[] PROGMEM = "D02.wav";
static const char wav_2[] PROGMEM = "D03.wav";
static const char wav_3[] PROGMEM = "D04.wav";
static const char wav_4[] PROGMEM = "D05.wav";
static const char wav_5[] PROGMEM = "D06.wav";
static const char wav_6[] PROGMEM = "D07.wav";
static const char wav_7[] PROGMEM = "D08.wav";


const char *wav_table[] = 
  { wav_0, wav_1, wav_2, wav_3, wav_4, wav_5, wav_6, wav_7 };


int KEYdown = 0;
int KEY = 0;
int KEYup = 0;
int startplay = 0;
int num = 0 ;
int countnum = 1 ;


//音量閾值變量，用於比較感應器數值   default=380
const int volumeThreshold = 200;   


//每個鼓墊振動分離過程中使用的變數
boolean signalTest[8]={0,0,0,0,0,0,0,0};

void setup()
{
  digitalWrite(pinReset, HIGH);
  //設定按鈕的接腳為輸入
  pinMode(BUTTON1_PIN, INPUT); 
  pinMode(BUTTON2_PIN, INPUT); 
  pinMode(BUTTON3_PIN, INPUT); 
  pinMode(pinReset, OUTPUT);
  
  //喇叭正極接口
  tmrpcm.speakerPin = 9;
  tmrpcm.setVolume(5);
  
  Serial.begin(115200); 

  //檢查SD模組是否正常
  if (!SD.begin(SD_ChipSelectPin)) {  
    Serial.println(F("SD card 未設置"));
    return; } 
  else {
    Serial.println(F("SD card read 功能正常"));
    tmrpcm.play(F("00-sdok.wav"));
  }

   //設定亂數種子
  randomSeed(micros()); 
  password = random(1, 100);
  bomb = random(1, 9);
  bingo_get();
  
}


void loop()
{ 

    button1State = digitalRead(BUTTON1_PIN);
    button2State = digitalRead(BUTTON2_PIN);
    button3State = digitalRead(BUTTON3_PIN);
   
     
    //按鍵1功能
    
    if(button1State == 0){ 
       
          if( startplay == 0 ){ 
                    countnum = countnum+1 ;   
                    Serial.println(F("按鍵Next"));
                   // tmrpcm.play("00-don.wav"); 
                     
                  if( countnum == 10 ){ tmrpcm.play(F("00-drum.wav")); }  
                  if( countnum == 2 ){ tmrpcm.play(F("00-vp.wav")); }
                  if( countnum == 3 ){ tmrpcm.play(F("00-piano.wav")); }
                  if( countnum == 4 ){ tmrpcm.play(F("00-vi.wav")); } 
                  if( countnum == 5 ){ tmrpcm.play(F("00-cust.wav")); }  //自訂音樂
                  if( countnum == 6 ){ tmrpcm.play(F("00-code.wav")); }  // 終極密碼 語音
                  if( countnum == 7 ){ tmrpcm.play(F("00-bomb.wav")); } 
                  if( countnum == 8 ){ tmrpcm.play(F("0-bingo.wav")); } 
                  
                  if( countnum == 9 ){ tmrpcm.play(F("00-dice.wav")); }
                  
                  delay(500);
                    
                               }   
        
          if( startplay == 1) {    Serial.println(F("低音"));  KEYdown = 1;  KEY = 0; KEYup = 0;  }   
                         }


        //按鍵2功能
         if(button2State == 0){ 
           if( startplay == 0) { 
                     tmrpcm.play(F("00-ok.wav")); 
                     Serial.println(F("確定"));
                     startplay = 1;
                     } 
                   
                   // delay(3000); 
                if( startplay == 1) {    Serial.println(F("中音"));  KEYdown = 0;  KEY = 1; KEYup = 0;  }      
                if( countnum == 7 && startplay == 1 && KEY == 1 ){ tmrpcm.play(F("00-bomb.wav"));     }      
                         } 
                         
     //按鍵3功能
     if(button3State == 0){   
       
           if( startplay == 1){     Serial.println(F("高音"));     KEYdown = 0;   KEY = 0; KEYup = 1; }
           if( countnum == 1 ){ tmrpcm.play(F("00-china.wav")); }

          
           if( countnum == 7 && startplay == 1 && KEYup == 1 ){ tmrpcm.play(F("0-2bomb.wav"));
                                                                password = random(1, 100);
                                                                bomb_no =1 ;
                                                              } 
                                             
           if( countnum == 9 && startplay == 1 && KEYup == 1 ){      
                                           Serial.println(F("run擲骰子"));
                                             run_dice();
                                             } 
                      
                           } //end btn
        


   if(button1State == 0 && button2State == 0){  
         tmrpcm.play(F("00-reset.wav"));
         delay(2000); 
         digitalWrite(pinReset, LOW);
                       }
 

   //循環模式
   if (countnum >=10){   countnum = 1; } 

      //Serial.println(num);   
      //  if(countnum == 1 && startplay == 1 && KEYdown == 1){ playotherdrum();   }
        if(countnum == 1 && startplay == 1 && KEY == 1){ playdrum();   }
        if(countnum == 1 && startplay == 1 && KEYup == 1){ playchinadrum();  } 
        
        if(countnum == 2 && startplay == 1 && KEYdown == 1){ playvibD();   }
        if(countnum == 2 && startplay == 1 && KEY == 1){ playvib();  }  
        if(countnum == 2 && startplay == 1 && KEYup == 1){ playvibU();  }  

        if(countnum == 3 && startplay == 1 && KEYdown == 1){ playpianoD();   }
        if(countnum == 3 && startplay == 1 && KEY == 1){ playpiano();  }  
        if(countnum == 3 && startplay == 1 && KEYup == 1){ playpianoU();  }  

        if(countnum == 4 && startplay == 1 && KEYdown == 1){ playviD();   }
        if(countnum == 4 && startplay == 1 && KEY == 1){ playvi();  }  
        if(countnum == 4 && startplay == 1 && KEYup == 1){ playviU();  }                                                           

        if(countnum == 5 && startplay == 1 && KEYdown == 1){ playCD();   }
        if(countnum == 5 && startplay == 1 && KEY == 1){ playC();  }  
        if(countnum == 5 && startplay == 1 && KEYup == 1){ playCU();  } 

        if(countnum == 6 && startplay == 1 && KEY == 1){ Run_addno_L();   
                                                         Run_addno_R();
                                                         Run_deno_L();
                                                         Run_deno_R();
                                                         Run_ok();
                                                        } 

        if(countnum == 7 && startplay == 1 && KEY == 1){ bomb_run();  }  //踩地雷
        if(countnum == 7 && startplay == 1 && KEYup == 1){ no_bomb();  }  //地雷
        if(countnum == 8 && startplay == 1 && KEY == 1){ bingo(); }  //bingo
     

             }  //end loop






  void playdrum(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, wav_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放tone")); 
            Serial.println(i); 
   
           //sets signalTest again to 0
          signalTest[i] = 0;
          }} }


     void playchinadrum(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, cd_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放c drum")); 
            Serial.println(i); 
   
           //sets signalTest again to 0
          signalTest[i] = 0;
          }} }
          
       //low 設定的鼓組
      void playotherdrum(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, md_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放m drum")); 
            Serial.println(i); 
   
           //sets signalTest again to 0
          signalTest[i] = 0;
          }} }  


  void playvibD(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, vibD_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放(低)phone")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }




  void playvib(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, vib_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放phone")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }


  void playvibU(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, vibU_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放(高)phone")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }



  void playpianoD(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, pianoD_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放(低)piano")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }



  void playpiano(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, piano_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放piano")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }


  void playpianoU(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, pianoU_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放(高)piano")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }



  void playviD(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, viD_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放(低)小提琴")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }




  void playvi(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, vi_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放-小提琴")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }


  void playviU(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, viU_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放(高)小提琴")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }



          


//自訂義
    void playCD(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {

            char wavFile[33];
            strcpy_P(wavFile, CD_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放(低)自訂")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }    

              void playC(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, C_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放-自訂")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }       

    
    void playCU(){
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
           
            //播放sd卡中的tone
            char wavFile[33];
            strcpy_P(wavFile, CU_table[i]);
            tmrpcm.play(wavFile);
            Serial.print(F("播放(高)自訂:")); 
            Serial.println(i); 
            
          signalTest[i] = 0;
          }} }  

          

 //     ===============    終極密碼  遊戲 程式碼  ==============


 //  調整輸入數值 的功能  
 void Run_addno_L(){
        
           if(analogRead(Pad[0]) >= volumeThreshold) {  signalTest[0] = 1; }

          if(signalTest[0] == 1 && analogRead(Pad[0]) <= (volumeThreshold-20))   {
           
            L_no = L_no+1 ;
            if (L_no >=10) {L_no =0 ;}
            char wavFile[33];
            strcpy_P(wavFile, no_table[L_no]);
            tmrpcm.play(wavFile); 
              
            signalTest[0] = 0;
            delay(200);
                                                                                 } 
                }  //end  



 void Run_addno_R(){
        
           if(analogRead(Pad[1]) >= volumeThreshold) {  signalTest[1] = 1; }

          if(signalTest[1] == 1 && analogRead(Pad[1]) <= (volumeThreshold-20))   {
           
            R_no = R_no+1 ;
            if (R_no >=10) {R_no =0 ;}
            char wavFile[33];
            strcpy_P(wavFile, no_table[R_no]);
            tmrpcm.play(wavFile);
   
            signalTest[1] = 0;
            delay(200);
                                                                                  } 
                    } //end  


 
 void Run_deno_L(){
        
          if(analogRead(Pad[4]) >= volumeThreshold) {  signalTest[4] = 1; }

          if(signalTest[4] == 1 && analogRead(Pad[4]) <= (volumeThreshold-20))   {
           
            L_no = L_no-1 ;
            if (L_no < 0) {L_no = 9 ;}
            char wavFile[33];
            strcpy_P(wavFile, no_table[L_no]);
            tmrpcm.play(wavFile);   
            
           signalTest[4] = 0;
           delay(200);
                                                                               } 
          } //end   


 void Run_deno_R(){
        
           if(analogRead(Pad[5]) >= volumeThreshold) {  signalTest[5] = 1; }

          if(signalTest[5] == 1 && analogRead(Pad[5]) <= (volumeThreshold-20))   {
           
            R_no = R_no-1 ;
            if (R_no <0) {R_no =9 ;}
            char wavFile[33];
            strcpy_P(wavFile, no_table[R_no]);
            tmrpcm.play(wavFile);
   
            signalTest[5] = 0;
            delay(200);
                                                                                }
            }   


          
   //  "確認" 的按鍵功能       
 void Run_ok(){
        
           if(analogRead(Pad[7]) >= volumeThreshold) {  signalTest[7] = 1; }

          if(signalTest[7] == 1 && analogRead(Pad[7]) <= (volumeThreshold-20))   {
           run_pw_game();
          
           signalTest[7] = 0;
           delay(1500);
                                                                                 } 
              }   



 //主要判別式
 void run_pw_game(){
                      
           guess = L_no*10 + R_no ;
             //      Serial.print("guess: "); 
             //      Serial.println(guess); 
         
         //檢查輸入數值是否符合範圍
         checkvule();
      //   if (guess == 0 ) { Serial.println(F("輸入數值錯誤"));
                        //       tmrpcm.play(F("00-error.wav"));
                          //     delay(1000);
                          //     tmrpcm.play(F("00-code1.wav"));}  

         // 符合最大小值 內範圍才能進入下一步           
         if(guess >= min_no && guess <= max_no ){
               
                if (guess != min_no && guess != max_no && guess != 0 && guess < password  ) { 
                                           min_no = guess ; 
                                           tmrpcm.play(F("00-code8.wav")); // 滴答聲
                                           delay(3000);
                                           process();                       
                                        }
                                          
                                   
                if (guess != min_no && guess != max_no && guess > password ) { 
                                         max_no = guess ;  
                                         tmrpcm.play(F("00-code8.wav")); // 滴答聲
                                         delay(3000);
                                         process();            
                                        }
                                                 }  //end 
                                                    

                if (guess == password) { 
                                      pw_split();
                                      Serial.println(F("遊戲結束。重新開始"));
                                      tmrpcm.play(F("00-code7.wav"));
                                      delay(2000); 
                                      L_no=0;
                                      R_no=0;
                                      min_no = 0 ;
                                      max_no = 99 ;
                                      password = random(1, 100);
                                      delay(2000);
                                      tmrpcm.play(F("00-code.wav"));
                                      delay(5000); 
                                      }

                                               }//end

                   
//公布終極密碼
void pw_split(){

          tmrpcm.play(F("00-code8.wav"));  
          delay(3000);
          
          tmrpcm.play("00-code5.wav");  //播放爆炸音效
          delay(3000);
          Serial.print(F("password: ")); 
          Serial.println(password); 

          tmrpcm.play(F("00-code6.wav"));  // 語音"終極密碼:  數字為: "
          delay(3000);
          
          int  P_L_no = password/10;
          int  P_R_no = password - P_L_no*10 ;

           char wavFile[33];
            strcpy_P(wavFile, no_table[P_L_no]);
            tmrpcm.play(wavFile);
            delay(800);

            strcpy_P(wavFile, no_table[P_R_no]);
            tmrpcm.play(wavFile);
            delay(2000);

               } //end

//語音說明 密碼範圍
void process(){
          int    min_L = min_no/10;
          int    min_R = min_no - min_L*10 ;
          int    max_L = max_no/10;
          int    max_R = max_no - max_L*10 ;
       
            Serial.print(F("密碼介於:  ")); 
            tmrpcm.play(F("00-code2.wav"));  // 語音" 密碼介於: "
            delay(800);            
            Serial.print(min_no); 
            
            char wavFile[33];
            strcpy_P(wavFile, no_table[min_L]);
            tmrpcm.play(wavFile);
            delay(800);

            strcpy_P(wavFile, no_table[min_R]);
            tmrpcm.play(wavFile);
            delay(800);
          
            Serial.print(F(" - "));     
            tmrpcm.play(F("00-code3.wav"));  //語音"到" 
            delay(800); 
            Serial.println(max_no); 
             
            strcpy_P(wavFile, no_table[max_L]);
            tmrpcm.play(wavFile);
            delay(800);

            strcpy_P(wavFile, no_table[max_R]);
            tmrpcm.play(wavFile);
            delay(800);

            tmrpcm.play(F("00-code4.wav"));  //語音"請輸入號碼" 
               }//end

//檢查輸入範圍是否符合
void checkvule(){
  
           if(guess <= min_no || guess >= max_no ){
                     Serial.println(F("輸入數值錯誤"));
                     tmrpcm.play(F("00-error.wav"));
                     delay(1000);
                     tmrpcm.play(F("00-code1.wav"));
                     delay(4000);  
                     process();
                                                  }                  
                   }    //end

 
 
 
 //     ===============    踩地雷  遊戲 程式碼  ================ //


      void bomb_run(){

            Serial.println(F("炸彈:")); 
            Serial.println(bomb);   
            bomb_in();
                          }//end

                     

     void bomb_in(){
          
        for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold) 
                        {  signalTest[i] = 1; }

          if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {

            Serial.println(F("炸彈 NO:")); 
            Serial.println(i); 
            bomb_no = i ;
            tmrpcm.play(F("00-code8.wav"));  
            delay(3000);

            bomb_check();
            
            signalTest[i] = 0;
          }} }    



       void bomb_check(){                        
            if (bomb_no != bomb) { 
                                  Serial.println(F(" Pass! , 地雷重新隨機設定 ")); 
                                  tmrpcm.play(F("00-bomb1.wav"));
                                //  delay(7000);
                                  bomb = random(0, 8);
                                  
                                 }

                                
            if (bomb_no == bomb) { 
                                   Serial.println(F(" Boom! , 炸彈!~引爆!~ "));  
                                   tmrpcm.play(F("00-bomb2.wav"));
                                   delay(3000);
                                   tmrpcm.play("00-code5.wav");  //播放爆炸音效
                                   delay(3000);
                                   tmrpcm.play(F("00-bomb3.wav"));
                                   delay(6000);
                                   tmrpcm.play(F("00-bomb.wav"));
                                   
                                   bomb = random(1, 9);
                                 
                                 }
  
                     }



 //     ===============   數字炸彈  遊戲 程式碼  ================ //

 void no_bomb(){

    //借用password 取號範圍 1-99
     bomb = password;

      for(int i=0;i<8;i++){
           if(analogRead(Pad[i]) >= volumeThreshold){  signalTest[i] = 1; }

           if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {
            
                 Serial.print(F("炸彈數字: ")); 
                 Serial.println(bomb); 
                 
                 tmrpcm.play(F("D14.wav"));  //點擊的音效
                 no_bomb_wav();

           if(bomb == bomb_no){  tmrpcm.play(F("00-code5.wav"));  //播放爆炸音效
                                 delay(3000);
                                 tmrpcm.play(F("0-2bomb1.wav"));  //
                                 delay(3000);
                                 
                                 L_no = bomb/10 ;
                                 R_no = bomb - L_no*10 ;

                                 char wavFile[33];
                                 strcpy_P(wavFile, no_table[L_no]);
                                 tmrpcm.play(wavFile); 
                                 delay(800);
                                 strcpy_P(wavFile, no_table[R_no]);
                                 tmrpcm.play(wavFile);
                                 delay(2000);
                                 
                                 tmrpcm.play(F("0-2bomb2.wav"));  //初始化
                                 password = random(1, 100);
                                 bomb_no = 0 ;
                                 }
            
          signalTest[i] = 0;
          bomb_no++ ;
          delay(400);
                                                                                  }
                         }
  
 }//end


void no_bomb_wav(){
 
       for(int i=1;i<10;i++){
                 
               if (bomb_no == i*10 ){
            
                                //拆分語音
                            L_no = bomb_no/10 ;
                            R_no = bomb_no - L_no*10 ;
                
                            char wavFile[33];
                            strcpy_P(wavFile, no_table[L_no]);
                            tmrpcm.play(wavFile); 
                            delay(800);
                            strcpy_P(wavFile, no_table[R_no]);
                            tmrpcm.play(wavFile);

                                }
                           }
                   } //end
 










 //     ===============    賭骰子  程式碼  ================ 

 void run_dice(){

    dice = random(1, 7);
    
    tmrpcm.play("00-dice1.wav");  //dice的音效
    delay(4000);
    tmrpcm.play("00-dice2.wav");  //擲出的骰子點數為  
    delay(3000);
    dice_no(); //點
    delay(800);
    
    
    if (dice == 1 || dice == 2 || dice == 3 ){ 
          tmrpcm.play("00-dice3.wav");  // 點 , 1 2 3 
          delay(2000);
          dice_no();  //點
          delay(800);
          tmrpcm.play("00-dice4.wav"); // 點 小
                                       }

    if (dice == 4 || dice == 5 || dice == 6 ){  
          tmrpcm.play("00-dice5.wav");  // 點 , 4 5 6 
          delay(2000);
          dice_no();  //點
          delay(800);
          tmrpcm.play("00-dice6.wav"); // 點 大
                                            }
    
         delay(3000);
         tmrpcm.play("00-dice7.wav");  // 請重新押注

              } //end
 

//播放點數
 void dice_no(){
           if (dice == 1){  tmrpcm.play("N01.wav");  }
           if (dice == 2){  tmrpcm.play("N02.wav");  }
           if (dice == 3){  tmrpcm.play("N03.wav");  }
           if (dice == 4){  tmrpcm.play("N04.wav");  }
           if (dice == 5){  tmrpcm.play("N05.wav");  }
           if (dice == 6){  tmrpcm.play("N06.wav");  }    
                }       




 //     ===============    BinGO 賓果  程式碼  ================ 
      //  預設 4*4=16
      //設定按下確認按鈕(BTN2)時已先  bingo_get()取號了
      
   void bingo() {

            for(int i=0;i<8;i++){
                if(analogRead(Pad[i]) >= volumeThreshold){  signalTest[i] = 1; }

                if(signalTest[i] == 1 && analogRead(Pad[i]) <= (volumeThreshold-20))   {

             
                  //運行取號主程式 
                    bingo_run();
             
                    signalTest[i] = 0;
                    
                    delay(2000);  //delay 以免短時間再次取號
                                                                                        }
                                 }                                                 
                 } //end bingo



                 
   //取號主程式 
   void bingo_run() {   
                                
          // 再範圍內
         if (bingo_count < 17){
              bingo_no = n[bingo_count];//設定隨機取號後的順序
                          
             Serial.print("按鍵次數 "); 
             Serial.print(bingo_count);
             Serial.print(":"); 
             Serial.println(bingo_no);

             tmrpcm.play(F("0-bingo1.wav")); //取號的音效
             delay(5500);
             tmrpcm.play(F("0-bingo2.wav")); //抽出的號碼為:
             delay(2000);
             
             bingo_wav();   // 號碼
             delay(3000);
             
             tmrpcm.play(F("0-bingo3.wav")); //請按下按鍵繼續抽號
             
             bingo_count ++ ;  //放最後+1
                            } 

          //超過取號範圍  遊戲結束
          if (bingo_count >= 16){
                                 Serial.print("遊戲結束"); 
                                 tmrpcm.play(F("0-bingo4.wav")); //已經抽完所有號碼,遊戲結束  重新開始新的遊戲
                                 delay(6000);
                                 tmrpcm.play(F("0-bingo.wav")); //初始語音

                                 //歸零/初始設定
                                 bingo_count =0;  
                                 bingo_get();
                                }
                            
                            
                 } //end bingo_run




    void bingo_get(){

     //Serial.print("產生隨機順序 不重複 ");
    
        for(int i=0; i<16; i++){
                                n[i]=random(1, 17);
      
            for (int j = 0; j < i; j++) { 
                       
                   if (n[j] == n[i]){  i--; 
                                       break;
                                    }
                                        } 
                               }                 
                    }//end bingo_get


     //播放抽出號碼語音
    void bingo_wav(){
      
       //bingo_no=取號的數值
            //拆分語音
            L_no = bingo_no/10 ;
            R_no = bingo_no - L_no*10 ;
                
            char wavFile[33];
            strcpy_P(wavFile, no_table[L_no]);
            tmrpcm.play(wavFile); 
            delay(800);
            strcpy_P(wavFile, no_table[R_no]);
            tmrpcm.play(wavFile);
         
                              
                   }  //end bingo_wav       

                     
                                    
