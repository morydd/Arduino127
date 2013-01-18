/*
  UDPSendReceive.pde:
 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender
 
 A Processing sketch is included at the end of file that can be used to send 
 and received messages for testing with a computer.
 
 created 21 Aug 2010
 by Michael Margolis
 
 Updates and Modifications
 Sept-Oct 2012
 John Sartoris
 
 This code was and should be again in the public domain.
 */


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>        
#define short_get_high_byte(x) ((HIGH_BYTE & x) >> 8)
#define short_get_low_byte(x)  (LOW_BYTE & x)
#define bytes_to_short(h,l) ( ((h << 8) & 0xff00) | (l & 0x00FF) );

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
// Leonardo
//byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x19, 0x9B };
//IPAddress ip(192,168,4, 245);

//MEGA
byte mac[] = {  0x90, 0xA2, 0xDA, 0x0D, 0x19, 0xA0  };//the mac adress of ethernet shield or uno shield board


//unsigned int localPort = 6454;      // local port to listen on

// buffers for receiving and sending data
//char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged";       // a string to send back

//customisation: edit this if you want for example read and copy only 4 or 6 channels from channel 12 or 48 or whatever.
const int number_of_channels=10; //512 for 512 channels
const int channel_position=450; // 1 if you want to read from channel 1

// buffers
const int MAX_BUFFER_UDP=1024;//definition to do to make work UDP lib
const int SerialDEBUG=0;
char packetBuffer[MAX_BUFFER_UDP]; //buffer to store incoming data
int buffer_dmx[number_of_channels+channel_position]; //buffer to store filetered DMX data

// art net parameters
unsigned int localPort = 6454;      // artnet UDP port is by default 6454
const int art_net_header_size=17;
const int max_packet_size=1024;
char ArtNetHead[8]="Art-Net";
char OpHbyteReceive=0;
char OpLbyteReceive=0;
short is_artnet_version_1=0;
short is_artnet_version_2=0;
short seq_artnet=0;
short artnet_physical=0;
short incoming_universe=0;
boolean is_opcode_is_dmx=0;
boolean is_opcode_is_artpoll=0;
boolean match_artnet=1;
short Opcode=0;



// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setup() {
  
  Serial.begin(9600);
     while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Starting up" );
  // start the Ethernet and UDP Static:
 // Ethernet.begin(mac,ip);
 //  Serial.print("server is at ");
 //  Serial.println(Ethernet.localIP());
                                    
  // start the Ethernet connection DHCP:
  if (Ethernet.begin(mac) == 0) {
   Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    //Serial.print("server is at ");
    //Serial.println(Ethernet.localIP());
    Serial.print("."); 
  }
  Serial.println();
 
  Udp.begin(localPort);

 
    pinMode(30, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(31, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(32, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(33, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(34, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(35, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(36, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(37, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(38, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(39, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield

    pinMode(40, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(41, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(42, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(43, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(44, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(45, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(46, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(47, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(48, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
    pinMode(49, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield


}

void clearAndHome()
{
//Serial.write(27, BYTE); // ESC
//Serial.write("[2J"); // clear screen
//Serial.write(27, BYTE); // ESC
//Serial.write("[H"); // cursor to home
Serial.write(0x0C);
}

void loop() {
  
    //Clear the buffers
//    char ReplyBuffer[]= "                                    ";  
//    char packetBuffer[] = "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    ";
  
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    if(SerialDEBUG==1){ 
      clearAndHome();
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
         
          Serial.print(".");
         
      }
    }
          Serial.println();
          Serial.print("port ");
          Serial.println(Udp.remotePort());
    }

    // read the packet into packetBufffer
    Udp.read(packetBuffer,530);
         if(SerialDEBUG==1){
           Serial.print("imported packet size: ");
           Serial.println(sizeof(packetBuffer));
         }
    match_artnet=1;//valeur de stockage
     for (int i=0;i<7;i++)
     {
      if(char(packetBuffer[i])!=ArtNetHead[i])
      {
         if(SerialDEBUG==1){
           Serial.println("not Art-Net");
         }
        match_artnet=0;
        break;
    
      }//if not corresponding, this is not an artnet packet, so we stop reading
      } 
      if (match_artnet==1)//if its an artnet header
         {
          if(SerialDEBUG==1){
            Serial.println("Art-Net");
          }
         
              /*artnet protocole revision, not really needed
              is_artnet_version_1=packetBuffer[10]; 
              is_artnet_version_2=packetBuffer[11];*/
          
              /*sequence of data, to avoid lost packets on routeurs
              seq_artnet=packetBuffer[12];*/
              
              /*physical port of  dmx N°
              //artnet_physical=packetBuffer[13];*/
          
         //operator code enables to know wich type of message Art-Net it is
         Opcode=bytes_to_short(packetBuffer[9],packetBuffer[8]);
//         Serial.println("byte-to-short");
         if(Opcode==0x5000)//if opcode is DMX type
          {
//            Serial.println("opcode is DMX");
           is_opcode_is_dmx=1;is_opcode_is_artpoll=0;
           }   
           
         else if(Opcode==0x2000)//if opcode is artpoll 
         {
//           Serial.println("opcode is artpoll");
         is_opcode_is_artpoll=1;is_opcode_is_dmx=0;
         //( we should normally reply to it, giving ip adress of the device)
         } 
     
         if(  is_opcode_is_dmx=1)//if its DMX data we will read it now
         {
         if(SerialDEBUG==1){
           Serial.println("Read DMX Data");
         }
         //if you need to filter DMX universes, uncomment next line to have the universe rceived
         //incoming_universe= bytes_to_short(packetBuffer[15],packetBuffer[14])
         
         //getting data from a channel position, on a precise amount of channels, this to avoid to much operation if you need only 4 channels for example
         for(int i=1;i<= number_of_channels;i++)//channel position
          {
         buffer_dmx[i]= int(byte(packetBuffer[i+channel_position+17-1]));
         if(SerialDEBUG==1){
           Serial.print("input ");
          Serial.print(i);
           Serial.print(" - chan ");
          Serial.print(i+channel_position-1);
          Serial.print(" = '" );
         // Serial.print(" *** DUMMY NO DATA FOR TESTING *** " );
          Serial.print(buffer_dmx[i]);
          Serial.println("'" );
         }
           }
          }
     }//end of sniffing    
//     Serial.println("write led");


for (int chan=1;chan <= number_of_channels;chan++){
  int ledoffset = (chan - 1) * 2;
  if( buffer_dmx[chan]==0 ) {
      if(SerialDEBUG==1){
           Serial.print("LEDASet: ");
           Serial.print(chan);
           Serial.print(" - ");
           Serial.print(buffer_dmx[chan]);
           Serial.print(" - ");
           Serial.print(30+ledoffset);
           Serial.print(",");
           Serial.print(30+ledoffset+1);
                     
      }
      digitalWrite(30+ledoffset,LOW);
      if(SerialDEBUG==1){
           Serial.print(" 0 ");
      }
      digitalWrite(30+ledoffset+1,LOW);
      if(SerialDEBUG==1){
           Serial.println(" 0 ");
      }
      
  } 
  if( buffer_dmx[chan]<=127 and buffer_dmx[chan]>0  ) {
      if(SerialDEBUG==1){
           Serial.print("LEDBSet: ");
           Serial.print(chan);
           Serial.print(" - ");
           Serial.print(buffer_dmx[chan]);
           Serial.print(" - ");
           Serial.print(30+ledoffset);
           Serial.print(",");
           Serial.print(30+ledoffset+1);

      }
      digitalWrite(30+ledoffset,HIGH);
      if(SerialDEBUG==1){
           Serial.print(" 1 ");
      }
      digitalWrite(30+ledoffset+1,LOW);
      if(SerialDEBUG==1){
           Serial.println(" 0 ");
      }

  } 
  if( buffer_dmx[chan]>=128 ) {
      if(SerialDEBUG==1){
           Serial.print("LEDCSet: ");
           Serial.print(chan);
           Serial.print(" - ");
           Serial.print(buffer_dmx[chan]);
           Serial.print(" - ");
           Serial.print(30+ledoffset);
           Serial.print(",");
           Serial.print(30+ledoffset+1);

      }
      digitalWrite(30+ledoffset,LOW);
      if(SerialDEBUG==1){
           Serial.print(" 0 ");
      }
      digitalWrite(30+ledoffset+1,HIGH);
      if(SerialDEBUG==1){
           Serial.println(" 1 ");
      }

  } 
}
    
   // send a reply, to the IP address and port that sent us the packet we received
   // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
   // Udp.write(ReplyBuffer);
   // Udp.endPacket();
  }
//delay(100);
}


