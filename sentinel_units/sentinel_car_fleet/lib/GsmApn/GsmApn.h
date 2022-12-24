#include <Arduino.h>

typedef struct {
  char * mcc;       //mcc code - mobile country code
  char * mnc;       //mnc code - mobile network code
  char * code;      //country code
  char * iso;       //country iso code
  char * country;   //country name
  char * network;   //network name
  char * apn;       //apn name
  char * user;      //apn user name
  char * pass;      //apn password
}
Apn_List_t;

Apn_List_t apn_list[] = {
  // Generic
  
  // Czech Republic
  {.mcc="230", .mnc="01", .code="420", .iso="cz", .country="Czech Republic", .network="T-Mobile", .apn="", .user="", .pass=""},
  {.mcc="230", .mnc="02", .code="420", .iso="cz", .country="Czech Republic", .network="O2", .apn="", .user="", .pass=""},
  {.mcc="230", .mnc="03", .code="420", .iso="cz", .country="Czech Republic", .network="Vodafone", .apn="", .user="", .pass=""},
  {.mcc="230", .mnc="04", .code="420", .iso="cz", .country="Czech Republic", .network="Nordic Telecom", .apn="", .user="", .pass=""},
  {.mcc="230", .mnc="05", .code="420", .iso="cz", .country="Czech Republic", .network="PODA", .apn="", .user="", .pass=""},
  {.mcc="230", .mnc="06", .code="420", .iso="cz", .country="Czech Republic", .network="Nordic Telecom", .apn="", .user="", .pass=""},
  {.mcc="230", .mnc="07", .code="420", .iso="cz", .country="Czech Republic", .network="T-Mobile", .apn="", .user="", .pass=""},
  {.mcc="230", .mnc="98", .code="420", .iso="cz", .country="Czech Republic", .network="SZDC", .apn="", .user="", .pass=""},
  // Slovakia
  {.mcc="231", .mnc="01", .code="421", .iso="sk", .country="Slovakia", .network="Orange", .apn="", .user="", .pass=""},
  {.mcc="231", .mnc="02", .code="421", .iso="sk", .country="Slovakia", .network="Telekom", .apn="", .user="", .pass=""},
  {.mcc="231", .mnc="03", .code="421", .iso="sk", .country="Slovakia", .network="4ka", .apn="", .user="", .pass=""},
  {.mcc="231", .mnc="04", .code="421", .iso="sk", .country="Slovakia", .network="Telekom", .apn="", .user="", .pass=""},
  {.mcc="231", .mnc="05", .code="421", .iso="sk", .country="Slovakia", .network="Orange", .apn="", .user="", .pass=""},
  {.mcc="231", .mnc="06", .code="421", .iso="sk", .country="Slovakia", .network="O2", .apn="", .user="", .pass=""},
  {.mcc="231", .mnc="99", .code="421", .iso="sk", .country="Slovakia", .network="ZSR", .apn="", .user="", .pass=""},
  // Poland
  {.mcc="260", .mnc="01", .code="48", .iso="pl", .country="Poland", .network="Plus", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="02", .code="48", .iso="pl", .country="Poland", .network="T-Mobile", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="03", .code="48", .iso="pl", .country="Poland", .network="Orange", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="06", .code="48", .iso="pl", .country="Poland", .network="Play", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="07", .code="48", .iso="pl", .country="Poland", .network="Netia", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="09", .code="48", .iso="pl", .country="Poland", .network="Lycamobile", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="11", .code="48", .iso="pl", .country="Poland", .network="Plus", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="12", .code="48", .iso="pl", .country="Poland", .network="Cyfrowy Polsat", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="13", .code="48", .iso="pl", .country="Poland", .network="Move", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="15", .code="48", .iso="pl", .country="Poland", .network="Plus", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="16", .code="48", .iso="pl", .country="Poland", .network="Plus", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="17", .code="48", .iso="pl", .country="Poland", .network="Plus", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="25", .code="48", .iso="pl", .country="Poland", .network="TeleCube", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="33", .code="48", .iso="pl", .country="Poland", .network="Truphone", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="34", .code="48", .iso="pl", .country="Poland", .network="T-Mobile", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="35", .code="48", .iso="pl", .country="Poland", .network="PKP", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="39", .code="48", .iso="pl", .country="Poland", .network="Voxbone", .apn="", .user="", .pass=""},
  {.mcc="260", .mnc="45", .code="48", .iso="pl", .country="Poland", .network="Virgin Mobile", .apn="", .user="", .pass=""},
  // Germany
  {.mcc="262", .mnc="01", .code="49", .iso="de", .country="Germany", .network="Telekom", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="02", .code="49", .iso="de", .country="Germany", .network="Vodafone", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="03", .code="49", .iso="de", .country="Germany", .network="O2", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="04", .code="49", .iso="de", .country="Germany", .network="Vodafone", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="05", .code="49", .iso="de", .country="Germany", .network="O2", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="06", .code="49", .iso="de", .country="Germany", .network="Telekom", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="08", .code="49", .iso="de", .country="Germany", .network="O2", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="09", .code="49", .iso="de", .country="Germany", .network="Vodafone", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="10", .code="49", .iso="de", .country="Germany", .network="DB Netz", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="11", .code="49", .iso="de", .country="Germany", .network="O2", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="12", .code="49", .iso="de", .country="Germany", .network="Sipgate", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="14", .code="49", .iso="de", .country="Germany", .network="Lebara", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="16", .code="49", .iso="de", .country="Germany", .network="Airdata", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="18", .code="49", .iso="de", .country="Germany", .network="NetCologne", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="19", .code="49", .iso="de", .country="Germany", .network="Alliander", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="20", .code="49", .iso="de", .country="Germany", .network="Enreach", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="23", .code="49", .iso="de", .country="Germany", .network="1&1", .apn="", .user="", .pass=""},
  {.mcc="262", .mnc="43", .code="49", .iso="de", .country="Germany", .network="Lycamobile", .apn="", .user="", .pass=""},

};
