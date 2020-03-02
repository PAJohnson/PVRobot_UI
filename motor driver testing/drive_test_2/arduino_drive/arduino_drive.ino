/*
drive testing code
*/

//TODO
//I2C for lift TOF sensors
//Read encoders for steering (analog) and drive (digital)
//PID for steering and drive
//Conversion for rosserial

#define STEER_RAMP 10 //1 percent of full scale per cycle time
#define DRIVE_RAMP 10 //1 percent of full scale per cycle time
#define DRIVE_MAX 255
#define STEER_MAX 255
#define DRIVE_MIN 255
#define STEER_MIN 255

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

const int ledPin = 13; // the pin that the LED is attached to
int incomingByte;      // a variable to read incoming serial data into
unsigned long time_new;
unsigned long time_old;

volatile int drive_cmd = 0;
volatile int steer_cmd = 0;
volatile int drive_act = 0;
volatile int steer_act = 0;

void drive(int drive_act){
  //RPWM forward, LPWM reverse
  if(drive_act == 0){
    digitalWrite(DRIVE_EN, LOW);
    analogWrite(DRIVE_LPWM, 0);
    analogWrite(DRIVE_RPWM, 0);
  }
  else if(drive_act > 0){
    digitalWrite(DRIVE_EN, HIGH);
    analogWrite(DRIVE_LPWM, drive_act);
    analogWrite(DRIVE_RPWM, 0);
  }
  else if(drive_act < 0){
    digitalWrite(DRIVE_EN, HIGH);
    analogWrite(DRIVE_LPWM, 0);
    analogWrite(DRIVE_RPWM, drive_act*(-1));
  }
}

void steer(int steer_act){
  //RPWM right, LPWM left
  if(steer_act == 0){
    digitalWrite(STEER_EN, LOW);
    analogWrite(STEER_LPWM, 0);
    analogWrite(STEER_RPWM, 0);
  }
  else if(steer_act > 0){
    digitalWrite(STEER_EN, HIGH);
    analogWrite(STEER_LPWM, steer_act);
    analogWrite(STEER_RPWM, 0);
  }
  else if(steer_act < 0){
    digitalWrite(STEER_EN, HIGH);
    analogWrite(STEER_LPWM, 0);
    analogWrite(STEER_RPWM, steer_act*(-1));
  }
}

void halt(){
  digitalWrite(ledPin, LOW);
  
}

void setup() {
  //set up timer
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 625; // compare match register for 100Hz rate (10ms cycle)
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts
  
  // initialize serial communication:
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(DRIVE_EN, OUTPUT);
  pinMode(DRIVE_LPWM, OUTPUT);
  pinMode(DRIVE_RPWM, OUTPUT);

  pinMode(STEER_EN, OUTPUT);
  pinMode(STEER_LPWM, OUTPUT);
  pinMode(STEER_RPWM, OUTPUT);
  
  time_new = 0;
  time_old = 0;
}

void loop() {
  // see if there's incoming serial data:
  if (Serial.available() > 0) {
    time_old = time_new;
    time_new = millis();
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    if (incomingByte == 'F') {
      drive_cmd = 255;
      steer_cmd = 0;
    }
    else if (incomingByte == 'B') {
      drive_cmd = -255;
      steer_cmd = 0;
    }
    else if (incomingByte == 'L'){
      drive_cmd = 0;
      steer_cmd = 255;
    }
    else if (incomingByte == 'R'){
      drive_cmd = 0;
      steer_cmd = -255;
    }
    else if (incomingByte == 'S'){
      drive_cmd = 0;
      steer_cmd = 0;
    }
  }
  if((time_new - time_old) > 250){
    drive_cmd = 0;
    steer_cmd = 0;
  }
  drive(drive_act);
  steer(steer_act);
  if(steer_act == 0 && drive_act == 0){
    digitalWrite(ledPin, LOW);
  }
  else{
    digitalWrite(ledPin, HIGH);
  }
}

SIGNAL(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
  //take commands, ramp them, apply to pins.
  //drive ramp
  if((drive_cmd - drive_act) >= DRIVE_RAMP * DRIVE_MAX / 100){
    drive_act += DRIVE_RAMP * DRIVE_MAX / 100;
  }
  else if((drive_cmd - drive_act) <= -1 * DRIVE_RAMP * DRIVE_MAX / 100){
    drive_act -= DRIVE_RAMP * DRIVE_MAX / 100;
  }
  else if(abs(drive_cmd - drive_act) < DRIVE_RAMP * DRIVE_MAX / 100){
    drive_act = drive_cmd;
  }
  else{
    ;
  }
  //steer ramp
  if((steer_cmd - steer_act) >= STEER_RAMP * STEER_MAX / 100){
    steer_act += STEER_RAMP * STEER_MAX / 100;
  }
  else if((steer_cmd - steer_act) <= -1 * STEER_RAMP * STEER_MAX / 100){
    steer_act -= STEER_RAMP * STEER_MAX / 100;
  }
  else if(abs(steer_cmd - steer_act) < STEER_RAMP * STEER_MAX / 100){
    steer_act = steer_cmd;
  }
  else{
    ;
  }
}
