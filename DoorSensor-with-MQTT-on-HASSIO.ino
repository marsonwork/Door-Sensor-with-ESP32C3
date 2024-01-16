#include <PubSubClient.h>
#include <WiFi.h>

#define DOOR_SENSOR_PIN  D1  // ESP32 pin GPIO19 connected to door sensor's pin

#define MQTT_SERVER "192.168.29.159"  // Enter your MQTT broker address
#define MQTT_PORT 1883                      // Enter your MQTT broker port
#define MQTT_USER "mqtt-user"           // Enter your MQTT username
#define MQTT_PASSWORD "o$Lmqtt!@1729"       // Enter your MQTT password
#define MQTT_CLIENT_ID "office door"        // Unique client ID for the door sensor

WiFiClient espClient;
PubSubClient client(espClient);

int currentDoorState; // current state of door sensor
int lastDoorState;    // previous state of door sensor

void setup() {
  Serial.begin(115200);                     // initialize serial
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // set ESP32 pin to input pull-up mode

  currentDoorState = digitalRead(DOOR_SENSOR_PIN); // read state

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi");
  WiFi.begin("Potato", "Qwertyuiop!@1729");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  lastDoorState = currentDoorState;
  currentDoorState = digitalRead(DOOR_SENSOR_PIN);

  if (lastDoorState == LOW && currentDoorState == HIGH) {
    Serial.println("The door-opening event is detected");
    client.publish("office/balcony/door", "OPEN");  // Publish MQTT message for door open
    // TODO: turn on alarm, light or send notification ...
  } else if (lastDoorState == HIGH && currentDoorState == LOW) {
    Serial.println("The door-closing event is detected");
    client.publish("office/balcony/door", "CLOSED");  // Publish MQTT message for door closed
    // TODO: turn off alarm, light or send notification ...
  }
}
