// Demo using DHCP and DNS to perform a web client request.
// 2011-06-08 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <EtherCard.h>

// which analog pin to connect
#define THERMISTORPIN0 A1
#define THERMISTORPIN1 A2
#define THERMISTORPIN2 A0
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 7500

float steinhart;
float reading;
char v0[]="0.000";
char v1[]="0.000";
char v2[]="0.000";

char tempsql[]="0.000&t1=0.000&t2=0.000";

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];
static uint32_t timer;

const char website[] PROGMEM = "89.80.195.205";

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}

void setup () {
  Serial.begin(57600);
  Serial.println(F("\n[webClient]"));

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    
  ether.printIp("SRV: ", ether.hisip);
}

void loop () {
  reading = analogRead(THERMISTORPIN0);
 
  reading = (1023 / reading)  - 1;
  reading = SERIESRESISTOR * reading;
  
  steinhart = reading / THERMISTORNOMINAL; // (R/Ro)
  steinhart = log(steinhart); // ln(R/Ro)
  steinhart /= BCOEFFICIENT; // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart; // Invert
  steinhart -= 273.15; // convert to C
  
  dtostrf(steinhart,5,2,v0);
 
 if (steinhart<10){
    if(steinhart>0){ 
      dtostrf(steinhart,5,3,v0);
     }
  }
  
    reading = analogRead(THERMISTORPIN1);
 
  reading = (1023 / reading)  - 1;
  reading = SERIESRESISTOR * reading;
  
  steinhart = reading / THERMISTORNOMINAL; // (R/Ro)
  steinhart = log(steinhart); // ln(R/Ro)
  steinhart /= BCOEFFICIENT; // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart; // Invert
  steinhart -= 273.15; // convert to C
  
  dtostrf(steinhart,5,2,v1);
 
 if (steinhart<10){
    if(steinhart>0){ 
      dtostrf(steinhart,5,3,v1);
     }
  }
  
  reading = analogRead(THERMISTORPIN2);
 
  reading = (1023 / reading)  - 1;
  reading = SERIESRESISTOR * reading;
  
  steinhart = reading / THERMISTORNOMINAL; // (R/Ro)
  steinhart = log(steinhart); // ln(R/Ro)
  steinhart /= BCOEFFICIENT; // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart; // Invert
  steinhart -= 273.15; // convert to C
  
  dtostrf(steinhart,5,2,v2);
 
 if (steinhart<10){
    if(steinhart>0){ 
      dtostrf(steinhart,5,3,v2);
     }
  }
  
  ether.packetLoop(ether.packetReceive());
  
  if (millis() > timer) {
    timer = millis() + 600000;
    sprintf(tempsql,"%s&t1=%s&t2=%s",v0,v1,v2);
    //tempsql=v2;
    ether.browseUrl(PSTR("/temp_salon.php?t0="), tempsql, website, my_callback);
  }
  //delay(60000);
}
