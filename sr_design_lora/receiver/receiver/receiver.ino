// Arduino9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Arduino9x_TX


//PV Robotics Senior Design Project 2019

#define SER_DEBUG 1

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 4
#define RFM95_RST 3
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

//lora error status defines
#define OK 0
#define NO_RESPONSE 1
#define TIMEOUT 2
#define RX_FAILURE 3
#define UI_ESTOP_ACTIVE 4

typedef struct LoRa_struct{
  char message[20];
  uint32_t error_status;
} LoRa_struct;

bool start_latch = false;
bool crawler_e_stop_status = true;
LoRa_struct LoRa_struct_ts;
LoRa_struct * LoRa_ptr = &(LoRa_struct_ts);

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt


#define RELAY_PIN A15
#define LED_PIN   A4 //green light
#define LED_PIN_2 A7 //red light

void setup() 
{   
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN_2, HIGH);

#ifdef SER_DEBUG
  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa RX Test!");
#endif

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    #ifdef SER_DEBUG
    Serial.println("LoRa radio init failed");
    #endif
    while (1);
  }
  #ifdef SER_DEBUG
  Serial.println("LoRa radio init OK!");
  #endif

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    #ifdef SER_DEBUG
    Serial.println("setFrequency failed");
    #endif
    while (1);
  }
  #ifdef SER_DEBUG
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  #endif

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loop()
{
  if(start_latch == false){
    #ifdef SER_DEBUG
    Serial.println("start latch is false");
    #endif
    start_latch = check_start_latch();
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN_2, LOW);
    digitalWrite(RELAY_PIN, HIGH);
  }
  if(start_latch){
    #ifdef SER_DEBUG
    Serial.println("start latch is true");
    #endif
    LoRa_rx(LoRa_ptr);
    business_logic();
  }
}

void business_logic(void){
  if(LoRa_ptr->error_status == OK){
      //turn on led pin
      digitalWrite(LED_PIN, LOW);
    }
    else if(LoRa_ptr->error_status != OK){
      //turn off led pin
      digitalWrite(LED_PIN, HIGH);
    }
    else{
      ;
    }
    if(crawler_e_stop_status == HIGH){
      //turn on relay pin
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(LED_PIN_2, LOW);
    }
    else if(crawler_e_stop_status == LOW){
      //turn off relay pin
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PIN_2, HIGH);
    }
    else{
      ;
    }
}

bool check_start_latch(){
  bool start_latch = false;
  //call LoRa and check the error status
  LoRa_rx(LoRa_ptr);
  if(LoRa_ptr->error_status == OK){
    start_latch = true;
  }
  else{
    start_latch = false;
  }
  return start_latch;
}

void LoRa_rx(LoRa_struct * LoRa){
  //call this after checking if rf95 is available
  // Should be a message for us now   

  //bool crawler_e_stop_status = false;
  
  if (rf95.waitAvailableTimeout(1000))
  {
    
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      LoRa->error_status = OK;
      if(buf[10] == '1'){
        crawler_e_stop_status = true;
        #ifdef SER_DEBUG
        Serial.println("UI OK");
        #endif
      }
      else{
        crawler_e_stop_status = false;
        //LoRa->error_status = UI_ESTOP_ACTIVE;
        //digitalWrite(RELAY_PIN, HIGH);
        //Serial.println("UI ESTOP ACTIVE");
      }
      // Send a reply
      uint8_t data[] = "ACK";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      #ifdef SER_DEBUG
      Serial.println("Sent a reply");
      #endif
    }
    else
    {
      #ifdef SER_DEBUG
      Serial.println("Receive failed");
      #endif
      LoRa->error_status = RX_FAILURE;
    }
  }
  else{
    LoRa->error_status = TIMEOUT;
    #ifdef SER_DEBUG
    Serial.println("TIMEOUT");
    #endif
    //digitalWrite(LED_PIN, LOW);
  }
}
