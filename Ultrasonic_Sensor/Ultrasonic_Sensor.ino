
#define US_ECHO D3 // attach pin D2 Arduino to pin Echo of HC-SR04
#define US_TRIGGER D5 //attach pin D3 Arduino to pin Trig of HC-SR04

// defines variables
float duration; // variable for the duration of sound wave travel
float distance; // variable for the distance measurement

void setup() {
  pinMode(US_TRIGGER, OUTPUT);
  pinMode(US_ECHO, INPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(US_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIGGER, LOW);
  duration = pulseIn(US_ECHO, HIGH); // return time in microseconds
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(100);
}
