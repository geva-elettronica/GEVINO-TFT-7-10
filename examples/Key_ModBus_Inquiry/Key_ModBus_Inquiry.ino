// Inizializzare la tastiera al reset con 0014   (Indirizzo 1 e 9600 baud)

#include "SimpleModbusMaster.h"


// led to indicate that a communication error is present
#define connection_error_led LED_BUILTIN

//////////////////// Port information ///////////////////
#define baud 9600
#define timeout 1000
#define polling 200 // the scan rate


// If the packets internal retry register matches
// the set retry count then communication is stopped
// on that packet. To re-enable the packet you must
// set the "connection" variable to true.
#define retry_count 1 

// used to toggle the receive/transmit pin on the driver
#define TxEnablePin 7   // GEVINO TFT
//#define TxEnablePin A0  // GEVINO Opto

// This is the easiest way to create new packets
// Add as many as you want. TOTAL_NO_OF_PACKETS
// is automatically updated.
enum
{
  PACKET1,
  PACKET2,
  // leave this last entry
  TOTAL_NO_OF_PACKETS
};

// Create an array of Packets for modbus_update()
Packet packets[TOTAL_NO_OF_PACKETS];

// Create a packetPointer to access each packet
// individually. This is not required you can access
// the array explicitly. E.g. packets[PACKET1].id = 2;
// This does become tedious though...
packetPointer packet1 = &packets[PACKET1];
packetPointer packet2 = &packets[PACKET2];

// The data from the PLC will be stored
// in the regs array
unsigned int input[30];
unsigned int output[30];

void setup()
{
  SerialUSB.begin(115200);
  
  // read 7 registers starting at address 0
  packet1->id = 1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = 0;
  packet1->no_of_registers = 7;
  packet1->register_array = input;
  
  // write the 1 registers to the PLC starting at address 6
  packet2->id = 1;
  packet2->function = PRESET_MULTIPLE_REGISTERS;
  packet2->address = 6;
  packet2->no_of_registers = 1;
  packet2->register_array = output;
  
  // P.S. the register_array entries above can be different arrays
  
  // Initialize communication settings etc...
  modbus_configure(baud, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);
  
  pinMode(connection_error_led, OUTPUT);
}

unsigned long Timer;
void loop()
{
  unsigned int connection_status = modbus_update(packets);
  
  if (connection_status != TOTAL_NO_OF_PACKETS)
  {
    digitalWrite(connection_error_led, HIGH);
    // re-enable the connection
    // After retry_count errors it stops and turns on the LED; the command below restarts the retries
    packets[connection_status].connection = true;
  }
  else
    digitalWrite(connection_error_led, LOW);
  
  // update the array with the counter data
//  xx = packet1->requests;
//  xx = packet1->successful_requests;
//  xx = packet1->total_errors;
//  xx = packet2->requests;
//  xx = packet2->successful_requests;
//  xx = packet2->total_errors; 

  if( millis() -  Timer > 1000){
    Timer = millis();
    SerialUSB.print((int)input[0]);
    SerialUSB.print("\t");
    SerialUSB.print((char)input[1]);
    SerialUSB.print("\t");
    SerialUSB.print((char)(input[1]/256));
    SerialUSB.print("\t");
    SerialUSB.print((char)input[2]);
    SerialUSB.print("\t");
    SerialUSB.print((char)(input[2]/256));
    SerialUSB.print("\t");
    SerialUSB.print((char)input[3]);
    SerialUSB.print("\t");
    SerialUSB.print((char)(input[3]/256));
    SerialUSB.print("\t");
    SerialUSB.print((char)input[4]);
    SerialUSB.print("\t");
    SerialUSB.print((char)(input[4]/256));
    SerialUSB.print("\t");
    SerialUSB.print((char)input[5]);
    SerialUSB.print("\t");
    SerialUSB.println((char)(input[5]/256));

  }
}
