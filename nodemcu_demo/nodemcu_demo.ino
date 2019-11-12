/*
{"broker_host":"smartcampus.inatel.br:1883",
"topic_to_save":"application_5dca9e86767120003b698e8f_device_5dca9e99767120003b698e96/save",
"topic_to_listen":"application_5dca9e86767120003b698e8f_device_5dca9e99767120003b698e96/listen",
"user":"5dca9e99767120003b698e96",
"password":"4ef5QDWuPBpiTN9LGXYuw7Ls"}
 */


#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <String.h>
#include <ArduinoJson.h>

#define WIFI_SSID "WLL-Inatel"                 //Inserir SSID Wifi
#define WIFI_PASSWORD "inatelsemfio"          //Inserir Password Wifi
#define MQTT_SERVER "smartcampus.inatel.br"
#define MQTT_PORT 1883
#define DEV_USERNAME "5dca9e99767120003b698e96"
#define DEV_PASSWD "4ef5QDWuPBpiTN9LGXYuw7Ls"
#define LISTEN_TOPIC "application_5dca9e86767120003b698e8f_device_5dca9e99767120003b698e96/listen"
#define SAVE_TOPIC "application_5dca9e86767120003b698e8f_device_5dca9e99767120003b698e96/save"

#define DELAY 1000
#define LED D4

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;



int value = 0;

String aux = "";
char caractere;
int temperatura=0;



void callback(char* topic, byte* payload, unsigned int length) {
  
  StaticJsonBuffer<200> jsonBuffer;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char json[length];
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    json[i] = (char)payload[i];  
  }
  Serial.println();
  
  JsonObject& root = jsonBuffer.parseObject(json);
  if(!root.success()){
    Serial.println("parseObject() failed");
    return;
  }
  const char* lamp = root["lamp"];
  String lampStatus(lamp);
  if (lampStatus.equalsIgnoreCase("on")){
    digitalWrite(LED, LOW);
  }else if(lampStatus.equalsIgnoreCase("off")){
    digitalWrite(LED, HIGH);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client";
    if (client.connect(clientId.c_str(), DEV_USERNAME, DEV_PASSWD)) {     //Login e senha para a plataforma

      Serial.println("connected to the Platform!");
      String aux = String (LISTEN_TOPIC);
      char topic[aux.length()+1];
      aux.toCharArray(topic, aux.length()+1);
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}






void setup() {
  Serial.begin(9600);
  
  pinMode(LED, OUTPUT);
  
  
  // connecta ao Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("conectando");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Conectado: ");
  Serial.println(WiFi.localIP());

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  
  
}





void loop() 
{
   
   
   if (!client.connected()) {
      reconnect();
   }
   client.loop();
   delay(10);

   
   long now = millis();
   
   if (now - lastMsg > 5000) {
      lastMsg = now;

      //Monta o tópico
      String aux = String (SAVE_TOPIC);
      char topic[aux.length()+1];
      aux.toCharArray(topic, aux.length()+1);

      //Monta o payload
      temperatura = random(20, 30);
      String str = "{\"temperature\":" + String(temperatura) +" }";
      int lng = str.length();
      char msg[lng+1];
      snprintf (msg, lng+1, "{\"temp\":%.2d }", temperatura);
      
      Serial.println();
      Serial.print("Publicando temperatura: ");
      Serial.println(str);
      Serial.println(msg);
      client.publish(topic,msg);
   }
   
   
   

}
