#include "configuration.h" // project configuration file
#include <Arduino.h>

#include <Preferences.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <sqlite3.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Ticker.h>
#include <TinyGsmClient.h>
#include <RTClib.h>

// ----------------------------------------------------------------------------------------------------------------
/*

TODO - gms and gps
TODO - card reader
TODO - connection to cloud-relay server
TODO - ntp and rtc
TODO - database

? - module working using gsm through sentinel cloud gateway

*/
// ----------------------------------------------------------------------------------------------------------------

Preferences preferences;
sqlite3 *fs_db;
TinyGsm modem(SerialAT);
RTC_DS3231 rtc;

DNSServer dnsServer;
WebServer webServer(80);
Ticker led_ticker;

IPAddress wifi_ap_ip(192,168,11,1);
IPAddress wifi_ap_gw(192,168,11,1);
IPAddress wifi_ap_mask(255,255,255,0);

typedef struct tzStruct_ {
  String name; 
  String value;
} tzStruct_t;

const tzStruct_t tzArray[] PROGMEM = {
  {"Africa/Abidjan", "GMT0"},
  {"Africa/Accra", "GMT0"},
  {"Africa/Addis_Ababa", "EAT-3"},
  {"Africa/Algiers", "CET-1"},
  {"Africa/Asmara", "EAT-3"},
  {"Africa/Bamako", "GMT0"},
  {"Africa/Bangui", "WAT-1"},
  {"Africa/Banjul", "GMT0"},
  {"Africa/Bissau", "GMT0"},
  {"Africa/Blantyre", "CAT-2"},
  {"Africa/Brazzaville", "WAT-1"},
  {"Africa/Bujumbura", "CAT-2"},
  {"Africa/Cairo", "EET-2"},
  {"Africa/Casablanca", "<+01>-1"},
  {"Africa/Ceuta", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Africa/Conakry", "GMT0"},
  {"Africa/Dakar", "GMT0"},
  {"Africa/Dar_es_Salaam", "EAT-3"},
  {"Africa/Djibouti", "EAT-3"},
  {"Africa/Douala", "WAT-1"},
  {"Africa/El_Aaiun", "<+01>-1"},
  {"Africa/Freetown", "GMT0"},
  {"Africa/Gaborone", "CAT-2"},
  {"Africa/Harare", "CAT-2"},
  {"Africa/Johannesburg", "SAST-2"},
  {"Africa/Juba", "EAT-3"},
  {"Africa/Kampala", "EAT-3"},
  {"Africa/Khartoum", "CAT-2"},
  {"Africa/Kigali", "CAT-2"},
  {"Africa/Kinshasa", "WAT-1"},
  {"Africa/Lagos", "WAT-1"},
  {"Africa/Libreville", "WAT-1"},
  {"Africa/Lome", "GMT0"},
  {"Africa/Luanda", "WAT-1"},
  {"Africa/Lubumbashi", "CAT-2"},
  {"Africa/Lusaka", "CAT-2"},
  {"Africa/Malabo", "WAT-1"},
  {"Africa/Maputo", "CAT-2"},
  {"Africa/Maseru", "SAST-2"},
  {"Africa/Mbabane", "SAST-2"},
  {"Africa/Mogadishu", "EAT-3"},
  {"Africa/Monrovia", "GMT0"},
  {"Africa/Nairobi", "EAT-3"},
  {"Africa/Ndjamena", "WAT-1"},
  {"Africa/Niamey", "WAT-1"},
  {"Africa/Nouakchott", "GMT0"},
  {"Africa/Ouagadougou", "GMT0"},
  {"Africa/Porto-Novo", "WAT-1"},
  {"Africa/Sao_Tome", "GMT0"},
  {"Africa/Tripoli", "EET-2"},
  {"Africa/Tunis", "CET-1"},
  {"Africa/Windhoek", "CAT-2"},
  {"America/Adak", "HST10HDT,M3.2.0,M11.1.0"},
  {"America/Anchorage", "AKST9AKDT,M3.2.0,M11.1.0"},
  {"America/Anguilla", "AST4"},
  {"America/Antigua", "AST4"},
  {"America/Araguaina", "<-03>3"},
  {"America/Argentina/Buenos_Aires", "<-03>3"},
  {"America/Argentina/Catamarca", "<-03>3"},
  {"America/Argentina/Cordoba", "<-03>3"},
  {"America/Argentina/Jujuy", "<-03>3"},
  {"America/Argentina/La_Rioja", "<-03>3"},
  {"America/Argentina/Mendoza", "<-03>3"},
  {"America/Argentina/Rio_Gallegos", "<-03>3"},
  {"America/Argentina/Salta", "<-03>3"},
  {"America/Argentina/San_Juan", "<-03>3"},
  {"America/Argentina/San_Luis", "<-03>3"},
  {"America/Argentina/Tucuman", "<-03>3"},
  {"America/Argentina/Ushuaia", "<-03>3"},
  {"America/Aruba", "AST4"},
  {"America/Asuncion", "<-04>4<-03>,M10.1.0/0,M3.4.0/0"},
  {"America/Atikokan", "EST5"},
  {"America/Bahia", "<-03>3"},
  {"America/Bahia_Banderas", "CST6CDT,M4.1.0,M10.5.0"},
  {"America/Barbados", "AST4"},
  {"America/Belem", "<-03>3"},
  {"America/Belize", "CST6"},
  {"America/Blanc-Sablon", "AST4"},
  {"America/Boa_Vista", "<-04>4"},
  {"America/Bogota", "<-05>5"},
  {"America/Boise", "MST7MDT,M3.2.0,M11.1.0"},
  {"America/Cambridge_Bay", "MST7MDT,M3.2.0,M11.1.0"},
  {"America/Campo_Grande", "<-04>4"},
  {"America/Cancun", "EST5"},
  {"America/Caracas", "<-04>4"},
  {"America/Cayenne", "<-03>3"},
  {"America/Cayman", "EST5"},
  {"America/Chicago", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Chihuahua", "MST7MDT,M4.1.0,M10.5.0"},
  {"America/Costa_Rica", "CST6"},
  {"America/Creston", "MST7"},
  {"America/Cuiaba", "<-04>4"},
  {"America/Curacao", "AST4"},
  {"America/Danmarkshavn", "GMT0"},
  {"America/Dawson", "MST7"},
  {"America/Dawson_Creek", "MST7"},
  {"America/Denver", "MST7MDT,M3.2.0,M11.1.0"},
  {"America/Detroit", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Dominica", "AST4"},
  {"America/Edmonton", "MST7MDT,M3.2.0,M11.1.0"},
  {"America/Eirunepe", "<-05>5"},
  {"America/El_Salvador", "CST6"},
  {"America/Fort_Nelson", "MST7"},
  {"America/Fortaleza", "<-03>3"},
  {"America/Glace_Bay", "AST4ADT,M3.2.0,M11.1.0"},
  {"America/Godthab", "<-03>3<-02>,M3.5.0/-2,M10.5.0/-1"},
  {"America/Goose_Bay", "AST4ADT,M3.2.0,M11.1.0"},
  {"America/Grand_Turk", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Grenada", "AST4"},
  {"America/Guadeloupe", "AST4"},
  {"America/Guatemala", "CST6"},
  {"America/Guayaquil", "<-05>5"},
  {"America/Guyana", "<-04>4"},
  {"America/Halifax", "AST4ADT,M3.2.0,M11.1.0"},
  {"America/Havana", "CST5CDT,M3.2.0/0,M11.1.0/1"},
  {"America/Hermosillo", "MST7"},
  {"America/Indiana/Indianapolis", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Indiana/Knox", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Indiana/Marengo", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Indiana/Petersburg", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Indiana/Tell_City", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Indiana/Vevay", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Indiana/Vincennes", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Indiana/Winamac", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Inuvik", "MST7MDT,M3.2.0,M11.1.0"},
  {"America/Iqaluit", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Jamaica", "EST5"},
  {"America/Juneau", "AKST9AKDT,M3.2.0,M11.1.0"},
  {"America/Kentucky/Louisville", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Kentucky/Monticello", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Kralendijk", "AST4"},
  {"America/La_Paz", "<-04>4"},
  {"America/Lima", "<-05>5"},
  {"America/Los_Angeles", "PST8PDT,M3.2.0,M11.1.0"},
  {"America/Lower_Princes", "AST4"},
  {"America/Maceio", "<-03>3"},
  {"America/Managua", "CST6"},
  {"America/Manaus", "<-04>4"},
  {"America/Marigot", "AST4"},
  {"America/Martinique", "AST4"},
  {"America/Matamoros", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Mazatlan", "MST7MDT,M4.1.0,M10.5.0"},
  {"America/Menominee", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Merida", "CST6CDT,M4.1.0,M10.5.0"},
  {"America/Metlakatla", "AKST9AKDT,M3.2.0,M11.1.0"},
  {"America/Mexico_City", "CST6CDT,M4.1.0,M10.5.0"},
  {"America/Miquelon", "<-03>3<-02>,M3.2.0,M11.1.0"},
  {"America/Moncton", "AST4ADT,M3.2.0,M11.1.0"},
  {"America/Monterrey", "CST6CDT,M4.1.0,M10.5.0"},
  {"America/Montevideo", "<-03>3"},
  {"America/Montserrat", "AST4"},
  {"America/Nassau", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/New_York", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Nipigon", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Nome", "AKST9AKDT,M3.2.0,M11.1.0"},
  {"America/Noronha", "<-02>2"},
  {"America/North_Dakota/Beulah", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/North_Dakota/Center", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/North_Dakota/New_Salem", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Ojinaga", "MST7MDT,M3.2.0,M11.1.0"},
  {"America/Panama", "EST5"},
  {"America/Pangnirtung", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Paramaribo", "<-03>3"},
  {"America/Phoenix", "MST7"},
  {"America/Port-au-Prince", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Port_of_Spain", "AST4"},
  {"America/Porto_Velho", "<-04>4"},
  {"America/Puerto_Rico", "AST4"},
  {"America/Rainy_River", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Rankin_Inlet", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Recife", "<-03>3"},
  {"America/Regina", "CST6"},
  {"America/Resolute", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Rio_Branco", "<-05>5"},
  {"America/Santarem", "<-03>3"},
  {"America/Santiago", "<-04>4<-03>,M9.1.6/24,M4.1.6/24"},
  {"America/Santo_Domingo", "AST4"},
  {"America/Sao_Paulo", "<-03>3"},
  {"America/Scoresbysund", "<-01>1<+00>,M3.5.0/0,M10.5.0/1"},
  {"America/Sitka", "AKST9AKDT,M3.2.0,M11.1.0"},
  {"America/St_Barthelemy", "AST4"},
  {"America/St_Johns", "NST3:30NDT,M3.2.0,M11.1.0"},
  {"America/St_Kitts", "AST4"},
  {"America/St_Lucia", "AST4"},
  {"America/St_Thomas", "AST4"},
  {"America/St_Vincent", "AST4"},
  {"America/Swift_Current", "CST6"},
  {"America/Tegucigalpa", "CST6"},
  {"America/Thule", "AST4ADT,M3.2.0,M11.1.0"},
  {"America/Thunder_Bay", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Tijuana", "PST8PDT,M3.2.0,M11.1.0"},
  {"America/Toronto", "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Tortola", "AST4"},
  {"America/Vancouver", "PST8PDT,M3.2.0,M11.1.0"},
  {"America/Whitehorse", "MST7"},
  {"America/Winnipeg", "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Yakutat", "AKST9AKDT,M3.2.0,M11.1.0"},
  {"America/Yellowknife", "MST7MDT,M3.2.0,M11.1.0"},
  {"Antarctica/Casey", "<+11>-11"},
  {"Antarctica/Davis", "<+07>-7"},
  {"Antarctica/DumontDUrville", "<+10>-10"},
  {"Antarctica/Macquarie", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
  {"Antarctica/Mawson", "<+05>-5"},
  {"Antarctica/McMurdo", "NZST-12NZDT,M9.5.0,M4.1.0/3"},
  {"Antarctica/Palmer", "<-03>3"},
  {"Antarctica/Rothera", "<-03>3"},
  {"Antarctica/Syowa", "<+03>-3"},
  {"Antarctica/Troll", "<+00>0<+02>-2,M3.5.0/1,M10.5.0/3"},
  {"Antarctica/Vostok", "<+06>-6"},
  {"Arctic/Longyearbyen", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Asia/Aden", "<+03>-3"},
  {"Asia/Almaty", "<+06>-6"},
  {"Asia/Amman", "EET-2EEST,M3.5.4/24,M10.5.5/1"},
  {"Asia/Anadyr", "<+12>-12"},
  {"Asia/Aqtau", "<+05>-5"},
  {"Asia/Aqtobe", "<+05>-5"},
  {"Asia/Ashgabat", "<+05>-5"},
  {"Asia/Atyrau", "<+05>-5"},
  {"Asia/Baghdad", "<+03>-3"},
  {"Asia/Bahrain", "<+03>-3"},
  {"Asia/Baku", "<+04>-4"},
  {"Asia/Bangkok", "<+07>-7"},
  {"Asia/Barnaul", "<+07>-7"},
  {"Asia/Beirut", "EET-2EEST,M3.5.0/0,M10.5.0/0"},
  {"Asia/Bishkek", "<+06>-6"},
  {"Asia/Brunei", "<+08>-8"},
  {"Asia/Chita", "<+09>-9"},
  {"Asia/Choibalsan", "<+08>-8"},
  {"Asia/Colombo", "<+0530>-5:30"},
  {"Asia/Damascus", "EET-2EEST,M3.5.5/0,M10.5.5/0"},
  {"Asia/Dhaka", "<+06>-6"},
  {"Asia/Dili", "<+09>-9"},
  {"Asia/Dubai", "<+04>-4"},
  {"Asia/Dushanbe", "<+05>-5"},
  {"Asia/Famagusta", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Asia/Gaza", "EET-2EEST,M3.4.4/48,M10.4.4/49"},
  {"Asia/Hebron", "EET-2EEST,M3.4.4/48,M10.4.4/49"},
  {"Asia/Ho_Chi_Minh", "<+07>-7"},
  {"Asia/Hong_Kong", "HKT-8"},
  {"Asia/Hovd", "<+07>-7"},
  {"Asia/Irkutsk", "<+08>-8"},
  {"Asia/Jakarta", "WIB-7"},
  {"Asia/Jayapura", "WIT-9"},
  {"Asia/Jerusalem", "IST-2IDT,M3.4.4/26,M10.5.0"},
  {"Asia/Kabul", "<+0430>-4:30"},
  {"Asia/Kamchatka", "<+12>-12"},
  {"Asia/Karachi", "PKT-5"},
  {"Asia/Kathmandu", "<+0545>-5:45"},
  {"Asia/Khandyga", "<+09>-9"},
  {"Asia/Kolkata", "IST-5:30"},
  {"Asia/Krasnoyarsk", "<+07>-7"},
  {"Asia/Kuala_Lumpur", "<+08>-8"},
  {"Asia/Kuching", "<+08>-8"},
  {"Asia/Kuwait", "<+03>-3"},
  {"Asia/Macau", "CST-8"},
  {"Asia/Magadan", "<+11>-11"},
  {"Asia/Makassar", "WITA-8"},
  {"Asia/Manila", "PST-8"},
  {"Asia/Muscat", "<+04>-4"},
  {"Asia/Nicosia", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Asia/Novokuznetsk", "<+07>-7"},
  {"Asia/Novosibirsk", "<+07>-7"},
  {"Asia/Omsk", "<+06>-6"},
  {"Asia/Oral", "<+05>-5"},
  {"Asia/Phnom_Penh", "<+07>-7"},
  {"Asia/Pontianak", "WIB-7"},
  {"Asia/Pyongyang", "KST-9"},
  {"Asia/Qatar", "<+03>-3"},
  {"Asia/Qyzylorda", "<+05>-5"},
  {"Asia/Riyadh", "<+03>-3"},
  {"Asia/Sakhalin", "<+11>-11"},
  {"Asia/Samarkand", "<+05>-5"},
  {"Asia/Seoul", "KST-9"},
  {"Asia/Shanghai", "CST-8"},
  {"Asia/Singapore", "<+08>-8"},
  {"Asia/Srednekolymsk", "<+11>-11"},
  {"Asia/Taipei", "CST-8"},
  {"Asia/Tashkent", "<+05>-5"},
  {"Asia/Tbilisi", "<+04>-4"},
  {"Asia/Tehran", "<+0330>-3:30<+0430>,J79/24,J263/24"},
  {"Asia/Thimphu", "<+06>-6"},
  {"Asia/Tokyo", "JST-9"},
  {"Asia/Tomsk", "<+07>-7"},
  {"Asia/Ulaanbaatar", "<+08>-8"},
  {"Asia/Urumqi", "<+06>-6"},
  {"Asia/Ust-Nera", "<+10>-10"},
  {"Asia/Vientiane", "<+07>-7"},
  {"Asia/Vladivostok", "<+10>-10"},
  {"Asia/Yakutsk", "<+09>-9"},
  {"Asia/Yangon", "<+0630>-6:30"},
  {"Asia/Yekaterinburg", "<+05>-5"},
  {"Asia/Yerevan", "<+04>-4"},
  {"Atlantic/Azores", "<-01>1<+00>,M3.5.0/0,M10.5.0/1"},
  {"Atlantic/Bermuda", "AST4ADT,M3.2.0,M11.1.0"},
  {"Atlantic/Canary", "WET0WEST,M3.5.0/1,M10.5.0"},
  {"Atlantic/Cape_Verde", "<-01>1"},
  {"Atlantic/Faroe", "WET0WEST,M3.5.0/1,M10.5.0"},
  {"Atlantic/Madeira", "WET0WEST,M3.5.0/1,M10.5.0"},
  {"Atlantic/Reykjavik", "GMT0"},
  {"Atlantic/South_Georgia", "<-02>2"},
  {"Atlantic/St_Helena", "GMT0"},
  {"Atlantic/Stanley", "<-03>3"},
  {"Australia/Adelaide", "ACST-9:30ACDT,M10.1.0,M4.1.0/3"},
  {"Australia/Brisbane", "AEST-10"},
  {"Australia/Broken_Hill", "ACST-9:30ACDT,M10.1.0,M4.1.0/3"},
  {"Australia/Currie", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
  {"Australia/Darwin", "ACST-9:30"},
  {"Australia/Eucla", "<+0845>-8:45"},
  {"Australia/Hobart", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
  {"Australia/Lindeman", "AEST-10"},
  {"Australia/Lord_Howe", "<+1030>-10:30<+11>-11,M10.1.0,M4.1.0"},
  {"Australia/Melbourne", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
  {"Australia/Perth", "AWST-8"},
  {"Australia/Sydney", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
  {"Europe/Amsterdam", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Andorra", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Astrakhan", "<+04>-4"},
  {"Europe/Athens", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Belgrade", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Berlin", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Bratislava", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Brussels", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Bucharest", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Budapest", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Busingen", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Chisinau", "EET-2EEST,M3.5.0,M10.5.0/3"},
  {"Europe/Copenhagen", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Dublin", "IST-1GMT0,M10.5.0,M3.5.0/1"},
  {"Europe/Gibraltar", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Guernsey", "GMT0BST,M3.5.0/1,M10.5.0"},
  {"Europe/Helsinki", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Isle_of_Man", "GMT0BST,M3.5.0/1,M10.5.0"},
  {"Europe/Istanbul", "<+03>-3"},
  {"Europe/Jersey", "GMT0BST,M3.5.0/1,M10.5.0"},
  {"Europe/Kaliningrad", "EET-2"},
  {"Europe/Kiev", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Kirov", "<+03>-3"},
  {"Europe/Lisbon", "WET0WEST,M3.5.0/1,M10.5.0"},
  {"Europe/Ljubljana", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/London", "GMT0BST,M3.5.0/1,M10.5.0"},
  {"Europe/Luxembourg", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Madrid", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Malta", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Mariehamn", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Minsk", "<+03>-3"},
  {"Europe/Monaco", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Moscow", "MSK-3"},
  {"Europe/Oslo", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Paris", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Podgorica", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Prague", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Riga", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Rome", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Samara", "<+04>-4"},
  {"Europe/San_Marino", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Sarajevo", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Saratov", "<+04>-4"},
  {"Europe/Simferopol", "MSK-3"},
  {"Europe/Skopje", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Sofia", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Stockholm", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Tallinn", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Tirane", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Ulyanovsk", "<+04>-4"},
  {"Europe/Uzhgorod", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Vaduz", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Vatican", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Vienna", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Vilnius", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Volgograd", "<+03>-3"},
  {"Europe/Warsaw", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Zagreb", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/Zaporozhye", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Zurich", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Indian/Antananarivo", "EAT-3"},
  {"Indian/Chagos", "<+06>-6"},
  {"Indian/Christmas", "<+07>-7"},
  {"Indian/Cocos", "<+0630>-6:30"},
  {"Indian/Comoro", "EAT-3"},
  {"Indian/Kerguelen", "<+05>-5"},
  {"Indian/Mahe", "<+04>-4"},
  {"Indian/Maldives", "<+05>-5"},
  {"Indian/Mauritius", "<+04>-4"},
  {"Indian/Mayotte", "EAT-3"},
  {"Indian/Reunion", "<+04>-4"},
  {"Pacific/Apia", "<+13>-13<+14>,M9.5.0/3,M4.1.0/4"},
  {"Pacific/Auckland", "NZST-12NZDT,M9.5.0,M4.1.0/3"},
  {"Pacific/Bougainville", "<+11>-11"},
  {"Pacific/Chatham", "<+1245>-12:45<+1345>,M9.5.0/2:45,M4.1.0/3:45"},
  {"Pacific/Chuuk", "<+10>-10"},
  {"Pacific/Easter", "<-06>6<-05>,M9.1.6/22,M4.1.6/22"},
  {"Pacific/Efate", "<+11>-11"},
  {"Pacific/Enderbury", "<+13>-13"},
  {"Pacific/Fakaofo", "<+13>-13"},
  {"Pacific/Fiji", "<+12>-12<+13>,M11.2.0,M1.2.3/99"},
  {"Pacific/Funafuti", "<+12>-12"},
  {"Pacific/Galapagos", "<-06>6"},
  {"Pacific/Gambier", "<-09>9"},
  {"Pacific/Guadalcanal", "<+11>-11"},
  {"Pacific/Guam", "ChST-10"},
  {"Pacific/Honolulu", "HST10"},
  {"Pacific/Kiritimati", "<+14>-14"},
  {"Pacific/Kosrae", "<+11>-11"},
  {"Pacific/Kwajalein", "<+12>-12"},
  {"Pacific/Majuro", "<+12>-12"},
  {"Pacific/Marquesas", "<-0930>9:30"},
  {"Pacific/Midway", "SST11"},
  {"Pacific/Nauru", "<+12>-12"},
  {"Pacific/Niue", "<-11>11"},
  {"Pacific/Norfolk", "<+11>-11<+12>,M10.1.0,M4.1.0/3"},
  {"Pacific/Noumea", "<+11>-11"},
  {"Pacific/Pago_Pago", "SST11"},
  {"Pacific/Palau", "<+09>-9"},
  {"Pacific/Pitcairn", "<-08>8"},
  {"Pacific/Pohnpei", "<+11>-11"},
  {"Pacific/Port_Moresby", "<+10>-10"},
  {"Pacific/Rarotonga", "<-10>10"},
  {"Pacific/Saipan", "ChST-10"},
  {"Pacific/Tahiti", "<-10>10"},
  {"Pacific/Tarawa", "<+12>-12"},
  {"Pacific/Tongatapu", "<+13>-13"},
  {"Pacific/Wake", "<+12>-12"},
  {"Pacific/Wallis", "<+12>-12"},
  {"UTC", "UTC0"},
};

const char captive_root_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta content="text/html; charset=ISO-8859-1" http-equiv="content-type">
<meta name = "viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0">
<link rel="icon" type="image/png" href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAQCAYAAAAmlE46AAABgmlDQ1BzUkdCIElFQzYxOTY2LTIuMQAAKJF1kblLQ0EQhz+jomg88AALiyDRKpEYIWhjEfECtUgiGLVJXi4hx+O9BAm2gq2gINp4FfoXaCtYC4KiCGInWCvaqDznJYEEMbPMzre/3Rl2Z8ESSCopvc4FqXRW8015bYvBJVvDC81000oHrpCiq3P+yQBV7fOeGjPeOs1a1c/9a82RqK5ATaPwmKJqWeFp4dm1rGryjnCXkghFhM+EHZpcUPjO1MNFfjU5XuRvk7WAbxws7cK2eAWHK1hJaClheTn2VDKnlO5jvsQaTS/4JfaJ96LjYwovNmaYYBwPQ4zK7MGJm0FZUSXfVcifJyO5iswqeTRWiZMgi0PUnFSPSoyJHpWRJG/2/29f9diwu1jd6oX6Z8N474eGbfjZMoyvI8P4OYbaJ7hMl/MzhzDyIfpWWbMfQNsGnF+VtfAuXGxCz6Ma0kIFqVbcEovB2ym0BKHzBpqWiz0r7XPyAIF1+apr2NuHATnftvILiEJn9X8wM74AAAAJcEhZcwAAHYcAAB2HAY/l8WUAAAG3SURBVCiRhdI9SNVhFMfxz//+nz+N1VDgpiXlloJIRdDQCw1R4FBE72TvcUMiTKUyK1MrJIuG1GiRsqEtMMghyBJzKDKICLEhCiJ6oaLAvA33KppGv+3wfL+H5xxOZLpUtC3C0Vx1XvvuZ38jkb038jET87AA5SjDE4xgCfpxB68whM9BnAwhwg88xwAOurp5ABzoLMIObEABZmM0ku7KYAtuat04Ou3XJybdtQmdQZzAFy3l/5cgTr4hSomTjDjJTAFq7s/6h5gRJ7+DOPmKNlXdHWjXtOZNloiuq+mZiw7c1rDie058i8eRmp5CVGAn5qAb18ThEw5hPX7iFtqdXt4PkbrePHw0OgLrsAer8A5VUqEb27ELRbKbPxyp7yvAQ6x2YvELcOrR/ByYcXJp7fh89X1H0ISSCJwduIISLFNbOnVRWWYGnuKB2tJ9KZAKZ6RCsVTYP62UZWqkQp5UOA5Zsbr4vTi0iEOj5sGVU6TmwbXicEwc6lUXf4Aw/hiHOtl7vefiywbU5RqfQyUacWkMjyZ1bnkd4wLS6EWChdiqsvDuRHSyOJbW4TJcxi9sk84f/hv5A3wVcXHcw0KKAAAAAElFTkSuQmCC" />
<title>STL-CF Setup</title>
<style>
body { background-color: #0067B3; font-family: Arial, Helvetica, Sans-Serif; color: #FFFFFF; }
input[type="submit"] { height: 50px; cursor: pointer; background-color: #3498DB; border: none; color: #FFFFFF; padding: 15px 48px; margin: 25px auto; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; }
form { padding: 25px 0px; }
table { width: 100%; max-width: 400px; }
.table-header { width: 50%; text-align: left; }
.table-value { width: 50%; text-align: right; }
.header { width: 100%; max-width: 400px; color: #FFFFFF; font-family: verdana; padding: 25px 0px; font-size: 36px; }
.info { width: 100%; max-width: 400px; color: #FFFFFF; font-family: verdana; padding: 25px 0px; font-size: 12px; }
.label { font-family: Times New Roman }
input[type="text"], select { height: 30px; width: 100%; max-width: 400px; padding: 5px 10px; margin: 8px auto; border: 2px solid #3498DB; border-radius: 4px; box-sizing: border-box; }
select { cursor: pointer; }
input[type="checkbox"] { cursor: pointer; margin: 8px auto; width: 20px; height: 20px; }
.credentials { display: none; }
</style>
</head>
<body>
<center>
<h1 class="header">STL-CF Setup</h1>
<table>
<tr><td class="table-header">Module type:</td><td class="table-value"><span id="type"></td></tr>
<tr><td class="table-header">Firmware version:</td><td class="table-value"><span id="firmware"></td></tr>
<tr><td class="table-header">Unit ID:</td><td class="table-value"><span id="unit"></span></td></tr>
<tr><td class="table-header">MAC Address:</td><td class="table-value"><span id="mac"></span></td></tr>
</table>
<form id="form1" action="/setup" method="POST">
<p><label class="label">Sentinel Cloud-Relay Server URL / IP</label><br><input type="text" name="server" placeholder="sentinel-cloud-relay.example.com" autocapitalize="off" required><br></p>
<p><label class="label">Sentinel Cloud-Relay Server Port</label><br><input type="text" pattern="[0-9]{2,4}" name="port" placeholder="443" autocapitalize="off" required><br></p>
<p><label class="label">NTP Server</label><br><input type="text" name="ntp" placeholder="pool.ntp.org" autocapitalize="off" required><br></p> 
<p><label class="label">Timezone</label><br><select name="timezone">
<option value="Africa/Abidjan">Africa/Abidjan</option>
<option value="Africa/Accra">Africa/Accra</option>
<option value="Africa/Addis_Ababa">Africa/Addis_Ababa</option>
<option value="Africa/Algiers">Africa/Algiers</option>
<option value="Africa/Asmara">Africa/Asmara</option>
<option value="Africa/Bamako">Africa/Bamako</option>
<option value="Africa/Bangui">Africa/Bangui</option>
<option value="Africa/Banjul">Africa/Banjul</option>
<option value="Africa/Bissau">Africa/Bissau</option>
<option value="Africa/Blantyre">Africa/Blantyre</option>
<option value="Africa/Brazzaville">Africa/Brazzaville</option>
<option value="Africa/Bujumbura">Africa/Bujumbura</option>
<option value="Africa/Cairo">Africa/Cairo</option>
<option value="Africa/Casablanca">Africa/Casablanca</option>
<option value="Africa/Ceuta">Africa/Ceuta</option>
<option value="Africa/Conakry">Africa/Conakry</option>
<option value="Africa/Dakar">Africa/Dakar</option>
<option value="Africa/Dar_es_Salaam">Africa/Dar_es_Salaam</option>
<option value="Africa/Djibouti">Africa/Djibouti</option>
<option value="Africa/Douala">Africa/Douala</option>
<option value="Africa/El_Aaiun">Africa/El_Aaiun</option>
<option value="Africa/Freetown">Africa/Freetown</option>
<option value="Africa/Gaborone">Africa/Gaborone</option>
<option value="Africa/Harare">Africa/Harare</option>
<option value="Africa/Johannesburg">Africa/Johannesburg</option>
<option value="Africa/Juba">Africa/Juba</option>
<option value="Africa/Kampala">Africa/Kampala</option>
<option value="Africa/Khartoum">Africa/Khartoum</option>
<option value="Africa/Kigali">Africa/Kigali</option>
<option value="Africa/Kinshasa">Africa/Kinshasa</option>
<option value="Africa/Lagos">Africa/Lagos</option>
<option value="Africa/Libreville">Africa/Libreville</option>
<option value="Africa/Lome">Africa/Lome</option>
<option value="Africa/Luanda">Africa/Luanda</option>
<option value="Africa/Lubumbashi">Africa/Lubumbashi</option>
<option value="Africa/Lusaka">Africa/Lusaka</option>
<option value="Africa/Malabo">Africa/Malabo</option>
<option value="Africa/Maputo">Africa/Maputo</option>
<option value="Africa/Maseru">Africa/Maseru</option>
<option value="Africa/Mbabane">Africa/Mbabane</option>
<option value="Africa/Mogadishu">Africa/Mogadishu</option>
<option value="Africa/Monrovia">Africa/Monrovia</option>
<option value="Africa/Nairobi">Africa/Nairobi</option>
<option value="Africa/Ndjamena">Africa/Ndjamena</option>
<option value="Africa/Niamey">Africa/Niamey</option>
<option value="Africa/Nouakchott">Africa/Nouakchott</option>
<option value="Africa/Ouagadougou">Africa/Ouagadougou</option>
<option value="Africa/Porto-Novo">Africa/Porto-Novo</option>
<option value="Africa/Sao_Tome">Africa/Sao_Tome</option>
<option value="Africa/Tripoli">Africa/Tripoli</option>
<option value="Africa/Tunis">Africa/Tunis</option>
<option value="Africa/Windhoek">Africa/Windhoek</option>
<option value="America/Adak">America/Adak</option>
<option value="America/Anchorage">America/Anchorage</option>
<option value="America/Anguilla">America/Anguilla</option>
<option value="America/Antigua">America/Antigua</option>
<option value="America/Araguaina">America/Araguaina</option>
<option value="America/Argentina/Buenos_Aires">America/Argentina/Buenos_Aires</option>
<option value="America/Argentina/Catamarca">America/Argentina/Catamarca</option>
<option value="America/Argentina/Cordoba">America/Argentina/Cordoba</option>
<option value="America/Argentina/Jujuy">America/Argentina/Jujuy</option>
<option value="America/Argentina/La_Rioja">America/Argentina/La_Rioja</option>
<option value="America/Argentina/Mendoza">America/Argentina/Mendoza</option>
<option value="America/Argentina/Rio_Gallegos">America/Argentina/Rio_Gallegos</option>
<option value="America/Argentina/Salta">America/Argentina/Salta</option>
<option value="America/Argentina/San_Juan">America/Argentina/San_Juan</option>
<option value="America/Argentina/San_Luis">America/Argentina/San_Luis</option>
<option value="America/Argentina/Tucuman">America/Argentina/Tucuman</option>
<option value="America/Argentina/Ushuaia">America/Argentina/Ushuaia</option>
<option value="America/Aruba">America/Aruba</option>
<option value="America/Asuncion">America/Asuncion</option>
<option value="America/Atikokan">America/Atikokan</option>
<option value="America/Bahia">America/Bahia</option>
<option value="America/Bahia_Banderas">America/Bahia_Banderas</option>
<option value="America/Barbados">America/Barbados</option>
<option value="America/Belem">America/Belem</option>
<option value="America/Belize">America/Belize</option>
<option value="America/Blanc-Sablon">America/Blanc-Sablon</option>
<option value="America/Boa_Vista">America/Boa_Vista</option>
<option value="America/Bogota">America/Bogota</option>
<option value="America/Boise">America/Boise</option>
<option value="America/Cambridge_Bay">America/Cambridge_Bay</option>
<option value="America/Campo_Grande">America/Campo_Grande</option>
<option value="America/Cancun">America/Cancun</option>
<option value="America/Caracas">America/Caracas</option>
<option value="America/Cayenne">America/Cayenne</option>
<option value="America/Cayman">America/Cayman</option>
<option value="America/Chicago">America/Chicago</option>
<option value="America/Chihuahua">America/Chihuahua</option>
<option value="America/Costa_Rica">America/Costa_Rica</option>
<option value="America/Creston">America/Creston</option>
<option value="America/Cuiaba">America/Cuiaba</option>
<option value="America/Curacao">America/Curacao</option>
<option value="America/Danmarkshavn">America/Danmarkshavn</option>
<option value="America/Dawson">America/Dawson</option>
<option value="America/Dawson_Creek">America/Dawson_Creek</option>
<option value="America/Denver">America/Denver</option>
<option value="America/Detroit">America/Detroit</option>
<option value="America/Dominica">America/Dominica</option>
<option value="America/Edmonton">America/Edmonton</option>
<option value="America/Eirunepe">America/Eirunepe</option>
<option value="America/El_Salvador">America/El_Salvador</option>
<option value="America/Fort_Nelson">America/Fort_Nelson</option>
<option value="America/Fortaleza">America/Fortaleza</option>
<option value="America/Glace_Bay">America/Glace_Bay</option>
<option value="America/Godthab">America/Godthab</option>
<option value="America/Goose_Bay">America/Goose_Bay</option>
<option value="America/Grand_Turk">America/Grand_Turk</option>
<option value="America/Grenada">America/Grenada</option>
<option value="America/Guadeloupe">America/Guadeloupe</option>
<option value="America/Guatemala">America/Guatemala</option>
<option value="America/Guayaquil">America/Guayaquil</option>
<option value="America/Guyana">America/Guyana</option>
<option value="America/Halifax">America/Halifax</option>
<option value="America/Havana">America/Havana</option>
<option value="America/Hermosillo">America/Hermosillo</option>
<option value="America/Indiana/Indianapolis">America/Indiana/Indianapolis</option>
<option value="America/Indiana/Knox">America/Indiana/Knox</option>
<option value="America/Indiana/Marengo">America/Indiana/Marengo</option>
<option value="America/Indiana/Petersburg">America/Indiana/Petersburg</option>
<option value="America/Indiana/Tell_City">America/Indiana/Tell_City</option>
<option value="America/Indiana/Vevay">America/Indiana/Vevay</option>
<option value="America/Indiana/Vincennes">America/Indiana/Vincennes</option>
<option value="America/Indiana/Winamac">America/Indiana/Winamac</option>
<option value="America/Inuvik">America/Inuvik</option>
<option value="America/Iqaluit">America/Iqaluit</option>
<option value="America/Jamaica">America/Jamaica</option>
<option value="America/Juneau">America/Juneau</option>
<option value="America/Kentucky/Louisville">America/Kentucky/Louisville</option>
<option value="America/Kentucky/Monticello">America/Kentucky/Monticello</option>
<option value="America/Kralendijk">America/Kralendijk</option>
<option value="America/La_Paz">America/La_Paz</option>
<option value="America/Lima">America/Lima</option>
<option value="America/Los_Angeles">America/Los_Angeles</option>
<option value="America/Lower_Princes">America/Lower_Princes</option>
<option value="America/Maceio">America/Maceio</option>
<option value="America/Managua">America/Managua</option>
<option value="America/Manaus">America/Manaus</option>
<option value="America/Marigot">America/Marigot</option>
<option value="America/Martinique">America/Martinique</option>
<option value="America/Matamoros">America/Matamoros</option>
<option value="America/Mazatlan">America/Mazatlan</option>
<option value="America/Menominee">America/Menominee</option>
<option value="America/Merida">America/Merida</option>
<option value="America/Metlakatla">America/Metlakatla</option>
<option value="America/Mexico_City">America/Mexico_City</option>
<option value="America/Miquelon">America/Miquelon</option>
<option value="America/Moncton">America/Moncton</option>
<option value="America/Monterrey">America/Monterrey</option>
<option value="America/Montevideo">America/Montevideo</option>
<option value="America/Montserrat">America/Montserrat</option>
<option value="America/Nassau">America/Nassau</option>
<option value="America/New_York">America/New_York</option>
<option value="America/Nipigon">America/Nipigon</option>
<option value="America/Nome">America/Nome</option>
<option value="America/Noronha">America/Noronha</option>
<option value="America/North_Dakota/Beulah">America/North_Dakota/Beulah</option>
<option value="America/North_Dakota/Center">America/North_Dakota/Center</option>
<option value="America/North_Dakota/New_Salem">America/North_Dakota/New_Salem</option>
<option value="America/Ojinaga">America/Ojinaga</option>
<option value="America/Panama">America/Panama</option>
<option value="America/Pangnirtung">America/Pangnirtung</option>
<option value="America/Paramaribo">America/Paramaribo</option>
<option value="America/Phoenix">America/Phoenix</option>
<option value="America/Port-au-Prince">America/Port-au-Prince</option>
<option value="America/Port_of_Spain">America/Port_of_Spain</option>
<option value="America/Porto_Velho">America/Porto_Velho</option>
<option value="America/Puerto_Rico">America/Puerto_Rico</option>
<option value="America/Rainy_River">America/Rainy_River</option>
<option value="America/Rankin_Inlet">America/Rankin_Inlet</option>
<option value="America/Recife">America/Recife</option>
<option value="America/Regina">America/Regina</option>
<option value="America/Resolute">America/Resolute</option>
<option value="America/Rio_Branco">America/Rio_Branco</option>
<option value="America/Santarem">America/Santarem</option>
<option value="America/Santiago">America/Santiago</option>
<option value="America/Santo_Domingo">America/Santo_Domingo</option>
<option value="America/Sao_Paulo">America/Sao_Paulo</option>
<option value="America/Scoresbysund">America/Scoresbysund</option>
<option value="America/Sitka">America/Sitka</option>
<option value="America/St_Barthelemy">America/St_Barthelemy</option>
<option value="America/St_Johns">America/St_Johns</option>
<option value="America/St_Kitts">America/St_Kitts</option>
<option value="America/St_Lucia">America/St_Lucia</option>
<option value="America/St_Thomas">America/St_Thomas</option>
<option value="America/St_Vincent">America/St_Vincent</option>
<option value="America/Swift_Current">America/Swift_Current</option>
<option value="America/Tegucigalpa">America/Tegucigalpa</option>
<option value="America/Thule">America/Thule</option>
<option value="America/Thunder_Bay">America/Thunder_Bay</option>
<option value="America/Tijuana">America/Tijuana</option>
<option value="America/Toronto">America/Toronto</option>
<option value="America/Tortola">America/Tortola</option>
<option value="America/Vancouver">America/Vancouver</option>
<option value="America/Whitehorse">America/Whitehorse</option>
<option value="America/Winnipeg">America/Winnipeg</option>
<option value="America/Yakutat">America/Yakutat</option>
<option value="America/Yellowknife">America/Yellowknife</option>
<option value="Antarctica/Casey">Antarctica/Casey</option>
<option value="Antarctica/Davis">Antarctica/Davis</option>
<option value="Antarctica/DumontDUrville">Antarctica/DumontDUrville</option>
<option value="Antarctica/Macquarie">Antarctica/Macquarie</option>
<option value="Antarctica/Mawson">Antarctica/Mawson</option>
<option value="Antarctica/McMurdo">Antarctica/McMurdo</option>
<option value="Antarctica/Palmer">Antarctica/Palmer</option>
<option value="Antarctica/Rothera">Antarctica/Rothera</option>
<option value="Antarctica/Syowa">Antarctica/Syowa</option>
<option value="Antarctica/Troll">Antarctica/Troll</option>
<option value="Antarctica/Vostok">Antarctica/Vostok</option>
<option value="Arctic/Longyearbyen">Arctic/Longyearbyen</option>
<option value="Asia/Aden">Asia/Aden</option>
<option value="Asia/Almaty">Asia/Almaty</option>
<option value="Asia/Amman">Asia/Amman</option>
<option value="Asia/Anadyr">Asia/Anadyr</option>
<option value="Asia/Aqtau">Asia/Aqtau</option>
<option value="Asia/Aqtobe">Asia/Aqtobe</option>
<option value="Asia/Ashgabat">Asia/Ashgabat</option>
<option value="Asia/Atyrau">Asia/Atyrau</option>
<option value="Asia/Baghdad">Asia/Baghdad</option>
<option value="Asia/Bahrain">Asia/Bahrain</option>
<option value="Asia/Baku">Asia/Baku</option>
<option value="Asia/Bangkok">Asia/Bangkok</option>
<option value="Asia/Barnaul">Asia/Barnaul</option>
<option value="Asia/Beirut">Asia/Beirut</option>
<option value="Asia/Bishkek">Asia/Bishkek</option>
<option value="Asia/Brunei">Asia/Brunei</option>
<option value="Asia/Chita">Asia/Chita</option>
<option value="Asia/Choibalsan">Asia/Choibalsan</option>
<option value="Asia/Colombo">Asia/Colombo</option>
<option value="Asia/Damascus">Asia/Damascus</option>
<option value="Asia/Dhaka">Asia/Dhaka</option>
<option value="Asia/Dili">Asia/Dili</option>
<option value="Asia/Dubai">Asia/Dubai</option>
<option value="Asia/Dushanbe">Asia/Dushanbe</option>
<option value="Asia/Famagusta">Asia/Famagusta</option>
<option value="Asia/Gaza">Asia/Gaza</option>
<option value="Asia/Hebron">Asia/Hebron</option>
<option value="Asia/Ho_Chi_Minh">Asia/Ho_Chi_Minh</option>
<option value="Asia/Hong_Kong">Asia/Hong_Kong</option>
<option value="Asia/Hovd">Asia/Hovd</option>
<option value="Asia/Irkutsk">Asia/Irkutsk</option>
<option value="Asia/Jakarta">Asia/Jakarta</option>
<option value="Asia/Jayapura">Asia/Jayapura</option>
<option value="Asia/Jerusalem">Asia/Jerusalem</option>
<option value="Asia/Kabul">Asia/Kabul</option>
<option value="Asia/Kamchatka">Asia/Kamchatka</option>
<option value="Asia/Karachi">Asia/Karachi</option>
<option value="Asia/Kathmandu">Asia/Kathmandu</option>
<option value="Asia/Khandyga">Asia/Khandyga</option>
<option value="Asia/Kolkata">Asia/Kolkata</option>
<option value="Asia/Krasnoyarsk">Asia/Krasnoyarsk</option>
<option value="Asia/Kuala_Lumpur">Asia/Kuala_Lumpur</option>
<option value="Asia/Kuching">Asia/Kuching</option>
<option value="Asia/Kuwait">Asia/Kuwait</option>
<option value="Asia/Macau">Asia/Macau</option>
<option value="Asia/Magadan">Asia/Magadan</option>
<option value="Asia/Makassar">Asia/Makassar</option>
<option value="Asia/Manila">Asia/Manila</option>
<option value="Asia/Muscat">Asia/Muscat</option>
<option value="Asia/Nicosia">Asia/Nicosia</option>
<option value="Asia/Novokuznetsk">Asia/Novokuznetsk</option>
<option value="Asia/Novosibirsk">Asia/Novosibirsk</option>
<option value="Asia/Omsk">Asia/Omsk</option>
<option value="Asia/Oral">Asia/Oral</option>
<option value="Asia/Phnom_Penh">Asia/Phnom_Penh</option>
<option value="Asia/Pontianak">Asia/Pontianak</option>
<option value="Asia/Pyongyang">Asia/Pyongyang</option>
<option value="Asia/Qatar">Asia/Qatar</option>
<option value="Asia/Qyzylorda">Asia/Qyzylorda</option>
<option value="Asia/Riyadh">Asia/Riyadh</option>
<option value="Asia/Sakhalin">Asia/Sakhalin</option>
<option value="Asia/Samarkand">Asia/Samarkand</option>
<option value="Asia/Seoul">Asia/Seoul</option>
<option value="Asia/Shanghai">Asia/Shanghai</option>
<option value="Asia/Singapore">Asia/Singapore</option>
<option value="Asia/Srednekolymsk">Asia/Srednekolymsk</option>
<option value="Asia/Taipei">Asia/Taipei</option>
<option value="Asia/Tashkent">Asia/Tashkent</option>
<option value="Asia/Tbilisi">Asia/Tbilisi</option>
<option value="Asia/Tehran">Asia/Tehran</option>
<option value="Asia/Thimphu">Asia/Thimphu</option>
<option value="Asia/Tokyo">Asia/Tokyo</option>
<option value="Asia/Tomsk">Asia/Tomsk</option>
<option value="Asia/Ulaanbaatar">Asia/Ulaanbaatar</option>
<option value="Asia/Urumqi">Asia/Urumqi</option>
<option value="Asia/Ust-Nera">Asia/Ust-Nera</option>
<option value="Asia/Vientiane">Asia/Vientiane</option>
<option value="Asia/Vladivostok">Asia/Vladivostok</option>
<option value="Asia/Yakutsk">Asia/Yakutsk</option>
<option value="Asia/Yangon">Asia/Yangon</option>
<option value="Asia/Yekaterinburg">Asia/Yekaterinburg</option>
<option value="Asia/Yerevan">Asia/Yerevan</option>
<option value="Atlantic/Azores">Atlantic/Azores</option>
<option value="Atlantic/Bermuda">Atlantic/Bermuda</option>
<option value="Atlantic/Canary">Atlantic/Canary</option>
<option value="Atlantic/Cape_Verde">Atlantic/Cape_Verde</option>
<option value="Atlantic/Faroe">Atlantic/Faroe</option>
<option value="Atlantic/Madeira">Atlantic/Madeira</option>
<option value="Atlantic/Reykjavik">Atlantic/Reykjavik</option>
<option value="Atlantic/South_Georgia">Atlantic/South_Georgia</option>
<option value="Atlantic/St_Helena">Atlantic/St_Helena</option>
<option value="Atlantic/Stanley">Atlantic/Stanley</option>
<option value="Australia/Adelaide">Australia/Adelaide</option>
<option value="Australia/Brisbane">Australia/Brisbane</option>
<option value="Australia/Broken_Hill">Australia/Broken_Hill</option>
<option value="Australia/Currie">Australia/Currie</option>
<option value="Australia/Darwin">Australia/Darwin</option>
<option value="Australia/Eucla">Australia/Eucla</option>
<option value="Australia/Hobart">Australia/Hobart</option>
<option value="Australia/Lindeman">Australia/Lindeman</option>
<option value="Australia/Lord_Howe">Australia/Lord_Howe</option>
<option value="Australia/Melbourne">Australia/Melbourne</option>
<option value="Australia/Perth">Australia/Perth</option>
<option value="Australia/Sydney">Australia/Sydney</option>
<option value="Europe/Amsterdam">Europe/Amsterdam</option>
<option value="Europe/Andorra">Europe/Andorra</option>
<option value="Europe/Astrakhan">Europe/Astrakhan</option>
<option value="Europe/Athens">Europe/Athens</option>
<option value="Europe/Belgrade">Europe/Belgrade</option>
<option value="Europe/Berlin">Europe/Berlin</option>
<option value="Europe/Bratislava">Europe/Bratislava</option>
<option value="Europe/Brussels">Europe/Brussels</option>
<option value="Europe/Bucharest">Europe/Bucharest</option>
<option value="Europe/Budapest">Europe/Budapest</option>
<option value="Europe/Busingen">Europe/Busingen</option>
<option value="Europe/Chisinau">Europe/Chisinau</option>
<option value="Europe/Copenhagen">Europe/Copenhagen</option>
<option value="Europe/Dublin">Europe/Dublin</option>
<option value="Europe/Gibraltar">Europe/Gibraltar</option>
<option value="Europe/Guernsey">Europe/Guernsey</option>
<option value="Europe/Helsinki">Europe/Helsinki</option>
<option value="Europe/Isle_of_Man">Europe/Isle_of_Man</option>
<option value="Europe/Istanbul">Europe/Istanbul</option>
<option value="Europe/Jersey">Europe/Jersey</option>
<option value="Europe/Kaliningrad">Europe/Kaliningrad</option>
<option value="Europe/Kiev">Europe/Kiev</option>
<option value="Europe/Kirov">Europe/Kirov</option>
<option value="Europe/Lisbon">Europe/Lisbon</option>
<option value="Europe/Ljubljana">Europe/Ljubljana</option>
<option value="Europe/London">Europe/London</option>
<option value="Europe/Luxembourg">Europe/Luxembourg</option>
<option value="Europe/Madrid">Europe/Madrid</option>
<option value="Europe/Malta">Europe/Malta</option>
<option value="Europe/Mariehamn">Europe/Mariehamn</option>
<option value="Europe/Minsk">Europe/Minsk</option>
<option value="Europe/Monaco">Europe/Monaco</option>
<option value="Europe/Moscow">Europe/Moscow</option>
<option value="Europe/Oslo">Europe/Oslo</option>
<option value="Europe/Paris">Europe/Paris</option>
<option value="Europe/Podgorica">Europe/Podgorica</option>
<option value="Europe/Prague">Europe/Prague</option>
<option value="Europe/Riga">Europe/Riga</option>
<option value="Europe/Rome">Europe/Rome</option>
<option value="Europe/Samara">Europe/Samara</option>
<option value="Europe/San_Marino">Europe/San_Marino</option>
<option value="Europe/Sarajevo">Europe/Sarajevo</option>
<option value="Europe/Saratov">Europe/Saratov</option>
<option value="Europe/Simferopol">Europe/Simferopol</option>
<option value="Europe/Skopje">Europe/Skopje</option>
<option value="Europe/Sofia">Europe/Sofia</option>
<option value="Europe/Stockholm">Europe/Stockholm</option>
<option value="Europe/Tallinn">Europe/Tallinn</option>
<option value="Europe/Tirane">Europe/Tirane</option>
<option value="Europe/Ulyanovsk">Europe/Ulyanovsk</option>
<option value="Europe/Uzhgorod">Europe/Uzhgorod</option>
<option value="Europe/Vaduz">Europe/Vaduz</option>
<option value="Europe/Vatican">Europe/Vatican</option>
<option value="Europe/Vienna">Europe/Vienna</option>
<option value="Europe/Vilnius">Europe/Vilnius</option>
<option value="Europe/Volgograd">Europe/Volgograd</option>
<option value="Europe/Warsaw">Europe/Warsaw</option>
<option value="Europe/Zagreb">Europe/Zagreb</option>
<option value="Europe/Zaporozhye">Europe/Zaporozhye</option>
<option value="Europe/Zurich">Europe/Zurich</option>
<option value="Indian/Antananarivo">Indian/Antananarivo</option>
<option value="Indian/Chagos">Indian/Chagos</option>
<option value="Indian/Christmas">Indian/Christmas</option>
<option value="Indian/Cocos">Indian/Cocos</option>
<option value="Indian/Comoro">Indian/Comoro</option>
<option value="Indian/Kerguelen">Indian/Kerguelen</option>
<option value="Indian/Mahe">Indian/Mahe</option>
<option value="Indian/Maldives">Indian/Maldives</option>
<option value="Indian/Mauritius">Indian/Mauritius</option>
<option value="Indian/Mayotte">Indian/Mayotte</option>
<option value="Indian/Reunion">Indian/Reunion</option>
<option value="Pacific/Apia">Pacific/Apia</option>
<option value="Pacific/Auckland">Pacific/Auckland</option>
<option value="Pacific/Bougainville">Pacific/Bougainville</option>
<option value="Pacific/Chatham">Pacific/Chatham</option>
<option value="Pacific/Chuuk">Pacific/Chuuk</option>
<option value="Pacific/Easter">Pacific/Easter</option>
<option value="Pacific/Efate">Pacific/Efate</option>
<option value="Pacific/Enderbury">Pacific/Enderbury</option>
<option value="Pacific/Fakaofo">Pacific/Fakaofo</option>
<option value="Pacific/Fiji">Pacific/Fiji</option>
<option value="Pacific/Funafuti">Pacific/Funafuti</option>
<option value="Pacific/Galapagos">Pacific/Galapagos</option>
<option value="Pacific/Gambier">Pacific/Gambier</option>
<option value="Pacific/Guadalcanal">Pacific/Guadalcanal</option>
<option value="Pacific/Guam">Pacific/Guam</option>
<option value="Pacific/Honolulu">Pacific/Honolulu</option>
<option value="Pacific/Kiritimati">Pacific/Kiritimati</option>
<option value="Pacific/Kosrae">Pacific/Kosrae</option>
<option value="Pacific/Kwajalein">Pacific/Kwajalein</option>
<option value="Pacific/Majuro">Pacific/Majuro</option>
<option value="Pacific/Marquesas">Pacific/Marquesas</option>
<option value="Pacific/Midway">Pacific/Midway</option>
<option value="Pacific/Nauru">Pacific/Nauru</option>
<option value="Pacific/Niue">Pacific/Niue</option>
<option value="Pacific/Norfolk">Pacific/Norfolk</option>
<option value="Pacific/Noumea">Pacific/Noumea</option>
<option value="Pacific/Pago_Pago">Pacific/Pago_Pago</option>
<option value="Pacific/Palau">Pacific/Palau</option>
<option value="Pacific/Pitcairn">Pacific/Pitcairn</option>
<option value="Pacific/Pohnpei">Pacific/Pohnpei</option>
<option value="Pacific/Port_Moresby">Pacific/Port_Moresby</option>
<option value="Pacific/Rarotonga">Pacific/Rarotonga</option>
<option value="Pacific/Saipan">Pacific/Saipan</option>
<option value="Pacific/Tahiti">Pacific/Tahiti</option>
<option value="Pacific/Tarawa">Pacific/Tarawa</option>
<option value="Pacific/Tongatapu">Pacific/Tongatapu</option>
<option value="Pacific/Wake">Pacific/Wake</option>
<option value="Pacific/Wallis">Pacific/Wallis</option>
<option value="UTC">UTC</option>
</select><br></p>
<p><label class="label">GNSS Mode</label><br><select name="gnss">
<option value="1">GLONASS</option>
<option value="2">BEIDOU</option>
<option value="4">GALILEO</option>
<option value="8">QZSS</option>
<option value="3">GLONASS + BEIDOU</option>
<option value="5">GLONASS + GALILEO</option>
<option value="6">BEIDOU + GALILEO</option>
<option value="7">GLONASS + BEIDOU + GALILEO</option>
<option value="15">GLONASS + BEIDOU + GALILEO + QZSS</option>
</select>
<p><label class="label">GPRS APN</label><br><input type="text" name="apn" autocapitalize="off" required><br></p>
<p><label class="label">GPRS Credentials</label><br><input type="checkbox" name="credentials"><br></p>
<p class="credentials"><label class="label">GPRS User</label><br><input type="text" name="user" autocapitalize="off"><br></p>
<p class="credentials"><label class="label">GPRS Password</label><br><input type="text" name="pass" autocapitalize="off"><br></p>
<input type="submit" value="Save & Reboot">
<h2 class="info">You need to save the configuration and reboot the device, otherwise the device will not work</h2>
</form>
</center>
<script>
  window.onload = function async () {
    fetch("http://" + location.host + "/config").then((response) => response.json()).then((json) => {
      var type = document.getElementById("type");
      type.innerHTML = json.module_type;
      var fw = document.getElementById("firmware");
      fw.innerHTML = json.firmware;
      var unit = document.getElementById("unit");
      unit.innerHTML = json.unit_id;
      var mac = document.getElementById("mac");
      mac.innerHTML = json.mac_address;
      var form1 = document.getElementById("form1");
      form1.server.value = json.server;
      form1.port.value = json.port;
      form1.ntp.value = json.ntp;
      form1.timezone.value = json.timezone;
      form1.gnss.value = json.gnss_mode;
      form1.apn.value = json.apn;
      form1.credentials.checked = json.credentials;
      if (json.credentials) {
        var cred = document.getElementsByClassName('credentials');
        for(i = 0; i < cred.length; i++) {
          cred[i].style.display = 'block';
        }
        form1.user.required = true;
        form1.pass.required = true;
      }
      form1.user.value = json.user;
      form1.pass.value = json.pass;
    });
    var form1 = document.getElementById("form1");
    var cred = document.getElementsByClassName('credentials');
    form1.credentials.onclick = function () {
      if (this.form.credentials.checked) {
        for(i = 0; i < cred.length; i++) {
          cred[i].style.display = 'block';
        }
        form1.user.required = true;
        form1.pass.required = true;
      } else {
        for(i = 0; i < cred.length; i++) {
          cred[i].style.display = 'none';
        }
        form1.user.required = false;
        form1.pass.required = false;
      }
    }
  }
</script>
</body>
</html>
)rawliteral";

const char captive_setup_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta content="text/html; charset=ISO-8859-1" http-equiv="content-type">
<meta name = "viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0">
<link rel="icon" type="image/png" href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAQCAYAAAAmlE46AAABgmlDQ1BzUkdCIElFQzYxOTY2LTIuMQAAKJF1kblLQ0EQhz+jomg88AALiyDRKpEYIWhjEfECtUgiGLVJXi4hx+O9BAm2gq2gINp4FfoXaCtYC4KiCGInWCvaqDznJYEEMbPMzre/3Rl2Z8ESSCopvc4FqXRW8015bYvBJVvDC81000oHrpCiq3P+yQBV7fOeGjPeOs1a1c/9a82RqK5ATaPwmKJqWeFp4dm1rGryjnCXkghFhM+EHZpcUPjO1MNFfjU5XuRvk7WAbxws7cK2eAWHK1hJaClheTn2VDKnlO5jvsQaTS/4JfaJ96LjYwovNmaYYBwPQ4zK7MGJm0FZUSXfVcifJyO5iswqeTRWiZMgi0PUnFSPSoyJHpWRJG/2/29f9diwu1jd6oX6Z8N474eGbfjZMoyvI8P4OYbaJ7hMl/MzhzDyIfpWWbMfQNsGnF+VtfAuXGxCz6Ma0kIFqVbcEovB2ym0BKHzBpqWiz0r7XPyAIF1+apr2NuHATnftvILiEJn9X8wM74AAAAJcEhZcwAAHYcAAB2HAY/l8WUAAAG3SURBVCiRhdI9SNVhFMfxz//+nz+N1VDgpiXlloJIRdDQCw1R4FBE72TvcUMiTKUyK1MrJIuG1GiRsqEtMMghyBJzKDKICLEhCiJ6oaLAvA33KppGv+3wfL+H5xxOZLpUtC3C0Vx1XvvuZ38jkb038jET87AA5SjDE4xgCfpxB68whM9BnAwhwg88xwAOurp5ABzoLMIObEABZmM0ku7KYAtuat04Ou3XJybdtQmdQZzAFy3l/5cgTr4hSomTjDjJTAFq7s/6h5gRJ7+DOPmKNlXdHWjXtOZNloiuq+mZiw7c1rDie058i8eRmp5CVGAn5qAb18ThEw5hPX7iFtqdXt4PkbrePHw0OgLrsAer8A5VUqEb27ELRbKbPxyp7yvAQ6x2YvELcOrR/ByYcXJp7fh89X1H0ISSCJwduIISLFNbOnVRWWYGnuKB2tJ9KZAKZ6RCsVTYP62UZWqkQp5UOA5Zsbr4vTi0iEOj5sGVU6TmwbXicEwc6lUXf4Aw/hiHOtl7vefiywbU5RqfQyUacWkMjyZ1bnkd4wLS6EWChdiqsvDuRHSyOJbW4TJcxi9sk84f/hv5A3wVcXHcw0KKAAAAAElFTkSuQmCC" />
<title>STL-CF Setup</title>
<style>
body { background-color: #0067B3; font-family: Arial, Helvetica, Sans-Serif; color: #FFFFFF; }
.header { color: #FFFFFF; font-family: verdana; padding: 25px 0px; font-size: 36px; }
.message { color: #FFFFFF; font-family: verdana; padding: 25px 0px; font-size: 22px; }
.info { font-size: 16px; }
.loader { margin: 48px auto; width: 84px; height: 84px; border-radius: 50%; position: relative; animation: rotate 1s linear infinite; }
.loader::before { content: ""; box-sizing: border-box; position: absolute; inset: 0px; border-radius: 50%; border: 5px solid #FFF; animation: prixClipFix 2s linear infinite; }
@keyframes rotate { 100% { transform: rotate(360deg); } }
@-webkit-keyframes rotate { 100% { -webkit-transform: rotate(360deg); } }
@keyframes prixClipFix {
0% { clip-path: polygon(50% 50%,0 0,0 0,0 0,0 0,0 0); }
25% { clip-path: polygon(50% 50%,0 0,100% 0,100% 0,100% 0,100% 0); }
50% { clip-path: polygon(50% 50%,0 0,100% 0,100% 100%,100% 100%,100% 100%); }
75% { clip-path: polygon(50% 50%,0 0,100% 0,100% 100%,0 100%,0 100%); }
100% { clip-path: polygon(50% 50%,0 0,100% 0,100% 100%,0 100%,0 0); } }
@-webkit-keyframes prixClipFix {
0% { -webkit-clip-path: polygon(50% 50%,0 0,0 0,0 0,0 0,0 0); }
25% { -webkit-clip-path: polygon(50% 50%,0 0,100% 0,100% 0,100% 0,100% 0); }
50% { -webkit-clip-path: polygon(50% 50%,0 0,100% 0,100% 100%,100% 100%,100% 100%); }
75% { -webkit-clip-path: polygon(50% 50%,0 0,100% 0,100% 100%,0 100%,0 100%); }
100% { -webkit-clip-path: polygon(50% 50%,0 0,100% 0,100% 100%,0 100%,0 0); } }
</style>
</head>
<body>
<center>
<h1 class="header">STL-CF Setup</h1>
<div class="loader"></div>
<h2 class="message">Saving configuration<br><span class="info">Device will be rebooted</span></h2>
</center>
</body>
</html>
)rawliteral";

// ----------------------------------------------------------------------------------------------------------------

const char* dbData = "Callback function called";
char *dbErrorMessage = 0;
sqlite3_stmt *dbRes;
const char *dbTail;
int dbRc;

bool extra_reboot = DEFAULT_EXTRA_REBOOT;
bool is_setup_done = DEFAULT_IS_SETUP_DONE;
String backend_server = DEFAULT_BACKEND_SERVER;
int backend_port = DEFAULT_BACKEND_PORT;
String ntp_server = DEFAULT_NTP_SERVER;
String timezone = DEFAULT_TIMEZONE;
String timezone_name = DEFAULT_TIMEZONE_NAME;
int gnss_mode = DEFAULT_GNSS_MODE;
String fw_version = MODULE_FIRMWARE_VERSION;
String mac_address_raw;
String mac_address_nag;
String wifi_hostname;
String gprs_apn = DEFAULT_GPRS_APN;
bool gprs_credentials = DEFAULT_GPRS_CREDENTIALS;
String gprs_user = DEFAULT_GPRS_USER;
String gprs_pass = DEFAULT_GPRS_PASS;

bool is_network_connected = false;
bool is_gprs_connected = false;


// String current_ride_id; // TODO
// String current_driver_card; // TODO

int config_button_last_state = 1;
bool config_button_pressed = false;
bool config_button_active = false;
unsigned long config_button_last_time = 0;
unsigned long config_button_period = 5000;

Ticker gps_ticker;
const int gps_period = 5000; // 10000 

float gps_last_lat = 0.0;
float gps_last_lon = 0.0;
float gps_last_alt = 0.0;
float gps_last_speed = 0.0;
float gps_last_accuracy = 0.0;
DateTime gps_last_datetime = DateTime(2000, 1, 1, 0, 0, 0);

// ----------------------------------------------------------------------------------------------------------------

String mac2String(byte ar[]) { // Done
  String s;
  for (byte i = 0; i < 6; ++i) {
    char buf[3];
    sprintf(buf, "%02X", ar[i]); // J-M-L: slight modification, added the 0 in the format for padding 
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

String defaultHostname() { // Done
  uint64_t esp_id = ESP.getEfuseMac();
  String mac_address = mac2String((byte*) &esp_id);
  mac_address.replace(":", "");
  String hostname = MODULE_TYPE + String('-') + mac_address;
  return hostname;
}

void clearWifi() { // Done
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
}

void setupUnitVariables() { // Done
  uint64_t esp_id = ESP.getEfuseMac();
  String mac_address = mac2String((byte*) &esp_id);
  mac_address_raw = mac_address;
  mac_address.replace(":", "");
  mac_address_nag = mac_address;
  wifi_hostname = defaultHostname();
}

void setupEspPins() { // TODO

  // TODO

  pinMode(ESP_WIFI_BUTTON, INPUT_PULLUP);

  pinMode(ESP_BOARD_LED, OUTPUT);
  digitalWrite(ESP_BOARD_LED, HIGH); // off
  pinMode(GSM_MODULE_POWER, OUTPUT);
  pinMode(GSM_MODULE_PWR, OUTPUT);
  pinMode(GSM_MODULE_INT, INPUT);

  SerialMon.println("[ESP] Success");
}

void setupRtc() { // Done
  if (!rtc.begin()) {
    SerialMon.println("[RTC] Error");
    while (1);
  }
  SerialMon.println("[RTC] Success");
  if (rtc.lostPower()) {
    SerialMon.println("[RTC] Lost power, setting default date (1/1/2000 00:00:00)");
     rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0)); // 1.1. 1970 00:00:00
  }
  SerialMon.print("[RTC] Temperature is: "); 
  SerialMon.print(rtc.getTemperature());
  SerialMon.println(" °C");
}

int openDb(const char *filename, sqlite3 **db) { // Done
  dbRc = sqlite3_open(filename, db);
  if (dbRc) {
    SerialMon.print("[DB] Can't open database: ");
    SerialMon.println(sqlite3_errmsg(*db));
    return dbRc;
  } 
  return dbRc;
}

static int dbCallback(void *data, int dataLength, char **colValue, char **colName) { // Done
  SerialMon.println((const char*)data);
  // no output printed - use only for data in to database
  return 0;
}

int dbExec(sqlite3 *db, const char *sql) { // Done
  SerialMon.print("[DB] "); 
  SerialMon.println(sql); // TODO - test

  dbRc = sqlite3_exec(db, sql, dbCallback, (void*)dbData, &dbErrorMessage);
  if (dbRc != SQLITE_OK) {
    SerialMon.print("[DB] SQL error (");
    SerialMon.print(dbRc);
    SerialMon.print("): ");
    SerialMon.println(dbErrorMessage);
    sqlite3_free(dbErrorMessage);
  } 
  return dbRc;
}

// void dbExecPrep(String sql) {
//   dbRc = sqlite3_prepare_v2(fs_db, sql.c_str(), strlen(sql.c_str()), &dbRes, &dbTail);
//   if (dbRc != SQLITE_OK) {
//     SerialMon.print("[DB] Failed to update data: ");
//     SerialMon.println(sqlite3_errmsg(fs_db));
//   }
//   if (sqlite3_step(dbRes) != SQLITE_DONE) {
//     SerialMon.print("[DB] ERROR executing stmt (");
//     SerialMon.print(sqlite3_step(dbRes));
//     SerialMon.print("): ");
//     SerialMon.println(sqlite3_errmsg(fs_db)); // TODO
//   }
//   sqlite3_finalize(dbRes);
// }

// String loadConfigStringVariable(String sql, String default_value) { // TODO - how to check when column value is null => set column not null
//   dbRc = sqlite3_prepare_v2(fs_db, sql.c_str(), strlen(sql.c_str()), &dbRes, &dbTail); // sqlite3_prepare_v2(fs_db, sql.c_str(), -1, &dbRes, &dbTail) || sqlite3_prepare_v2(fs_db, sql.c_str(), 1000, &res, &tail)
//   if (dbRc != SQLITE_OK) {
//     SerialMon.print("Failed to fetch data: ");
//     SerialMon.println(sqlite3_errmsg(fs_db));
//     return default_value;
//   }
//   while (sqlite3_step(dbRes) == SQLITE_ROW) {
//     return (const char *) sqlite3_column_text(dbRes, 0);
//   }
//   sqlite3_finalize(dbRes);
//   return default_value; // failover if no rows
// }

// int loadConfigIntVariable(String sql, int default_value) { // TODO - how to check when column value is null => set column not null
//   dbRc = sqlite3_prepare_v2(fs_db, sql.c_str(), strlen(sql.c_str()), &dbRes, &dbTail); // sqlite3_prepare_v2(fs_db, sql.c_str(), -1, &dbRes, &dbTail) || sqlite3_prepare_v2(fs_db, sql.c_str(), 1000, &res, &tail)
//   if (dbRc != SQLITE_OK) {
//     SerialMon.print("Failed to fetch data: ");
//     SerialMon.println(sqlite3_errmsg(fs_db));
//     return default_value;
//   }
//   while (sqlite3_step(dbRes) == SQLITE_ROW) {
//     return sqlite3_column_int(dbRes, 0);
//   }
//   sqlite3_finalize(dbRes);
//   return default_value; // failover if no rows
// }

void loadDatabaseConfig() { // ?
  // if (openDb("/spiffs/database.db", &fs_db))
  //   return;
  // is_setup_done = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'is_setup_done'", DEFAULT_IS_SETUP_DONE);
  // fw_version = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'fw_version'", MODULE_FIRMWARE_VERSION);
  // wifi_hostname = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'hostname'", defaultHostname());
  // ntp_server = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'ntp_server'", DEFAULT_NTP_SERVER);
  // timezone = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'time_zone'", DEFAULT_TIMEZONE);
  // timezone_name = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'time_zone_name'", DEFAULT_TIMEZONE_NAME);
  // gnss_mode = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'gnss_mode'", DEFAULT_GNSS_MODE);
  // backend_server = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'backend_server'", DEFAULT_BACKEND_SERVER);
  // backend_port = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'backend_port'", DEFAULT_BACKEND_PORT);
  // gprs_apn = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'gprs_apn'", DEFAULT_GPRS_APN);
  // gprs_credentials = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'gprs_credentials'", DEFAULT_GPRS_CREDENTIALS);
  // gprs_user = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'gprs_user'", DEFAULT_GPRS_USER);
  // gprs_pass = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'gprs_pass'", DEFAULT_GPRS_PASS);
  // sqlite3_close(fs_db);

  SerialMon.println("[FS] Database Values Loaded");
}

void initDatabaseFile() { // ?
  // String sqlArray[] = {
  //   "CREATE TABLE IF NOT EXISTS config (variable TEXT, value)",
  //   "INSERT INTO config VALUES ('is_setup_done','" + String(DEFAULT_IS_SETUP_DONE) + "')",
  //   "INSERT INTO config VALUES ('fw_version','" + String(MODULE_FIRMWARE_VERSION) + "')",
  //   "INSERT INTO config VALUES ('hostname','" + defaultHostname() + "')",
  //   "INSERT INTO config VALUES ('ntp_server','" + String(DEFAULT_NTP_SERVER) + "')",
  //   "INSERT INTO config VALUES ('time_zone_name','" + String(DEFAULT_TIMEZONE_NAME) + "')",
  //   "INSERT INTO config VALUES ('time_zone','" + String(DEFAULT_TIMEZONE) + "')",
  //   "INSERT INTO config VALUES ('gnss_mode','" + String(DEFAULT_GNSS_MODE) + "')",
  //   "INSERT INTO config VALUES ('backend_server','" + String(DEFAULT_BACKEND_SERVER) + "')",
  //   "INSERT INTO config VALUES ('backend_port','" + String(DEFAULT_BACKEND_PORT) + "')",
  //   "INSERT INTO config VALUES ('gprs_apn','" + String(DEFAULT_GPRS_APN) + "')",
  //   "INSERT INTO config VALUES ('gprs_credentials','" + String(DEFAULT_GPRS_CREDENTIALS) + "')",
  //   "INSERT INTO config VALUES ('gprs_user','" + String(DEFAULT_GPRS_USER) + "')",
  //   "INSERT INTO config VALUES ('gprs_pass','" + String(DEFAULT_GPRS_PASS) + "')"
  // };

  // if (openDb("/spiffs/database.db", &fs_db))
  //   return;

  // for (int i = 0; i < sizeof(sqlArray)/sizeof(sqlArray[0]); i++) {
  //   dbRc = dbExec(fs_db, sqlArray[i].c_str());
  //   if (dbRc != SQLITE_OK) {
  //     sqlite3_close(fs_db);
  //     return;
  //   }
  //   // dbExecPrep(sqlArray[i]);
  // }

  // sqlite3_close(fs_db);
  SerialMon.println("[FS] Database Structure Written Into Database File");
}

void handlePreferences() { // Done
  preferences.begin("module-config", false);
  if (preferences.isKey("setup_done")) { // config exist
    fw_version = preferences.getString("fw_version");
    wifi_hostname = preferences.getString("hostname");
    ntp_server = preferences.getString("ntp_server");
    timezone = preferences.getString("time_zone");
    timezone_name = preferences.getString("tz_name");
    gnss_mode = preferences.getInt("gnss_mode");
    backend_server = preferences.getString("server");
    backend_port = preferences.getInt("port");
    gprs_apn = preferences.getString("gprs_apn");
    gprs_credentials = preferences.getBool("gprs_cred");
    gprs_user = preferences.getString("gprs_user");
    gprs_pass = preferences.getString("gprs_pass");
    extra_reboot = preferences.getBool("extra_reboot");
    gps_last_lat = preferences.getFloat("last_lat");
    gps_last_lon = preferences.getFloat("last_lon");
    gps_last_alt = preferences.getFloat("last_alt");
    gps_last_speed = preferences.getFloat("last_speed");
    gps_last_accuracy = preferences.getFloat("last_accuracy");
    gps_last_datetime = DateTime(preferences.getString("last_datetime").c_str());
    is_setup_done = preferences.getBool("setup_done");
    SerialMon.println("[CONFIG] Preferences Loaded");
  } else { // config not exist
    preferences.putString("fw_version", MODULE_FIRMWARE_VERSION);
    preferences.putString("hostname", defaultHostname());
    preferences.putString("ntp_server", DEFAULT_NTP_SERVER);
    preferences.putString("time_zone", DEFAULT_TIMEZONE);
    preferences.putString("tz_name", DEFAULT_TIMEZONE_NAME);
    preferences.putInt("gnss_mode", DEFAULT_GNSS_MODE);
    preferences.putString("server", DEFAULT_BACKEND_SERVER);
    preferences.putInt("port", DEFAULT_BACKEND_PORT);
    preferences.putString("gprs_apn", DEFAULT_GPRS_APN);
    preferences.putBool("gprs_cred", DEFAULT_GPRS_CREDENTIALS);
    preferences.putString("gprs_user", DEFAULT_GPRS_USER);
    preferences.putString("gprs_pass", DEFAULT_GPRS_PASS);
    preferences.putBool("extra_reboot", DEFAULT_EXTRA_REBOOT);
    preferences.putFloat("last_lat", 0.0);
    preferences.putFloat("last_lon", 0.0);
    preferences.putFloat("last_alt", 0.0);
    preferences.putFloat("last_speed", 0.0);
    preferences.putFloat("last_accuracy", 0.0);
    preferences.putString("last_datetime", DateTime(2000,1,1,0,0,0).timestamp());
    preferences.putBool("setup_done", DEFAULT_IS_SETUP_DONE);
    SerialMon.println("[CONFIG] Preferences Writtent Into Memory");
  }
}

void setupFileSystem() { // Done
  if (!SPIFFS.begin(FORMAT_FS_IF_FAILED)) {
    SerialMon.println("[FS] File System Mount Failed");
    while(1);
  } else {
    SerialMon.println("[FS] File System Mounted");
    if (SPIFFS.exists("/database.db")) {
      SerialMon.println("[FS] Database File Exists");
      loadDatabaseConfig();
    } else {
      SerialMon.println("[FS] Database File Does Not Exist");
      File dbFile;
      dbFile = SPIFFS.open("/database.db", FILE_WRITE);
      dbFile.close();
      SerialMon.println("[FS] Database File Created");
      initDatabaseFile();
    }
    handlePreferences();
  }
}

// void sendSMS(String recipient, String message) {
//   bool sms_send = modem.sendSMS(recipient, message);
//   SerialMon.print("[GSM] SMS: ");
//   SerialMon.println(sms_send ? "Sent" : "Failed");
// }

void getGpsPosition() { // TODO
  // float lat, lon, speed, alt, accuracy;
  int year, month, day, hour, minute, second;
  if (modem.getGPS(&gps_last_lat, &gps_last_lon, &gps_last_speed, &gps_last_alt, NULL, NULL, &gps_last_accuracy, &year, &month, &day, &hour, &minute, &second)) {
    
    gps_last_datetime = DateTime(year, month, day, hour, minute, second);
    rtc.adjust(gps_last_datetime);

    preferences.putFloat("last_lat", gps_last_lat);
    preferences.putFloat("last_lon", gps_last_lon);
    preferences.putFloat("last_alt", gps_last_alt);
    preferences.putFloat("last_speed", gps_last_speed);
    preferences.putFloat("last_accuracy", gps_last_accuracy);
    preferences.putString("last_datetime", gps_last_datetime.timestamp());

    SerialMon.print("[GPS]");
  } else {
    SerialMon.print("[GPS] timed out | Last known location -> ");
  }

  SerialMon.print(" lat: ");
  SerialMon.print(gps_last_lat, 6);
  SerialMon.print(" lon: ");
  SerialMon.print(gps_last_lon, 6);
  SerialMon.print(" speed: ");
  SerialMon.print(gps_last_speed);
  SerialMon.print(" alt: ");
  SerialMon.print(gps_last_alt);
  SerialMon.print(" accuracy: ");
  SerialMon.print(gps_last_accuracy);
  SerialMon.print(" (");
  SerialMon.print(gps_last_datetime.timestamp());
  SerialMon.println(")");

  // TODO - here sending to server -> heartbeat + gps realtime + car monitoring realtime
}

void setupGsmModule() { // Done
  digitalWrite(GSM_MODULE_POWER, HIGH); // POWER_PIN: This pin controls the power supply of the SIM7600
  digitalWrite(GSM_MODULE_PWR, HIGH); // PWR_PIN：This Pin is the PWR-KEY of the SIM7600, The time of active low level impulse of PWRKEY pin to power on module , type 500 ms
  delay(500);
  digitalWrite(GSM_MODULE_PWR, LOW);
  attachInterrupt(GSM_MODULE_INT, []() { // If SIM7600 starts normally, then set the onboard LED to flash once every 1 second
    detachInterrupt(GSM_MODULE_INT);
    led_ticker.attach_ms(1000, []() {
      digitalWrite(ESP_BOARD_LED, !digitalRead(ESP_BOARD_LED));
    });
  }, RISING);
          
  delay(5000);
  SerialAT.begin(GSM_MODULE_UART_BAUD, SERIAL_8N1, GSM_MODULE_RX, GSM_MODULE_TX);
  
  if (digitalRead(GSM_MODULE_INT) == 0) {
    SerialMon.println("[GSM] Waiting For Power On...");
    delay(15000); // ? - maybe use while until pin rising
  } else {
    detachInterrupt(GSM_MODULE_INT);
    led_ticker.attach_ms(1000, []() {
      digitalWrite(ESP_BOARD_LED, !digitalRead(ESP_BOARD_LED));
    });
  }
  
  SerialMon.print("[GSM] Initializing modem: ");
  if (modem.init()) {
    SerialMon.println("OK");
  } else {
    SerialMon.println("failed");
  }

  SerialMon.print("[GSM] Setting Network Mode: ");
  if (modem.setNetworkMode(DEFAULT_NETWORK_MODE)) {
    SerialMon.println("OK");
  } else {
    SerialMon.println("failed");
  }

  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    SerialMon.print("[GSM] Unlocking SIM: ");
    if (modem.simUnlock(GSM_PIN)) {
      SerialMon.println("OK");
    } else {
      SerialMon.println("failed");
    }
  }

  SerialMon.print("[GSM] Setting GNSS Mode: ");
  String gnss_res = modem.setGNSSMode(gnss_mode, 1); // String gnss_res = modem.setGNSSMode(gnss_mode, 0);
  if (gnss_res == "OK") {
    SerialMon.println("OK");
  } else {
    SerialMon.println("failed");
  }

  SerialMon.print("[GSM] Enabling GPS: ");
  if(modem.enableGPS()) {
    SerialMon.println("OK");
  } else {
    SerialMon.println("failed");
  }

  String name = modem.getModemName();
  SerialMon.println("[GSM] Modem Name: " + name);
  String modemInfo = modem.getModemInfo();
  SerialMon.println("[GSM] Modem Info: " + modemInfo);
  SimStatus simStatus = modem.getSimStatus();
  SerialMon.println("[GSM] SIM Status: " + String(simStatus));
  int16_t networkMode = modem.getNetworkMode(); 
  SerialMon.println("[GSM] Network Mode: " + String(networkMode));
  RegStatus registrationStatus = modem.getRegistrationStatus();
  SerialMon.println("[GSM] Registration Status: " + String(registrationStatus));
  uint8_t gnssMode = modem.getGNSSMode();
  SerialMon.println("[GSM] GNSS Mode: " + String(gnssMode));
  int16_t csq = modem.getSignalQuality();
  SerialMon.println("[GSM] Signal quality: " + String(csq));
  String imei = modem.getIMEI();
  SerialMon.println("[GSM] IMEI: " + imei);
  String cop = modem.getOperator();
  SerialMon.println("[GSM] Operator: " + cop);

  SerialMon.print("[GSM] Waiting for network: ");
  if (modem.waitForNetwork()) {
    SerialMon.println("OK");
  } else {
    delay(10000);
    SerialMon.println("failed");
    return;
  }

  is_network_connected = modem.isNetworkConnected();
  SerialMon.print("[GSM] Network status: ");
  SerialMon.println(is_network_connected ? "connected" : "not connected");

  SerialMon.print("[GSM] Starting GPRS (apn: " + gprs_apn);
  SerialMon.print(", user: " + gprs_user);
  SerialMon.print(", pass: " + gprs_pass);
  SerialMon.print("): ");
  if (modem.gprsConnect(gprs_apn.c_str(), gprs_user.c_str(), gprs_pass.c_str())) {
    SerialMon.println("OK");
  } else {
    delay(10000);
    SerialMon.println("failed");
    return;
  }

  is_gprs_connected = modem.isGprsConnected();
  SerialMon.print("[GSM] GPRS status: ");
  SerialMon.println(is_gprs_connected ? "connected" : "not connected");

  IPAddress local = modem.localIP();
  SerialMon.println("[GSM] Local IP: " + String(local));
  String imsi = modem.getIMSI();
  SerialMon.println("[GSM] IMSI: " + imsi);
  String ccid = modem.getSimCCID();
  SerialMon.println("[GSM] CCID: " + ccid);

  // sendSMS("+420xxxxxxxxx", String("Hello from ") + imei);

  SerialMon.println("[GSM] Setup Done");
  
  if (is_setup_done && !extra_reboot) {
    led_ticker.detach();
    digitalWrite(ESP_BOARD_LED, LOW); // module LED -> ON
    SerialMon.println("[GSM] Starting GPS Scheduler");
    gps_ticker.attach_ms(gps_period, getGpsPosition);
  }
}

void onCardReaderReceive() { // TODO
  SerialMon.println("[READER] Receiving data");
  if (SerialCardReader.available()) {
    String incoming = SerialCardReader.readString();
    SerialMon.println("[READER] Data: " + incoming);
  }
}

void setupCardReader() { // Done
  SerialCardReader.begin(9600, SERIAL_8N1, ESP_RX_CARD_READER, ESP_TX_CARD_READER);
  SerialCardReader.onReceive(onCardReaderReceive);
  SerialMon.println("[READER] Card Reader Setup Done");
}

String getTimezoneValue(String timezoneName) { // Done
  String out = DEFAULT_TIMEZONE;
  for (int x = 0; x <= sizeof(tzArray)/sizeof(tzArray[0]); x++) {
    if (timezoneName == tzArray[x].name) {
      out = tzArray[x].value;
    }
  }
  return out;
}

void handleWebServerRootGet() { // Done
  webServer.send_P(200, "text/html", captive_root_html);
}

void handleWebServerConfigGet() { // Done
  DynamicJsonDocument json_doc(1024);
  json_doc["module_type"] = MODULE_TYPE;
  json_doc["firmware"] = fw_version;
  json_doc["unit_id"] = mac_address_nag;
  json_doc["mac_address"] = mac_address_raw;
  json_doc["server"] = backend_server;
  json_doc["port"] = backend_port;
  json_doc["ntp"] = ntp_server;
  json_doc["timezone"] = timezone_name;
  json_doc["gnss_mode"] = gnss_mode;
  json_doc["apn"] = gprs_apn;
  json_doc["credentials"] = gprs_credentials;
  json_doc["user"] = gprs_user;
  json_doc["pass"] = gprs_pass;
  String data_out;
  serializeJson(json_doc, data_out);
  webServer.send(200, "application/json", data_out);
}

void handleWebServerSetupPost() { // Done
  webServer.send_P(200, "text/html", captive_setup_html);
  if (webServer.arg("server") != backend_server) {
    preferences.putString("server", webServer.arg("server"));
  }
  if (webServer.arg("port") != String(backend_port)) {
    preferences.putInt("port", webServer.arg("port").toInt());
  }
  if (webServer.arg("ntp") != ntp_server) {
    preferences.putString("ntp_server", webServer.arg("ntp"));
  }
  if (webServer.arg("timezone") != timezone_name) {
    preferences.putString("tz_name", webServer.arg("timezone"));
  }
  if (getTimezoneValue(webServer.arg("timezone")) != timezone) {
    preferences.putString("time_zone", getTimezoneValue(webServer.arg("timezone")));
  }
  if (webServer.arg("gnss") != String(gnss_mode)) {
    preferences.putInt("gnss_mode", webServer.arg("gnss").toInt());
  }
  if (webServer.arg("apn") != gprs_apn) {
    preferences.putString("gprs_apn", webServer.arg("apn"));
  }
  if(webServer.arg("credentials") == "on") {
    if (!gprs_credentials) {
      preferences.putBool("gprs_cred", true);
    }
    if (webServer.arg("user") != gprs_user) {
      preferences.putString("gprs_user", webServer.arg("user"));
    }
    if (webServer.arg("pass") != gprs_pass) {
      preferences.putString("gprs_pass", webServer.arg("pass"));
    }
  } else {
    SerialMon.println("[DBG] gprs_credentials off");
    if (gprs_credentials) {
      preferences.putBool("gprs_cred", false);
    }
    if (String(DEFAULT_GPRS_USER) != gprs_user) {
      preferences.putString("gprs_user", DEFAULT_GPRS_USER);
    }
    if (String(DEFAULT_GPRS_PASS) != gprs_pass) {
      preferences.putString("gprs_pass", DEFAULT_GPRS_PASS);
    }
  }
  if (!is_setup_done) {
    preferences.putBool("setup_done", true);
  }
  preferences.putBool("extra_reboot", true);
  delay(250);
  ESP.restart();
}

void setupWebServer() { // Done
  webServer.on("/", HTTP_GET, handleWebServerRootGet);
  webServer.on("/config", HTTP_GET, handleWebServerConfigGet);
  webServer.on("/setup", HTTP_POST, handleWebServerSetupPost);
  webServer.onNotFound(handleWebServerRootGet);
  webServer.begin();
  SerialMon.println("[AP] Captive Setup Done");
}

void setupCaptivePortal() { // Done
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(wifi_ap_ip, wifi_ap_gw, wifi_ap_mask);
  WiFi.softAP(wifi_hostname.c_str(), NULL, 1, false, 1);
  SerialMon.print("[AP] Created with IP Gateway ");
  SerialMon.println(WiFi.softAPIP());
  dnsServer.start(53, "*", wifi_ap_ip);
  setupWebServer();
}

void handleCaptivePortal() { // Done
  webServer.handleClient();
  dnsServer.processNextRequest();
}

void switchToConfigMode() { // Done
  SerialMon.println("[CONFIG] Switching to config mode");
  // TODO - play tone
  preferences.putBool("setup_done", false);
  delay(250);
  ESP.restart();
}

void configButtonHandler() { // Done
  int button_current_state = digitalRead(ESP_WIFI_BUTTON);
  if (button_current_state != config_button_last_state) {
    if (millis() - config_button_last_time >= 250) {
      if (button_current_state == LOW) {
        if (!config_button_pressed) {
          SerialMon.println("[CONFIG] BUTTON PRESS");
          config_button_pressed = true;
        }
      } else {
        if (config_button_pressed) {
          SerialMon.println("[CONFIG] BUTTON RELEASE");
          config_button_active = false;
          config_button_pressed = false;
        }
      }
      config_button_last_state = button_current_state;
      config_button_last_time = millis();
    }
  } else {
    if (button_current_state == LOW) {
      if  (!config_button_active) {
        if (config_button_pressed) {
          if ((millis() - config_button_last_time > config_button_period)) {
            SerialMon.println("[CONFIG] LONG PRESS");
            config_button_active = true;
            switchToConfigMode();
          }
        }
      }
    }
  }
}

void core0Task( void * parameter ) { // TODO
  // SerialMon.print("[CORE_");
  // SerialMon.print(xPortGetCoreID());
  // SerialMon.println("] Task Started");
  for( ;; ){
    if (is_setup_done) {
      configButtonHandler(); // ?

      // TODO 
    } else {
      handleCaptivePortal();
    }
  }
}

// ----------------------------------------------------------------------------------------------------------------

void setup() {
  SerialMon.begin(9600);
  SerialMon.println();
  SerialMon.println("----------------------------------------------------------------------------------------------------------------");
  SerialMon.println("[Module Unit Name] " + String(MODULE_NAME) + " (" + String(MODULE_TYPE) + ")");
  SerialMon.println("[Hardware Version] " + String(MODULE_HARDWARE_VERSION));
  SerialMon.println("[Firmware Version] " + String(MODULE_FIRMWARE_VERSION));
  SerialMon.println("----------------------------------------------------------------------------------------------------------------");
  // ----------------------------------------------------------------------------------------------------------------
  setupEspPins();
  Wire.setPins(ESP_WIRE_SDA, ESP_WIRE_SCL);
  clearWifi();
  sqlite3_initialize();
  setupFileSystem();
  setupUnitVariables();
  setupRtc();
  setupCardReader();  // TODO - setup rfid reader
  // TODO - setup buzzer
  // ----------------------------------------------------------------------------------------------------------------
  if (is_setup_done) {
    led_ticker.attach_ms(500, []() {
      digitalWrite(ESP_BOARD_LED, !digitalRead(ESP_BOARD_LED));
    });
    setupGsmModule(); // TODO
  } else {
    led_ticker.attach_ms(200, []() {
      digitalWrite(ESP_BOARD_LED, !digitalRead(ESP_BOARD_LED));
    });
    setupCaptivePortal();
  }
  // ----------------------------------------------------------------------------------------------------------------
  SerialMon.println("[BOOT] Completed");
  SerialMon.println("----------------------------------------------------------------------------------------------------------------");
  if (extra_reboot) {
    SerialMon.println("[BOOT] Handling requested software reboot");
    attachInterrupt(GSM_MODULE_INT, []() { // If SIM7600 starts normally, then set the onboard LED to flash once every 1 second
      detachInterrupt(GSM_MODULE_INT);
      SerialMon.println("[GSM] Modem Rebooted");
      SerialMon.println("----------------------------------------------------------------------------------------------------------------");
      preferences.putBool("extra_reboot", false);
      ESP.restart();
    }, RISING); // }, CHANGE); // }, FALLING);
    SerialMon.println("[GSM] Rebooting Modem");
    modem.restart();
    SerialMon.println("----------------------------------------------------------------------------------------------------------------");
  }
  // ----------------------------------------------------------------------------------------------------------------
  xTaskCreatePinnedToCore(core0Task, "Core0Task", 10000, NULL, tskIDLE_PRIORITY, NULL, 0);
  // ----------------------------------------------------------------------------------------------------------------
}

// ----------------------------------------------------------------------------------------------------------------

void loop() { // This is Core 1
  // if (is_setup_done) {

  // }
  // else {

  // }
}

// ----------------------------------------------------------------------------------------------------------------
