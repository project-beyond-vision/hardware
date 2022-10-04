#define SWITCH D4
#define BUZZER D3

volatile byte is_pressed = LOW;
byte button_state = LOW;
byte sound_state = LOW;
unsigned short cnt = 0;
unsigned long last_sound_time = 0;
unsigned long last_pressed_time = 0;
#define THRESHOLD 200

IRAM_ATTR void toggle() {
  is_pressed = HIGH;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  pinMode(SWITCH, INPUT_PULLUP);
  attachInterrupt(
  digitalPinToInterrupt(SWITCH), toggle, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(is_pressed == HIGH) {
    is_pressed = LOW;
    if(millis() - last_pressed_time > THRESHOLD) {
      button_state = !button_state;
      last_pressed_time = millis();
    }
  }
  
  if (button_state) {
    Serial.println("Toggle On");
    playBuzzer();
  } else {
    digitalWrite(BUZZER, LOW);
    Serial.println("Toggle Off");
  }
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
