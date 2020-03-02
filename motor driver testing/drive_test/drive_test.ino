
//pin defines
//lift driver
#define LIFT_LPWM 3
#define LIFT_RPWM 5
#define LIFT_EN 4

//steering driver
#define STEER_LPWM 10
#define STEER_RPWM 11
#define STEER_EN 8
#define STEER_IS_L 2  //analog
#define STEER_IS_R 3  //analog

//drive driver
#define DRIVE_LPWM 6
#define DRIVE_RPWM 9
#define DRIVE_EN 7
#define DRIVE_IS_L 0  //analog
#define DRIVE_IS_R 1  //analog

int incomingByte = 0; // for incoming serial data

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(DRIVE_EN, OUTPUT);
  pinMode(DRIVE_LPWM, OUTPUT);
  pinMode(DRIVE_RPWM, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    if(incomingByte == 48){
      digitalWrite(DRIVE_EN, HIGH);
      digitalWrite(DRIVE_LPWM, LOW);
      digitalWrite(DRIVE_RPWM, LOW);
    }
    if(incomingByte == 49){
      analogWrite(DRIVE_LPWM, 32);
      digitalWrite(DRIVE_RPWM, LOW);
    }
    if(incomingByte == 50){
      digitalWrite(DRIVE_LPWM, LOW);
      analogWrite(DRIVE_RPWM, 32);
    }
  }
}
