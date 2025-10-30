#define LEFT_BUTTON  25
#define RIGHT_BUTTON 26
#define SCROLL_UP    27
#define SCROLL_DOWN  14

void setup() {
  Serial.begin(115200);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(SCROLL_UP, INPUT_PULLUP);
  pinMode(SCROLL_DOWN, INPUT_PULLUP);
}

void loop() {
  Serial.print("L:");
  Serial.print(digitalRead(LEFT_BUTTON));
  Serial.print(" R:");
  Serial.print(digitalRead(RIGHT_BUTTON));
  Serial.print(" SU:");
  Serial.print(digitalRead(SCROLL_UP));
  Serial.print(" SD:");
  Serial.println(digitalRead(SCROLL_DOWN));
  delay(200);
}
