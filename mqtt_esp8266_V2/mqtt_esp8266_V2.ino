#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int dataLine = 2;

const char* ssid = "MyNetwork";
const char* password = "password";
const char* mqtt_server = "192.168.4.1";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

String subTopic = "Lampen";
String newTopic;
String message;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  newTopic = topic; // ???
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
        Serial.println("connected");
        // Once connected, publish an announcement...
        client.publish("Lampen", "Lamp 1 connected");
        // ... and resubscribe
        client.subscribe("Lampen");
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
  //Serial.begin(115200);
  pinMode(dataLine, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


void loop() {
  if (!client.connected()) {
      reconnect();
  }
  client.loop();

  if(newTopic.equals(subTopic)) {
    if(message.equals("On")) {
      onMsg();
    }
    if(message.equals("Off")) {
      offMsg();
    }
  }
}

void onMsg() {
  digitalWrite(dataLine, LOW);
  clearMsg();
}

void offMsg() {
  digitalWrite(dataLine, HIGH);
  clearMsg();
}

void clearMsg(){
  message = "";
  newTopic = "";
}
