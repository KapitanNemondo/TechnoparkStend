#include <Arduino.h>
#include "C:\Projects\Tech Stend\RX_TX_command.h"

// подключаем софт юарт
#include "softUART.h"
// делаем только приёмником (экономит память)
softUART<4, GBUS_FULL> UART(1000); // пин 4, скорость 1000

// подключаем GBUS
#include "GBUS.h"
GBUS bus(&UART, ADDRESS_SMART_CLASS, 64); // обработчик UART, адрес 5, буфер 20 байт

byte TX_address;

//приемник данных с фоторезистора и с датчика кислорода, управление сервой и RGB
#include "Adafruit_NeoPixel.h"

#include <Servo.h>
#include <Wire.h>
#include <Multiservo.h>
//#include <QuadDisplay2.h>
//#include <SPI.h>
#define PINSERVO      3 //пин сервы
#define servo_close   74
#define data_pin      14
#define clk           15
#define latch         16
#define enable        17

bool flag_led = false;

bool servo_state = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(64, 6, NEO_GRB + NEO_KHZ800);
Multiservo  servo_3;
//QuadDisplay qd(7);
int len = 64; //количество светодиодов
int angle = 74; //начальный угол в который идет серва
uint32_t white = strip.Color(255, 255, 255); //белый
uint32_t yellow = strip.Color(255, 100, 5); //желтый
uint32_t whyel = strip.Color(255, 100, 20); //желто-белый
uint32_t off = strip.Color(0, 0, 0); //выключение


//byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

byte digits [10] = {
  0b00111111,//0
  0b00000110,//1
  0b01011011,//2
  0b01001111,//3
  0b01100110,//4
  0b01101101,//5
  0b01111101,//6
  0b00000111,//7
  0b01111111,//8
  0b01101111//9
};

void setup() {
  pinMode(3, OUTPUT);
  servo_3.attach(3);
  servo_3.write(servo_close);
  servo_state = 0;
  Serial.begin(9600); //открываем порт для связи с ПК

  /*
  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах
  // Первый передатчик (фоторезистор)
  radio.openReadingPipe (0, 0x7878787878);
  // Второй передатчик (датчики температуры, влажности)
  radio.openReadingPipe (1, 0xB4B5B6B7F1);
  //radio.openReadingPipe(1,address[0]);      //хотим слушать трубу 0
  radio.setChannel(0x68);  //выбираем канал (в котором нет шумов!)
  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!
  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
  */
  
  strip.begin(); // Функция begin() настроит порт Arduino и выставит значения по умолчанию
  strip.show();  // Функция show() передаст команду на выключение всем пикселям.
  for (int i = 14; i < 18; i++)
  {
    pinMode(i, OUTPUT);
  }
  digitalWrite(enable, LOW);
  Serial.println("ok");

}

void setMode1() //белый
{
  for (int i = 0; i < len; i++)
  {
    strip.setPixelColor(i, white);
  }
  strip.show();  // Функция show() передаст команду на выключение всем пикселям.
}

void setMode2()//средний
{
  for (int i = 0; i < len; i++)
  {
    strip.setPixelColor(i, whyel);
  }
  strip.show();  // Функция show() передаст команду на выключение всем пикселям.
}

void setMode3()//яркий желтый
{
  for (int i = 0; i < len; i++)
  {
    strip.setPixelColor(i, yellow);
  }
  strip.show();  // Функция show() передаст команду на выключение всем пикселям.
}

void setOff() //выключение
{
  strip.setBrightness(64);
  for (int i = 0; i < len; i++)
  {
    strip.setPixelColor(i, off);
  }
  strip.show();  // Функция show() передаст команду на выключение всем пикселям.
}

void power(byte valPower) //настройка яркости
{ 
  Serial.print("Bri: "); Serial.println(valPower);
  strip.setBrightness(valPower);
  strip.show();  // Функция show() передаст команду на выключение всем пикселям.
}

void openWin()
{
  Serial.println("open");
  servo_3.write(10);
}

void closeWin()
{
  Serial.println("close");
  servo_3.write(servo_close);
}


void display_time (int dhours, int dminutes)
{
  digitalWrite(latch, LOW);
  shiftOut(data_pin, clk, MSBFIRST, digits[dminutes % 10]);
  shiftOut(data_pin, clk, MSBFIRST, digits[dminutes / 10]);
  shiftOut(data_pin, clk, MSBFIRST, digits[dhours % 10]);
  shiftOut(data_pin, clk, MSBFIRST, digits[dhours / 10]);
  digitalWrite(latch, HIGH);
}

void Settings_Smart(smart_class *data) {

  switch (data->stat_light) {
    case LED_ON: {
      flag_led = true;
      break;
    }

    case LED_OFF: {
      flag_led = false;
      break;
    }

    
    default:
      break;
  }

  switch (data->stat_windows) {
    case WINDOWS_OPEN: {
      openWin();
      break;
    }

    case WINDOWS_CLOSE: {
      closeWin();
      break;
    }
    
    default:
      break;
  }

}

byte pipeNo, gotByte;
int minutes = 0, hours = 8;
long del = 0;
struct dht_data {
  byte humidity;
  byte temperature;
};

void loop()
{

  /*
  // Если поступили данные, определяем номер трубы передатчика по ссылке на pipe
  if (radio.available(&pipeNo)) {
    if (pipeNo == 1) // Если данные пришли от 1-го передатчика
    {
      radio.read(&gotByte, sizeof(gotByte)); // Считываем данные в массив
      //setMode2();
      power(gotByte);
      //Serial.print("Recieved photo: "); Serial.println(gotByte);
    }
    if (pipeNo == 0) // Если данные пришли от 2-го передатчика
    {
      dht_data data;
      radio.read(&data, 2);
      Serial.print(F("Temperature: "));
      Serial.println(data.temperature);
      Serial.print(F("Humidity: "));
      Serial.println(data.humidity);

      if ((data.temperature > 30) || (data.humidity > 55))
      {

        openWin();
        //delay(2000);
      }
      if ((data.temperature < 31) && (data.humidity < 56))
      {

        closeWin();
        //delay(2000);
      }
    }
  }

  */

  bus.tick();

  if (bus.statusChanged()) {  // если статус изменился
    Serial.print("Status: "); Serial.println(bus.getStatus());  // выводим код
  }

  if (bus.gotData()) {
    TX_address = bus.getTXaddress();
    Serial.print("TX Address: "); Serial.println(TX_address);
    switch (TX_address)
    {
      case ADDRESS_BLOCK_BRI: {
        blok_bri data;
        bus.readData(data);

        power(data.bri);

        break;
      }

      case ADDRESS_MAIN_BLOCK: {
        smart_class data;
        bus.readData(data);

        Settings_Smart(&data);

      }

      default: {
        break;
      }
    }
  }

  if (millis() - del > 10)
  {

    del = millis();
    if (minutes < 59) {
      minutes++;
    }
    else
    {
      minutes = 0;
      if (hours < 23) {
        hours++;
      }
      else
      {
        hours = 0;
      }
    }
    //Serial.println(servo_3.read());
    //Serial.println("hours "+String(hours)+"minutes "+String(minutes));
    display_time(hours, minutes);
  }

  if ((hours > 6) && (hours < 11))
  {
    setMode1();
  }
  if ((hours > 11) && (hours < 17))
  {
    setMode2();
  }
  if ((hours > 17) && (hours < 22))
  {
    setMode3();
  }
  if (hours < 7)
  {
    setOff(); //белый
  }
}
