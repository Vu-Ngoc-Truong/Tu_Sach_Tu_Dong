
// Chương trình viết cho điều khiển 2 động cơ


#include <EEPROM.h>              // Thư viện EEPROM để lưu giá trị cho lần sau
#include "TS_IO_define.h"          // Thư viện định nghĩa các chân IO của máy
#include "TS_DongCo.h"         // Thư viện điều khiển các động cơ của máy
#include "TS_CamBien.h"
// --------------------------
// >>> Khai báo tham số máy <<<<<<
int Status = 0 ;
String QRcodeSach ; // Chuỗi ký tự QR code của sách



// Về gốc toàn máy
void VeGocAll()
{
  myservo2.write(Servo2_Pos0);   // Quay tay gạt lên
  FB_DayRa();    // Đẩy tay gạt vào
  myservo1.write(Servo1_Pos0);    // Thu máng sách vào
  veGoc(Y_DIR, Y_STP, Y_STOP,TIME_SPEED_HIGH);  // Hạ bộ gắp sách xuống
  veGoc(X_DIR, X_STP, X_STOP,TIME_SPEED_MID);  // Về gốc trục X
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
  veGoc(Y_DIR, Y_STP, Y_STOP,TIME_SPEED_HIGH);
  veGoc(X_DIR, X_STP, X_STOP,TIME_SPEED_LOW);
  step_X = 1;
  step_Y = 1;
  delay(100);
  // Den vi tri lay sach
  stepRun(true, X_DIR, X_STP,X_Index[viTriSach],TIME_SPEED_MID); // Chay den toa do X
  stepRun(true, Y_DIR, Y_STP,Y_Index[viTriSach],TIME_SPEED_HIGH); // Nang len toa do Y
  step_X = 2;
  step_Y = 2;
  delay(50);
  FB_DayVao();    // Đẩy vào
  // Lùi ra 1 chút
  FB_DayRa_NoLimit();
  delay(80);
  FB_Dung();
  FB_QuayXuong(); // Quay Xuong
  FB_DayRa();     // Đi ra
  veGoc(Y_DIR, Y_STP, Y_STOP,TIME_SPEED_HIGH);
  veGoc(X_DIR, X_STP, X_STOP,TIME_SPEED_LOW);
  FB_QuayLen();  // Quay Lên
  QRcodeSach =  QR_Read_CMD();
  lcd.clear();
  lcd.setCursor(0,0)            ; // Chọn vị trí con trỏ (cột,hàng)
  lcd.print("Ban muon sach : ") ; 
  lcd.setCursor(0,1)            ;
  lcd.print(QRcodeSach) ;
  FB_DayMangRa(); // Đẩy máng sách ra
  digitalWrite(EN, HIGH);
  // CHờ sách được lấy 
  previousMillis = millis();
  while ((digitalRead(MangSach) == SS_CoSach) && ((millis() - previousMillis) < TIMEOUT_SS_PHAT_HIEN_SACH) ) ;
  delay(2000);
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
  veGoc(X_DIR, X_STP, X_STOP,TIME_SPEED_MID);
  veGoc(Y_DIR, Y_STP, Y_STOP,TIME_SPEED_HIGH);
  step_X = 1;
  step_Y = 1;
  delay(500);
  FB_DayMangRa(); // Đẩy máng sách ra
  // CHờ sách được đưa vào
  Serial.print("Mang Sach:");
  Serial.println(digitalRead(MangSach));
  previousMillis = millis();
  while ((digitalRead(MangSach) == SS_KhongSach) && ((millis() - previousMillis) < TIMEOUT_SS_PHAT_HIEN_SACH) ) ;
   Serial.println(digitalRead(MangSach));
  if (digitalRead(MangSach) == SS_CoSach)
  // Nếu có sách trong máng
  {
    Serial.print("Co sach trong mang");
    delay(3000);    // Chờ ổn định
    FB_ThuMangVe(); // Đẩy máng sách vào
    FB_QuayXuong(); // Quay Xuong
    // Đến vị trí trả sách
    stepRun(true, X_DIR, X_STP, X_Index[viTriSach], TIME_SPEED_MID);               // Chạy đến tọa độ X
    stepRun(true, Y_DIR, Y_STP, Y_Index[viTriSach] + Y_tra_sach, TIME_SPEED_HIGH); // Nâng lên tọa độ Y
    step_X = 2;
    step_Y = 2;
    delay(100);
    FB_DayVao();       //Đẩy vào
    FB_DayVao_NoLimit; // Đẩy thêm cho hết cữ
    delay(50);
    FB_Dung();    // Dừng động cơ
    FB_QuayLen(); // Quay Lên
    FB_DayRa();   // Đi ra
    // Về gốc
    veGoc(Y_DIR, Y_STP, Y_STOP, TIME_SPEED_HIGH);
    veGoc(X_DIR, X_STP, X_STOP, TIME_SPEED_MID);
  }
  else
  // Nếu không có sách trong máng
  {
    Serial.print("Khong co sach trong mang :(");
    FB_ThuMangVe(); // Đẩy máng sách vào
    //VeGocAll();
  }

  digitalWrite(EN, HIGH);
  
}


void setup() {
  Serial.begin(9600);            // toc do cong noi tiep với máy tính
  finger.begin(57600);          // toc do cong noi tiep với cảm biến siêu âm
  QRcodeSerial.begin(9600);          // toc do cong noi tiep với đầu đọc QRcode
  Wire.begin();              
  // Input cum ra vao lay sach
  pinMode(FB_LimitP,INPUT_PULLUP);
  pinMode(FB_LimitN,INPUT_PULLUP);
  pinMode(RUN_P,INPUT_PULLUP);
  pinMode(RUN_N,INPUT_PULLUP);
  pinMode(MangSach,INPUT_PULLUP);

  // Output
  //pinMode(LED_BUILTIN, OUTPUT);  // Led bao chan D13
  pinMode(DCMotorP, OUTPUT);
  pinMode(DCMotorN, OUTPUT);

  // Servo
  myservo1.attach(12);  // Servo day sach vao ra
  myservo2.attach(13);  // Servo xoay tay gat sach
  myservo1.write(Servo1_Pos0);
  myservo2.write(Servo2_Pos0);

  // Dong co Step
  pinMode(X_DIR, OUTPUT);
  pinMode(X_STP, OUTPUT);

  pinMode(Y_DIR, OUTPUT);
  pinMode(Y_STP, OUTPUT);

  pinMode(X_STOP, INPUT_PULLUP);
  pinMode(Y_STOP, INPUT_PULLUP);
  pinMode(START, INPUT_PULLUP);
  pinMode(STOP, INPUT_PULLUP);

  // Đọc thời gian hiện tại
  readTime();

  pinMode(EN, OUTPUT);
  // Nháy đèn cảm biến vân tay
  VT_LED_ON();
  delay(100);
  VT_LED_OFF();
  VT_Info();
  
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
      veGoc(X_DIR, X_STP, X_STOP,TIME_SPEED_LOW);
      toa_do_X=0;
      do_toa_do_X();
      digitalWrite(EN, HIGH);
    }
    // Lệnh chạy đến vị trí của trục X
    if (dataIn.startsWith("XMOVE"))
    {
      int viTriDiChuyen;
      String dataInS = dataIn.substring(5, dataIn.length());
      viTriDiChuyen = dataInS.toInt(); // Lấy vị trí cần đến
      Serial.print("X den diem : ");
      Serial.println(viTriDiChuyen);
      chayDiemX(viTriDiChuyen);
    }
    // Dò trục Y
    if (dataIn.startsWith("DOY"))
    {
      Serial.println("Dang Do Truc Y");
      digitalWrite(EN, LOW);
      veGoc(Y_DIR, Y_STP, Y_STOP,TIME_SPEED_HIGH);
      toa_do_Y=0;
      do_toa_do_Y();
      digitalWrite(EN, HIGH);
    }
    // Lệnh chạy đến vị trí của trục Y
    if (dataIn.startsWith("YMOVE"))
    {
      int viTriDiChuyen;
      String dataInS = dataIn.substring(5, dataIn.length());
      viTriDiChuyen = dataInS.toInt(); // Lấy vị trí cần đến
      Serial.print("Y den diem : ");
      Serial.println(viTriDiChuyen);
      chayDiemY(viTriDiChuyen);
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
    
    // Đọc QRcode
    // Lệnh chạy đến vị trí của trục Y
    if (dataIn.startsWith("QRC"))
    {
      QR_Read_CMD();
    }

  }

  // >>>>>>>>>>>>>>>  Lấy sách  <<<<<<<<<<<<<<<
  if ( (HanhDong == 1 ) ) //(digitalRead(RUN_P) == 0)&& (digitalRead(RUN_N) == 1) &&
  {
    RUN_LaySach();
    HanhDong = 0; // Thuc hien xong giao dich
    LCD_ChonGD();
  }

// >>>>>>>>>>>>>>>  Trả sách  <<<<<<<<<<<<<<<
  if ((HanhDong == 2 )) //(digitalRead(RUN_P) == 1)&& (digitalRead(RUN_N) == 0) && 
  {
    RUN_TraSach();
    HanhDong = 0; // Thuc hien xong giao dich
    LCD_ChonGD();
  }


}
