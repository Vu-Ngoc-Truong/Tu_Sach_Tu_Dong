// >>>>>>>>>> Thư viện quản lý các cảm biến, LCD, Keypad của máy
#ifndef TSTD_CAMBIEN_H
#define TSTD_CAMBIEN_H

#include "TS_IO_define.h"
#include <Wire.h> 
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>      // Thư viện cho LCD
#include <Adafruit_Fingerprint.h>   // Thư viện cho cảm biến vân tay
#include <Keypad.h>                 // Thư viên cho Keypad

/**************************************************************************/
//Khởi tạo class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS) ;

// --------------------------
// Khởi tạo cho LCD
// Set the LCD address to 0x27 for a 16 chars and 2 line display
// LCD pin : GND , VCC, SDA ,SCL
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 la dia chi I2C cua LCD

// --------------------------
// Khởi tạo cảm biến vân tay
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FingerSerial);
uint8_t id;

// --------------------------
// Khởi tạo đồng hồ thời gian thực
RTClib myRTC;
DS3231 Clock;

/**************************************************************************/
// >>>>>>>>>>> Khai báo các hàm

// Hàm cho RTC
String readTime();
void setTime(String stime);

// Hàm cho LCD
void LCD_GhiChuoi(int Cot, int Hang, String CacKyTu);
void LCD_ChonGD();
void LCD_NhapSo();

// Hàm cho cảm biến vân tay
void VT_Info() ;
int VT_Lay_ID();
uint8_t VT_Lay_ID_CT();
void VT_LED_OFF();
void VT_LED_ON();

/**************************************************************************/
// Hàm đặt thời gian cho đồng hồ
// Biến thời gian
byte Year;
byte Month;
byte Day;
//byte DoW;
byte Hour;
byte Minute;
byte Second;
void setTime(String stime)
{
    byte Year;
    byte Month;
    byte Date;
    //byte DoW;
    byte Hour;
    byte Minute;
    byte Second;

    String dataS = stime.substring(0, 2);
    Year = dataS.toInt();
    Serial.print("Year :");
    Serial.println(Year);

    dataS = stime.substring(2, 4);
    Month = dataS.toInt();
    Serial.print("Month :");
    Serial.println(Month);

    dataS = stime.substring(4, 6);
    Date = dataS.toInt();
    Serial.print("Date :");
    Serial.println(Date);

    dataS = stime.substring(6, 8);
    Hour = dataS.toInt();
    Serial.print("Hour :");
    Serial.println(Hour);

    dataS = stime.substring(8, 10);
    Minute = dataS.toInt();
    Serial.print("Min :");
    Serial.println(Minute);

    dataS = stime.substring(10, 12);
    Second = dataS.toInt();
    Serial.print("Sec :");
    Serial.println(Second);

    Clock.setClockMode(false);  // set to 24h
    //setClockMode(true);	    // set to 12h

    Clock.setYear(Year);
    Clock.setMonth(Month);
    Clock.setDate(Date);
    //Clock.setDoW(DoW);
    Clock.setHour(Hour);
    Clock.setMinute(Minute);
    Clock.setSecond(Second);
}

String readTime()
{
    DateTime now = myRTC.now();
    
    String timeNow;
    Year = now.year();
    Month = now.month();
    Day = now.day();
    Hour = now.hour();
    Minute = now.minute();
    Second = now.second();
    
    timeNow = String(Day) + "/" + String(Month) + "   " + String(Hour)+":"+ String(Minute)+ ":" + String(Second) ;
    Serial.println(timeNow);
    // Serial.print(now.year(), DEC);
    // Serial.print('/');
    // Serial.print(now.month(), DEC);
    // Serial.print('/');
    // Serial.print(now.day(), DEC);
    // Serial.print(' ');
    // Serial.print(now.hour(), DEC);
    // Serial.print(':');
    // Serial.print(now.minute(), DEC);
    // Serial.print(':');
    // Serial.print(now.second(), DEC);
    // Serial.println();
    return timeNow;

}

/**************************************************************************/
// >>>>>>> Hàm cho LCD  <<<<<<<<
// Ghi một chuỗi ra màn hình
void LCD_GhiChuoi(int Cot, int Hang, String CacKyTu)
{
    lcd.setCursor(Cot, Hang); // Chọn vị trí con trỏ (cột,hàng)
    lcd.print(CacKyTu);
}

// Giao diện chọn giao dịch
void LCD_ChonGD()
{
    lcd.setCursor(0, 0); // Chọn vị trí con trỏ (cột,hàng)
    lcd.print("CHON GIAO DICH: ");
    lcd.setCursor(0, 1);
    lcd.print("A: Muon | B: Tra");
}

// LCD mượn sách
void LCD_MuonSach()
{
    lcd.setCursor(0,0)            ; // Chọn vị trí con trỏ (cột,hàng)
    lcd.print("Ban Chon :      ") ; 
    lcd.setCursor(0,1)            ;
    lcd.print("   A : Muon Sach") ;
}

// LCD trả sách
void LCD_TraSach()
{
    lcd.setCursor(0, 0); // Chọn vị trí con trỏ (cột,hàng)
    lcd.print("Ban Chon :      ");
    lcd.setCursor(0, 1);
    lcd.print("   B :  Tra Sach");
}

// LCD Hủy giao dich
void LCD_HuyGD()
{
    lcd.setCursor(0, 0); // Chọn vị trí con trỏ (cột,hàng)
    lcd.print("Ban Chon :      ");
    lcd.setCursor(0, 1);
    lcd.print("   B :  Tra Sach");
}

// LCD Đang lấy sách
void LCD_DangLaySach()
{
    lcd.setCursor(0, 0); // Chọn vị trí con trỏ (cột,hàng)
    lcd.print("Dang Thuc Hien: ");
    lcd.setCursor(0, 1);
    lcd.print("Lay Sach VT So  ");
    lcd.setCursor(15, 1);
    lcd.print(String(viTriSach));
}

// LCD Đang trả sách
void LCD_DangTraSach()
{
    lcd.setCursor(0, 0); // Chọn vị trí con trỏ (cột,hàng)
    lcd.print("Dang Thuc Hien: ");
    lcd.setCursor(0, 1);
    lcd.print("Tra Sach VT So  ");
    lcd.setCursor(15, 1);
    lcd.print(String(viTriSach));
}

// Giao diện cảnh báo
void LCD_CanhBao(String Waning)
{
    lcd.setCursor(0, 0); // Chọn vị trí con trỏ (cột,hàng)
    lcd.print("Xay ra loi:     ");
    lcd.setCursor(0, 1);
    lcd.print(Waning);
}

// Màn hình chờ giao dịch
void LCD_ChoGD(String msg)
{
    lcd.clear();
    lcd.setCursor(0, 0); // Chọn vị trí con trỏ (cột,hàng)
    lcd.print("Tu Sach Tu Dong ");
    lcd.setCursor(0, 1);
    lcd.print(msg);
}

// Giao diện nhập số
void LCD_NhapSo()
{
    lcd.setCursor(0, 0); // Chọn vị trí con trỏ (cột,hàng)
    lcd.print("Vi Tri Sach:    ");
    lcd.setCursor(0, 1);
    lcd.print("*: Huy | #: OK  ");
    lcd.setCursor(13, 0); // Chọn vị trí con trỏ nhập số
    lcd.print(String(viTriSach));
    lcd.setCursor(13, 0); // Chọn vị trí con trỏ nhập số

    char customKey; // Biến đọc giá trị nút ấn
    lcd.cursor();   // Hiển thị con trỏ
    lcd.blink();    // Nhap nhay con tro
    previousMillis = millis();
    do
    {
        customKey = customKeypad.getKey();
        switch (customKey)
        {
        case '0':
            lcd.print('0');
            viTriSach = 0;
            lcd.leftToRight();
            break;
        case '1':
            lcd.print('1');
            viTriSach = 1;
            lcd.leftToRight();
            break;
        case '2':
            lcd.print('2');
            viTriSach = 2;
            lcd.leftToRight();
            break;
        case '3':
            lcd.print('3');
            viTriSach = 3;
            lcd.leftToRight();
            break;
        case '4':
            lcd.print('4');
            viTriSach = 4;
            lcd.leftToRight();
            break;
        case '5':
            lcd.print('5');
            viTriSach = 5;
            lcd.leftToRight();
            break;
        case '6':
            lcd.print('6');
            viTriSach = 6;
            lcd.leftToRight();
            break;
        case '7':
            lcd.print('7');
            viTriSach = 7;
            lcd.leftToRight();
            break;
        case '8':
            lcd.print('8');
            viTriSach = 8;
            lcd.leftToRight();
            break;
        case '9':
            lcd.print('9');
            viTriSach = 9;
            lcd.leftToRight();
            break;
        case '*':
        {
            HanhDong = 0;
            lcd.setCursor(0, 1);
            lcd.print("Huy Giao Dich!!!");
            delay(2000);
            break;
        }
        case '#':
        {
            lcd.setCursor(0, 1);
            lcd.print(F("Nhap Thanh Cong!"));
            break;
        }
        }

    } while ((customKey != '*') && (customKey != '#') && ((millis() - previousMillis) < TIMEOUT_CHO_NHAP_PHIM)); // thoat khi phim # được ấn
    lcd.noBlink();                                                                                               // Tắt nhấp nháy con trỏ
    lcd.noCursor();                                                                                              // Ẩn con trỏ
    if ((millis() - previousMillis) >= TIMEOUT_CHO_NHAP_PHIM)
        {
            HanhDong = 0;
            lcd.setCursor(0, 1);
            lcd.print("Huy Giao Dich!!!");
            delay(2000);
        }
}

/**************************************************************************/
// >>> Hàm cho cảm biến vân tay

// Hàm đọc thông tin của cảm biến vân tay
void VT_Info()
{
    if (finger.verifyPassword())
    {
        Serial.println("Found fingerprint sensor!");
    }
    else
    {
        Serial.println("Did not find fingerprint sensor :(");
    }

    Serial.println(F("Reading sensor parameters"));
    finger.getParameters();
    Serial.print(F("Status: 0x"));
    Serial.println(finger.status_reg, HEX);
    Serial.print(F("Sys ID: 0x"));
    Serial.println(finger.system_id, HEX);
    Serial.print(F("Capacity: "));
    Serial.println(finger.capacity);
    Serial.print(F("Security level: "));
    Serial.println(finger.security_level);
    Serial.print(F("Device address: "));
    Serial.println(finger.device_addr, HEX);
    Serial.print(F("Packet len: "));
    Serial.println(finger.packet_len);
    Serial.print(F("Baud rate: "));
    Serial.println(finger.baud_rate);

    finger.getTemplateCount();

    if (finger.templateCount == 0)
    {
        Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    }
    else
    {
        Serial.println("Waiting for valid finger...");
        Serial.print("Sensor contains ");
        Serial.print(finger.templateCount);
        Serial.println(" templates");
    }
}

// Hàm lấy ID  trả về -1 nếu lỗi, ok trả về giá trị ID
int VT_Lay_ID()
{
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)
        return -1;

    // found a match!
    Serial.print("Tim thay ID #");
    Serial.print(finger.fingerID);
    Serial.print(" voi do tin cay ");
    Serial.println(finger.confidence);
    return finger.fingerID;
}

// Hàm lấy ID chi tiết, trả về giá trị ID
uint8_t VT_Lay_ID_CT()
{
    uint8_t p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Lay hinh anh OK.");
        break;
    case FINGERPRINT_NOFINGER:
        Serial.println("Khong thay van tay!!!");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Loi Ket Noi!!!");
        return p;
    case FINGERPRINT_IMAGEFAIL:
        Serial.println("Hinh anh loi !!!");
        return p;
    default:
        Serial.println("Loi khac!!!");
        return p;
    }

    // Lấy ảnh OK

    p = finger.image2Tz();
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Chuyen doi hinh anh.");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Hinh anh bi mo!!!");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Loi Ket Noi!!!");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Khong tim thay cac tinh nang cua van tay");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Khong tim thay cac tinh nang cua van tay");
        return p;
    default:
        Serial.println("Loi khac!!!");
        return p;
    }

    // Chuyển đổi ảnh OK
    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Tim thay van tay!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Loi Ket Noi!!!");
        return p;
    }
    else if (p == FINGERPRINT_NOTFOUND)
    {
        Serial.println("Khong tim thay van tay!!!");
        return p;
    }
    else
    {
        Serial.println("Loi khac!!!");
        return p;
    }

    // tìm vân tay
    Serial.print("Tim thay ID #");
    Serial.print(finger.fingerID);
    Serial.print(" voi do tin cay ");
    Serial.println(finger.confidence);

    return finger.fingerID;
}

// Hàm đọc số từ Serial
uint8_t readnumber(void)
{
    uint8_t num = 0;

    while (num == 0)
    {
        while (!Serial.available())
            ;
        num = Serial.parseInt();
    }
    return num;
}

// Hàm dang ky vân tay
uint8_t getFingerprintEnroll()
{

    int p = -1;
    Serial.print("Dang doi ngon tay hop le de dang ky cho ID #");
    Serial.println(id);
    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            Serial.println("Da chup anh van tay xong!");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Loi ket noi!!!");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Hinh anh loi!!!");
            break;
        default:
            Serial.println("Loi khac!!!");
            break;
        }
    }

    // OK success!

    p = finger.image2Tz(1);
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Da chuyen doi anh OK!");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Anh qua xau!!!");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Loi ket noi!!!");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Khong the tim thay cac dau hieu tren van tay!!!");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Khong the tim thay cac dau hieu tren van tay!!!");
        return p;
    default:
        Serial.println("Loi khac !!!");
        return p;
    }

    Serial.println("Bo ngon tay ra!");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER)
    {
        p = finger.getImage();
    }
    Serial.print("ID ");
    Serial.println(id);
    p = -1;
    Serial.println("Dat lai cung mot ngon tay!");
    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            Serial.println("Da chup anh van tay xong!");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.print(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
            break;
        }
    }

    // OK success!

    p = finger.image2Tz(2);
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Anh da duoc chuyen doi OK!");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Anh qua kem!!!");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Khong the tim thay cac dau hieu tren van tay!!!");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Khong the tim thay cac dau hieu tren van tay!!!");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    // OK converted!
    Serial.print("Tao model cho #");
    Serial.println(id);

    p = finger.createModel();
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Ban in van tay phu hop!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_ENROLLMISMATCH)
    {
        Serial.println("Khong tim thay van tay!!!");
        return p;
    }
    else
    {
        Serial.println("Unknown error");
        return p;
    }

    Serial.print("ID ");
    Serial.println(id);
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Da luu!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Loi ket noi!!!");
        return p;
    }
    else if (p == FINGERPRINT_BADLOCATION)
    {
        Serial.println("Khong the luu tai vị tri nay!!!");
        return p;
    }
    else if (p == FINGERPRINT_FLASHERR)
    {
        Serial.println("Loi khi ghi vao bo nho flash!!!");
        return p;
    }
    else
    {
        Serial.println("Unknown error");
        return p;
    }

    return true;
}

// Hàm đăng ký ID cho vân tay
void VT_DangKyID()
{
    Serial.println("San sang de dang ky van tay!");
    Serial.println("Nhap ID # (tu 1 - 127) ma ban muon luu ...");
    id = readnumber();
    if (id == 0)
    {
        Serial.println("ID phai khac 0"); // ID #0 not allowed, try again!
        return;
    }
    Serial.print("Dang ky ID #");
    Serial.println(id);

    while (!getFingerprintEnroll())
        ;
}
// Led cảm biến OFF
void VT_LED_OFF()
{
    finger.LEDcontrol(0);  // 0 : Off, 1 : ON
}

// Led cảm biến ON
void VT_LED_ON()
{
    finger.LEDcontrol(1);  // 0 : Off, 1 : ON
}

// Hàm cho QR code
// Gửi lệnh đọc QR 
void QR_Read_CMD()
{
    // Command Triggered Mode
    // Gửi lệnh : 7E 00 08 01 00 02 01 AB CD
    // Sensor sẽ trả về “ 02 00 00 01 00 33 31” và bắt đầu quét mã
    QRcodeSerial.write(0x7E);
    QRcodeSerial.write(0x00);
    QRcodeSerial.write(0x08);
    QRcodeSerial.write(0x01);
    QRcodeSerial.write(0x00);
    QRcodeSerial.write(0x02);
    QRcodeSerial.write(0x01);
    QRcodeSerial.write(0xAB);
    QRcodeSerial.write(0xCD);
}

// Lấy giá trị đọc đọc từ QR code
String QR_Read_Value()
{
    String MaCode ;
    if (QRcodeSerial.available() > 0)
    { // Check for incomding data
        MaCode = QRcodeSerial.readString(); // Lấy thông tin từ QR code
        Serial.println(MaCode);
        MaCode = MaCode.substring(7, MaCode.length());
        if ( MaCode == "") return "Error";
    }
    return MaCode;
}

#endif  // Kết thúc tiền xử lý
