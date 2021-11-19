// Chương trình viết cho điều khiển 2 động cơ
#include <Servo.h>
#include <Wire.h> 
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>     // Thư viện cho LCD
#include <Keypad.h>               // Thư viên cho Keypad
#include <EEPROM.h>              // Thư viện EEPROM để lưu giá trị cho lần sau

// --------------------------
// Khai báo số hàng và cột của Keypad
const byte ROWS = 4         ;   // rows
const byte COLS = 4         ;  // columns
//Định nghĩa tên gọi của các nút ấn trên bàn phím
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
}                       ;    

// Khai bao các chân sử dụng theo hàng và cột
byte rowPins[ROWS] = {A15, A14, A13, A12} ;    //connect to the row pinouts of the keypad
byte colPins[COLS] = {A11, A10, A9, A8}   ;   //connect to the column pinouts of the keypad
//Khởi tạo class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS) ;

// --------------------------
// Khởi tạo cho LCD
// Set the LCD address to 0x27 for a 16 chars and 2 line display
// LCD pin : GND , VCC, SDA ,SCL
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 la dia chi I2C cua LCD


// --------------------------
// Khởi tạo đồng hồ thời gian thực
RTClib myRTC;
DS3231 Clock;
void setTime(String stime)
{
  byte Year;
  byte Month;
  byte Date;
  //byte DoW;
  byte Hour;
  byte Minute;
  byte Second;
  String dataS =stime.substring(0,2);
  Year = dataS.toInt();
  Serial.print("Year :");
  Serial.println(Year);
  dataS =stime.substring(2,4);
  Month = dataS.toInt();
  Serial.print("Month :");
  Serial.println(Month);
  dataS =stime.substring(4,6);
  Date = dataS.toInt();
  Serial.print("Date :");
  Serial.println(Date);

  dataS =stime.substring(6,8);
  Hour = dataS.toInt();
  Serial.print("Hour :");
  Serial.println(Hour);

  dataS =stime.substring(8,10);
  Minute = dataS.toInt();
  Serial.print("Min :");
  Serial.println(Minute);

  dataS =stime.substring(10,12);
  Second = dataS.toInt();
  Serial.print("Sec :");
  Serial.println(Second);
  

  Clock.setClockMode(false);	// set to 24h
  //setClockMode(true);	// set to 12h

  Clock.setYear(Year);
  Clock.setMonth(Month);
  Clock.setDate(Date);
  //Clock.setDoW(DoW);
  Clock.setHour(Hour);
  Clock.setMinute(Minute);
  Clock.setSecond(Second);
}
// --------------------------
// Khai báo cụm lấy sác FB
Servo myservo1;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo

#define DCMotorP 7  // DC Motor +
#define DCMotorN 4  // DC Motor -

#define FB_LimitP  A0 // Công tắc giới hạn thuận
#define FB_LimitN  A3 // Công tắc giới hạn ngược

#define RUN_P A1  // Nút ấn quay chiều dương
#define RUN_N A2 // Nút ấn quay chiều âm
// >>>Khai bao bien va hang so
int Status = 0 ;

#define EN        8

//Direction pin
#define X_DIR     5
#define Y_DIR     6

//Step pin
#define X_STP     2
#define Y_STP     3

// Stop limit
#define X_STOP 9
#define Y_STOP 10

// Cảm biến phát hiện có sách trong máng
#define MangSach 11

// Nút ấn điều khiển
#define START A0
#define STOP  A2

// Bước vít me
float BUOC_VIT_ME =  8; // 8 mm
float CHU_VI_PULY = 40; // 40 mm
int XUNG_1VONG = 200;
float LX_XUNG_MM = XUNG_1VONG / BUOC_VIT_ME; // Số xung trục lên xuống di chuyển 1 mm =25
float TP_XUNG_MM = XUNG_1VONG / CHU_VI_PULY; // Số xung trục trái phải di chuyển 1 mm =5

// Biến trạng thái
int step_X = 0; // Động cơ trái phải
int step_Y = 0; // Động cơ lên xuống

// Modun A4988
int delayTime=1200; //Delay between each pause (uS)

// Các tọa độ trục ( xung)

// Toa do truc X cho cac vi tri sach 0-9
int X_Index[10] = {0,470,950,1425,1905,20,475,955,1425,1910};
// toa do truc Y cho cac vi tri lay sach 0-9
int Y_Index[10] = {0,0,0,0,0,6410,6410,6410,6410,6410} ; // 6500 ~ 260 mm
// Toa do chenh lech giua vi tri lay va tra sach
int Y_tra_sach = 250 ; // 10 mm
int toa_do_X = 0; // Toa do hien tai cua X theo xung
int toa_do_Y = 0; // Toa do hien tai cua Y theo xung
int viTriSach=0 ; // Vị trí sách cần lấy, cất
int HanhDong = 0 ; // Hành động, lấy, cất sách ...



// Hàm xuất xung cho động cơ step : Chiều quay, chân chọn chiều, chân xung, số bước
void stepRun(boolean dir, byte dirPin, byte stepperPin, int steps)
{

  digitalWrite(dirPin, dir);
  //delay(100);
  for (int i = 0; i < steps; i++)
  {
    digitalWrite(stepperPin, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(stepperPin, LOW);
    delayMicroseconds(delayTime);
  }
}

// Hàm về gốc cho trục X,Y
bool veGoc( byte dirPin, byte stepperPin,byte stopPin)
{
  FB_DayRa();
  digitalWrite(EN, LOW);
  while ( digitalRead(stopPin) == 0 ) // CTHT chưa chạm
  {
    stepRun(false, dirPin, stepperPin,1); //Chạy lùi về gốc

  }
  return ( digitalRead(stopPin) ) ; // ve goc xong tra ve gia tri 1
  digitalWrite(EN, HIGH);
  delay(100); // Chờ chút

}

// Do toa do X cua cac vi tri de sach
void do_toa_do_X()
{
  while(1)
  {
    if ((digitalRead(RUN_P) == 0)&& (digitalRead(RUN_N) == 1))
    {
      stepRun(true, X_DIR, X_STP, 5);
      toa_do_X +=5;
      Serial.println(toa_do_X);
      delay(100);
    }
    if ((digitalRead(RUN_P) == 1)&& (digitalRead(RUN_N) == 0))
    {
      stepRun(false, X_DIR, X_STP, 5);
      toa_do_X -=5;
      Serial.println(toa_do_X);
      delay(100);
    }
    if ((digitalRead(RUN_P) == 0)&& (digitalRead(RUN_N) == 0))
    break;
  }
}

// Do toa do Y
void do_toa_do_Y()
{
  while(1)
  {
    if ((digitalRead(RUN_P) == 0)&& (digitalRead(RUN_N) == 1))
    {
      stepRun(true, Y_DIR, Y_STP, 25);
      toa_do_Y +=25;
      Serial.println(toa_do_Y);
      delay(100);
    }
    if ((digitalRead(RUN_P) == 1)&& (digitalRead(RUN_N) == 0))
    {
      stepRun(false, Y_DIR, Y_STP, 25);
      toa_do_Y -=25;
      Serial.println(toa_do_Y);
      delay(100);
    }
    if ((digitalRead(RUN_P) == 0)&& (digitalRead(RUN_N) == 0))
    break;
  }
}

// >>>>>>>> Ham dieu khien cum lay sach vao ra <<<<<<<<<
// Dừng động cơ
void FB_Dung()
{
  digitalWrite(DCMotorP, HIGH);
  digitalWrite(DCMotorN, HIGH);
}

// Tay gạtđẩy vào
void FB_DayVao()
{
  while (digitalRead(FB_LimitP) ==0)
    {
    digitalWrite(DCMotorP, HIGH);
    digitalWrite(DCMotorN, LOW);
    }
  FB_Dung();
}

// Tay gat day ra
void FB_DayRa()
{
  while (digitalRead(FB_LimitN) ==0)
    {
      digitalWrite(DCMotorP, LOW);
      digitalWrite(DCMotorN, HIGH);
    }
    FB_Dung();
}

// Tay gat quay xuong
void FB_QuayXuong()
{
  for (int j=90;j>=0 ; j--)
  {
    myservo2.write(j);
    delay(10);
  }
}

// Quay Lên
void FB_QuayLen()
{
  for (int j=0;j<=90 ; j++)
    {
      myservo2.write(j);
      delay(10);
    }
}

// Xuất sách ra
void FB_DayMangRa()
{
  for (int j=0;j<=140 ; j++)
    {
      myservo1.write(j);
      delay(10);
    }
}

// Thu sách về
void FB_ThuMangVe()
{
  for (int j=140;j>=0 ; j--)
    {
      myservo1.write(j);
      delay(10);
    }
}

// Về gốc toàn máy
void VeGocAll()
{
  myservo2.write(90);   // Quay tay gạt lên
  FB_DayRa();    // Đẩy tay gạt vào
  myservo1.write(0);
  veGoc(Y_DIR, Y_STP, Y_STOP);  // Hạ bộ gắp sách xuống
  veGoc(X_DIR, X_STP, X_STOP);  // Về gốc trục X
  step_X = 1;
  step_Y = 1;
  digitalWrite(EN, HIGH);
}

// Hàm lấy sách
void RUN_LaySach()
{
  lcd.setCursor(0,0)            ; // Chọn vị trí con trỏ (cột,hàng)
  lcd.print("Dang Thuc Hien: ") ; 
  lcd.setCursor(0,1)            ;
  lcd.print("Lay Sach O So   ") ;
  lcd.setCursor(15,1)           ;
  lcd.print(String(viTriSach)) ;
  // Dong co buoc ve goc
  digitalWrite(EN, LOW);
  veGoc(X_DIR, X_STP, X_STOP);
  veGoc(Y_DIR, Y_STP, Y_STOP);
  step_X = 1;
  step_Y = 1;
  delay(100);
  // Den vi tri lay sach
  stepRun(true, X_DIR, X_STP,X_Index[viTriSach]); // Chay den toa do X
  stepRun(true, Y_DIR, Y_STP,Y_Index[viTriSach]); // Nang len toa do Y
  step_X = 2;
  step_Y = 2;
  delay(100);
  FB_DayVao();    // Đẩy vào
  FB_QuayXuong(); // Quay Xuong
  FB_DayRa();     // Đi ra
  veGoc(X_DIR, X_STP, X_STOP);
  FB_QuayLen();  // Quay Lên
  FB_DayMangRa(); // Đẩy máng sách ra
  digitalWrite(EN, HIGH);
  delay(5000);   // Chờ 10 giây
  FB_ThuMangVe(); // Thu máng sách về
}

// Hàm trả sách
void RUN_TraSach()
{
  lcd.setCursor(0,0)            ; // Chọn vị trí con trỏ (cột,hàng)
  lcd.print("Dang Thuc Hien: ") ; 
  lcd.setCursor(0,1)            ;
  lcd.print("Tra Sach O So   ") ;
  lcd.setCursor(15,1)           ;
  lcd.print(String(viTriSach))  ;
  // Về gốc
  digitalWrite(EN, LOW);
  veGoc(X_DIR, X_STP, X_STOP);
  veGoc(Y_DIR, Y_STP, Y_STOP);
  step_X = 1;
  step_Y = 1;
  delay(500);
  FB_DayMangRa(); // Đẩy máng sách ra
  delay(5000);   // Chờ 10 giây
  FB_ThuMangVe(); // Đẩy máng sách vào
  stepRun(true, X_DIR, X_STP, X_Index[viTriSach]); //X, Counterclockwise
  stepRun(true, Y_DIR, Y_STP, Y_Index[viTriSach] + 70); //Y, Counterclockwise
  step_X = 2;
  step_Y = 2;
  delay(500);
  FB_QuayXuong(); // Quay Xuong
  FB_DayVao();    //Đẩy vào
  FB_Dung();      // Dừng động cơ
  FB_QuayLen(); // Quay Lên
  FB_DayRa();   // Đi ra
  FB_Dung();      // Dừng động cơ
  // Về gốc
  veGoc(X_DIR, X_STP, X_STOP);
  veGoc(Y_DIR, Y_STP, Y_STOP);
  digitalWrite(EN, HIGH);
  

}
// Hàm cho LCD
// Giao diện chọn giao dịch
void LCD_ChonGD()
{
  lcd.setCursor(0,0)            ; // Chọn vị trí con trỏ (cột,hàng)
  lcd.print("CHON GIAO DICH: ") ; 
  lcd.setCursor(0,1)            ;
  lcd.print("A: Muon | B: Tra") ;
}

// Giao diện nhập số
void LCD_NhapSo()
{
  lcd.setCursor(0,0)            ; // Chọn vị trí con trỏ (cột,hàng)
  lcd.print("Vi Tri Sach:    ") ; 
  lcd.setCursor(0,1)            ;
  lcd.print("*: Huy | #: OK  ") ;
  lcd.setCursor(13,0)           ; // Chọn vị trí con trỏ nhập số
  lcd.print(String(viTriSach));
  lcd.setCursor(13,0)           ; // Chọn vị trí con trỏ nhập số
  
  char customKey    ;  // Biến đọc giá trị nút ấn
  lcd.cursor()      ;  // Hiển thị con trỏ
  lcd.blink()       ;  // Nhap nhay con tro
  do
    {
      customKey = customKeypad.getKey();
      switch (customKey)
        {
          case '0' :  lcd.print('0'); viTriSach = 0;lcd.leftToRight();break;
          case '1' :  lcd.print('1'); viTriSach = 1;lcd.leftToRight();break;
          case '2' :  lcd.print('2'); viTriSach = 2;lcd.leftToRight();break;
          case '3' :  lcd.print('3'); viTriSach = 3;lcd.leftToRight();break;
          case '4' :  lcd.print('4'); viTriSach = 4;lcd.leftToRight();break;
          case '5' :  lcd.print('5'); viTriSach = 5;lcd.leftToRight();break;
          case '6' :  lcd.print('6'); viTriSach = 6;lcd.leftToRight();break;
          case '7' :  lcd.print('7'); viTriSach = 7;lcd.leftToRight();break;
          case '8' :  lcd.print('8'); viTriSach = 8;lcd.leftToRight();break;
          case '9' :  lcd.print('9'); viTriSach = 9;lcd.leftToRight();break;
          case '*' : 
          {
            HanhDong =0; 
            lcd.setCursor(0,1)            ;
            lcd.print("Huy Giao Dich!!!") ;
            break;
          }
          case '#' : 
          {
            lcd.setCursor(0,1)            ;
            lcd.print("Nhap Thanh Cong!") ;
            break;
          } 
        }
  
    }
  while ( (customKey != '*' ) && (customKey != '#' ))  ;  // thoat khi phim # được ấn
  lcd.noBlink()             ;  // Tắt nhấp nháy con trỏ
  lcd.noCursor()            ;  // Ẩn con trỏ
  delay(1000);
 

}

void setup() {
  Serial.begin(9600);            // toc do cong noi tiep
  // Input cum ra vao lay sach
  pinMode(FB_LimitP,INPUT_PULLUP);
  pinMode(FB_LimitN,INPUT_PULLUP);
  pinMode(RUN_P,INPUT_PULLUP);
  pinMode(RUN_N,INPUT_PULLUP);

  // Output
  //pinMode(LED_BUILTIN, OUTPUT);  // Led bao chan D13
  pinMode(DCMotorP, OUTPUT);
  pinMode(DCMotorN, OUTPUT);

  // Servo
  myservo1.attach(12);  // Servo day sach vao ra
  myservo2.attach(13);  // Servo xoay tay gat sach
  myservo1.write(0);
  myservo2.write(90);

  // Dong co Step
  pinMode(X_DIR, OUTPUT);
  pinMode(X_STP, OUTPUT);

  pinMode(Y_DIR, OUTPUT);
  pinMode(Y_STP, OUTPUT);

  pinMode(X_STOP, INPUT_PULLUP);
  pinMode(Y_STOP, INPUT_PULLUP);
  pinMode(START, INPUT_PULLUP);
  pinMode(STOP, INPUT_PULLUP);

  pinMode(EN, OUTPUT);

  
  
  // Thời gian
  Wire.begin();
  // initialize the LCD
	lcd.begin();
	// Turn on the blacklight and print a message.
	lcd.backlight();
	// Hiển thị màn hình khởi động
	lcd.print("!!! WELLCOME !!!") ; // Nội dung dòng 1
  lcd.setCursor(0,1)            ; // Chọn dòng 2
  lcd.print("TU SACH TU DONG ") ; // Nội dung dòng 2
  VeGocAll();
  digitalWrite(EN, HIGH);
  delay(2000);
  LCD_ChonGD();
  delay(1000);

}

void loop()
{
  // Đọc phím được ấn
  char customKey = customKeypad.getKey();
  if (customKey)
  {
    Serial.println(customKey);
    // Muon Sach
    if(customKey == 'A')
    {
      lcd.setCursor(0,0)            ; // Chọn vị trí con trỏ (cột,hàng)
      lcd.print("Ban Chon :      ") ; 
      lcd.setCursor(0,1)            ;
      lcd.print("   A : Muon Sach") ;
      HanhDong = 1;
      delay(2000);
      LCD_NhapSo();
    }
    // Tra sach
    if(customKey == 'B')
    {
      lcd.setCursor(0,0)            ; // Chọn vị trí con trỏ (cột,hàng)
      lcd.print("Ban Chon :      ") ; 
      lcd.setCursor(0,1)            ;
      lcd.print("   B :  Tra Sach") ;
      HanhDong = 2;
      delay(2000);
      LCD_NhapSo();

    }
  }
  // DateTime now = myRTC.now();
    
  //   Serial.print(now.year(), DEC);
  //   Serial.print('/');
  //   Serial.print(now.month(), DEC);
  //   Serial.print('/');
  //   Serial.print(now.day(), DEC);
  //   Serial.print(' ');
  //   Serial.print(now.hour(), DEC);
  //   Serial.print(':');
  //   Serial.print(now.minute(), DEC);
  //   Serial.print(':');
  //   Serial.print(now.second(), DEC);
  //   Serial.println();
  //   delay(1000);

  //Đọc lệnh từ serial
  if (Serial.available() > 0)
  {      // Check for incomding data
    String dataIn = Serial.readString();
    if (dataIn.startsWith("L"))
    {
      String dataInS = dataIn.substring(1, dataIn.length());
      HanhDong = 1; // Lấy sách
      viTriSach = dataInS.toInt(); // Change servo speed (delay time)
      Serial.print("Lay Sach: ");
      Serial.println(viTriSach);
    }
    if (dataIn.startsWith("C"))
    {
      String dataInS = dataIn.substring(1, dataIn.length());
      HanhDong = 2; // Cất sách
      viTriSach = dataInS.toInt(); // Change servo speed (delay time)
      Serial.print("Cat Sach: ");
      Serial.println(viTriSach);
    }
    if (dataIn.startsWith("DOX"))
    {
      Serial.println("Dang Do Truc X");
      digitalWrite(EN, LOW);
      veGoc(X_DIR, X_STP, X_STOP);
      toa_do_X=0;
      do_toa_do_X();
      digitalWrite(EN, HIGH);
    }
    if (dataIn.startsWith("DOY"))
    {
      Serial.println("Dang Do Truc Y");
      digitalWrite(EN, LOW);
      veGoc(Y_DIR, Y_STP, Y_STOP);
      toa_do_Y=0;
      do_toa_do_Y();
      digitalWrite(EN, HIGH);
    }
    // Đặt thời gian cho đồng hồ
    if (dataIn.startsWith("STIME"))
    {
      String Stime = dataIn.substring(5, dataIn.length());
      Serial.println(Stime);
      setTime(Stime);
    }
    // Về gốc All
    if (dataIn.startsWith("GOCA"))
    {
      Serial.println("Ve goc toan may");
      VeGocAll();
    }

    // Đọc các tham số
     // Đặt thời gian cho đồng hồ
    if (dataIn.startsWith("HD"))
    {
      Serial.print("Hanh Dong : ");
      Serial.println(HanhDong);
    }
    if (dataIn.startsWith("VTS"))
    {
      Serial.print("Vi Tri Sach : ");
      Serial.println(viTriSach);
    }
    
  }

  // >>>>>>>>>>>>>>>  Lấy sách  <<<<<<<<<<<<<<<
  if ((digitalRead(RUN_P) == 0)&& (digitalRead(RUN_N) == 1) && (HanhDong == 1 ) )
  {
    RUN_LaySach();
    HanhDong = 0; // Thuc hien xong giao dich
    LCD_ChonGD();
  }

// >>>>>>>>>>>>>>>  Trả sách  <<<<<<<<<<<<<<<
  if ((digitalRead(RUN_P) == 1)&& (digitalRead(RUN_N) == 0) && (HanhDong == 2 ))
  {
    RUN_TraSach();
    HanhDong = 0; // Thuc hien xong giao dich
    LCD_ChonGD();
  }


}
