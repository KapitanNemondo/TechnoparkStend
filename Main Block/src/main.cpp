#include <Arduino.h>
#include "C:\Projects\Tech Stend\RX_TX_command.h"

// приём данных по однопроводному юарту

// подключаем софт юарт
#include "softUART.h"
// делаем только приёмником (экономит память)
softUART<4> UART(1000); // пин 4, скорость 1000

// подключаем GBUS
#include "GBUS.h"
GBUS bus(&UART, ADDRESS_MAIN_BLOCK, 64); // обработчик UART, адрес 5, буфер 20 байт

byte TX_address;

bool last_state_tmb_1, last_state_tmb_2, last_state_tmb_3, last_state_tmb_4;
bool tmb_state_on_off_1, tmb_state_on_off_2, tmb_state_on_off_3, tmb_state_on_off_4;

bool tmb_flag;

#define TUMBLER_1     3
#define TUMBLER_2     2
#define TUMBLER_3     5
#define TUMBLER_4     6

home data;

byte count_flag = 0; 

void setup() {
  // put your setup code here, to run once:
  pinMode(TUMBLER_1, INPUT_PULLUP);
  pinMode(TUMBLER_2, INPUT_PULLUP);
  pinMode(TUMBLER_3, INPUT_PULLUP);
  pinMode(TUMBLER_4, INPUT_PULLUP);

  last_state_tmb_1 = !digitalRead(TUMBLER_1);
  last_state_tmb_2 = !digitalRead(TUMBLER_2);
  last_state_tmb_3 = !digitalRead(TUMBLER_3);
  last_state_tmb_4 = !digitalRead(TUMBLER_4);

  // data.stat_lump = LUMP_ON;
  // bus.sendData(ADDRESS_HOME, data);

  Serial.begin(9600);
  
}

void loop() {
  bus.tick();


  static uint32_t tmr;
  if (millis() - tmr >= 300 && count_flag < 5) {
    tmr = millis();
    Serial.println("Send...");
    bus.sendData(ADDRESS_HOME, data);
    count_flag++;
  }

  if (bus.statusChanged()) {  // если статус изменился
    Serial.println(bus.getStatus());  // выводим код
  }
  
  // put your main code here, to run repeatedly:
  tmb_state_on_off_1 = !digitalRead(TUMBLER_1);
  tmb_state_on_off_2 = !digitalRead(TUMBLER_2);
  tmb_state_on_off_3 = !digitalRead(TUMBLER_3);
  tmb_state_on_off_4 = !digitalRead(TUMBLER_4);

  if (tmb_state_on_off_1 != last_state_tmb_1) {
    last_state_tmb_1 = !digitalRead(TUMBLER_1);
    tmb_flag = true;

    data.stat_lump = LUMP_ON;
    data.stat_curtains = REG_GM;

    Serial.println("Lump ON");

    count_flag = 0;

  }

  if (tmb_state_on_off_2 != last_state_tmb_2) {
    last_state_tmb_2 = !digitalRead(TUMBLER_2);
    tmb_flag = true;

    data.stat_lump = LUMP_OFF;
    data.stat_curtains = REG_GOODNIGHT;
    Serial.println("Lump OFF");

    count_flag = 0;
  }

  if (tmb_state_on_off_3 != last_state_tmb_3) {
    last_state_tmb_3 = !digitalRead(TUMBLER_3);
    tmb_flag = true;

    data.stat_lump = LUMP_OFF;
    data.stat_curtains = REG_GOODNIGHT;
    Serial.println("LED OFF");

    count_flag = 0;
  }


  // if ((last_state_tmb_1 != tmb_state_on_off_1) || (last_state_tmb_2 != tmb_state_on_off_2) || (last_state_tmb_3 != tmb_state_on_off_3) ) {
  //   last_state_tmb_1 = !digitalRead(TUMBLER_1);
  //   last_state_tmb_2 = !digitalRead(TUMBLER_2);

  //   tmb_flag = true;

  //   data.stat_lump = LED_ON;

  //   // Serial.print("State Flag: "); Serial.println(tmb_flag);
  //   // Serial.println();
  // }

  

}