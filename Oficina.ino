#include "DHT.h" // Include DHT library
#include <ArduinoJson.h>  // https://arduinojson.org/
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient
#include <WiFi.h>


#define DHT_PIN 22     // Defines pin number to which the sensor is connected
#define DHT_TYPE DHT11 // Defines the sensor type. It can be DHT11 or DHT22



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
  
  static float h_oficina = dhtSensor.readHumidity();
  static float t_oficina = dhtSensor.readTemperature();

  static int nowTime = millis();
  static int startTime = 0;
  static int elapsedTime = 0;
  nowTime = millis();
  elapsedTime = nowTime - startTime;
  if (elapsedTime >= 2000) {
    publishOficina(t_oficina,h_oficina);
    startTime = nowTime;
  }
}

void publishOficina( float t_oficina, float h_oficina) {
  static const String topicStr1 = String("DMENGINEERING") + "/"  + "planta/oficina" + "/"  + String(macAddress)+ "/" + "temp";
  static const String topicStr2 = String("DMENGINEERING") + "/"  + "planta/oficina" + "/"  + String(macAddress)+ "/" + "hum";
  static const char *topic1 = topicStr1.c_str();
  static const char *topic2 = topicStr2.c_str();
  String text1 = String(t_oficina);
  String text2 = String(h_oficina);
  mqttClient.publish(topic1, text1.c_str(), RETAINED);
  mqttClient.publish(topic2, text2.c_str(), RETAINED);
  Serial.println(" <= " + String(topic1) + ": " + text1);
  Serial.println(" <= " + String(topic2) + ": " + text2);

}

//COMUNICACIONES
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
