#include <Wire.h>
#include "TM1637.h"
#include <DS3231.h>

#define CLK 9   //11 пин дисплей
#define DIO 10    //10 пин дисплей
TM1637 disp(CLK, DIO);
byte TIME[4];

#define light 3 // на 3 пин вешаю подсветку изза шим
#define filter 7 //пин фильтра чтоб не мешал жрать
#define compressor 8 //пин компрессора
#define button_eat 2 //кнопка жрать
#define button_hour 4 //кнопка часов
#define button_min 5 //кнопка минут
#define buzzer 6 //пишалка
unsigned long last_time;
unsigned long off_time = 900000;
int sss;
int hhh;
int mmm;
int level_light;
int flag_eat = 0;
DS3231  rtc(SDA, SCL);

Time t;

void setup() {
  rtc.begin();
  Serial.begin(9600);

  disp.init();  // инициализация
  disp.set(5);  // яркость, 0 - 7 (минимум - максимум)

  pinMode(filter , OUTPUT);
  pinMode(compressor , OUTPUT);
  pinMode(light , OUTPUT);

  byte welcome_banner[] = {_H, _E, _L, _L, _O, _empty, _empty,
                           _A, _q, _u, _A, _empty, _empty,
                          };
  // выводим на дисплей со смещением на один шаг
  Serial.println(sizeof(welcome_banner));
  disp.runningString(welcome_banner, sizeof(welcome_banner), 100);
  delay(700);
  disp.clearDisplay();
  digitalWrite(filter, LOW);
  digitalWrite(compressor, LOW);
  delay (500);
}

void loop() {
  t = rtc.getTime();
  disp.point(1);
  //Serial.println (t.sec, DEC);
  sss = t.sec, DEC;
  mmm = t.min, DEC;
  hhh = t.hour, DEC;

  TIME[0] = hhh / 10;           // получить десятки минут
  TIME[1] = hhh % 10;           // получить единицы минут
  TIME[2] = mmm / 10;            // получить десятки секунд
  TIME[3] = mmm % 10;            // получить единицы секунд
  disp.display(TIME);          // вывести массив на дисплей

  /*if(hhh > 21){
    digitalWrite (light, LOW);
    }*/

  if (hhh == 23 && mmm < 25) {
    level_light = (mmm * 60 + sss);
    level_light = map(level_light, 900, 1489, 255, 1);
    level_light = constrain(level_light, 1, 255);
    analogWrite (light, level_light);
    disp.set(1);
  }
  if (hhh == 23 && mmm > 24) {
    analogWrite (light, 1);
    disp.set(0);
  }

  if (hhh >= 0 && hhh < 9) {
    digitalWrite (light, LOW);
    disp.set(0);
  }
  if (hhh == 9 && mmm > 39) { //заменить на ммм!!!!!
    level_light = (mmm * 60 + sss);
    level_light = map(level_light, 2461, 3599, 0, 255);
    level_light = constrain(level_light, 0, 255);
    analogWrite (light, level_light);
    disp.set(5);
  } else {
    //analogWrite (light, 0);
  }

  if (hhh > 9 && hhh < 23) {
    if (hhh > 14 && hhh < 18) {
      digitalWrite (light, LOW);
      disp.set(5);
    } else
      digitalWrite (light, HIGH);
    disp.set(5);
  }

  /*if(hhh > 22) {
    level_light = sss;
    level_light = map(level_light, 0, 30, 0, 255);
    level_light = constrain(level_light, 0, 255);
    analogWrite (light, (255 - level_light));
    }*/

  if (digitalRead(button_hour) == HIGH) {
    hhh = t.hour, DEC;
    hhh++;
    if (hhh > 23) {
      hhh = 0;
    }
    rtc.setTime(hhh, mmm, sss);
  }

  if (digitalRead(button_min) == HIGH) {
    mmm = t.min, DEC;
    mmm++;
    if (mmm > 59) {
      mmm = 0;
    }
    rtc.setTime(hhh, mmm, sss);
  }

  if (hhh == 22 && mmm == 10) {
    flag_eat = 1;
  } else {
    flag_eat = 0;
  }


  if (digitalRead(button_eat) == HIGH || flag_eat == 1) {
    last_time = millis();
    //  disp.clearDisplay();
    disp.point(0);
    disp.displayByte(0x79, 0x77, 0x78, 0x00);  // вывести eat "вручную"
    delay(500); //15 минут это 900000
//    void(* resetFunc) (void) = 0;//объявляем функцию reset с адресом 0
 //   resetFunc(); //вызываем reset
    //  disp.clearDisplay();
  }
  // Serial.println(level_light);
  if (millis() > off_time){
    if (millis()-last_time < off_time){
      digitalWrite(filter, HIGH);
    digitalWrite(compressor, HIGH);
    } else{
      digitalWrite(filter, LOW);
    digitalWrite(compressor, LOW);
    }
  }
  
  delay (200);
}
