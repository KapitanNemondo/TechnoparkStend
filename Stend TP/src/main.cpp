#include <Arduino.h>

#include <GyverTimer.h>

#include "C:\Projects\Tech Stend\RX_TX_command.h"

#include <Servo.h>
Servo myservo;

int pos = 0;

// приём данных по однопроводному юарту

// подключаем софт юарт
#include "softUART.h"
// делаем только приёмником (экономит память)
softUART<4> UART(1000); // пин 4, скорость 1000

// подключаем GBUS
#include "GBUS.h"
GBUS bus(&UART, ADDRESS_HOME, 64); // обработчик UART, адрес 5, буфер 20 байт

byte TX_address;

// #include <GyverRGB.h>
// #include <PWM.h>
// GRGB strip(5, 3, 6);  // куда подключены цвета (R, G, B)

#define NUM_LEDS    38
#define LED_PIN     2

// настройки пламени
#define FIRE_STEP 15    // шаг огня
#define HUE_GAP 21      // заброс по hue
#define HUE_START 0     // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
#define MIN_BRIGHT 70   // мин. яркость огня
#define MAX_BRIGHT 255  // макс. яркость огня
#define MIN_SAT 245     // мин. насыщенность
#define MAX_SAT 255     // макс. насыщенность

// Настройка параметров пламени
#define HUE_GAP 21
#define HUE_START_sea 172
#define HUE_START_lightning 140
#define HUE_START_Embiend 180
#define HUE_START_forest 80

// для разработчиков
#include <FastLED.h>
#define ORDER_GRB       // порядок цветов ORDER_GRB / ORDER_RGB / ORDER_BRG
#define COLOR_DEBTH 2   // цветовая глубина: 1, 2, 3 (в байтах)
// на меньшем цветовом разрешении скетч будет занимать в разы меньше места,
// но уменьшится и количество оттенков и уровней яркости!

// ВНИМАНИЕ! define настройки (ORDER_GRB и COLOR_DEBTH) делаются до подключения библиотеки!
#include <microLED.h>

LEDdata leds[NUM_LEDS];  // буфер ленты типа LEDdata (размер зависит от COLOR_DEBTH)
microLED strip(leds, NUM_LEDS, LED_PIN);  // объект лента
int counter = 0;

// ленивая жопа
#define FOR_i(from, to) for(int i = (from); i < (to); i++)
#define FOR_j(from, to) for(int j = (from); j < (to); j++)

// Настройка эффектов
#define HUE_START 140     // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
#define HUE_GAP 10      // коэффициент цвета огня (чем больше - тем дальше заброс по цвету)
#define SMOOTH_K 0.15   // коэффициент плавности огня
#define MIN_BRIGHT 80   // мин. яркость огня
#define MAX_BRIGHT 255  // макс. яркость огня
#define MIN_SAT 255     // мин. насыщенность
#define MAX_SAT 255     // макс. насыщенность

#define Period_1 1000

// Таймеры для света
#define time_strip_GM 40
#define time_strip_GN 40
#define time_strip_Buy 350

GTimer_ms time_s_GM;
GTimer_ms time_s_GN;
GTimer_ms time_s_Buy;


//#define Helloing 2
//#define GoodNighting 3
//#define GMing 4
//#define USSRing 5
//#define Breakfasting 6
//#define Chilling 7

unsigned long timer_1;

byte R = 255, G = 200, B = 230;
byte Bri = 0;

#define RELE_1        5
#define RELE_2        6
#define RELE_3        7
#define RELE_4        8

int Rejim;
byte Op_cl;

int deg;

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

//RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

//byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

// возвращает цвет огня для одного пикселя
LEDdata getFireColor(int val) {
  // чем больше val, тем сильнее сдвигается цвет, падает насыщеность и растёт яркость
  return mHSV(
           HUE_START + map(val, 0, 255, 0, HUE_GAP),                    // H
           constrain(map(val, 0, 255, MAX_SAT, MIN_SAT), 0, 255),       // S
           constrain(map(val, 0, 255, MIN_BRIGHT, MAX_BRIGHT), 0, 255)  // V
         );
}

// Функция Огня
void fireTick() {
  static uint32_t prevTime;

  // двигаем пламя
  if (millis() - prevTime > 20) {
    prevTime = millis();
    int thisPos = 0, lastPos = 0;
    FOR_i(0, NUM_LEDS) {
      leds[i] = getFireColor((inoise8(i * FIRE_STEP, counter)));
    }
    counter += 20;
    strip.show();
  }
}



void Chill() {
  fireTick();
}

void Breakfast() {
  R = 248;
  G = 0;
  B = 0;

  strip.clear();
  strip.fill(mRGB(R, G, B));
  strip.show();
}

void Buy() {
  if (time_s_Buy.isReady()) {
    Bri--;   // "сбросить" таймер
    strip.setBrightness(Bri);
    Serial.println(Bri);
    strip.clear();
    strip.fill(mRGB(R, G, B));
    strip.show();

  }
}


void GoodNight() {
  Bri = 255;
  strip.setBrightness(Bri);
  Serial.println("start");
  while (Bri != 0) {
    if (time_s_GN.isReady()) {
      Bri--;   // "сбросить" таймер
      strip.setBrightness(Bri);
      Serial.println(Bri);
      strip.clear();
      strip.fill(mRGB(R, G, B));
      strip.show();
    }
  }
}

void OpenCurtains() {
  myservo.writeMicroseconds(1400); // Крутит по часовой в течение секунды
  delay(6000);                     //
  myservo.writeMicroseconds(1500); // Останавливается
}

void CloseCurtains() {
  myservo.writeMicroseconds(1600); // Крутит по часовой в течение секунды закрытие
  delay(5300);                     //
  myservo.writeMicroseconds(1500); // Останавливается
}

void GM() {
  Bri = 0;

  strip.setBrightness(Bri);
  while (Bri != 255) {
    if (time_s_GM.isReady()) {
      Bri++;
      strip.setBrightness(Bri);
      strip.clear();
      strip.fill(mRGB(R, G, B));
      strip.show();
    }

    
    if (Bri == 180) {
      digitalWrite(RELE_1, HIGH);
    }
  }
    
}

void Hellou() {
  digitalWrite(RELE_1, HIGH);

  while (Bri < 255) {
    if (millis() - timer_1 >= Period_1) {
      timer_1 = millis();   // "сбросить" таймер

      strip.clear();
      strip.fill(mRGB(255, 255, 255));
      strip.show();                // Заливаем белым

      Bri = Bri + 15;
      strip.setBrightness(Bri);
    }
  }
}

void Settings_Home(home *data) {
  switch (data->regim_led) {
    case REG_HELLOU: {
        Serial.println("Hellou");
        Hellou();
        break;
      }
    case REG_GOODNIGHT: {
        Serial.println("GoodNight");
        GoodNight();
        break;
      }
    case REG_GM: {
        Serial.println("GM");
        GM();
        break;
      }
    // case 5: {
    //     Serial.println("USSR");
    //     USSR();
    //     break;
    //   }
    case REG_BREAKFAST: {
        Serial.println("Breakfast");
        Breakfast();
        break;
      }
    case REG_CHILL: {
        Serial.println("Chill");
        Chill();
        break;
      }
    case REG_BUY: {
        Serial.println("Buy");
        Buy();
        break;
      }
    }
      
    int Op_cl = data->stat_lump;
    if (Op_cl == LUMP_ON) {
      Serial.println("Включить свет");
      digitalWrite(RELE_4, HIGH);
    }
    else if (Op_cl == LUMP_OFF) {
      Serial.println("Выключить свет");
      digitalWrite(RELE_4, LOW);
    }
    else {
      Serial.println("Error Lump");
    }

    int curtains = data->stat_curtains;
    if (curtains == CURTAINS_OPEN) {
      Serial.println("Serv to go Nigga");
      Serial.println();
      OpenCurtains();
    }
    else if (curtains == CURTAINS_CLOSE) {
      Serial.println("Nigga go out");
      Serial.println();
      CloseCurtains();
    }
    else {
      Serial.println("Error Curtains");
    }
}


void setup() {

  pinMode(RELE_1, OUTPUT);
  pinMode(RELE_2, OUTPUT);
  pinMode(RELE_3, OUTPUT);
  pinMode(RELE_4, OUTPUT);

  for (int i = RELE_1; i <= RELE_4; i++) {
    digitalWrite(i, HIGH);
  }

  Serial.begin(9600); //открываем порт для связи с ПК
  Serial.println("SUCC");

  //strip.setDirection(REVERSE);
  //  R = 0;
  //  G = 250;
  //  B = 0;
  //  strip.setBrightness(Bri);

  myservo.attach(3);

  /*
  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
  */

  time_s_GM.setInterval(time_strip_GM);
  time_s_GN.setInterval(time_strip_GN);
  time_s_Buy.setInterval(time_strip_Buy);

  strip.clear();
  strip.setBrightness(0);
  strip.show();

}

void loop() {
  // com recived;
  // byte pipeNo, gotByte;
  // while ( radio.available(&pipeNo)) {                                 // слушаем эфир со всех труб
  //   //radio.read( &gotByte, sizeof(gotByte) );                          // чиатем входящий сигнал
  //   radio.read( &recived, sizeof(com) );

  //   Serial.print("Recieved command: ");
  //   Serial.println(recived.comand);

  //   switch (recived.comand) {
  //     case chg_mode: {
  //         Rejim = recived.data[0];
  //         switch (Rejim) {
  //           case 2: {
  //               Serial.println("Hellou");
  //               Hellou();
  //               break;
  //             }
  //           case 3: {
  //               Serial.println("GoodNight");
  //               GoodNight();
  //               break;
  //             }
  //           case 4: {
  //               Serial.println("GM");
  //               GM();
  //               break;
  //             }
  //           //            case 5: {
  //           //                Serial.println("USSR");
  //           //                USSR();
  //           //                break;
  //           //              }
  //           case 6: {
  //               Serial.println("Breakfast");
  //               Breakfast();
  //               break;
  //             }
  //           case 7: {
  //               Serial.println("Chill");
  //               Chill();
  //               break;
  //             }
  //           case 8: {
  //               Serial.println("Buy");
  //               Buy();
  //               break;
  //             }
  //         }
  //         break;
  //       }
  //     case chg_color: {
  //         R = recived.data[0];
  //         G = recived.data[1];
  //         B = recived.data[2];
  //         Serial.print("R: ");  Serial.print(R);
  //         Serial.print(" G: "); Serial.print(G);
  //         Serial.print(" B: "); Serial.println(B);
  //         strip.setRGB(R, G, B);
  //         break;
  //       }
  //     case chg_rosetka: {
  //         Op_cl = recived.data[0];
  //         if (Op_cl == 1) {
  //           Serial.println("Открыть");
  //           digitalWrite(rele, HIGH);
  //         }
  //         if (Op_cl == 2) {
  //           Serial.println("Закрыть");
  //           digitalWrite(rele, LOW);
  //         }
  //         else {
  //           Serial.println("Eror Rosetka");
  //         }
  //         break;
  //       }
  //     case chg_serv: {
  //         if (recived.data[0] == 3) {
  //           Serial.println("Serv to go Niga");
  //           Serial.println();
  //         }
  //         else {
  //           Serial.println("Niga go out");
  //           Serial.println();
  //         }
  //         break;
  //       }
  //   }
  // }

  bus.tick();

  if (bus.statusChanged()) {  // если статус изменился
    Serial.println(bus.getStatus());  // выводим код
  }

  if (bus.gotData()) {
    TX_address = bus.getTXaddress();
    Serial.print("Address: "); Serial.println(TX_address);
    switch (TX_address)
    {
      case ADDRESS_MAIN_BLOCK: {
        home data;
        bus.readData(data);
        Settings_Home(&data);
        break;
      }
      default: {
        break;
      }
    }
  }

  // if (bus.gotAck()) {
  //   Serial.println("ASK");
  //   bus.sendAck(bus.getTXaddress());
  // }
}
