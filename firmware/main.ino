
#include <EEPROM.h>   //библиотека для работы со внутренней памятью ардуино



#include <LiquidCrystal_I2C.h>  // подключаем библу

LiquidCrystal_I2C lcd(0x27, 16, 2);  // адрес, столбцов, строк

unsigned long lastturn, time_press; //переменные хранения времени
float SPEED; //переменная хранения скорости в виде десятичной дроби
float DIST; //переменная хранения расстояния в виде десятичной дроби
float w_length=2.050; //длина окружности колеса в метрах
bool flag = false; //флажок для хранения (что выводим на дисплее, скорость или расстояние)
boolean state, button; //флажки для обработчика нажатия кнопки

byte speed_icon[8] = {
  0b00111,
  0b01110,
  0b11100,
  0b11111,
  0b00111,
  0b01110,
  0b11100,
  0b00000
};

byte dist_icon[8] = {
  0b01110,
  0b11111,
  0b11011,
  0b11111,
  0b01110,
  0b01110,
  0b00100,
  0b00000
};

void setup() {

  Serial.begin(9600);  //открыть порт
  attachInterrupt(0,sens,RISING); //подключить прерывание на 2 пин при повышении сигнала
  pinMode(3, OUTPUT);   //3 пин как выход
  digitalWrite(3, HIGH);  //подать 5 вольт на 3 пин
  pinMode(6, INPUT);   //сюда подключена кнопка
  DIST=(float)EEPROM.read(0)/10.0; //вспоминаем пройденное расстояние при запуске системы (деление на 10 нужно для сохранения десятых долей расстояния, см. запись)


  lcd.init();           // инициализация
  lcd.backlight();      // включить подсветку  
//  lcd.setCursor(1, 0);  // столбец 1 строка 0
//  lcd.print("Hello, world!");
//  lcd.setCursor(4, 1);  // столбец 4 строка 1
//  lcd.print("GyverKIT");
}

void sens() {
  if (millis()-lastturn > 80) {  //защита от случайных измерений (основано на том, что велосипед не будет ехать быстрее 120 кмч)
    SPEED=w_length/((float)(millis()-lastturn)/1000)*3.6;  //расчет скорости, км/ч
    lastturn=millis();  //запомнить время последнего оборота
    DIST=DIST+w_length/1000;  //прибавляем длину колеса к дистанции при каждом обороте оного
  }
}

void loop() {
  int cel_sp=floor(SPEED);
  int sot_sp=(((float)cel_sp/1000)-floor((float)cel_sp/1000))*10;
  int des_sp=(((float)cel_sp/100)-floor((float)cel_sp/100))*10;
  int ed_sp=(((float)cel_sp/10)-floor((float)cel_sp/10))*10;
  int dr_sp=(float)(SPEED-floor(SPEED))*10;

  lcd.createChar(0, speed_icon);
  lcd.createChar(1, dist_icon);
  
//  if (flag==0) {
//    //disp.set(LED_0F[sot_sp],3);  //вывод сотен скорости (для велосипеда не нужно =)
//    disp.set(LED_0F[des_sp],2);  //вывод десятков скорости
//    disp.set(LED_0F[ed_sp+10],1);  //вывод единиц скорости, с точкой
//    disp.set(LED_0F[dr_sp],0);  //вывод после точки   
//  }

  lcd.setCursor(1, 0);  // столбец 1 строка 0
  lcd.write(byte(0));  lcd.print(" "); lcd.print(SPEED); // lcd.print("."); lcd.print(des_sp);


  int cel_di=floor(DIST);  //целые
  int sot_di=(((float)cel_di/1000)-floor((float)cel_di/1000))*10;  //сотни
  int des_di=(((float)cel_di/100)-floor((float)cel_di/100))*10;  //десятки
  int ed_di=floor(((float)((float)cel_di/10)-floor((float)cel_di/10))*10);  //единицы (с точкой)
  int dr_di=(float)(DIST-floor(DIST))*10;  //десятые части
  
//  if (flag==1) {
//    ]disp.set(LED_0F[sot_di],3);  //вывод сотен расстояния
//    disp.set(LED_0F[des_di],2);  //вывод десятков расстояния
//    disp.set(LED_0F[ed_di+10,1);  //вывод единиц расстояния, с точкой
//    disp.set(LED_0F[dr_di],0);  //вывод после точки  
//  }

  lcd.setCursor(1, 1);  // столбец 1 строка 0
  lcd.write(byte(1)); lcd.print(" "); lcd.print(DIST);

  if ((millis()-lastturn)>2000){ //если сигнала нет больше 2 секунды
    SPEED=0;  //считаем что SPEED 0
    EEPROM.write(0,(float)DIST*10.0); //записываем DIST во внутреннюю память. Сделал так хитро, потому что внутренняя память не любит частой перезаписи. Также *10, чтобы сохранить десятую долю
  }



}
