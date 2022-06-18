#include "DHT.h" // Include DHT library
//#include "HX711.h"
#include <ArduinoJson.h>  // https://arduinojson.org/
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient
#include <WiFi.h>

#define DHT_PIN 22     // Defines pin number to which the sensor is connected
#define DHT_TYPE DHT11 // Defines the sensor type. It can be DHT11 or DHT22

//#define LOADCELL_DOUT_PIN 19;
//#define LOADCELL_SCK_PIN 18;
//HX711 scale;

// Replace the next variables with your Wi-Fi SSID/Password
const char *WIFI_SSID = "MIWIFI_Acj3";
const char *WIFI_PASSWORD = "bYCdNGFn";
char macAddress[18];

const char *MQTT_BROKER_IP = "iiot-upc.gleeze.com";
const int MQTT_PORT = 1883;
const char *MQTT_USER = "iiot-upc";
const char *MQTT_PASSWORD = "cimupc";
const bool RETAINED = true;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
DHT dhtSensor(DHT_PIN, DHT_TYPE); // Defines the sensor

void setup() {
  Serial.begin(9600);
  Serial.println("\nBooting device...");
    
  dhtSensor.begin(); 
  
  mqttClient.setServer(MQTT_BROKER_IP,MQTT_PORT);
  
  connectToWiFiNetwork(); 
  connectToMqttBroker();  
}

void loop() {
  checkConnections();
          
  static float t_camion = dhtSensor.readTemperature();
  static float lat_camion = 42.05606;
  static float lon_camion = 2.05234;
  static float h_fabrica = dhtSensor.readHumidity();           
  static float t_fabrica = dhtSensor.readTemperature();
  static float h_granja = dhtSensor.readHumidity(); 
  static float t_granja = dhtSensor.readTemperature();
  static float tc_granja = dhtSensor.readTemperature();
  static float h_oficina = dhtSensor.readHumidity();
  static float t_oficina = dhtSensor.readTemperature();
  static float t_vaca = dhtSensor.readTemperature();
  static float lat_vaca = 42.606;
  static float lon_vaca = 2.234;
  static float comida = 100;  //scale.get_units(10);
  static float peso = 540;   //scale.get_units(10);
    
  static int nowTime = millis();
  static int startTime = 0;
  static int elapsedTime = 0;
  nowTime = millis();
  elapsedTime = nowTime - startTime;
  if (elapsedTime >= 5000) {
    Vacas(t_vaca);
    Granjas(t_granja,h_granja,tc_granja);
    Camiones(t_camion);
    Fabrica(t_fabrica,h_fabrica);
    Oficina(t_oficina,h_oficina);
    startTime = nowTime;
  }
}


void Vacas(float t_vaca) {
  static const String topicStr = String("DMENGINEERING") + "/"  + "vacas";
  static const char *topic = topicStr.c_str();

  StaticJsonDocument<128> doc; // Create JSON document of 128 bytes
  char buffer[128]; // Create the buffer where we will print the JSON document
                   
  doc["id"] = String(macAddress);
  doc["temp"] = String(t_vaca);
  doc["comida"] = 100;
  doc["peso"] = 537;
  doc["lat"] = String(t_vaca);
  doc["lon"] = String(t_vaca);
    
  // Serialize the JSON document to a buffer in order to publish it
  serializeJson(doc, buffer);
  mqttClient.publish(topic, buffer, RETAINED);
  Serial.println(" <= " + String(topic) + ": " + String(buffer));
}

void Camiones(float t_camion) {
  static const String topicStr = String("DMENGINEERING")+ "/"  + "camiones";
  static const char *topic = topicStr.c_str();

  StaticJsonDocument<128> doc; // Create JSON document of 128 bytes
  char buffer[128]; // Create the buffer where we will print the JSON document
                    // to publish through MQTT

  doc["id"] = String(macAddress); // Add names and values to the JSON document
  doc["temp"] = String(t_camion);
  doc["sol"] = randomBoolean(random(2));
  
  // Serialize the JSON document to a buffer in order to publish it
  serializeJson(doc, buffer);
  mqttClient.publish(topic, buffer, RETAINED);
  Serial.println(" <= " + String(topic) + ": " + String(buffer));
}

void Fabrica(float t_fabrica,float h_fabrica) {
  static const String topicStr = String("DMENGINEERING")+ "/"  + "fabricas";
  static const char *topic = topicStr.c_str();

  StaticJsonDocument<512> doc; // Create JSON document of 128 bytes
  char buffer[512]; // Create the buffer where we will print the JSON document
                    // to publish through MQTT

  doc["id"] = String(macAddress); // Add names and values to the JSON document
  doc["temp"] = String(t_fabrica);
  doc["hum"] = String(h_fabrica);
  doc["ccf"] = String(random(0,15));
  doc["cco"] = String(random(0,6));
  doc["t1ma"] = String(random(-4,80));
  doc["t2ma"] = String(random(-4,80));
  doc["t1mb"] = String(random(-4,80));
  doc["t2mb"] = String(random(-4,80));
  doc["t1mc"] = String(random(-4,80));
  doc["t2mc"] = String(random(-4,80));
  doc["t1mg"] = String(random(-4,80));
  doc["t2mg"] = String(random(-4,80));
  doc["t1cs"] = String(random(-4,80));
  doc["t2cs"] = String(random(-4,80));

  
  // Serialize the JSON document to a buffer in order to publish it
  serializeJson(doc, buffer);
  mqttClient.publish(topic, buffer, RETAINED);
  Serial.println(" <= " + String(topic) + ": " + String(buffer));
}

void Oficina(float t_oficina,float h_oficina) {
  static const String topicStr = String("DMENGINEERING")+ "/"  + "oficinas";
  static const char *topic = topicStr.c_str();

  StaticJsonDocument<128> doc; // Create JSON document of 128 bytes
  char buffer[128]; // Create the buffer where we will print the JSON document
           
  doc["id"] = String(macAddress); // Add names and values to the JSON document
  doc["temp"] = String(t_oficina);
  doc["hum"] = String(h_oficina);
    
  // Serialize the JSON document to a buffer in order to publish it
  serializeJson(doc, buffer);
  mqttClient.publish(topic, buffer, RETAINED);
  Serial.println(" <= " + String(topic) + ": " + String(buffer));
}

void Granjas(float t_granja,float h_granja,float tc_granja) {
  static const String topicStr = String("DMENGINEERING")+ "/"  + "granjas";
  static const char *topic = topicStr.c_str();

  StaticJsonDocument<256> doc; // Create JSON document of 128 bytes
  char buffer[256]; // Create the buffer where we will print the JSON document

  doc["id"] = String(macAddress); 
  doc["temp"] = String(t_granja);
  doc["hum"] = String(h_granja);
  doc["temp_cuba"] = String(tc_granja/7);
  doc["sol"] = randomBoolean(random(2));
  
  // Serialize the JSON document to a buffer in order to publish it
  serializeJson(doc, buffer);
  mqttClient.publish(topic, buffer, RETAINED);
  Serial.println(" <= " + String(topic) + ": " + String(buffer));
}

//FUNCIONES////////////////////////////////////////////////////////////////////////////////////////////////////////
String createTopic(char *topic) {
  String topicStr = String(macAddress) + "/" + topic;
  return topicStr;
}
String randomBoolean(int x) {
  if(x==0){return "false";}
  else{return "true";}
}

//COMUNICACIONES///////////////////////////////////////////////////////////////////////////////////////////////////
void connectToWiFiNetwork() {
  Serial.print("Connecting with Wi-Fi: " + String(WIFI_SSID)); // Print the network which you want to connect
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".."); // Connecting effect
    }
  Serial.print("..connected!  (ip: "); // After being connected to a network, our ESP32 should have a IP
  Serial.print(WiFi.localIP());
  Serial.println(")");
  String macAddressStr = WiFi.macAddress().c_str();
  strcpy(macAddress, macAddressStr.c_str());
}

void connectToMqttBroker() {
  Serial.print("Connecting with MQTT Broker:" + String(MQTT_BROKER_IP));    // Print the broker which you want to connect
  mqttClient.connect(macAddress, MQTT_USER, MQTT_PASSWORD);// Using unique mac address from ESP32
    while (!mqttClient.connected()) {
      delay(500);
      Serial.print("..");             // Connecting effect
      mqttClient.connect(macAddress); // Using unique mac address from ESP32
    }
  Serial.println("..connected! (ClientID: " + String(macAddress) + ")");
}

void checkConnections() {
    if (mqttClient.connected()) {
      mqttClient.loop();
  } else { // Try to reconnect
   Serial.println("Connection has been lost with MQTT Broker");
    if (WiFi.status() != WL_CONNECTED) { // Check wifi connection
      Serial.println("Connection has been lost with Wi-Fi");
      connectToWiFiNetwork(); // Reconnect Wifi
    }
    connectToMqttBroker(); // Reconnect Server MQTT Broker
  }
}
