#include "barco.h"
#define TEST

#ifdef TEST
  char ssid1[] = "MOVISTAR_CASA";
  char password1[] = "Los1Morras2somos3lo4mas";
  char ssid2[] = "MOVISTAR_CASA";
  char password2[] = "Los1Morras2somos3lo4mas";
  char server[] = "192.168.1.15";
#else
  #ifdef DENIA
      char ssid1[] = "VIVA_DENIA";
      char password1[] = "Tiene1mar2y3montanya";
      char ssid2[] = "VIVA_DENIA";
      char password2[] = "Tiene1mar2y3montanya";
      char server[] = "192.168.1.11";
  #else
      #ifdef DENIA
        char ssid1[] = "VIVA_DENIA";
        char password1[] = "Tiene1mar2y3montanya";
        char ssid2[] = "VIVA_DENIA";
        char password2[] = "Tiene1mar2y3montanya";
        char server[] = "192.168.1.11";
      #else  
        char ssid1[] = "MATERIA_OSCURA";
        char password1[] = "biba1SailingMFlo";
        char ssid2[] = "MATERIA_OSCURA";
        char password2[] = "biba1SailingMFlo";
        char server[] = "192.168.1.11";
      #endif      
   #endif
#endif
