#include <Arduino.h>

#include "SPI.h"

#define CS_GO 10 // CS SPI contact

#define TX_RX_COMMAND_PATH "C:\Users\\Admin\\Documents\\PUM_Start_Blynk-main\\TechnoparkStend\\RX_TX_command.h"; // path to tx rx file 

#include TX_RX_COMMAND_PATH

#include <mcp2515.h>

// подключаем софт юарт
#include "softUART.h"
// делаем только приёмником (экономит память)
softUART<4, GBUS_TX> UART(1000); // пин 4, скорость 1000

// подключаем GBUS
#include "GBUS.h"
GBUS bus(&UART, ADDRESS_BLOCK_BRI, 64); // обработчик UART, адрес 5, буфер 20 байт

byte TX_address;

blok_bri data;

#define PHOTO_RESISTOR        A0

byte sensor, last_sensor = 0;

MCP2515 mcp2515(10);

struct can_frame canMsg1;

union float_bytes_t
{
    float value;
    byte bytes[sizeof(float)];
};

void setup() {
  // put your setup code here, to run once:
  pinMode(PHOTO_RESISTOR, INPUT);
  Serial.begin(9600);
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
  mcp2515.reset();
  mcp2515.setNormalMode();
  SPI.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  bus.tick();

  sensor = map(analogRead(PHOTO_RESISTOR), 0, 1024, 0, 255);

  byte korrekt = sensor * 3 / 100;

  /*
  if (sensor > last_sensor + korrekt || sensor < last_sensor - korrekt) {
    last_sensor = sensor;
    Serial.println(sensor);
    data.bri = sensor;

    
    bus.sendData(ADDRESS_SMART_CLASS, data);

  }

  if (bus.statusChanged()) {  // если статус изменился
    Serial.print("Status: "); Serial.println(bus.getStatus());  // выводим код
  }
  */
  canMsg1.can_id  = 0x0F6;
  canMsg1.can_dlc = 1;
  canMsg1.data[0] = sensor;
  
}
