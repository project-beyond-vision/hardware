#include "EspMQTTClient.h"
#include "config.h"
#include "MPU9250.h"

MPU9250 mpu; // You can also use MPU9255 as is
unsigned long timer = 0;

EspMQTTClient client(
  WIFISSID,
  WIFIPASSWORD,
  MQTTIP,  // MQTT Broker server ip
  "MQTTUsername",   // Can be omitted if not needed
  "MQTTPassword",   // Can be omitted if not needed
  "TestClient",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("group_05/panic", [](const String & payload) {
    Serial.println(payload);
  });
  // actually dont need anything here for waist
}

void setup(void){
  Serial.begin(115200);
  
  Wire.begin();
  delay(2000); // Wait for mpu to initialize
  mpu.verbose(true); // For debug 
  mpu.setup(0x68);  // Set to i2c address of mpu
}

void loop(void){
  client.loop();
  
  Serial.print("Sending...");

  if (millis() - timer >= PUBLISHINTERVAL) { //50 milliseconds
    if (mpu.update()) {
      String payload = "{";
      payload.concat("\"x\":");
      payload.concat(mpu.getAccX());
      payload.concat(",\"y\":");
      payload.concat(mpu.getAccY());
      payload.concat(",\"z\":");
      payload.concat(mpu.getAccZ());
      payload.concat(",\"rx\":");
      payload.concat(mpu.getGyroX());
      payload.concat(",\"ry\":");
      payload.concat(mpu.getGyroY());
      payload.concat(",\"rz\":");
      payload.concat(mpu.getGyroZ());
      payload.concat("}");
      client.publish(IMUTOPIC, payload);
    }
  }
}
