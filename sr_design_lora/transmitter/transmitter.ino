// LoRa 9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example LoRa9x_RX

//PV Robotics Senior Design Project 2019

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 3
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

//e-stop pin
#define ESTOP_PIN 7

//lora error status defines
#define OK 0
#define NO_RESPONSE 1
#define TIMEOUT 2

typedef struct LoRa_struct{
  char message[20];
  uint32_t error_status;
} LoRa_struct;

bool start_latch = false;
bool e_stop_status = true;
LoRa_struct LoRa_struct_ts;
LoRa_struct * LoRa_ptr = &(LoRa_struct_ts);
  

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  pinMode(ESTOP_PIN, INPUT_PULLUP);
  digitalWrite(ESTOP_PIN, HIGH);
  e_stop_status = digitalRead(ESTOP_PIN);

  while (!Serial);
  Serial.begin(115200);
  delay(100);

  Serial.println("Arduino LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop(){
  if(start_latch == false){
    start_latch = check_start_latch(ESTOP_PIN);    //send heartbeat signal until a response is received from the crawler.
                                                  //when response received, set start_latch = true;
                                                  //also reads e-stop switch
    Serial.println("start latch is not true!");
    delay(500);
  }
  if(start_latch){
    //run main code...
    e_stop_status = read_inputs();      //reads hardware inputs (e-stop) and puts inputs status into variables
    LoRa(LoRa_ptr, e_stop_status);      //send heartbeat, receives response. heartbeat message also contains status of E-stop
    business_logic(LoRa_ptr);        //acts on the status of the variables, controls state of E-stop system.
    delay(500);
  }
}

void business_logic(LoRa_struct * LoRa_ptr){
  //nothing here for now!
  //return;
}

bool read_inputs(){
  e_stop_status = digitalRead(ESTOP_PIN);
  return e_stop_status;
}

bool check_start_latch(uint8_t e_stop_pin){
  bool start_latch = false;
  
  //check e-stop status
  e_stop_status = digitalRead(e_stop_pin);

  //digitalWrite(ESTOP_PIN, !e_stop_status);
  
  //check for connection with crawler over LoRa
  //*LoRa_ptr.message
  //*LoRa_ptr.error_status
  
  LoRa(LoRa_ptr,e_stop_status);
  
  if(LoRa_ptr->error_status == OK && e_stop_status == HIGH){
    start_latch = true;
  }
  
  return start_latch;
}

/*
 * what does LoRa() do?
 * LoRa takes a pointer and the reading of the e-stop as inputs
 * it modifies the heartbeat message based on the e-stop status
 * it sends the heartbeat message
 * it waits for a response from the crawler
 * it modifies the error_status field of the message pointer to reflect whether a response was received
 */

void LoRa(LoRa_struct * LoRa, bool e_stop_status){
  Serial.println("Sending to crawler");
  // Send a message to rf95_server
  
  //LoRa->message = "heartbeat #        ";
  strcpy(LoRa->message, "heartbeat #        ");
  
  itoa((int)e_stop_status, LoRa->message+10, 10);
  Serial.print("Sending "); Serial.println(LoRa->message);
  //why is this here?
  //radiopacket[19] = 0;
  
  Serial.println("Sending...");
  rf95.send((uint8_t *)LoRa->message, 20);

  Serial.println("Waiting for packet to transmit");
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply from crawler...");
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
      LoRa->error_status = OK;
    }
    else
    {
      Serial.println("Receive failed");
      LoRa->error_status = NO_RESPONSE;
    }
  }
  else
  {
    //we need a counter here to track how many times we missed messages
    Serial.println("No reply, is there a listener around?");
    LoRa->error_status = NO_RESPONSE;
  }
  //delay(1000);
  //return receive;
  return;
}
