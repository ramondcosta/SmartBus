#include <stdint.h>
#include <string.h>
#include <EmonLib.h>
#include <DHT.h>

#define DEFAULT_ADJUST 3.3 //111.1

// Constants to connection with the broker
#define DEVICE_NAME   "ufbaino03"
#define MQTT_PORT     1883

// Pins used
#define CURRENT01     A6
#define LUMINOSITY    A4
#define DHTTYPE       11
#define INTTIME       1
#define DHTPIN        4

//Hash that represents the attributes
#define H_temperatureSensor 0x5821FBAD
#define H_luminositySensor  0xF8D7A29C
#define H_currentSensor01   0x1A5B1C23
#define H_humiditySensor    0x83D62D4C

#define int_T     0
#define float_T   1
#define str_T     2

#define sensorCount 4

// Allow Software Serial 
#define ENABLE_SOFTWARE_SERIAL

#define DEBUG_PORT ATMSerial
//#define DEBUG_PORT 

// If enabled Software Serial
#ifdef ENABLE_SOFTWARE_SERIAL
#include <SoftwareSerial.h>
// Software Serial should be static since this file can be called multiple times
SoftwareSerial static ATMSerial(7,6);
#endif

/*  This is a example sketch of a simple device who sends simple values to in response
    To another's device request.
    The objective is that he would be ables to...

    That device is in the scenario where he comunicates, through a serial port, 
    with another device.

    Each sensor(or any other funcionality like actuactors) has an associated ID
*/

// Variveis
int                 aux;
DHT                 dht(DHTPIN, DHTTYPE);
float               luz;
float               v_out;
EnergyMonitor       emon1;
volatile int        t;
volatile int        h;
volatile int        luminosity;
volatile int        flag = 1;
volatile double     irms = 0;

byte ip[4] =        { 127, 0, 0, 1 };

//FLOW
unsigned long int lastTime;
char vector_response[60];


typedef struct sensorStruct{
  int id;
  uint8_t type;
}sensorStruct;

sensorStruct sensores[sensorCount];

bool set(uint32_t hash, uint8_t code, void* response) {
  ATMSerial.println("CHEGOU ");

  switch (hash) {
    case H_temperatureSensor:
      break;
    default:
      return false;
  }
  return true;
} 

double current_sensor(EnergyMonitor emon){
  return emon.calcIrms(1480);

}
int luminosity_sensor(int PIN){
  return analogRead(PIN);
}

int dht_temperature_sensor(DHT dht){
  return (int)dht.readTemperature();
}
int dht_humidity_sensor(DHT dht){
  return (int)dht.readHumidity();
}

#define ID_current  0
#define ID_temp     1
#define ID_humid    2
#define ID_lumin    3

bool mapID(uint32_t hash, void* response, uint8_t code) {
  uint8_t id;
  switch (hash) {
    case H_currentSensor01:
      id = ID_current;
      break;
    case H_temperatureSensor:
      // The dht_temperatures_sensor supports INFO and VALUE requests.
      id = ID_temp;
      break;
    case H_humiditySensor:
      // The dht_humidity_sensor supports INFO and VALUE requests.
      id = ID_humid;
      break;
    case H_luminositySensor:
      // The lumisity_sensor supports INFO and VALUE,requests.
      id = ID_lumin;
      break;
    default:
      return false;
  }
  //get(id, response);
  return true;
}

bool get(uint8_t id, char* response) {
  //ATMSerial.println("aki2");
  double aux;
  switch(id){
    case ID_current:
      aux = current_sensor(emon1);
      break;
    case ID_temp:
      // The dht_temperatures_sensor supports INFO and VALUE requests.
      aux = dht_temperature_sensor(dht);
      break;
    case ID_humid:
      // The dht_humidity_sensor supports INFO and VALUE requests.
      aux = dht_humidity_sensor(dht);
      break;
    case ID_lumin:
      // The lumisity_sensor supports INFO and VALUE,requests.
      aux = luminosity_sensor(LUMINOSITY);
      break;
    default:
      //ATMSerial.println("falso!");
      return false;
  }
  //ATMSerial.println("aki3");
  switch(sensores[id].type){
      case float_T:
        ATMSerial.println("É float");
        dtostrf(aux,2,2,response);
        // do something
        break;
      case int_T:
        ATMSerial.println("É int");
        itoa((int)aux,response,10);
        // do something
        break;
      default:
        ATMSerial.println("É alguma coisa...");
        return false;
  }
  Serial.print(id);
  Serial.print(">");
  Serial.println(response);
  ATMSerial.print(id);
  ATMSerial.print(">");
  ATMSerial.println(response);
  return true;
}

void serial_publish(char *topic, char *output) {
  Serial.print(topic);
  Serial.print('>');
  Serial.println(output);
}

void callback(char* buffer,int length){
  //ATMSerial.println("aki1");
  char random[20];
  get(atoi(&buffer[2]),random);
}

void serial_read(char readch, byte *buffer, int len) {
  static int pos = 0;

  if (readch > 0) {
    switch (readch) {
      case '\n':
        //ATMSerial.print("MESSAGE RECEIVED ::::: ");
        //ATMSerial.print("RECEIVED:");
        ATMSerial.println((char*)buffer);
        callback((char*)buffer, pos);

        //delay(1500);
        pos = 0;
        break;
      default:
        if (pos < len - 1) {
          if (readch == '\r') return;
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
}
// SETUP
void setup() {

  Serial.begin(115200);
  ATMSerial.begin(115200);
  dht.begin();
  pinMode(DHTPIN, INPUT);

  emon1.current(CURRENT01, DEFAULT_ADJUST);

  sensores[0].type = float_T;
  sensores[1].type = int_T;
  sensores[2].type = float_T;
  sensores[3].type = int_T;

  ATMSerial.println("ATMEGA ready and waiting for instructions!");

}

// LOOP
void loop() {
  static byte buffer[100];
  serial_read(ATMSerial.read(), buffer, 100);

}