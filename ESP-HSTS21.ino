/**
    ESP-HSTS21 ESPP8266/ESP32) manejado desde IBM IoT
    Morrastronics -- by chuRRuscat
    v1.0 2021 initial version
    v3.0 2021 Integrate ESP12 and ESP32, use #define ESP32      
*/

#define I_NOMINAL 50 // Nominal amperaje of HSTS21 amperimeter
#define MANUAL_ADJUST  2.5-2.285 // Volts when I=0 (adjust manually)
#define PRINT_SI
#ifdef  PRINT_SI
  #define DPRINT(...)    Serial.print(__VA_ARGS__)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)
  #define DPRINTF(...)   Serial.printf(__VA_ARGS__)
#else
  #define DPRINT(...)     //blank line
  #define DPRINTLN(...)   //blank line
  #define DPRINTF(...)
#endif

/*************************************************
 ** -------- Personalised values ----------- **
 * *****************************************/
/* select sensor and its values */ 
#include "barco.h"
#include <ArduinoJson.h>
#include "mqtt_mosquitto.h"  /* mqtt values */

#if defined(ESP32)
  #include <ESPmDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #define SCL 22
  #define SDA 21
  #define ADC1_CH0 36  // 
  #define ADC1_CH6 34  // used for V_REF 
  #define ADC1_CH3 39  // to test when engine is started 
  #define V_REF ADC1_CH6       //  GPIO 34
  #define AMPERE_PIN ADC1_CH0  //  GPIO 36
  #define RPM_PIN    ADC1_CH3 //   GPIO 39 
#else    // it is an ESP12 (NodeMCU)
  #undef WITH_VREF
  #include <ESP8266mDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #define SDA D5   // for BME280 I2C 
  #define SCL D6
  #define AMPERE_PIN A0
  #define RPM_PIN    D8   // when engine is started 
#endif

/*************************************************
 ** ----- End of Personalised values ------- **
 * ***********************************************/
#define AJUSTA_T 10000 // To adjust delay in some places along the program
#include <Wire.h>             //libraries for sensors and so on
#include "Pin_NodeMCU.h"
#define _BUFFSIZE 250
#define DATOS_SIZE 250

/* ********* these are the sensor variables that will be exposed **********/ 

#define MAX_MEDIDAS 100
int amperios[MAX_MEDIDAS+1];
#define JSONBUFFSIZE 350
#define DATOSJSONSIZE 350   
char datosJson[DATOS_SIZE];
StaticJsonDocument<DATOSJSONSIZE> docJson;
JsonObject valores=docJson.createNestedObject();  //Read values
JsonObject claves=docJson.createNestedObject();   // key values (location and deviceId)
// A UDP instance to let us send and receive packets over UDP
WiFiUDP clienteUDP;
#define UDP_PORT   4000

// let's start, setup variables
void setup() {
boolean status;
int amperes1,amperes2,RPM;
    
  Serial.begin(111500);
  DPRINTLN("starting ... "); 
  Wire.begin(SDA,SCL);
  claves["deviceId"]=DEVICE_ID;
  claves["location"]=LOCATION;
  wifiConnect();
  mqttConnect();
  clienteUDP.begin(UDP_PORT);
  DPRINTLN(" los dos connect hechos, ahora OTA");
  ArduinoOTA.setHostname(DEVICE_ID); 
  ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
          type = "sketch";
      } else { // U_FS
          type = "filesystem";
      }
      // NOTE: if updating FS this would be the place to unmount FS using FS.end()
      DPRINTLN("Start updating " + type);
   });
   ArduinoOTA.onEnd([]() {
     DPRINTLN("\nEnd");
   });
   ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      DPRINTF("Progress: %u%%\r\n",(progress / (total / 100)));
   });
   ArduinoOTA.onError([](ota_error_t error) {
            DPRINTF("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            DPRINTLN("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) { 
            DPRINTLN("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            DPRINTLN("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            DPRINTLN("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            DPRINTLN("End Failed");
        }
    });
  ArduinoOTA.begin(); 
  claves["deviceId"]=DEVICE_ID;
  claves["location"]=LOCATION;
  delay(50);
  pinMode(RPM_PIN, INPUT);
  // analogReadResolution(12); // doesnot work in ESP
}

uint32_t ultimaRPM=0,ultimaAmpere=0,nMedidas=0;

void loop() {
  int i=0;
    DPRINT("#");
    if (!loopMQTT()) {  // Check if there are MQTT messages and if the device is connected  
        DPRINTLN("Connection lost; retrying");
        sinConectividad();        
        mqttConnect();
    }
    ArduinoOTA.handle(); 
    if ((millis()-ultimaRPM)>INTERVALO_RPM) {   // if it is time to send data, do it
      DPRINT("interval:");DPRINT(INTERVALO_RPM);
      DPRINT("\tmillis :");DPRINT(millis());
      DPRINT("\tultimaRPM :");DPRINTLN(ultimaRPM);
      publicaRPM();  // Send it (UDP:4000)
      if (nMedidas == MAX_MEDIDAS) nMedidas=0;
      calculaAmperio(nMedidas);
      nMedidas++;
      ultimaRPM=millis();
    }
    if ((millis()-ultimaAmpere)>INTERVALO_AMPERIO) {   // if it is time to send data, do it
      DPRINT("interval:");DPRINT(INTERVALO_AMPERIO);
      DPRINT("\tmillis :");DPRINT(millis());
      DPRINT("\tultimoAmperio :");DPRINTLN(ultimaAmpere);
      publicaAmperio(nMedidas);
      nMedidas=0;
      ultimaAmpere=millis();
    }    
    espera(1000); //and wait
}

/****************************************** 
* this function sends RPM data to MQTT broker, 
*********************************************/
boolean publicaRPM() {
    boolean pubresult=true;
    int RPM;
    char msg[20];
    IPAddress broadcastIp(192,168,8,255);

  RPM = digitalRead(RPM_PIN);
  if (RPM == HIGH) RPM=1 ;
  else RPM=0;
  sprintf(msg,"$ERRPM,E,%d,1,1,A*00",RPM);
/*  clienteUDP.beginPacket(server, UDP_PORT);
  clienteUDP.print(msg);
  clienteUDP.endPacket();
*/  
  clienteUDP.beginPacket(broadcastIp, UDP_PORT);
  clienteUDP.print(msg);
  clienteUDP.endPacket();
  DPRINT("RPM: ");DPRINTLN(RPM);
  return true;  
}

boolean calculaAmperio(int i) {

#ifdef WITH_VREF
  amperios[i] = analogRead(AMPERE_PIN)-analogRead(V_REF); // If VREF Connected
#else
  amperios[i] = analogRead(AMPERE_PIN)-MANUAL_ADJUST; // If VREF adjusted Manually 
#endif
  DPRINT("amperios: ");DPRINT(amperios[i]);
  DPRINT("\tmedida: ");DPRINTLN(i);
 return true;
}

boolean publicaAmperio(int num) {
    int i=0;
    float total=0, media=0, amperes=0;
    boolean pubresult=true;
         
  for (i=0; i<num;i++) {
    total+=amperios[i];
  }
  #ifdef ESP32
     media=total*3.3/(num*4095); // convert into amp
  #else
     media=total*3.3/(num*1023); // convert into amp
  #endif
  valores["medida"]=media;
  amperes= trunc((media)*I_NOMINAL/0.625*10)/10; // to round to first decimal
  if ((int(amperes)-amperes)==0) {
    amperes=amperes+0.01;
  }
  valores["Amp"]=amperes;
  
  DPRINT("amperios total: "); DPRINTLN((float)valores["Amp"]);
  DPRINT("\tnum: ");DPRINTLN(num);
  valores["RPM"]=0;
  valores.remove("RPM");
  serializeJson(docJson,datosJson);
    // and publish them.
  DPRINTLN("preparing to send");
  pubresult = enviaDatos(publishTopic,datosJson); 

  return pubresult;
}
