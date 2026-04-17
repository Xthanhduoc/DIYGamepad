//
#define Up 2
#define Right 25
#define Down 5
#define Left 4
#define JoyRbtn 27
#define SDA 21
#define SCL 22

void setup() {
  Serial.begin(9600);

  pinMode(Up, INPUT_PULLUP);
  pinMode(Right, INPUT_PULLUP);
  pinMode(Down, INPUT_PULLUP);
  pinMode(Left, INPUT_PULLUP);
  pinMode(JoyRbtn, INPUT_PULLUP);
  

}

void loop() {
  
}
