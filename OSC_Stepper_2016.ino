//DHCP-based OSC server test code
//for use with IDE 1.0.5
//for use with W5100 or W5200 based ethernet shields

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCBundle.h>
#include <AccelStepper.h>

// you can find this written on the board of some Arduino Ethernets or shields
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED } ;

// NOTE: Alternatively, you can assign a fixed IP to configure your
//       Ethernet shield.
       byte ip[] = { 192, 168, 0, 100 };


int serverPort  = 10000; //TouchOSC (incoming port)
int destPort = 9000;    //TouchOSC (outgoing port)

//create servo class
AccelStepper stepper1(4,2,3,5,6);
long val1 = 0;


//Create UDP message object
EthernetUDP Udp;

void setup(){
 


  // start the Ethernet connection
        
          Ethernet.begin(mac, ip);  
          Udp.begin(serverPort);
        stepper1.setMaxSpeed(500.0);
        stepper1.setAcceleration(200.0);

}

void loop(){
  //process received messages
  OSCMsgReceive();
  stepper1.run();
} 

void OSCMsgReceive(){
  OSCMessage msgIN;
  int size;
  if((size = Udp.parsePacket())>0){
    while(size--)
      msgIN.fill(Udp.read());
    if(!msgIN.hasError()){
      msgIN.route("/compass/2",funcValue1);
    }
  }
}



void funcValue1(OSCMessage &msg, int addrOffset ){

  float value = msg.getFloat(0);
  val1 = (long)(value*200);
  OSCMessage msgOUT("/compass/2");
  stepper1.moveTo(val1);
  //Serial.print("Value = : ");
  //Serial.println(value);

  msgOUT.add(val1);

  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp); // send the bytes
  Udp.endPacket(); // mark the end of the OSC Packet
  msgOUT.empty(); // free space occupied by message
}


