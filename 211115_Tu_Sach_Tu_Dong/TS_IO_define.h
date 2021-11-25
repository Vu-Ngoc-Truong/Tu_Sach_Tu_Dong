// Định nghĩa các chân IO cho máy
#ifndef TSTD_IO_H_
#define TSTD_IO_H_

/**************************************************************************/
// >>>>> Các pin sử dụng qua Sheild Arduino CNC V3 <<<<<<

// Cụm FB
#define DCMotorP 7  // DC Motor +
#define DCMotorN 4  // DC Motor -
#define FB_LimitP  A0 // Công tắc giới hạn thuận
#define FB_LimitN  A3 // Công tắc giới hạn ngược
#define RUN_P 23  // Nút ấn quay chiều dương
#define RUN_N 25 // Nút ấn quay chiều âm

//Step pin
#define X_STP       2
#define Y_STP       3

//Direction pin
#define X_DIR       5
#define Y_DIR       6

// Stop limit
#define X_STOP      9
#define Y_STOP      10

// Step Enable
#define EN          8  

// Cảm biến phát hiện có sách trong máng
#define MangSach    11
#define VT_Touch     A1



/**************************************************************************/
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

/**************************************************************************/
// Các cổng Serial
#define QRcodeSerial Serial1        // Cổng Serial giao tiếp với Barcode Sensor
#define FingerSerial Serial2        // Cổng Serial giao tiếp với cảm biến vân tay

/**************************************************************************/
// Khai báo các hằng số
#define Servo1_Pos0 0          // Vị trí 0 của động cơ đẩy máng sách
#define Servo1_Pos1 140          // Vị trí 0 của động cơ  đẩy máng sách
#define Servo2_Pos0 120          // Vị trí 0 của động cơ xoay tay gạt
#define Servo2_Pos1 20          // Vị trí 0 của động cơ xoay tay gạt

// Timeout cho các hoạt động
#define TIMEOUT_SS_PHAT_HIEN_SACH 15000     // ms

// Trạng thái cảm biến
#define SS_CoSach 0         // Trạng thái sensor khi máng có sách
#define SS_KhongSach 1      // Trạng thái sensor khi máng không có sách


/**************************************************************************/
// Khai báo các biến chung
unsigned long previousMillis = 0;        // Thời gian hiện tại
int viTriSach=0 ; // Vị trí sách cần lấy, cất
int HanhDong = 0 ; // Hành động, lấy, cất sách ...


#endif