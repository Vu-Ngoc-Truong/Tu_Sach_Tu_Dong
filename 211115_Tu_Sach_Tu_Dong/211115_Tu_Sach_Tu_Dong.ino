
// Chương trình viết cho điều khiển 2 động cơ


#include <EEPROM.h>              // Thư viện EEPROM để lưu giá trị cho lần sau
#include "TS_IO_define.h"          // Thư viện định nghĩa các chân IO của máy
#include "TS_DongCo.h"         // Thư viện điều khiển các động cơ của máy
#include "TS_CamBien.h"
// --------------------------
// >>> Khai báo tham số máy <<<<<<
int Status = 0 ;
String QRcodeSach ; // Chuỗi ký tự QR code của sách
String MaSach;      // Mã số của sách 001 --> 255
String TenGoiNhoSach; // Tên gợi nhớ của sách (bé hơn 16 ký tự)
bool coPhimAn ;   // Báo có phím ấn được ấn

// Biến liên quan đến vân tay
bool ID_OK = false;  // True Nếu ID hợp lệ
int VTmaID; // ID cua người đang thực hiện

// Biến liên quan đến  EEPROM
bool eeFull; // Báo bộ nhớ đã được sử dụng hết hay chưa
int eeAddrSave; // Con trỏ địa chỉ lưu giao dịch trong EEPROM
int addrEEPROM; // địa chỉ truy xuất EEPROM
int eeSoGiaoDichOK ; // Số giao dịch thành công
int eeDiaChiLuuTiepTheo; // Địa chỉ lưu cho giao dịch tiếp theo



// Về gốc toàn máy
void VeGocAll()
{
  myservo2.write(Servo2_Pos0);   // Quay tay gạt lên
  FB_DayRa();    // Đẩy tay gạt vào
  myservo1.write(Servo1_Pos0);    // Thu máng sách vào
  veGoc(Y_DIR, Y_STP, Y_STOP,TIME_SPEED_HIGH);  // Hạ bộ gắp sách xuống
  veGoc(X_DIR, X_STP, X_STOP,TIME_SPEED_LOW);  // Về gốc trục X
  step_X = 1;
  step_Y = 1;
  digitalWrite(EN, HIGH);

}

// Hàm lấy sách
void RUN_LaySach()
{
  LCD_DangLaySach();
  // Dong co buoc ve goc
  digitalWrite(EN, LOW);
  veGoc(Y_DIR, Y_STP, Y_STOP, TIME_SPEED_HIGH);
  veGoc(X_DIR, X_STP, X_STOP, TIME_SPEED_LOW);
  step_X = 1;
  step_Y = 1;
  delay(100);
  // Kiểm tra trong máng sách có còn sách không
  // Nếu có thì đẩy máng sách ra để lấy sách ra
  if ((digitalRead(MangSach) == SS_CoSach))
  {
    LCD_CanhBao("Mang Co Sach !!!");
    FB_DayMangRa();
    LCD_CanhBao("Lay Sach Ra !!! ");
    delay(5000); //  CHờ lấy sách ra
    FB_ThuMangVe();
    if ((digitalRead(MangSach) == SS_CoSach))
    {
      LCD_CanhBao("Ket Thuc GD !!!!");
      delay(2000);
      LCD_ChonGD();
    }
  }
  if ((digitalRead(MangSach) == SS_KhongSach))
  {
    LCD_DangLaySach();
    // Den vi tri lay sach
    if (viTriSach == 5)
    {
      stepRun(true, X_DIR, X_STP, X_Index[1], TIME_SPEED_LOW);          // Tránh đi lên thẳng vướng vào tấm mica
      stepRun(true, Y_DIR, Y_STP, Y_Index[viTriSach], TIME_SPEED_HIGH); // Nâng lên tọa độ Y
      veGoc(X_DIR, X_STP, X_STOP, TIME_SPEED_LOW);                      //  về lại gốc cho X
      stepRun(true, X_DIR, X_STP, X_Index[viTriSach], TIME_SPEED_LOW) ;  // Chạy đến tọa độ X
    }
    else
    {
      stepRun(true, X_DIR, X_STP, X_Index[viTriSach], TIME_SPEED_LOW) ;  // Chay den toa do X
      stepRun(true, Y_DIR, Y_STP, Y_Index[viTriSach], TIME_SPEED_HIGH); // Nang len toa do Y
    }
    step_X = 2;
    step_Y = 2;
    delay(50);
    FB_DayVao(); // Đẩy vào
    // Lùi ra 1 chút
    FB_DayRa_NoLimit();
    delay(50);
    FB_Dung();
    FB_QuayXuong(); // Quay Xuong
    FB_DayRa();     // Đi ra
    if ( viTriSach == 5 )
    {
      stepRun(true, X_DIR, X_STP, X_Index[1], TIME_SPEED_LOW);  // Tránh đi lên thẳng vướng vào tấm mica
    }
    veGoc(Y_DIR, Y_STP, Y_STOP, TIME_SPEED_HIGH);
    QR_Read_CMD(); // Đọc mã vạch
    veGoc(X_DIR, X_STP, X_STOP, TIME_SPEED_LOW);
    FB_QuayLen();                                              // Quay Lên
    delay(1000); // Chờ đọc QR code ổn định
    QRcodeSach = QR_Read_Value();
    if ( QRcodeSach == "Error")
    {
      MaSach = "000";
      TenGoiNhoSach = "Khong Doc Duoc!!";
    }
    else
    {
      MaSach = QRcodeSach.substring(0, 3);
      TenGoiNhoSach = QRcodeSach.substring(4, QRcodeSach.length()-1);
    }
    // Hiển thị ra LCD
    lcd.clear();
    LCD_GhiChuoi(0,0,"Ma Sach Muon:   ");
    LCD_GhiChuoi(13,0,MaSach);
    LCD_GhiChuoi(0,1,TenGoiNhoSach);
    // Xử lý theo sách có trong máng hay không
    if ((digitalRead(MangSach) == SS_KhongSach ) && (MaSach == "000"))
    // Không có sách trong máng và QR code không đọc được
    {
      digitalWrite(EN, HIGH);
      LCD_CanhBao("Lay Sach Loi !!!");
    }
    else
    // Có sách trong máng
    {
      FB_DayMangRa(); // Đẩy máng sách ra
      digitalWrite(EN, HIGH);
      // CHờ sách được lấy
      previousMillis = millis();
      while ((digitalRead(MangSach) == SS_CoSach) && ((millis() - previousMillis) < TIMEOUT_SS_PHAT_HIEN_SACH));
      delay(2000);
      FB_ThuMangVe(); // Thu máng sách về
      luuDuLieuGD();
    }

  }
}

// Hàm trả sách
void RUN_TraSach()
{
  LCD_DangTraSach();
  // Về gốc
  digitalWrite(EN, LOW);
  veGoc(Y_DIR, Y_STP, Y_STOP,TIME_SPEED_HIGH);
  veGoc(X_DIR, X_STP, X_STOP,TIME_SPEED_LOW);
  step_X = 1;
  step_Y = 1;
  delay(100);
  FB_DayMangRa(); // Đẩy máng sách ra
  delay(2000);
  // CHờ sách được đưa vào
  Serial.print("Mang Sach:");
  Serial.println(digitalRead(MangSach));
  previousMillis = millis();
  while ((digitalRead(MangSach) == SS_KhongSach) && ((millis() - previousMillis) < TIMEOUT_SS_PHAT_HIEN_SACH) ) ;
  delay(200); // Xác nhận có sách ổn định, chống xung nhiễu
  while ((digitalRead(MangSach) == SS_KhongSach) && ((millis() - previousMillis) < TIMEOUT_SS_PHAT_HIEN_SACH) ) ;
  Serial.println(digitalRead(MangSach));
  if (digitalRead(MangSach) == SS_CoSach)
  // Nếu có sách trong máng
  {
    Serial.print("Co sach trong mang");
    delay(3000);    // Chờ ổn định
    FB_ThuMangVe(); // Đẩy máng sách vào
    QR_Read_CMD(); // Đọc mã vạch
    delay(2000); // Chờ đọc mã vach
    QRcodeSach = QR_Read_Value();
    if ( QRcodeSach == "Error")
    {
      MaSach = "000";
      TenGoiNhoSach = "Khong Doc Duoc!!";
    }
    else
    {
      MaSach = QRcodeSach.substring(0, 3);
      TenGoiNhoSach = QRcodeSach.substring(4, QRcodeSach.length()-1);
    }
    lcd.clear();
    LCD_GhiChuoi(0,0,"Ma Sach Tra:   ");
    LCD_GhiChuoi(13,0,MaSach);
    LCD_GhiChuoi(0,1,TenGoiNhoSach);
    delay(2000);
    LCD_DangTraSach();
    FB_QuayXuong(); // Quay Xuong
    // Đến vị trí trả sách
    veGoc(X_DIR, X_STP, X_STOP,TIME_SPEED_LOW); //  về lại gốc
    if ( viTriSach == 5 )
    {
      stepRun(true, X_DIR, X_STP, X_Index[1], TIME_SPEED_LOW);  // Tránh đi lên thẳng vướng vào tấm mica
      stepRun(true, Y_DIR, Y_STP, Y_Index[viTriSach] + Y_tra_sach, TIME_SPEED_HIGH); // Nâng lên tọa độ Y
      veGoc(X_DIR, X_STP, X_STOP,TIME_SPEED_LOW); //  về lại gốc cho X
      stepRun(true, X_DIR, X_STP, X_Index[viTriSach], TIME_SPEED_LOW);               // Chạy đến tọa độ X
    }
    else
    {

    stepRun(true, X_DIR, X_STP, X_Index[viTriSach], TIME_SPEED_LOW);               // Chạy đến tọa độ X
    stepRun(true, Y_DIR, Y_STP, Y_Index[viTriSach] + Y_tra_sach, TIME_SPEED_HIGH); // Nâng lên tọa độ Y
    }
    step_X = 2;
    step_Y = 2;
    delay(100);
    FB_DayVao();       //Đẩy vào
    FB_DayVao_NoLimit; // Đẩy thêm cho hết cữ
    delay(50);
    FB_Dung();    // Dừng động cơ
    FB_QuayLen(); // Quay Lên
    FB_DayRa();   // Đi ra
    luuDuLieuGD(); // Lưu dữ liệu vào EEPROM
    // Về gốc
    if ( viTriSach == 5 )
    {
      stepRun(true, X_DIR, X_STP, X_Index[1], TIME_SPEED_LOW);  // Tránh đi xuống thẳng vướng vào tấm mica
    }

    veGoc(Y_DIR, Y_STP, Y_STOP, TIME_SPEED_HIGH);
    veGoc(X_DIR, X_STP, X_STOP, TIME_SPEED_LOW);
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

// Lưu dữ liệu giao dịch vào EEPROM nếu giao dịch thành công
void luuDuLieuGD()
{
  // lấy các dữ liệu
  byte dataSave[10];
  dataSave[0] = byte(VTmaID);
  dataSave[1] = byte(MaSach.toInt());
  dataSave[2] = byte(HanhDong);
  dataSave[3] = byte(Year);
  dataSave[4] = byte(Month);
  dataSave[5] = byte(Day);
  dataSave[6] = byte(Hour);
  dataSave[7] = byte(Minute);
  dataSave[8] = byte(Second);
  dataSave[9] = 0 ;

  // ghi vào eeprom 10 byte
  EEPROM.get(ADDR_EEPROM_ADDR_SAVE,eeAddrSave)   ; // Lấy vị trí lưu trong EEPROM
  EEPROM.put(eeAddrSave, dataSave); // luu mang gia tri vao eeprom
  eeAddrSave +=10;  // tăng giá trị đến địa chỉ lưu tiếp theo
  // Khi bộ nhớ đầy thì lưu lại ở vị trí đầu
  if ( eeAddrSave >= ADDR_EEPROM_END_SAVE )
  {
    eeAddrSave = ADDR_EEPROM_START_SAVE ; // Quay lại vị trí lưu đầu tiên
  }

  EEPROM.put(ADDR_EEPROM_ADDR_SAVE,eeAddrSave)   ; // Lưu lại vị trí con trỏ trong EEPROM

}

// Đọc dữ liệu giao dịch từ EEPROM
void docDulieuGD(int soGD)
{
  // Tìm trong EEPROM theo số lượng giao dịch
  int eeAddrSaveTemp;
  byte dataArr[10];
  bool addrOver = false;
  EEPROM.get(ADDR_EEPROM_ADDR_SAVE,eeAddrSave)   ; // Lấy vị trí lưu hiện tại trong EEPROM
  eeAddrSaveTemp = eeAddrSave;
  if ( eeAddrSave == ADDR_EEPROM_START_SAVE)
  {
    eeAddrSave = ADDR_EEPROM_END_SAVE - 10;
  }
  else
  {
    eeAddrSave -= 10;
  }
  // In header
  Serial.print("Stt\t");
  Serial.print("ID\t");
  Serial.print("Ma Sach\t");
  Serial.print("Loai_GD\t");
  Serial.print("Nam\t");
  Serial.print("Thang\t");
  Serial.print("Ngay\t");
  Serial.print("Gio\t");
  Serial.print("Phut\t");
  Serial.println("Giay\t");
  // In data giao dịch
  for (int i = 0; i < soGD; i++)
  {
    // Lưu dữ liệu vòng tròn : start -->  end -- > start
    // đọc data từ giao dịch gần nhất nên bộ nhớ sẽ đọc từ địa chỉ cao xuống thấp
    if ( eeAddrSave < ADDR_EEPROM_START_SAVE)
    {
      eeAddrSave = ADDR_EEPROM_END_SAVE - 10;
      addrOver = true;
    }
    // Đọc hết 1 vòng bộ nhớ thì thoát
    if ( ( eeAddrSave < eeAddrSaveTemp ) && addrOver )
    {
      Serial.println("Đã đọc hết bộ nhớ !!!");
      break;

    }
    // Đọc giao dịch
    EEPROM.get(eeAddrSave,dataArr);
    // In các giao dịch ra màn hình
    Serial.print(i+1); // in số thứ tự
    Serial.print("\t");
    for (int j = 0; j < 9; j++)
    {
      Serial.print(dataArr[j]);
      Serial.print("\t");
    }
    Serial.println(); // Hết 1 dòng giao dịch
    eeAddrSave -= 10; // Chuyển đến địa chỉ lưu giao dịch trước đó

  }


}

// Hàm đọc phím ấn chọn GD
void DocPhimAn()
{
    previousMillis = millis();
    while ((millis() - previousMillis) < TIMEOUT_CHO_NHAP_PHIM)
    {
      // Đọc phím được ấn
      char customKey = customKeypad.getKey();
      if (customKey)
      {
        Serial.println(customKey);
        // Muon Sach
        if (customKey == 'A')
        {
          LCD_MuonSach();
          HanhDong = 1;
          delay(2000);
          LCD_NhapSo();
          break;
        }
        // Tra sach
        if (customKey == 'B')
        {
          LCD_TraSach();
          HanhDong = 2;
          delay(2000);
          LCD_NhapSo();
          break;
        }
      }
    }
}

void setup() {
  Serial.begin(9600);            // toc do cong noi tiep với máy tính
  finger.begin(57600);          // toc do cong noi tiep với cảm biến vân tay
  QRcodeSerial.begin(9600);     // toc do cong noi tiep với đầu đọc QRcode
  Wire.begin();
  delay(50);          // Chờ thiết lập

  // Input cum ra vao lay sach
  pinMode(FB_LimitP,INPUT_PULLUP);
  pinMode(FB_LimitN,INPUT_PULLUP);
  pinMode(RUN_P,INPUT_PULLUP);
  pinMode(RUN_N,INPUT_PULLUP);
  pinMode(MangSach,INPUT_PULLUP);
  pinMode(X_STOP, INPUT_PULLUP);
  pinMode(Y_STOP, INPUT_PULLUP);
  pinMode(VT_Touch, INPUT_PULLUP);

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
  pinMode(EN, OUTPUT);

  // Setup trạng thái các chân ban đầu
  digitalWrite(EN, HIGH);   // Dừng các động cơ bước
  // Thời gian
  Wire.begin();
  // initialize the LCD
	lcd.begin();

  readTime();   // Đọc thời gian hiện tại
  VT_Info(); // Đọc thông tin cảm biến vân tay
  VT_LED_ON();  // Nháy đèn cảm biến vân tay
  delay(100);
  VT_LED_OFF();

  // Lấy các tham số lưu ở EEPROM
  EEPROM.get(ADDR_EEPROM_ADDR_SAVE,eeAddrSave)   ;
  // Ghi giá trị vào EEPROM cho lần đầu tiên
  if ( eeAddrSave == 0 )
  {
    EEPROM.put(ADDR_EEPROM_ADDR_SAVE,ADDR_EEPROM_START_SAVE);
    eeAddrSave = ADDR_EEPROM_START_SAVE;
    Serial.println("Ghi gia tri ban dau vao EEPROM !");
  }
	// Hiển thị màn hình khởi động
	lcd.backlight(); // Turn on the blacklight and print a message.
	lcd.print("!!! WELLCOME !!!") ; // Nội dung dòng 1
  lcd.setCursor(0,1)            ; // Chọn dòng 2
  lcd.print("TU SACH TU DONG ") ; // Nội dung dòng 2
  VeGocAll();
  digitalWrite(EN, HIGH);
  delay(2000);
}

void loop()
{

  //VT_Lay_ID() :
  if (digitalRead(VT_Touch) == 1) // Có người chạm
  {
    VT_LED_ON(); // Sáng cảm biến vân tay
    VTmaID = VT_Lay_ID();
    Serial.println(VTmaID);
    delay(100);
    VT_LED_OFF();
    if (VTmaID == -1)
    {
      Serial.println("Khong thay van tay!");
      LCD_GhiChuoi(0, 1, "VT khong hop le!");
      delay(2000);
    }
    if ((VTmaID > 0) && (VTmaID < 128))
    {
      LCD_GhiChuoi(0, 1, "Van tay ID :" + String(VTmaID) + "   ");
      delay(2000);
      LCD_ChonGD(); // lên màn hình chọn giao dich
      // CHờ sách được lấy
      DocPhimAn();
    }

    // VT_LED_ON();
    // delay(100);
    // VT_LED_OFF();
  }
  else // Khi không có ai chạm
  {
    LCD_ChoGD(readTime());
    delay(1000);
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

    // Máng sách đẩy ra
    if (dataIn.startsWith("MANGRA"))
    {
      FB_DayMangRa();
    }

     // Máng sách thu vào
    if (dataIn.startsWith("MANGVAO"))
    {
      FB_ThuMangVe();
    }

    // Tay gạt đẩy vào
    if (dataIn.startsWith("GATVAO"))
    {
      FB_DayVao();
    }

     // Tay gạt đẩy vào
    if (dataIn.startsWith("GATRA"))
    {
      FB_DayRa();
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
    if (dataIn.startsWith("QRC"))
    {
      QR_Read_CMD();
      delay(2000); //  Chờ đọc QR
      QR_Read_Value();
    }
    // Lệnh đọc ID vân tay
    if (dataIn.startsWith("VTDOC"))
    {
      VT_Lay_ID_CT();
    }

    if (dataIn.startsWith("VTDOC1"))
    {
      VT_Lay_ID();
    }
    // Lệnh đăng ký cho vân tay
    if (dataIn.startsWith("VTDK"))
    {
      VT_DangKyID();
      delay(5000);
    }

    // Lệnh tắt LED
    if (dataIn.startsWith("VTLEDOFF"))
    {
      VT_LED_OFF();
      delay(500);
    }

    // Lệnh với EEPROM
    // Lệnh đọc giá trị int
    if (dataIn.startsWith("EEGET"))
    {
      int _eeAddr,_eeData;
      String dataInS = dataIn.substring(5, dataIn.length());
      _eeAddr = dataInS.toInt(); // Lấy địa chỉ EEPROM
      EEPROM.get(_eeAddr,_eeData)   ; // Lấy dữ liệu
      Serial.print("EEPROM Read Data: ");
      Serial.println(_eeData);
    }

    // Lệnh ghi giá trị int
    if (dataIn.startsWith("EEPUT"))
    {
      int _eeAddr,_eeData;
      String dataInS = dataIn.substring(5, 8);
      String dataInD = dataIn.substring(10, dataIn.length());
      _eeAddr = dataInS.toInt(); // Lấy địa chỉ EEPROM
      _eeData = dataInD.toInt(); // Lấy giá trị ghi vào
      EEPROM.put(_eeAddr,_eeData)   ; // Lấy dữ liệu
      Serial.print("EEPROM Write Data: ");
      Serial.println(_eeData);
    }

    // Lệnh đọc dữ liệu giao dịch
    if (dataIn.startsWith("GD"))
    {
      int slGD;
      String dataInS = dataIn.substring(2, dataIn.length());
      slGD = dataInS.toInt(); // Lấy địa chỉ EEPROM
      Serial.println("Doc du lieu GD: ");
      docDulieuGD(slGD) ; // Lấy dữ liệu
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
