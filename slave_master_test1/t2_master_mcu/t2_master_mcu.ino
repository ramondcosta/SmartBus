#include <ESP8266WiFi.h>
#include <pgmspace.h>
#include <SoftwareSerial.h>
#include <PubSubClient.h>
// Debug Software Serial
SoftwareSerial ESPSerial(12, 13);                 //Extra2 == 12 Extra3 == 13

// System variables
WiFiClient    espClient;
PubSubClient  client(espClient);

static const char PROGMEM Line1[] = {"String 1"};
static const char PROGMEM Line2[] = {"String 2"};
static const char PROGMEM Line3[] = {"String 3"}; 

// Update these with values suitable for your network.
const char*   device_name     = "ufbaino04";
const char*   mqtt_user       = "wiser";
const char*   mqtt_pass       = "wiser2014";
const int     mqttport        = 1883;

// Constants to connection with the broker
const char*   ssid            = "wiser";          //"PiratasDoValeDoCanela"
const char*   password        = "wiser2014";      //"naovaqueebarril"
const char*   mqtt_server     = "192.168.0.112";  //"192.168.0.103"
char          subsc_topic[20] = "dev/";

/*
  Sensor ID's defines
*/
//  <sensorsID>
#define ID_current  0
#define ID_temp     1
#define ID_humid    2
#define ID_lumin    3
//  </sensorsID>

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  ESPSerial.println();
  ESPSerial.print("Connecting to ");
  ESPSerial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    ESPSerial.print(".");
  }

  ESPSerial.println("");
  ESPSerial.println("WiFi connected");
  ESPSerial.println("IP address: ");
  ESPSerial.println(WiFi.localIP());
 
}
void reconnect() {
 
  // Loop until we're reconnected
  while (!client.connected()) {
    ESPSerial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(device_name, mqtt_user, mqtt_pass)) {
      ESPSerial.println("connected");
      // Once connected, publish an announcement...
      client.publish("CONNECTED", device_name);
      // ... and resubscribe
      client.subscribe("dev/");
      client.subscribe(subsc_topic);
    }
    else {
      ESPSerial.print("failed, rc = ");
      ESPSerial.print(client.state());
      ESPSerial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

}
void callback(char* topic, byte* payload, unsigned int length) {
 
  ESPSerial.print("Message arrived [");
  ESPSerial.print(topic);
  ESPSerial.print("] ");
 
  //Envia mensagem que chegou
  for (int i = 0; i < length; i++) {
    ESPSerial.write((char)payload[i]);
    Serial.write((char)payload[i]);
  }

  ESPSerial.println();
  Serial.println();
  delay(100);
}



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
  
  return get(id, response);
}
bool get(uint8_t id, char* response) {
  //ATMSerial.println("aki2");
  double aux;
  char req[5] = "s:";
  snprintf(&req[2], sizeof(req), "%d", id);
  callback("",req,strlen(req));
  
  return true;
}

void read_serial(){
  int i;
  char msg[255];
  char topic[20];

  ESPSerial.println("ATMEGA Response");
  ESPSerial.print("Topic = ");
  for(i = 0; Serial.available(); i++ ){
    topic[i] = Serial.read();
    if(topic[i] <= 0) i--;
    if(topic[i] == '>') break;
    ESPSerial.print(topic[i]);
  }
  ESPSerial.println();
 
  topic[i] = 0;

  ESPSerial.print("Message = ");
  for(i = 0; Serial.available(); i++){
    msg[i] = Serial.read();
    if(msg[i] <= 0) i--;
    ESPSerial.print(msg[i]);
    if(msg[i] == '\n') break;
  }
  ESPSerial.println();
  msg[i] = 0;
  i = strlen(msg);
  if (msg[i - 1] == '\"'){
    msg[i] = '}';
    msg[i+1] = '}';
    msg[i+2] = 0;
  }  



  client.publish(topic, msg);
}

//
void pubResponse(uint8_t id,,){

}

char req[200];
void setup(){
	
  Serial.begin(115200);
  ESPSerial.begin(115200);
  ESPSerial.println("ESP8266 ready and waiting for instructions!");
  setup_wifi();

  strcpy(&subsc_topic[4], device_name);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  client.subscribe("dev/");
  client.subscribe(subsc_topic);

  client.publish("dev/CONNECTIONS", device_name);

  //strcpy(&subsc_topic[4], device_name);

}
int count = 0;
void loop(){
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

	if(Serial.available()){
    	read_serial();
  	}

  	if(count < 6){
	  	delay(50);
	  	//strcpy(req,"SET INFO temperatureSensor {\"collect\":400,\"publish\":2000}");
      strcpy(req,"s:1");
	  	callback("dev/ufbaino03",(byte*)req,strlen(req));
	  	count++;
	}
  	//delay(10000);
  	
}
