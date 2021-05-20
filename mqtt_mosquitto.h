#include <ESP8266WiFi.h>
#define MQTT_MAX_PACKET_SIZE 455 //must be before docpatth\Arduino\libraries\pubsubclient-master\src\pubsubclient.h
//setBufferSize(455); introduced in version 2.8
#define MQTT_KEEP_ALIVE 60
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient
#include "Pin_NodeMCU.h"

/*************************************************
 ** ---------- Personalized Values ------------- **
 * ***********************************************/
#define ESPERA_NOCONEX 30000  // When no conection, wait 30 sec
#define DEVICE_TYPE "ESP12E"
char* ssid;
char* password;
/*********** personal.h should include SSID and passwords  ***********
 *  something like: 
char ssid1[] = "ssid1";
char password1[] = "Password_ssid1";
char ssid2[] = "ssid2";
char password2[] = "Password_ssid2";
 */
#include "personal.h"   

/*************************************************
 ** ----- Fin de Valores Personalizados ------- **
 * ***********************************************/
char * authMethod = NULL;
char * token = NULL;
char clientId[] = "d:" LOCATION":" DEVICE_TYPE ":" DEVICE_ID;

char publishTopic[] = "barco/envia";  // device send data to mqtt
char metadataTopic[]= "barco/envia/metadata"; //device send metadata to mqtt
char updateTopic[]  = "barco/update";    // device metadata is to be updated
char responseTopic[]= "barco/response";
char rebootTopic[]  = "barco/reboot";
