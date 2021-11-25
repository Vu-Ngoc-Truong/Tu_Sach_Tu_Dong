// >>>>> Các hàm liên quan đến điều khiển các động cơ của máy
#ifndef TSTD_DONGCO_H
#define TSTD_DONGCO_H

#include <Servo.h>
#include "TS_IO_define.h"

// Bước vít me
#define BUOC_VIT_ME  8      // 8 mm
#define CHU_VI_PULY  40     // 40 mm
#define XUNG_1VONG  200     // 200 xung / vòng

float LX_XUNG_MM = XUNG_1VONG / BUOC_VIT_ME; // Số xung trục lên xuống Y di chuyển 1 mm =25
float TP_XUNG_MM = XUNG_1VONG / CHU_VI_PULY; // Số xung trục trái phải X di chuyển 1 mm =5

// Thời gian delay khi xuất xung cho đông cơ Step (uS)
#define TIME_SPEED_LOW  1000    
#define TIME_SPEED_MID   800
#define TIME_SPEED_HIGH  500

// Biến trạng thái
int step_X = 0; // Động cơ trái phải
int step_Y = 0; // Động cơ lên xuống

// >>>>>>>>> Các tọa độ trục ( Xung)  <<<<<<<
// Toa do truc X cho cac vi tri sach 0-9  
int X_Index[10] = {10,475,950,1425,1905,20,490,955,1425,1915};
// toa do truc Y cho cac vi tri lay sach 0-9
int Y_Index[10] = {0,0,0,0,0,6500,6500,6500,6475,6450} ; // 6500 ~ 260 mm
// Toa do chenh lech giua vi tri lay va tra sach
int Y_tra_sach = 80 ; // độ cao giữa trả sách và lấy sách theo xung
int X_mangSachra = 75;  // Vị trí đẩy máng sách ra của trục X
int toa_do_X = 0; // Toa do hien tai cua X theo xung
int toa_do_Y = 0; // Toa do hien tai cua Y theo xung

// Khai báo cụm lấy sác FB
Servo myservo1;  // Servo xoay tay gạt lên xuống
Servo myservo2;  // Servo đẩy máng sách vào ra

// Các hàm cho động cơ bước trục X,Y
void stepRun(boolean dir, byte dirPin, byte stepperPin, int steps, int delayTime);
bool veGoc( byte dirPin, byte stepperPin,byte stopPin, int delayTime);
void do_toa_do_X();
void do_toa_do_Y();
void chayDiemX(int viTri);
void chayDiemY(int viTri);

// Các hàm cho động cơ DC 5V đẩy tay gạt ra vào
void FB_Dung();         // Dừng động cơ
void FB_DayVao();       // Đẩy tay gạt vào giá sách
void FB_DayRa();        // Đẩy tay gạt ra khỏi giá sách

// Các hàm cho Servo xoay tay gạt
void FB_QuayXuong();
void FB_QuayLen();

// Các hàm cho Servo đẩy máng sách vào ra
void FB_DayMangRa();
void FB_ThuMangVe();

// Hàm xuất xung cho động cơ step : Chiều quay, chân hướng quay, chân xung, số bước quay , thời gian tốc độ xung
void stepRun(boolean dir, byte dirPin, byte stepperPin, int steps, int delayTime)
{

digitalWrite(dirPin, dir);
for (int i = 0; i < steps; i++)
{
    digitalWrite(stepperPin, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(stepperPin, LOW);
    delayMicroseconds(delayTime);
}
}

// Hàm về gốc cho trục X,Y
bool veGoc( byte dirPin, byte stepperPin,byte stopPin, int delayTime)
{
FB_DayRa();
digitalWrite(EN, LOW);
while ( digitalRead(stopPin) == 0 ) // CTHT chưa chạm
{
    stepRun(false, dirPin, stepperPin,1, delayTime); //Chạy lùi về gốc

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
    stepRun(true, X_DIR, X_STP, 5,TIME_SPEED_LOW);
    toa_do_X +=5;
    Serial.println(toa_do_X);
    delay(100);
    }
    if ((digitalRead(RUN_P) == 1)&& (digitalRead(RUN_N) == 0))
    {
    stepRun(false, X_DIR, X_STP, 5,TIME_SPEED_LOW);
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
        stepRun(true, Y_DIR, Y_STP, 25,TIME_SPEED_HIGH);
        toa_do_Y +=25;
        Serial.println(toa_do_Y);
        delay(100);
        }
        if ((digitalRead(RUN_P) == 1)&& (digitalRead(RUN_N) == 0))
        {
        stepRun(false, Y_DIR, Y_STP, 25,TIME_SPEED_HIGH);
        toa_do_Y -=25;
        Serial.println(toa_do_Y);
        delay(100);
        }
        if ((digitalRead(RUN_P) == 0)&& (digitalRead(RUN_N) == 0))
        break;
    }
}

void chayDiemX(int viTri)
{
    // Về gốc trục X
    veGoc(X_DIR, X_STP, X_STOP,TIME_SPEED_LOW);
    // Chạy đến điểm yêu cầu
    digitalWrite(EN, LOW);
    stepRun(true, X_DIR, X_STP, X_Index[viTri],TIME_SPEED_LOW); //X, Counterclockwise
    digitalWrite(EN, HIGH);

}
void chayDiemY(int viTri)
{
    // Về gốc trục Y
    veGoc(Y_DIR, Y_STP, Y_STOP,TIME_SPEED_HIGH);
    // Chạy đến điểm yêu cầu
    digitalWrite(EN, LOW);
    stepRun(true, Y_DIR, Y_STP, Y_Index[viTri],TIME_SPEED_HIGH); //X, Counterclockwise
    digitalWrite(EN, HIGH);
}
// >>>>>>>> Ham dieu khien cum lay sach vao ra <<<<<<<<<
// Dừng động cơ
void FB_Dung()
{
    digitalWrite(DCMotorP, HIGH);
    digitalWrite(DCMotorN, HIGH);
}

// Tay gạt đẩy vào lấy sách
void FB_DayVao()
{
    while (digitalRead(FB_LimitP) ==0)
        {
            digitalWrite(DCMotorP, HIGH);
            digitalWrite(DCMotorN, LOW);
        }
    FB_Dung();
}

// Tay gạt đẩy vào không liên quan đến Limit Switch
void FB_DayVao_NoLimit()
{
    
    digitalWrite(DCMotorP, HIGH);
    digitalWrite(DCMotorN, LOW);
}

// Tay gạt thu về
void FB_DayRa()
{
    while (digitalRead(FB_LimitN) ==0)
        {
            digitalWrite(DCMotorP, LOW);
            digitalWrite(DCMotorN, HIGH);
        }
        FB_Dung();
}

// Tay gạt đẩy vào không liên quan đến Limit Switch
void FB_DayRa_NoLimit()
{
    
    digitalWrite(DCMotorP, LOW);
    digitalWrite(DCMotorN, HIGH);
}

// Tay gat quay xuong
void FB_QuayXuong()
{
    for (int j = Servo2_Pos0;j >= Servo2_Pos1 ; j--)
    {
        myservo2.write(j);
        delay(10);
    }
}

// Quay Lên
void FB_QuayLen()
{
    for (int j= Servo2_Pos1; j <= Servo2_Pos0 ; j++)
    {
        myservo2.write(j);
        delay(10);
    }
}

// Xuất sách ra
void FB_DayMangRa()
{
    veGoc(X_DIR, X_STP, X_STOP, TIME_SPEED_LOW);
    stepRun(true, X_DIR, X_STP, X_mangSachra, TIME_SPEED_LOW); // Chay den toa do X
    for (int j=Servo1_Pos0; j <= Servo1_Pos1; j++)
        {
            myservo1.write(j);
            delay(10);
        }
}

// Thu sách về
void FB_ThuMangVe()
{
for (int j = Servo1_Pos1; j >= Servo1_Pos0 ; j--)
    {
        myservo1.write(j);
        delay(10);
    }
}

#endif  // Kết thúc tiền xử lý