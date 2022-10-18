// SENSOR PINS
#define US_ECHO D6
#define US_TRIGGER D7
#define BUZZER D4
#define SWITCH D5

#define THRESHOLD 200
#define DISTANCE_THRESHOLD 20.00
#define DISTANCE_EDGE_THRESHOLD 1.00

// DEBOUNCING
volatile byte is_pressed = LOW;
byte button_state = LOW;
byte sound_state = LOW;
unsigned short cnt = 0;
unsigned long last_sound_time = 0;
unsigned long last_pressed_time = 0;

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

void setup() {
  pinMode(US_TRIGGER, OUTPUT);
  pinMode(US_ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  pinMode(SWITCH, INPUT_PULLUP);
  attachInterrupt(
  digitalPinToInterrupt(SWITCH), toggle, RISING);
  Serial.begin(19200);
}

void loop() {
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
    Serial.println("Toggle On");
  } else {
    panic_state = LOW;
    Serial.println("Toggle Off");
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
//  digitalWrite(US_TRIGGER, LOW);
//  delayMicroseconds(2);
//  digitalWrite(US_TRIGGER, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(US_TRIGGER, LOW);
//  duration = pulseIn(US_ECHO, HIGH); // return time in microseconds
//  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (to and fro)
//  // Displays the distance on the Serial Monitor
//  Serial.print("Distance: ");
//  Serial.print(distance);
//  Serial.println(" cm");
//
//  if(distance < DISTANCE_EDGE_THRESHOLD) {
//    distance_state = LOW;
//  }
//  else if(distance < DISTANCE_THRESHOLD)
//    distance_state = HIGH;
//  else
//    distance_state = LOW;
//    

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
