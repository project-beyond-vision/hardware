#include <EspMQTTClient.h>
#include "config.h"
#define MPU6500_ADDR 0x68

// SENSOR PINS
#define US_ECHO D6
#define US_TRIGGER D7
#define BUZZER D4
#define SWITCH D5
#define SWITCH2 D8

#define THRESHOLD 200
#define DISTANCE_THRESHOLD 20.00
#define DISTANCE_EDGE_THRESHOLD 1.00
#define PANIC_THRESHOLD 5000 // 5 seconds

EspMQTTClient client(
  ssid,
  wifiPassword,
  mqttBrokerIp,  // MQTT Broker server ip
  "MQTTUsername",   // Can be omitted if not needed
  "MQTTPassword",   // Can be omitted if not needed
  "cane",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

// DEBOUNCING
volatile byte is_pressed = LOW;
byte button_state = LOW;
byte sound_state = LOW;
unsigned short cnt = 0;
unsigned long last_sound_time = 0;
unsigned long last_pressed_time = 0;
unsigned long time_since_panic = 0;
unsigned long time_since_fire = 0;

// flame sensor connected to analog pins
// lowest and highest flame sensor readings:
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum

// Ultrasonic
float duration;
float distance;

// USER STATES
byte flame_state = LOW;
byte distance_state = LOW;
byte panic_state = LOW;

IRAM_ATTR void toggle() {
  is_pressed = HIGH;
}

// necessary method for mqtt
void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("group_05/stick/panic", [](const String & payload) {
    // this should sound the buzzer
    Serial.println(payload);
  });
}

void setup() {
  pinMode(US_TRIGGER, OUTPUT);
  pinMode(US_ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(SWITCH2, INPUT_PULLUP);
  attachInterrupt(
  digitalPinToInterrupt(SWITCH), toggle, RISING);
  Serial.begin(115200);
}

void loop() {
  client.loop();
  /**
   * 
   * PANIC BUTTON
   * 
   */
  if(is_pressed == HIGH) {
    is_pressed = LOW;
    if(millis() - last_pressed_time > THRESHOLD) {
      button_state = !button_state;
      last_pressed_time = millis();
    }
  }
  
  if (button_state) {
    panic_state = HIGH;
    if (millis() - time_since_panic > PANIC_THRESHOLD) {
      time_since_panic = millis();
    }
    Serial.println("Toggle On");
  } else {
    // TODO: change this part to change panic_state to LOW only if DEASSERT button is pressed
    panic_state = LOW;
    Serial.println("Toggle Off");
  }


  if (panic_state == HIGH && millis() - time_since_panic > PANIC_THRESHOLD) {
     sendMqttMessage("group_05/panic","PANIC BUTTON PRESSED!");
     panic_state = LOW; // deassert panic state after 5 seconds and sending mqtt message
  }
  /**
   * 
   * FLAME SENSOR
   * 
   */
  // read the sensor on analog A0:
  int sensorReading = analogRead(A0);
  // map the sensor range (four options):
  // ex: 'long int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);

  // range value:
  switch (range) {
  case 0:    // A fire closer than 1.5 feet away.
    Serial.println("** Close Fire **");
    // play immediate danger sound
    // SEND MESSAGE TO CLOSE CONTACT WITH GPS INFO + FIRE
    // send 1 message and block sending new messages until PANIC_THRESHOLD seconds pass
    if (millis() - time_since_fire > PANIC_THRESHOLD) {
      time_since_fire = millis();
      sendMqttMessage("group_05/flame", "FIRE DETECTED NEAR USER!");
    }
    flame_state = HIGH;
    break;
  case 1:    // A fire between 1-3 feet away.
    Serial.println("** Distant Fire **");
    flame_state = LOW;
    break;
  case 2:    // No fire detected.
    Serial.println("No Fire");
    flame_state = LOW;
    break;
   default:
    flame_state = LOW;
    break;
  }
  
  /**
   * 
   * ULTRASONIC
   * 
   */
  digitalWrite(US_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIGGER, LOW);
  duration = pulseIn(US_ECHO, HIGH); // return time in microseconds
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (to and fro)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if(distance < DISTANCE_EDGE_THRESHOLD) {
    distance_state = LOW;
  }
  else if(distance < DISTANCE_THRESHOLD)
    distance_state = HIGH;
  else
    distance_state = LOW;
 

  // PLAY BUZZER
  if(panic_state == HIGH || flame_state == HIGH || distance_state == HIGH) {
    playBuzzer();
    Serial.println(panic_state);
    Serial.println(flame_state);
    Serial.println(distance_state);
  }
  else
    digitalWrite(BUZZER, LOW);
}

void playBuzzer() {
  if (millis() - last_sound_time > 50) {
    cnt++;
    last_sound_time = millis();
  }

  if (cnt < 3)
    digitalWrite(BUZZER, HIGH);
  else if (cnt < 5)
    digitalWrite(BUZZER, LOW);
  else
    cnt = 0;
}

void sendMqttMessage(String topic, String message) {
  // client.publish("group_05/imu/threshold", "fall triggered");
  client.publish(topic, message);
}
