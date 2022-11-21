
struct com {
  byte comand;          // Принимает значение команд
  byte data[3];         // 
};

#define chg_mode 10             // Выбирает режим работы:   [st, peri] Hellou, GoodNight, GM, USSR, Breakfast, Chill 
#define chg_color 20            // Отправляет режим значение [R, G, B]
#define chg_bri 21              // Отправляет изменение яркости
#define chg_rosetka 30          // Принимает знаечение status включить/выключить розетку
#define chg_serv 40             // Отправляет режим работы сервы [v, stop_close] (скорость закрытие, максимальное закрытие)

#define REG_HELLOU      23
#define REG_GOODNIGHT   24
#define REG_GM          25
#define REG_USSR        26
#define REG_BREAKFAST   27
#define REG_CHILL       28
#define REG_BUY         29

#define ADDRESS_HOME          3
#define ADDRESS_SMART_CLASS   4
#define ADDRESS_BLOCK_BRI     5
#define ADDRESS_MAIN_BLOCK    6

#define LUMP_ON     10
#define LUMP_OFF    20

#define CURTAINS_OPEN     10
#define CURTAINS_CLOSE    20

#define WINDOWS_OPEN      10
#define WINDOWS_CLOSE     20

#define LED_ON            15
#define LED_OFF           25

/*
        3 - адресс дома
        4 - адресс умного класса
        5 - адресс блока освещения
        6 - адресс управляющего блока
*/

struct blok_bri {               // Данные с датчика освещенности для умного класса
  byte bri;                     // Яркость с датчика освещенности
};

struct home {                   // Данные от блока управение для дома
  int stat_kran;               // Состояние крана
  int stat_curtains;           // Состояние штор
  int stat_lump;                // Включение/выключение 220в лампы
  byte regim_led;               // Режим работы ленты в доме
};

struct smart_class {            // Данные от блока управления для умного класса
  byte time_h;                  // Время в часах для семисегментника
  byte time_m;                  // Время в минутах для семисегментника
  bool stat_windows;            // Состояние окон в классе
  bool stat_light;              // Вкючение/выключение освещения в классе
};

