#include "barco.h"

#ifdef POZUELO
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
      #ifdef MATERIA_OSCURA
        char ssid1[] = "Materia_Oscura";
        char password1[] = "biba1Sailing2MFlo";
        char ssid2[] = "Materia_Oscura";
        char password2[] = "biba1Sailing2MFlo";
        char server[] = "192.168.8.10";
      #else  
        char ssid1[] = "churruscat";
        char password1[] = "biba1Sailing";
        char ssid2[] = "churruscat";
        char password2[] = "biba1Sailing";
        char server[] = "192.168.8.10";
      #endif      
   #endif
#endif
