#include <Wire.h>
#include <mpu9250_blockly.h>
#include <SoftwareSerial.h>
MPU9250Block mpu;
SoftwareSerial mySerial(2, 5);
char byteGPS;
char GLL[] = "$GPGLL"   ;
char PGLL[5];
String er = "ERROR";
String longitude ;
String NS ;
String latitude ;
String WE;
String TIME;
String V;
int w ;
String kk;


void setup() {
  mpu.begin();
  Serial.begin(9600);//pc
  Serial1.begin(9600); //lora
  mySerial.begin(9600);//gps
}

void loop() {
  if (mySerial.available() > 0 ) {
    while (byteGPS != '\n') {
      byteGPS = (char)mySerial.read();
    }
    while (byteGPS == '\n') {
      for (int i = 0; i <= 5 ; i++) {
        byteGPS = (char)mySerial.read();
        delay(10);
        PGLL[i] = (char) byteGPS ;
      }
      if ( PGLL[0] == GLL[0] and PGLL[1] == GLL[1] and PGLL[2] == GLL[2] and PGLL[3] == GLL[3] and PGLL[4] == GLL[4] and PGLL[5] == GLL[5]  ) { //判斷是否為$GPGLL
        byteGPS = (char)mySerial.read(); // "把$GPGLL後的,取出"
        int i = 0;//目前資料為何 ， 以逗號判斷 ，第一個逗號為緯度，第二個為北半球 ...
        while (byteGPS != '\n') { //等於\n時代表GPGLL已全部取出
          String r ; //準備透過LORA傳出的資料
          byteGPS = (char)mySerial.read();
          while (byteGPS != ',' and byteGPS != '\n') { //以逗號區分 各類資料
            r = r + byteGPS;                                  //$GPGLL,2308.28715,N,11322.09875,E,023543.00,A,A*6A
            byteGPS = (char)mySerial.read();
          }
          switch (i) {  //依GPS格式 各自回傳 i=0時 為第二個逗號之前
            case 0 :
              latitude = r;
              break;
            case 1 :
              NS = r ;
              break;
            case 2 :
              longitude = r;
              break;
            case 3 :
              WE = r ;
              break;
            case 4 :
              TIME = r ;
              break;
            case 5 : {
                V = r;
                double gx = mpu.getGyroX();
                double gy = mpu.getGyroY();
                double gz = mpu.getGyroZ();
                double g = abs(gx) + abs(gy) + abs(gz);
                if (g >= 20) {
                  Serial.println("move");
                  w = 1;
                } else {
                  w = 0;
                }
                if (V == "A" and w == 1)
                {
                  Serial1.print("At\r");
                  delay(1000);
                  Serial1.print("AT+DTX=" + (String)latitude.length() + ",\"" + latitude + "\"\r");
                  delay(500);
                  if (Serial1.available() > 0) {
                    kk = Serial1.readString();
                    Serial.print(kk);

                  }
                  Serial1.print("AT+DTX=" + (String)NS.length() + ",\"" + NS + "\"\r");
                  delay(500);
                  if (Serial1.available() > 0) {
                    kk = Serial1.readString();
                    Serial.print(kk);

                  }
                  Serial1.print("AT+DTX=" + (String)longitude.length() + ",\"" + longitude + "\"\r");
                  delay(500);
                  if (Serial1.available() > 0) {
                    kk = Serial1.readString();
                    Serial.print(kk);

                  }

                  Serial1.print("AT+DTX=" + (String)WE.length() + ",\"" + WE + "\"\r");
                  delay(500);
                  if (Serial1.available() > 0) {
                    kk = Serial1.readString();
                    Serial.print(kk);

                  }

                  Serial1.print("AT+DTX=" + (String)TIME.length() + ",\"" + TIME + "\"\r");
                  delay(500);
                  if (Serial1.available() > 0) {
                    kk = Serial1.readString();
                    Serial.print(kk);

                  }
                  delay(10000);
                }
                break;
              }
            default:
              break;
          }
          i += 1;
          r = "";
        }
      }
    }
  }
}
