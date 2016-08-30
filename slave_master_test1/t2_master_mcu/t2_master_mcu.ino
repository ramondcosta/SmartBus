#include <ESP8266WiFi.h>
#include <pgmspace.h>
#include <SoftwareSerial.h>

// Debug Software Serial
SoftwareSerial ESPSerial(12, 13);                 //Extra2 == 12 Extra3 == 13

static const char PROGMEM Line1[] = {"String 1"};
static const char PROGMEM Line2[] = {"String 2"};
static const char PROGMEM Line3[] = {"String 3"}; 

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

}

char req[200];
void setup(){
	Serial.begin(115200);
  ESPSerial.begin(115200);
  ESPSerial.println("ESP8266 ready and waiting for instructions!");
  //strcpy(&subsc_topic[4], device_name);

}
int count = 0;
void loop(){
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
