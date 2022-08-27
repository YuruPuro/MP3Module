/**
 * 指定した音声ファイルを演奏するサンプル
 * A sample that plays the specified audio file
 */
#include <SoftwareSerial.h>
#define BUSY_PIN A5
#define LED_PIN 9
#define HBYTE(X) ((X & 0xFF00) >> 8) 
#define LBYTE(X) (X & 0xFF) 

const char playFile[] = {"12      WAV"}; // Short Filename 8+3 

SoftwareSerial SoftSerial(10, 11);
int fNum = 0 ;
char filename[16];

void setup() {
  pinMode(BUSY_PIN,INPUT);
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LOW) ;

  Serial.begin(9600);
  
  SoftSerial.begin(9600);
  volume(0x1E);
  fNum = fileNums( ) ;

  for (int i=1;i<=fNum ; i++) {
    selectTrack(i);
    getFileName( );
    if (strcmp(filename,playFile) ==0) {
      play(i) ;
      delay(500) ;
      digitalWrite(LED_PIN,HIGH) ;
      while(analogRead(BUSY_PIN) > 255) ;
      digitalWrite(LED_PIN,LOW) ;
    }
  }
}

void loop() {
}

// △▽△▽△▽△▽△▽△▽△▽△▽△▽△▽△▽△▽
uint8_t checkSum(uint8_t data[] , int dataSize) {
  uint8_t sum = 0 ;
  for (int i=0;i<dataSize;i++) {
    sum += data[i] ;
  }
  return sum ;
}

// ボリューム設定 vol:0x00-0x1E
void volume( unsigned char vol) {
  unsigned char commands[5] = {0xAA,0x13,0x01,0x00,0x00};
  commands[3] = vol ; 
  commands[4] = checkSum(commands,4) ; 
  SoftSerial.write(commands,5);
}

// 指定トラックを演奏
void play(int Track) {
  uint8_t commands[6] = {0xAA,0x07,0x02,0x00,0x00,0x00} ;
  commands[3] = HBYTE(Track) ; 
  commands[4] = LBYTE(Track) ; 
  commands[5] = checkSum(commands,5) ; 
  SoftSerial.write(commands,6);
}

// ファイル数を取得
int fileNums( ) {
  unsigned char commands[4] = {0xAA,0x0C,0x00,0xB6};
  SoftSerial.write(commands,4);
  
  int rByte = 0;
  uint8_t rcvData[6];
  while(rByte < 6) {
    if (SoftSerial.available() > 0) {
      uint8_t incomingByte = SoftSerial.read();
      rcvData[rByte] = incomingByte ;
      rByte ++;
    }
  }
  int fileNum = rcvData[3] * 256 + rcvData[4];
  return fileNum;
}

// トラックを選択（演奏しない）
void selectTrack(int Track) {
  uint8_t commands[6] = {0xAA,0x1F,0x02,0x00,0x00,0x00};
  commands[3] = HBYTE(Track); 
  commands[4] = LBYTE(Track); 
  commands[5] = checkSum(commands,5); 
  SoftSerial.write(commands,6);
}

// カレント・トラックのショートファイル名を取得
void getFileName() {
  uint8_t commands[4] = {0xAA,0x1E,0x00,0xC8};
  SoftSerial.write(commands,4);

  int rByte = 0;
  int rLen = 4;
  int nameLen = 0;
  uint8_t rcvData[24];
  while(rByte < rLen) {
    if (SoftSerial.available() > 0) {
      uint8_t incomingByte = SoftSerial.read();
      rcvData[rByte] = incomingByte ;
      if (rByte==2) {
        rLen += incomingByte ;
       }
      rByte ++ ;
    }
  }
  strcpy(filename,(char *)&rcvData[3]) ;
}
