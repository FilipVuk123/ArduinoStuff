#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const int DEBOUNCE_DELAY = 250;
int lastSteadyState = LOW;      
int lastFlickerableState = LOW; 
int currentState;      
unsigned long lastDebounceTime = 0;         

const int buttonPin = 4;
const int ledPin =  2;  
int ledon = 0;
int broken_wifi = 0;

const char broker[] = "broker.hivemq.com";
int        port     = 1883;
const char topic[]  = "aibnfjnevakjebvaikvc";
const int connectTimeoutMs = 5000;
long lastMillis = 0;

int button_pressed = 0;


/*************************************************************************/


void setup() {
  Serial.begin(115200);
  delay(50);
  pinMode(ledPin, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  wifiMulti.addAP("FilipNet", "Jedrilica");
  Serial.println("Connecting Wifi...");
  if(wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }else{
    while (1){
      errorLed(ledPin);
      delay(2000);
    }
  }
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1){
      errorLed(ledPin);
      delay(2000);
    }
  }
  Serial.println("You're connected to the MQTT broker!");
  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(topic);
  Serial.println();
  goodLed(32);
  goodLed(ledPin);
}
/***************************************************************************************************/
void loop() {
  
  if (wifiMulti.run(connectTimeoutMs) != WL_CONNECTED) {
    Serial.println("WiFi not connected! Reconnecting...");
    broken_wifi = 1;
    errorLed(ledPin);
  } else {
    if(broken_wifi){
      broken_wifi = 0;
      Serial.println("WiFi connected!");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      if (!mqttClient.connect(broker, port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());
      } else {
        mqttClient.subscribe(topic);
        Serial.println("You're connected to the MQTT broker!");
        goodLed(ledPin);
        Serial.println();
      }
    }
  }
  mqttClient.poll();

  // Button debouncing
  currentState = digitalRead(buttonPin);
  if (currentState != lastFlickerableState) {
    lastDebounceTime = millis();
    lastFlickerableState = currentState;
  }
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (lastSteadyState == HIGH && currentState == LOW){
      Serial.println("Button pushed!");
      button_pressed = 1;
    }
    lastSteadyState = currentState;
  }

  if (button_pressed){
    mqttClient.beginMessage(topic);
    mqttClient.print(1);
    mqttClient.endMessage();
    button_pressed = 0;
  }
  
  if(ledon){
    digitalWrite(ledPin, HIGH);
    lastMillis = millis();
    ledon = 0;
  }
  
  if (millis() - lastMillis > 2000){
    digitalWrite(ledPin, LOW);
  }
  delay(20);
  
}
/**************************************************************************************************/
void onMqttMessage(int messageSize) {
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.print(" bytes: ");
  Serial.println((char)mqttClient.read());

  // use the Stream interface to print the contents
  if((int)mqttClient.read() == -1){
    ledon = 1;
  }else{
    ledon = 0;
  }

  Serial.println();
  Serial.println();
}



void goodLed(int led){
  digitalWrite(led, HIGH);
  delay(5000);
  digitalWrite(led, LOW);
}

void errorLed(int led){
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
}
