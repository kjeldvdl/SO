#include <ESP8266WiFi.h>
#include "uMQTTBroker.h"

/*
 * Your WiFi config here
 */
char ssid[] = "MyNetwork";     // your network SSID (name)
char pass[] = "password"; // your network password
bool WiFiAP = true;      // Do yo want the ESP as AP?

String cTopic;
String cData;

int dataIn = 2;
int lastData = 0;

/*
 * Custom broker class with overwritten callback functions
 */
class myMQTTBroker: public uMQTTBroker {
public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
      Serial.println(addr.toString()+" connected");
      return true;
    }
    
    virtual bool onAuth(String username, String password) {
      Serial.println("Username/Password: "+username+"/"+password);
      return true;
    }
    
    virtual void onData(String topic, const char *data, uint32_t length) {
      char data_str[length+1];
      os_memcpy(data_str, data, length);
      data_str[length] = '\0';
      
      Serial.println("received topic '"+topic+"' with data '"+(String)data_str+"'");
      cTopic = topic;
      cData = (String)data_str;
    }
};

myMQTTBroker myBroker;

/*
 * WiFi init stuff
 */
void startWiFiClient() {
  Serial.println("Connecting to "+(String)ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void startWiFiAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void setup() {
  pinMode(dataIn, INPUT);
  
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  
  delay(5000);

  // Start WiFi
  if (WiFiAP)
    startWiFiAP();
  else
    startWiFiClient();

  // Start the broker
  Serial.println("Starting MQTT broker");
  myBroker.init();


  // Subscribe to anything
  myBroker.subscribe("#");
}

void loop() {
  if(digitalRead(dataIn) == HIGH) {
    if(lastData == 0) {
      myBroker.publish("Lampen", "On"); // turn all lights on in "Lampen"
      lastData = 1;
    }
  }
  else {
    if(lastData == 1) {
      myBroker.publish("Lampen", "Off"); // turn all lights off in "Lampen"
      lastData = 0;
    }
  }
  
  
  // test for return message
  if(cTopic.equals("test") && cData.equals("Hoi")) {
    message();
  }
}

void message() {
  myBroker.publish("test", "hello to u 2");
  clearData();
}

void clearData() {
  cTopic = "";
  cData = "";
}
