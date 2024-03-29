#define LED D4 // Use the onboard Uno LED
#define obstaclePin D3  // This is our input pin
#define enablePin D2  // This is our input pin
int isObstacle = HIGH;  // HIGH MEANS NO OBSTACLE

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(obstaclePin, INPUT);
  pinMode(enablePin, OUTPUT);
  Serial.begin(9600);  
}

void loop() {
  digitalWrite(enablePin, HIGH); 
  isObstacle = digitalRead(obstaclePin);
  if (isObstacle == LOW)
  {
    Serial.println("OBSTACLE!!, OBSTACLE!!");
    digitalWrite(LED, HIGH);
  }
  else
  {
    Serial.println("clear");
    digitalWrite(LED, LOW);
  }
  Serial.println(isObstacle);
  delay(200);
}
