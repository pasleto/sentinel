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
#include <LittleFS.h>
#include <RTClib.h>
#include <Ticker.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <SSLClient.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ELMduino.h> // TODO

#include <SoftwareSerial.h> // TODO - causing error in version 7.0.1 above -> dangerous relocation: l32r: literal placed after use [DNSServer || WebServer || SSLClient]

// ----------------------------------------------------------------------------------------------------------------
/*

TODO - NTP server not needed? -> time is grabbed from gps

TODO - when SSL is set to true, but mqtt config is wrong -> code blocked after boot ?

*/
// ----------------------------------------------------------------------------------------------------------------

Preferences preferences;
RTC_DS3231 rtc;
sqlite3 *fs_db;
WebServer web_server(80);
DNSServer dns_server;
HTTPAuthMethod web_server_auth_mode = BASIC_AUTH; // ? - not working with captive portal -> no captive popup
// HTTPAuthMethod web_server_auth_mode = DIGEST_AUTH; // ? - not working with captive portal -> no captive popup
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
SSLClient ssl_client(&client);
PubSubClient mqtt;
IPAddress wifi_ap_ip(192,168,11,1);
// IPAddress wifi_ap_ip(8,8,4,4); // The default android DNS
// IPAddress wifi_ap_gw(192,168,11,1);
IPAddress wifi_ap_mask(255,255,255,0);

Adafruit_MPU6050 mpu; // TODO

ELM327 elm_327; // TODO

SoftwareSerial SerialOBD; // TODO


const char favicon[] PROGMEM = {
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x26, 0x94, 0x4E,
0x3A, 0x00, 0x00, 0x01, 0x82, 0x69, 0x43, 0x43, 0x50, 0x73, 0x52, 0x47, 0x42, 0x20, 0x49, 0x45,
0x43, 0x36, 0x31, 0x39, 0x36, 0x36, 0x2D, 0x32, 0x2E, 0x31, 0x00, 0x00, 0x28, 0x91, 0x75, 0x91,
0xB9, 0x4B, 0x43, 0x41, 0x10, 0x87, 0x3F, 0xA3, 0xA2, 0x68, 0x3C, 0xF0, 0x00, 0x0B, 0x8B, 0x20,
0xD1, 0x2A, 0x91, 0x18, 0x21, 0x68, 0x63, 0x11, 0xF1, 0x02, 0xB5, 0x48, 0x22, 0x18, 0xB5, 0x49,
0x5E, 0x2E, 0x21, 0xC7, 0xE3, 0xBD, 0x04, 0x09, 0xB6, 0x82, 0xAD, 0xA0, 0x20, 0xDA, 0x78, 0x15,
0xFA, 0x17, 0x68, 0x2B, 0x58, 0x0B, 0x82, 0xA2, 0x08, 0x62, 0x27, 0x58, 0x2B, 0xDA, 0xA8, 0x3C,
0xE7, 0x25, 0x81, 0x04, 0x31, 0xB3, 0xCC, 0xCE, 0xB7, 0xBF, 0xDD, 0x19, 0x76, 0x67, 0xC1, 0x12,
0x48, 0x2A, 0x29, 0xBD, 0xCE, 0x05, 0xA9, 0x74, 0x56, 0xF3, 0x4D, 0x79, 0x6D, 0x8B, 0xC1, 0x25,
0x5B, 0xC3, 0x0B, 0xCD, 0x74, 0xD3, 0x4A, 0x07, 0xAE, 0x90, 0xA2, 0xAB, 0x73, 0xFE, 0xC9, 0x00,
0x55, 0xED, 0xF3, 0x9E, 0x1A, 0x33, 0xDE, 0x3A, 0xCD, 0x5A, 0xD5, 0xCF, 0xFD, 0x6B, 0xCD, 0x91,
0xA8, 0xAE, 0x40, 0x4D, 0xA3, 0xF0, 0x98, 0xA2, 0x6A, 0x59, 0xE1, 0x69, 0xE1, 0xD9, 0xB5, 0xAC,
0x6A, 0xF2, 0x8E, 0x70, 0x97, 0x92, 0x08, 0x45, 0x84, 0xCF, 0x84, 0x1D, 0x9A, 0x5C, 0x50, 0xF8,
0xCE, 0xD4, 0xC3, 0x45, 0x7E, 0x35, 0x39, 0x5E, 0xE4, 0x6F, 0x93, 0xB5, 0x80, 0x6F, 0x1C, 0x2C,
0xED, 0xC2, 0xB6, 0x78, 0x05, 0x87, 0x2B, 0x58, 0x49, 0x68, 0x29, 0x61, 0x79, 0x39, 0xF6, 0x54,
0x32, 0xA7, 0x94, 0xEE, 0x63, 0xBE, 0xC4, 0x1A, 0x4D, 0x2F, 0xF8, 0x25, 0xF6, 0x89, 0xF7, 0xA2,
0xE3, 0x63, 0x0A, 0x2F, 0x36, 0x66, 0x98, 0x60, 0x1C, 0x0F, 0x43, 0x8C, 0xCA, 0xEC, 0xC1, 0x89,
0x9B, 0x41, 0x59, 0x51, 0x25, 0xDF, 0x55, 0xC8, 0x9F, 0x27, 0x23, 0xB9, 0x8A, 0xCC, 0x2A, 0x79,
0x34, 0x56, 0x89, 0x93, 0x20, 0x8B, 0x43, 0xD4, 0x9C, 0x54, 0x8F, 0x4A, 0x8C, 0x89, 0x1E, 0x95,
0x91, 0x24, 0x6F, 0xF6, 0xFF, 0x6F, 0x5F, 0xF5, 0xD8, 0xB0, 0xBB, 0x58, 0xDD, 0xEA, 0x85, 0xFA,
0x67, 0xC3, 0x78, 0xEF, 0x87, 0x86, 0x6D, 0xF8, 0xD9, 0x32, 0x8C, 0xAF, 0x23, 0xC3, 0xF8, 0x39,
0x86, 0xDA, 0x27, 0xB8, 0x4C, 0x97, 0xF3, 0x33, 0x87, 0x30, 0xF2, 0x21, 0xFA, 0x56, 0x59, 0xB3,
0x1F, 0x40, 0xDB, 0x06, 0x9C, 0x5F, 0x95, 0xB5, 0xF0, 0x2E, 0x5C, 0x6C, 0x42, 0xCF, 0xA3, 0x1A,
0xD2, 0x42, 0x05, 0xA9, 0x56, 0xDC, 0x12, 0x8B, 0xC1, 0xDB, 0x29, 0xB4, 0x04, 0xA1, 0xF3, 0x06,
0x9A, 0x96, 0x8B, 0x3D, 0x2B, 0xED, 0x73, 0xF2, 0x00, 0x81, 0x75, 0xF9, 0xAA, 0x6B, 0xD8, 0xDB,
0x87, 0x01, 0x39, 0xDF, 0xB6, 0xF2, 0x0B, 0x88, 0x42, 0x67, 0xF5, 0x7F, 0x30, 0x33, 0xBE, 0x00,
0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x1D, 0x87, 0x00, 0x00, 0x1D, 0x87, 0x01,
0x8F, 0xE5, 0xF1, 0x65, 0x00, 0x00, 0x01, 0xB7, 0x49, 0x44, 0x41, 0x54, 0x28, 0x91, 0x85, 0xD2,
0x3D, 0x48, 0xD5, 0x61, 0x14, 0xC7, 0xF1, 0xCF, 0xFF, 0xFE, 0x9F, 0x3F, 0x8D, 0xD5, 0x50, 0xE0,
0xA6, 0x25, 0xE5, 0x96, 0x82, 0x48, 0x45, 0xD0, 0xD0, 0x0B, 0x0D, 0x51, 0xE0, 0x50, 0x44, 0xEF,
0x64, 0xEF, 0x71, 0x43, 0x22, 0x4C, 0xA5, 0x32, 0x2B, 0x53, 0x2B, 0x24, 0x8B, 0x86, 0xD4, 0x68,
0x91, 0xB2, 0xA1, 0x2D, 0x30, 0xC8, 0x21, 0xC8, 0x12, 0x73, 0x28, 0x32, 0x88, 0x08, 0xB1, 0x21,
0x0A, 0x22, 0x7A, 0xA1, 0xA2, 0xC0, 0xBC, 0x0D, 0xF7, 0x2A, 0x9A, 0x46, 0xBF, 0xED, 0xF0, 0x7C,
0xBF, 0x87, 0xE7, 0x1C, 0x4E, 0x64, 0xBA, 0x54, 0xB4, 0x2D, 0xC2, 0xD1, 0x5C, 0x75, 0x5E, 0xFB,
0xEE, 0x67, 0x7F, 0x23, 0x91, 0xBD, 0x37, 0xF2, 0x31, 0x13, 0xF3, 0xB0, 0x00, 0xE5, 0x28, 0xC3,
0x13, 0x8C, 0x60, 0x09, 0xFA, 0x71, 0x07, 0xAF, 0x30, 0x84, 0xCF, 0x41, 0x9C, 0x0C, 0x21, 0xC2,
0x0F, 0x3C, 0xC7, 0x00, 0x0E, 0xBA, 0xBA, 0x79, 0x00, 0x1C, 0xE8, 0x2C, 0xC2, 0x0E, 0x6C, 0x40,
0x01, 0x66, 0x63, 0x34, 0x92, 0xEE, 0xCA, 0x60, 0x0B, 0x6E, 0x6A, 0xDD, 0x38, 0x3A, 0xED, 0xD7,
0x27, 0x26, 0xDD, 0xB5, 0x09, 0x9D, 0x41, 0x9C, 0xC0, 0x17, 0x2D, 0xE5, 0xFF, 0x97, 0x20, 0x4E,
0xBE, 0x21, 0x4A, 0x89, 0x93, 0x8C, 0x38, 0xC9, 0x4C, 0x01, 0x6A, 0xEE, 0xCF, 0xFA, 0x87, 0x98,
0x11, 0x27, 0xBF, 0x83, 0x38, 0xF9, 0x8A, 0x36, 0x55, 0xDD, 0x1D, 0x68, 0xD7, 0xB4, 0xE6, 0x4D,
0x96, 0x88, 0xAE, 0xAB, 0xE9, 0x99, 0x8B, 0x0E, 0xDC, 0xD6, 0xB0, 0xE2, 0x7B, 0x4E, 0x7C, 0x8B,
0xC7, 0x91, 0x9A, 0x9E, 0x42, 0x54, 0x60, 0x27, 0xE6, 0xA0, 0x1B, 0xD7, 0xC4, 0xE1, 0x13, 0x0E,
0x61, 0x3D, 0x7E, 0xE2, 0x16, 0xDA, 0x9D, 0x5E, 0xDE, 0x0F, 0x91, 0xBA, 0xDE, 0x3C, 0x7C, 0x34,
0x3A, 0x02, 0xEB, 0xB0, 0x07, 0xAB, 0xF0, 0x0E, 0x55, 0x52, 0xA1, 0x1B, 0xDB, 0xB1, 0x0B, 0x45,
0xB2, 0x9B, 0x3F, 0x1C, 0xA9, 0xEF, 0x2B, 0xC0, 0x43, 0xAC, 0x76, 0x62, 0xF1, 0x0B, 0x70, 0xEA,
0xD1, 0xFC, 0x1C, 0x98, 0x71, 0x72, 0x69, 0xED, 0xF8, 0x7C, 0xF5, 0x7D, 0x47, 0xD0, 0x84, 0x92,
0x08, 0x9C, 0x1D, 0xB8, 0x82, 0x12, 0x2C, 0x53, 0x5B, 0x3A, 0x75, 0x51, 0x59, 0x66, 0x06, 0x9E,
0xE2, 0x81, 0xDA, 0xD2, 0x7D, 0x29, 0x90, 0x0A, 0x67, 0xA4, 0x42, 0xB1, 0x54, 0xD8, 0x3F, 0xAD,
0x94, 0x65, 0x6A, 0xA4, 0x42, 0x9E, 0x54, 0x38, 0x0E, 0x59, 0xB1, 0xBA, 0xF8, 0xBD, 0x38, 0xB4,
0x88, 0x43, 0xA3, 0xE6, 0xC1, 0x95, 0x53, 0xA4, 0xE6, 0xC1, 0xB5, 0xE2, 0x70, 0x4C, 0x1C, 0xEA,
0x55, 0x17, 0x7F, 0x80, 0x30, 0xFE, 0x18, 0x87, 0x3A, 0xD9, 0x7B, 0xBD, 0xE7, 0xE2, 0xCB, 0x06,
0xD4, 0xE5, 0x1A, 0x9F, 0x43, 0x25, 0x1A, 0x71, 0x69, 0x0C, 0x8F, 0x26, 0x75, 0x6E, 0x79, 0x1D,
0xE3, 0x02, 0xD2, 0xE8, 0x45, 0x82, 0x85, 0xD8, 0xAA, 0xB2, 0xF0, 0xEE, 0x44, 0x74, 0xB2, 0x38,
0x96, 0xD6, 0xE1, 0x32, 0x5C, 0xC6, 0x2F, 0x6C, 0x93, 0xCE, 0x1F, 0xFE, 0x1B, 0xF9, 0x03, 0x7C,
0x15, 0x71, 0x71, 0xDC, 0xC3, 0x42, 0x8A, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE,
0x42, 0x60, 0x82
};

const char main_html[] PROGMEM = R"rawliteral(
<!doctype html><html lang="en"><head><meta charset="utf-8"/><link rel="icon" type="image/png" href="./favicon.ico"/><meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=0"/><title>STL-CF Setup</title><script defer="defer" src="./main.js"></script><link href="./main.css" rel="stylesheet"></head><body><noscript>You need to enable JavaScript!</noscript><div id="root"></div></body></html>
)rawliteral";

const char main_css[] PROGMEM = R"rawliteral(
body{background-color:#0067b3;color:#fff;font-family:verdana,Segoe UI,Helvetica Neue,sans-serif;font-size:small;margin:0;min-width:320px;overflow-x:hidden;-webkit-user-select:none;user-select:none}.app{text-align:center}.loader{-webkit-animation:rotate 1s linear infinite;animation:rotate 1s linear infinite;border-radius:50%;height:84px;margin:74px auto;position:relative;width:84px}.loader:before{-webkit-animation:prixClipFix 2s linear infinite;animation:prixClipFix 2s linear infinite;border:5px solid #fff;border-radius:50%;box-sizing:border-box;content:"";inset:0;position:absolute}@keyframes rotate{to{-webkit-transform:rotate(1turn);transform:rotate(1turn)}}@-webkit-keyframes rotate{to{-webkit-transform:rotate(1turn)}}@keyframes prixClipFix{0%{-webkit-clip-path:polygon(50% 50%,0 0,0 0,0 0,0 0,0 0);clip-path:polygon(50% 50%,0 0,0 0,0 0,0 0,0 0)}25%{-webkit-clip-path:polygon(50% 50%,0 0,100% 0,100% 0,100% 0,100% 0);clip-path:polygon(50% 50%,0 0,100% 0,100% 0,100% 0,100% 0)}50%{-webkit-clip-path:polygon(50% 50%,0 0,100% 0,100% 100%,100% 100%,100% 100%);clip-path:polygon(50% 50%,0 0,100% 0,100% 100%,100% 100%,100% 100%)}75%{-webkit-clip-path:polygon(50% 50%,0 0,100% 0,100% 100%,0 100%,0 100%);clip-path:polygon(50% 50%,0 0,100% 0,100% 100%,0 100%,0 100%)}to{-webkit-clip-path:polygon(50% 50%,0 0,100% 0,100% 100%,0 100%,0 0);clip-path:polygon(50% 50%,0 0,100% 0,100% 100%,0 100%,0 0)}}@-webkit-keyframes prixClipFix{0%{-webkit-clip-path:polygon(50% 50%,0 0,0 0,0 0,0 0,0 0)}25%{-webkit-clip-path:polygon(50% 50%,0 0,100% 0,100% 0,100% 0,100% 0)}50%{-webkit-clip-path:polygon(50% 50%,0 0,100% 0,100% 100%,100% 100%,100% 100%)}75%{-webkit-clip-path:polygon(50% 50%,0 0,100% 0,100% 100%,0 100%,0 100%)}to{-webkit-clip-path:polygon(50% 50%,0 0,100% 0,100% 100%,0 100%,0 0)}}.container{display:flex;flex-direction:row;flex-wrap:nowrap;width:100%}.content{margin:auto 10px;padding:10px;width:100%}table{line-height:20px}.header,table{max-width:400px;width:100%}.header{color:#fff;font-size:xx-large;margin:0;padding:25px 0}select{border:none;border-radius:4px;box-sizing:border-box;color:#000;cursor:pointer;font-size:small;margin:1px auto;padding:4px;width:100%}.label{line-height:10px}.info,.label{font-size:x-small}.info{color:#fff;margin:0;max-width:400px;padding:15px 0;width:100%}form{padding:25px 0}input[type=checkbox]{cursor:pointer;height:18px;margin:4px auto;outline:none;width:18px}input[type=password],input[type=text]{margin:1px auto}input[type=password],input[type=text],textarea{border:none;border-radius:4px;box-sizing:border-box;color:#000;font-size:small;outline:none;padding:5px;width:100%}textarea{font-family:monospace;height:150px;margin:3px auto;overflow:overlay;resize:none;vertical-align:top}input[type=submit]{-webkit-appearance:none;background-color:#3498db;border:none;border-radius:4px;color:#fff;cursor:pointer;font-size:medium;margin:25px auto;outline:none;padding:15px 35px;text-align:center;text-decoration:none}input[type=submit]:hover{background-color:#3498dbbb}input[type=password]:invalid,input[type=text]:invalid,textarea:invalid{-webkit-appearance:none;box-shadow:inset 0 0 0 1px red}.error-sign{color:red;font-size:84px;margin:0;max-width:400px;padding:25px 0;width:100%}.error-header,.loader-header{font-size:large;margin:0;max-width:400px;padding:15px 0;width:100%}.error-message,.loader-message{font-size:medium;margin:0;max-width:400px;padding:15px 0;width:100%}
)rawliteral";

const char main_js[] PROGMEM = R"rawliteral(
!function(){"use strict";var e={534:function(e,t,n){var r=n(313),a=n(224);function l(e){for(var t="https://reactjs.org/docs/error-decoder.html?invariant="+e,n=1;n<arguments.length;n++)t+="&args[]="+encodeURIComponent(arguments[n]);return"Minified React error #"+e+"; visit "+t+" for the full message or use the non-minified dev environment for full errors and additional helpful warnings."}var o=new Set,i={};function u(e,t){s(e,t),s(e+"Capture",t)}function s(e,t){for(i[e]=t,e=0;e<t.length;e++)o.add(t[e])}var c=!("undefined"===typeof window||"undefined"===typeof window.document||"undefined"===typeof window.document.createElement),f=Object.prototype.hasOwnProperty,d=/^[:A-Z_a-z\u00C0-\u00D6\u00D8-\u00F6\u00F8-\u02FF\u0370-\u037D\u037F-\u1FFF\u200C-\u200D\u2070-\u218F\u2C00-\u2FEF\u3001-\uD7FF\uF900-\uFDCF\uFDF0-\uFFFD][:A-Z_a-z\u00C0-\u00D6\u00D8-\u00F6\u00F8-\u02FF\u0370-\u037D\u037F-\u1FFF\u200C-\u200D\u2070-\u218F\u2C00-\u2FEF\u3001-\uD7FF\uF900-\uFDCF\uFDF0-\uFFFD\-.0-9\u00B7\u0300-\u036F\u203F-\u2040]*$/,p={},h={};function m(e,t,n,r,a,l,o){this.acceptsBooleans=2===t||3===t||4===t,this.attributeName=r,this.attributeNamespace=a,this.mustUseProperty=n,this.propertyName=e,this.type=t,this.sanitizeURL=l,this.removeEmptyString=o}var v={};"children dangerouslySetInnerHTML defaultValue defaultChecked innerHTML suppressContentEditableWarning suppressHydrationWarning style".split(" ").forEach((function(e){v[e]=new m(e,0,!1,e,null,!1,!1)})),[["acceptCharset","accept-charset"],["className","class"],["htmlFor","for"],["httpEquiv","http-equiv"]].forEach((function(e){var t=e[0];v[t]=new m(t,1,!1,e[1],null,!1,!1)})),["contentEditable","draggable","spellCheck","value"].forEach((function(e){v[e]=new m(e,2,!1,e.toLowerCase(),null,!1,!1)})),["autoReverse","externalResourcesRequired","focusable","preserveAlpha"].forEach((function(e){v[e]=new m(e,2,!1,e,null,!1,!1)})),"allowFullScreen async autoFocus autoPlay controls default defer disabled disablePictureInPicture disableRemotePlayback formNoValidate hidden loop noModule noValidate open playsInline readOnly required reversed scoped seamless itemScope".split(" ").forEach((function(e){v[e]=new m(e,3,!1,e.toLowerCase(),null,!1,!1)})),["checked","multiple","muted","selected"].forEach((function(e){v[e]=new m(e,3,!0,e,null,!1,!1)})),["capture","download"].forEach((function(e){v[e]=new m(e,4,!1,e,null,!1,!1)})),["cols","rows","size","span"].forEach((function(e){v[e]=new m(e,6,!1,e,null,!1,!1)})),["rowSpan","start"].forEach((function(e){v[e]=new m(e,5,!1,e.toLowerCase(),null,!1,!1)}));var g=/[\-:]([a-z])/g;function y(e){return e[1].toUpperCase()}function b(e,t,n,r){var a=v.hasOwnProperty(t)?v[t]:null;(null!==a?0!==a.type:r||!(2<t.length)||"o"!==t[0]&&"O"!==t[0]||"n"!==t[1]&&"N"!==t[1])&&(function(e,t,n,r){if(null===t||"undefined"===typeof t||function(e,t,n,r){if(null!==n&&0===n.type)return!1;switch(typeof t){case"function":case"symbol":return!0;case"boolean":return!r&&(null!==n?!n.acceptsBooleans:"data-"!==(e=e.toLowerCase().slice(0,5))&&"aria-"!==e);default:return!1}}(e,t,n,r))return!0;if(r)return!1;if(null!==n)switch(n.type){case 3:return!t;case 4:return!1===t;case 5:return isNaN(t);case 6:return isNaN(t)||1>t}return!1}(t,n,a,r)&&(n=null),r||null===a?function(e){return!!f.call(h,e)||!f.call(p,e)&&(d.test(e)?h[e]=!0:(p[e]=!0,!1))}(t)&&(null===n?e.removeAttribute(t):e.setAttribute(t,""+n)):a.mustUseProperty?e[a.propertyName]=null===n?3!==a.type&&"":n:(t=a.attributeName,r=a.attributeNamespace,null===n?e.removeAttribute(t):(n=3===(a=a.type)||4===a&&!0===n?"":""+n,r?e.setAttributeNS(r,t,n):e.setAttribute(t,n))))}"accent-height alignment-baseline arabic-form baseline-shift cap-height clip-path clip-rule color-interpolation color-interpolation-filters color-profile color-rendering dominant-baseline enable-background fill-opacity fill-rule flood-color flood-opacity font-family font-size font-size-adjust font-stretch font-style font-variant font-weight glyph-name glyph-orientation-horizontal glyph-orientation-vertical horiz-adv-x horiz-origin-x image-rendering letter-spacing lighting-color marker-end marker-mid marker-start overline-position overline-thickness paint-order panose-1 pointer-events rendering-intent shape-rendering stop-color stop-opacity strikethrough-position strikethrough-thickness stroke-dasharray stroke-dashoffset stroke-linecap stroke-linejoin stroke-miterlimit stroke-opacity stroke-width text-anchor text-decoration text-rendering underline-position underline-thickness unicode-bidi unicode-range units-per-em v-alphabetic v-hanging v-ideographic v-mathematical vector-effect vert-adv-y vert-origin-x vert-origin-y word-spacing writing-mode xmlns:xlink x-height".split(" ").forEach((function(e){var t=e.replace(g,y);v[t]=new m(t,1,!1,e,null,!1,!1)})),"xlink:actuate xlink:arcrole xlink:role xlink:show xlink:title xlink:type".split(" ").forEach((function(e){var t=e.replace(g,y);v[t]=new m(t,1,!1,e,"http://www.w3.org/1999/xlink",!1,!1)})),["xml:base","xml:lang","xml:space"].forEach((function(e){var t=e.replace(g,y);v[t]=new m(t,1,!1,e,"http://www.w3.org/XML/1998/namespace",!1,!1)})),["tabIndex","crossOrigin"].forEach((function(e){v[e]=new m(e,1,!1,e.toLowerCase(),null,!1,!1)})),v.xlinkHref=new m("xlinkHref",1,!1,"xlink:href","http://www.w3.org/1999/xlink",!0,!1),["src","href","action","formAction"].forEach((function(e){v[e]=new m(e,1,!1,e.toLowerCase(),null,!0,!0)}));var w=r.__SECRET_INTERNALS_DO_NOT_USE_OR_YOU_WILL_BE_FIRED,S=Symbol.for("react.element"),x=Symbol.for("react.portal"),k=Symbol.for("react.fragment"),E=Symbol.for("react.strict_mode"),C=Symbol.for("react.profiler"),_=Symbol.for("react.provider"),j=Symbol.for("react.context"),P=Symbol.for("react.forward_ref"),N=Symbol.for("react.suspense"),O=Symbol.for("react.suspense_list"),z=Symbol.for("react.memo"),T=Symbol.for("react.lazy");Symbol.for("react.scope"),Symbol.for("react.debug_trace_mode");var L=Symbol.for("react.offscreen");Symbol.for("react.legacy_hidden"),Symbol.for("react.cache"),Symbol.for("react.tracing_marker");var R=Symbol.iterator;function A(e){return null===e||"object"!==typeof e?null:"function"===typeof(e=R&&e[R]||e["@@iterator"])?e:null}var F,D=Object.assign;function M(e){if(void 0===F)try{throw Error()}catch(n){var t=n.stack.trim().match(/\n( *(at )?)/);F=t&&t[1]||""}return"\n"+F+e}var I=!1;function U(e,t){if(!e||I)return"";I=!0;var n=Error.prepareStackTrace;Error.prepareStackTrace=void 0;try{if(t)if(t=function(){throw Error()},Object.defineProperty(t.prototype,"props",{set:function(){throw Error()}}),"object"===typeof Reflect&&Reflect.construct){try{Reflect.construct(t,[])}catch(s){var r=s}Reflect.construct(e,[],t)}else{try{t.call()}catch(s){r=s}e.call(t.prototype)}else{try{throw Error()}catch(s){r=s}e()}}catch(s){if(s&&r&&"string"===typeof s.stack){for(var a=s.stack.split("\n"),l=r.stack.split("\n"),o=a.length-1,i=l.length-1;1<=o&&0<=i&&a[o]!==l[i];)i--;for(;1<=o&&0<=i;o--,i--)if(a[o]!==l[i]){if(1!==o||1!==i)do{if(o--,0>--i||a[o]!==l[i]){var u="\n"+a[o].replace(" at new "," at ");return e.displayName&&u.includes("<anonymous>")&&(u=u.replace("<anonymous>",e.displayName)),u}}while(1<=o&&0<=i);break}}}finally{I=!1,Error.prepareStackTrace=n}return(e=e?e.displayName||e.name:"")?M(e):""}function B(e){switch(e.tag){case 5:return M(e.type);case 16:return M("Lazy");case 13:return M("Suspense");case 19:return M("SuspenseList");case 0:case 2:case 15:return e=U(e.type,!1);case 11:return e=U(e.type.render,!1);case 1:return e=U(e.type,!0);default:return""}}function V(e){if(null==e)return null;if("function"===typeof e)return e.displayName||e.name||null;if("string"===typeof e)return e;switch(e){case k:return"Fragment";case x:return"Portal";case C:return"Profiler";case E:return"StrictMode";case N:return"Suspense";case O:return"SuspenseList"}if("object"===typeof e)switch(e.$$typeof){case j:return(e.displayName||"Context")+".Consumer";case _:return(e._context.displayName||"Context")+".Provider";case P:var t=e.render;return(e=e.displayName)||(e=""!==(e=t.displayName||t.name||"")?"ForwardRef("+e+")":"ForwardRef"),e;case z:return null!==(t=e.displayName||null)?t:V(e.type)||"Memo";case T:t=e._payload,e=e._init;try{return V(e(t))}catch(n){}}return null}function q(e){var t=e.type;switch(e.tag){case 24:return"Cache";case 9:return(t.displayName||"Context")+".Consumer";case 10:return(t._context.displayName||"Context")+".Provider";case 18:return"DehydratedFragment";case 11:return e=(e=t.render).displayName||e.name||"",t.displayName||(""!==e?"ForwardRef("+e+")":"ForwardRef");case 7:return"Fragment";case 5:return t;case 4:return"Portal";case 3:return"Root";case 6:return"Text";case 16:return V(t);case 8:return t===E?"StrictMode":"Mode";case 22:return"Offscreen";case 12:return"Profiler";case 21:return"Scope";case 13:return"Suspense";case 19:return"SuspenseList";case 25:return"TracingMarker";case 1:case 0:case 17:case 2:case 14:case 15:if("function"===typeof t)return t.displayName||t.name||null;if("string"===typeof t)return t}return null}function H(e){switch(typeof e){case"boolean":case"number":case"string":case"undefined":case"object":return e;default:return""}}function $(e){var t=e.type;return(e=e.nodeName)&&"input"===e.toLowerCase()&&("checkbox"===t||"radio"===t)}function W(e){e._valueTracker||(e._valueTracker=function(e){var t=$(e)?"checked":"value",n=Object.getOwnPropertyDescriptor(e.constructor.prototype,t),r=""+e[t];if(!e.hasOwnProperty(t)&&"undefined"!==typeof n&&"function"===typeof n.get&&"function"===typeof n.set){var a=n.get,l=n.set;return Object.defineProperty(e,t,{configurable:!0,get:function(){return a.call(this)},set:function(e){r=""+e,l.call(this,e)}}),Object.defineProperty(e,t,{enumerable:n.enumerable}),{getValue:function(){return r},setValue:function(e){r=""+e},stopTracking:function(){e._valueTracker=null,delete e[t]}}}}(e))}function Q(e){if(!e)return!1;var t=e._valueTracker;if(!t)return!0;var n=t.getValue(),r="";return e&&(r=$(e)?e.checked?"true":"false":e.value),(e=r)!==n&&(t.setValue(e),!0)}function K(e){if("undefined"===typeof(e=e||("undefined"!==typeof document?document:void 0)))return null;try{return e.activeElement||e.body}catch(t){return e.body}}function G(e,t){var n=t.checked;return D({},t,{defaultChecked:void 0,defaultValue:void 0,value:void 0,checked:null!=n?n:e._wrapperState.initialChecked})}function Z(e,t){var n=null==t.defaultValue?"":t.defaultValue,r=null!=t.checked?t.checked:t.defaultChecked;n=H(null!=t.value?t.value:n),e._wrapperState={initialChecked:r,initialValue:n,controlled:"checkbox"===t.type||"radio"===t.type?null!=t.checked:null!=t.value}}function Y(e,t){null!=(t=t.checked)&&b(e,"checked",t,!1)}function J(e,t){Y(e,t);var n=H(t.value),r=t.type;if(null!=n)"number"===r?(0===n&&""===e.value||e.value!=n)&&(e.value=""+n):e.value!==""+n&&(e.value=""+n);else if("submit"===r||"reset"===r)return void e.removeAttribute("value");t.hasOwnProperty("value")?ee(e,t.type,n):t.hasOwnProperty("defaultValue")&&ee(e,t.type,H(t.defaultValue)),null==t.checked&&null!=t.defaultChecked&&(e.defaultChecked=!!t.defaultChecked)}function X(e,t,n){if(t.hasOwnProperty("value")||t.hasOwnProperty("defaultValue")){var r=t.type;if(!("submit"!==r&&"reset"!==r||void 0!==t.value&&null!==t.value))return;t=""+e._wrapperState.initialValue,n||t===e.value||(e.value=t),e.defaultValue=t}""!==(n=e.name)&&(e.name=""),e.defaultChecked=!!e._wrapperState.initialChecked,""!==n&&(e.name=n)}function ee(e,t,n){"number"===t&&K(e.ownerDocument)===e||(null==n?e.defaultValue=""+e._wrapperState.initialValue:e.defaultValue!==""+n&&(e.defaultValue=""+n))}var te=Array.isArray;function ne(e,t,n,r){if(e=e.options,t){t={};for(var a=0;a<n.length;a++)t["$"+n[a]]=!0;for(n=0;n<e.length;n++)a=t.hasOwnProperty("$"+e[n].value),e[n].selected!==a&&(e[n].selected=a),a&&r&&(e[n].defaultSelected=!0)}else{for(n=""+H(n),t=null,a=0;a<e.length;a++){if(e[a].value===n)return e[a].selected=!0,void(r&&(e[a].defaultSelected=!0));null!==t||e[a].disabled||(t=e[a])}null!==t&&(t.selected=!0)}}function re(e,t){if(null!=t.dangerouslySetInnerHTML)throw Error(l(91));return D({},t,{value:void 0,defaultValue:void 0,children:""+e._wrapperState.initialValue})}function ae(e,t){var n=t.value;if(null==n){if(n=t.children,t=t.defaultValue,null!=n){if(null!=t)throw Error(l(92));if(te(n)){if(1<n.length)throw Error(l(93));n=n[0]}t=n}null==t&&(t=""),n=t}e._wrapperState={initialValue:H(n)}}function le(e,t){var n=H(t.value),r=H(t.defaultValue);null!=n&&((n=""+n)!==e.value&&(e.value=n),null==t.defaultValue&&e.defaultValue!==n&&(e.defaultValue=n)),null!=r&&(e.defaultValue=""+r)}function oe(e){var t=e.textContent;t===e._wrapperState.initialValue&&""!==t&&null!==t&&(e.value=t)}function ie(e){switch(e){case"svg":return"http://www.w3.org/2000/svg";case"math":return"http://www.w3.org/1998/Math/MathML";default:return"http://www.w3.org/1999/xhtml"}}function ue(e,t){return null==e||"http://www.w3.org/1999/xhtml"===e?ie(t):"http://www.w3.org/2000/svg"===e&&"foreignObject"===t?"http://www.w3.org/1999/xhtml":e}var se,ce,fe=(ce=function(e,t){if("http://www.w3.org/2000/svg"!==e.namespaceURI||"innerHTML"in e)e.innerHTML=t;else{for((se=se||document.createElement("div")).innerHTML="<svg>"+t.valueOf().toString()+"</svg>",t=se.firstChild;e.firstChild;)e.removeChild(e.firstChild);for(;t.firstChild;)e.appendChild(t.firstChild)}},"undefined"!==typeof MSApp&&MSApp.execUnsafeLocalFunction?function(e,t,n,r){MSApp.execUnsafeLocalFunction((function(){return ce(e,t)}))}:ce);function de(e,t){if(t){var n=e.firstChild;if(n&&n===e.lastChild&&3===n.nodeType)return void(n.nodeValue=t)}e.textContent=t}var pe={animationIterationCount:!0,aspectRatio:!0,borderImageOutset:!0,borderImageSlice:!0,borderImageWidth:!0,boxFlex:!0,boxFlexGroup:!0,boxOrdinalGroup:!0,columnCount:!0,columns:!0,flex:!0,flexGrow:!0,flexPositive:!0,flexShrink:!0,flexNegative:!0,flexOrder:!0,gridArea:!0,gridRow:!0,gridRowEnd:!0,gridRowSpan:!0,gridRowStart:!0,gridColumn:!0,gridColumnEnd:!0,gridColumnSpan:!0,gridColumnStart:!0,fontWeight:!0,lineClamp:!0,lineHeight:!0,opacity:!0,order:!0,orphans:!0,tabSize:!0,widows:!0,zIndex:!0,zoom:!0,fillOpacity:!0,floodOpacity:!0,stopOpacity:!0,strokeDasharray:!0,strokeDashoffset:!0,strokeMiterlimit:!0,strokeOpacity:!0,strokeWidth:!0},he=["Webkit","ms","Moz","O"];function me(e,t,n){return null==t||"boolean"===typeof t||""===t?"":n||"number"!==typeof t||0===t||pe.hasOwnProperty(e)&&pe[e]?(""+t).trim():t+"px"}function ve(e,t){for(var n in e=e.style,t)if(t.hasOwnProperty(n)){var r=0===n.indexOf("--"),a=me(n,t[n],r);"float"===n&&(n="cssFloat"),r?e.setProperty(n,a):e[n]=a}}Object.keys(pe).forEach((function(e){he.forEach((function(t){t=t+e.charAt(0).toUpperCase()+e.substring(1),pe[t]=pe[e]}))}));var ge=D({menuitem:!0},{area:!0,base:!0,br:!0,col:!0,embed:!0,hr:!0,img:!0,input:!0,keygen:!0,link:!0,meta:!0,param:!0,source:!0,track:!0,wbr:!0});function ye(e,t){if(t){if(ge[e]&&(null!=t.children||null!=t.dangerouslySetInnerHTML))throw Error(l(137,e));if(null!=t.dangerouslySetInnerHTML){if(null!=t.children)throw Error(l(60));if("object"!==typeof t.dangerouslySetInnerHTML||!("__html"in t.dangerouslySetInnerHTML))throw Error(l(61))}if(null!=t.style&&"object"!==typeof t.style)throw Error(l(62))}}function be(e,t){if(-1===e.indexOf("-"))return"string"===typeof t.is;switch(e){case"annotation-xml":case"color-profile":case"font-face":case"font-face-src":case"font-face-uri":case"font-face-format":case"font-face-name":case"missing-glyph":return!1;default:return!0}}var we=null;function Se(e){return(e=e.target||e.srcElement||window).correspondingUseElement&&(e=e.correspondingUseElement),3===e.nodeType?e.parentNode:e}var xe=null,ke=null,Ee=null;function Ce(e){if(e=ba(e)){if("function"!==typeof xe)throw Error(l(280));var t=e.stateNode;t&&(t=Sa(t),xe(e.stateNode,e.type,t))}}function _e(e){ke?Ee?Ee.push(e):Ee=[e]:ke=e}function je(){if(ke){var e=ke,t=Ee;if(Ee=ke=null,Ce(e),t)for(e=0;e<t.length;e++)Ce(t[e])}}function Pe(e,t){return e(t)}function Ne(){}var Oe=!1;function ze(e,t,n){if(Oe)return e(t,n);Oe=!0;try{return Pe(e,t,n)}finally{Oe=!1,(null!==ke||null!==Ee)&&(Ne(),je())}}function Te(e,t){var n=e.stateNode;if(null===n)return null;var r=Sa(n);if(null===r)return null;n=r[t];e:switch(t){case"onClick":case"onClickCapture":case"onDoubleClick":case"onDoubleClickCapture":case"onMouseDown":case"onMouseDownCapture":case"onMouseMove":case"onMouseMoveCapture":case"onMouseUp":case"onMouseUpCapture":case"onMouseEnter":(r=!r.disabled)||(r=!("button"===(e=e.type)||"input"===e||"select"===e||"textarea"===e)),e=!r;break e;default:e=!1}if(e)return null;if(n&&"function"!==typeof n)throw Error(l(231,t,typeof n));return n}var Le=!1;if(c)try{var Re={};Object.defineProperty(Re,"passive",{get:function(){Le=!0}}),window.addEventListener("test",Re,Re),window.removeEventListener("test",Re,Re)}catch(ce){Le=!1}function Ae(e,t,n,r,a,l,o,i,u){var s=Array.prototype.slice.call(arguments,3);try{t.apply(n,s)}catch(c){this.onError(c)}}var Fe=!1,De=null,Me=!1,Ie=null,Ue={onError:function(e){Fe=!0,De=e}};function Be(e,t,n,r,a,l,o,i,u){Fe=!1,De=null,Ae.apply(Ue,arguments)}function Ve(e){var t=e,n=e;if(e.alternate)for(;t.return;)t=t.return;else{e=t;do{0!==(4098&(t=e).flags)&&(n=t.return),e=t.return}while(e)}return 3===t.tag?n:null}function qe(e){if(13===e.tag){var t=e.memoizedState;if(null===t&&(null!==(e=e.alternate)&&(t=e.memoizedState)),null!==t)return t.dehydrated}return null}function He(e){if(Ve(e)!==e)throw Error(l(188))}function $e(e){return null!==(e=function(e){var t=e.alternate;if(!t){if(null===(t=Ve(e)))throw Error(l(188));return t!==e?null:e}for(var n=e,r=t;;){var a=n.return;if(null===a)break;var o=a.alternate;if(null===o){if(null!==(r=a.return)){n=r;continue}break}if(a.child===o.child){for(o=a.child;o;){if(o===n)return He(a),e;if(o===r)return He(a),t;o=o.sibling}throw Error(l(188))}if(n.return!==r.return)n=a,r=o;else{for(var i=!1,u=a.child;u;){if(u===n){i=!0,n=a,r=o;break}if(u===r){i=!0,r=a,n=o;break}u=u.sibling}if(!i){for(u=o.child;u;){if(u===n){i=!0,n=o,r=a;break}if(u===r){i=!0,r=o,n=a;break}u=u.sibling}if(!i)throw Error(l(189))}}if(n.alternate!==r)throw Error(l(190))}if(3!==n.tag)throw Error(l(188));return n.stateNode.current===n?e:t}(e))?We(e):null}function We(e){if(5===e.tag||6===e.tag)return e;for(e=e.child;null!==e;){var t=We(e);if(null!==t)return t;e=e.sibling}return null}var Qe=a.unstable_scheduleCallback,Ke=a.unstable_cancelCallback,Ge=a.unstable_shouldYield,Ze=a.unstable_requestPaint,Ye=a.unstable_now,Je=a.unstable_getCurrentPriorityLevel,Xe=a.unstable_ImmediatePriority,et=a.unstable_UserBlockingPriority,tt=a.unstable_NormalPriority,nt=a.unstable_LowPriority,rt=a.unstable_IdlePriority,at=null,lt=null;var ot=Math.clz32?Math.clz32:function(e){return e>>>=0,0===e?32:31-(it(e)/ut|0)|0},it=Math.log,ut=Math.LN2;var st=64,ct=4194304;function ft(e){switch(e&-e){case 1:return 1;case 2:return 2;case 4:return 4;case 8:return 8;case 16:return 16;case 32:return 32;case 64:case 128:case 256:case 512:case 1024:case 2048:case 4096:case 8192:case 16384:case 32768:case 65536:case 131072:case 262144:case 524288:case 1048576:case 2097152:return 4194240&e;case 4194304:case 8388608:case 16777216:case 33554432:case 67108864:return 130023424&e;case 134217728:return 134217728;case 268435456:return 268435456;case 536870912:return 536870912;case 1073741824:return 1073741824;default:return e}}function dt(e,t){var n=e.pendingLanes;if(0===n)return 0;var r=0,a=e.suspendedLanes,l=e.pingedLanes,o=268435455&n;if(0!==o){var i=o&~a;0!==i?r=ft(i):0!==(l&=o)&&(r=ft(l))}else 0!==(o=n&~a)?r=ft(o):0!==l&&(r=ft(l));if(0===r)return 0;if(0!==t&&t!==r&&0===(t&a)&&((a=r&-r)>=(l=t&-t)||16===a&&0!==(4194240&l)))return t;if(0!==(4&r)&&(r|=16&n),0!==(t=e.entangledLanes))for(e=e.entanglements,t&=r;0<t;)a=1<<(n=31-ot(t)),r|=e[n],t&=~a;return r}function pt(e,t){switch(e){case 1:case 2:case 4:return t+250;case 8:case 16:case 32:case 64:case 128:case 256:case 512:case 1024:case 2048:case 4096:case 8192:case 16384:case 32768:case 65536:case 131072:case 262144:case 524288:case 1048576:case 2097152:return t+5e3;default:return-1}}function ht(e){return 0!==(e=-1073741825&e.pendingLanes)?e:1073741824&e?1073741824:0}function mt(){var e=st;return 0===(4194240&(st<<=1))&&(st=64),e}function vt(e){for(var t=[],n=0;31>n;n++)t.push(e);return t}function gt(e,t,n){e.pendingLanes|=t,536870912!==t&&(e.suspendedLanes=0,e.pingedLanes=0),(e=e.eventTimes)[t=31-ot(t)]=n}function yt(e,t){var n=e.entangledLanes|=t;for(e=e.entanglements;n;){var r=31-ot(n),a=1<<r;a&t|e[r]&t&&(e[r]|=t),n&=~a}}var bt=0;function wt(e){return 1<(e&=-e)?4<e?0!==(268435455&e)?16:536870912:4:1}var St,xt,kt,Et,Ct,_t=!1,jt=[],Pt=null,Nt=null,Ot=null,zt=new Map,Tt=new Map,Lt=[],Rt="mousedown mouseup touchcancel touchend touchstart auxclick dblclick pointercancel pointerdown pointerup dragend dragstart drop compositionend compositionstart keydown keypress keyup input textInput copy cut paste click change contextmenu reset submit".split(" ");function At(e,t){switch(e){case"focusin":case"focusout":Pt=null;break;case"dragenter":case"dragleave":Nt=null;break;case"mouseover":case"mouseout":Ot=null;break;case"pointerover":case"pointerout":zt.delete(t.pointerId);break;case"gotpointercapture":case"lostpointercapture":Tt.delete(t.pointerId)}}function Ft(e,t,n,r,a,l){return null===e||e.nativeEvent!==l?(e={blockedOn:t,domEventName:n,eventSystemFlags:r,nativeEvent:l,targetContainers:[a]},null!==t&&(null!==(t=ba(t))&&xt(t)),e):(e.eventSystemFlags|=r,t=e.targetContainers,null!==a&&-1===t.indexOf(a)&&t.push(a),e)}function Dt(e){var t=ya(e.target);if(null!==t){var n=Ve(t);if(null!==n)if(13===(t=n.tag)){if(null!==(t=qe(n)))return e.blockedOn=t,void Ct(e.priority,(function(){kt(n)}))}else if(3===t&&n.stateNode.current.memoizedState.isDehydrated)return void(e.blockedOn=3===n.tag?n.stateNode.containerInfo:null)}e.blockedOn=null}function Mt(e){if(null!==e.blockedOn)return!1;for(var t=e.targetContainers;0<t.length;){var n=Gt(e.domEventName,e.eventSystemFlags,t[0],e.nativeEvent);if(null!==n)return null!==(t=ba(n))&&xt(t),e.blockedOn=n,!1;var r=new(n=e.nativeEvent).constructor(n.type,n);we=r,n.target.dispatchEvent(r),we=null,t.shift()}return!0}function It(e,t,n){Mt(e)&&n.delete(t)}function Ut(){_t=!1,null!==Pt&&Mt(Pt)&&(Pt=null),null!==Nt&&Mt(Nt)&&(Nt=null),null!==Ot&&Mt(Ot)&&(Ot=null),zt.forEach(It),Tt.forEach(It)}function Bt(e,t){e.blockedOn===t&&(e.blockedOn=null,_t||(_t=!0,a.unstable_scheduleCallback(a.unstable_NormalPriority,Ut)))}function Vt(e){function t(t){return Bt(t,e)}if(0<jt.length){Bt(jt[0],e);for(var n=1;n<jt.length;n++){var r=jt[n];r.blockedOn===e&&(r.blockedOn=null)}}for(null!==Pt&&Bt(Pt,e),null!==Nt&&Bt(Nt,e),null!==Ot&&Bt(Ot,e),zt.forEach(t),Tt.forEach(t),n=0;n<Lt.length;n++)(r=Lt[n]).blockedOn===e&&(r.blockedOn=null);for(;0<Lt.length&&null===(n=Lt[0]).blockedOn;)Dt(n),null===n.blockedOn&&Lt.shift()}var qt=w.ReactCurrentBatchConfig,Ht=!0;function $t(e,t,n,r){var a=bt,l=qt.transition;qt.transition=null;try{bt=1,Qt(e,t,n,r)}finally{bt=a,qt.transition=l}}function Wt(e,t,n,r){var a=bt,l=qt.transition;qt.transition=null;try{bt=4,Qt(e,t,n,r)}finally{bt=a,qt.transition=l}}function Qt(e,t,n,r){if(Ht){var a=Gt(e,t,n,r);if(null===a)Hr(e,t,r,Kt,n),At(e,r);else if(function(e,t,n,r,a){switch(t){case"focusin":return Pt=Ft(Pt,e,t,n,r,a),!0;case"dragenter":return Nt=Ft(Nt,e,t,n,r,a),!0;case"mouseover":return Ot=Ft(Ot,e,t,n,r,a),!0;case"pointerover":var l=a.pointerId;return zt.set(l,Ft(zt.get(l)||null,e,t,n,r,a)),!0;case"gotpointercapture":return l=a.pointerId,Tt.set(l,Ft(Tt.get(l)||null,e,t,n,r,a)),!0}return!1}(a,e,t,n,r))r.stopPropagation();else if(At(e,r),4&t&&-1<Rt.indexOf(e)){for(;null!==a;){var l=ba(a);if(null!==l&&St(l),null===(l=Gt(e,t,n,r))&&Hr(e,t,r,Kt,n),l===a)break;a=l}null!==a&&r.stopPropagation()}else Hr(e,t,r,null,n)}}var Kt=null;function Gt(e,t,n,r){if(Kt=null,null!==(e=ya(e=Se(r))))if(null===(t=Ve(e)))e=null;else if(13===(n=t.tag)){if(null!==(e=qe(t)))return e;e=null}else if(3===n){if(t.stateNode.current.memoizedState.isDehydrated)return 3===t.tag?t.stateNode.containerInfo:null;e=null}else t!==e&&(e=null);return Kt=e,null}function Zt(e){switch(e){case"cancel":case"click":case"close":case"contextmenu":case"copy":case"cut":case"auxclick":case"dblclick":case"dragend":case"dragstart":case"drop":case"focusin":case"focusout":case"input":case"invalid":case"keydown":case"keypress":case"keyup":case"mousedown":case"mouseup":case"paste":case"pause":case"play":case"pointercancel":case"pointerdown":case"pointerup":case"ratechange":case"reset":case"resize":case"seeked":case"submit":case"touchcancel":case"touchend":case"touchstart":case"volumechange":case"change":case"selectionchange":case"textInput":case"compositionstart":case"compositionend":case"compositionupdate":case"beforeblur":case"afterblur":case"beforeinput":case"blur":case"fullscreenchange":case"focus":case"hashchange":case"popstate":case"select":case"selectstart":return 1;case"drag":case"dragenter":case"dragexit":case"dragleave":case"dragover":case"mousemove":case"mouseout":case"mouseover":case"pointermove":case"pointerout":case"pointerover":case"scroll":case"toggle":case"touchmove":case"wheel":case"mouseenter":case"mouseleave":case"pointerenter":case"pointerleave":return 4;case"message":switch(Je()){case Xe:return 1;case et:return 4;case tt:case nt:return 16;case rt:return 536870912;default:return 16}default:return 16}}var Yt=null,Jt=null,Xt=null;function en(){if(Xt)return Xt;var e,t,n=Jt,r=n.length,a="value"in Yt?Yt.value:Yt.textContent,l=a.length;for(e=0;e<r&&n[e]===a[e];e++);var o=r-e;for(t=1;t<=o&&n[r-t]===a[l-t];t++);return Xt=a.slice(e,1<t?1-t:void 0)}function tn(e){var t=e.keyCode;return"charCode"in e?0===(e=e.charCode)&&13===t&&(e=13):e=t,10===e&&(e=13),32<=e||13===e?e:0}function nn(){return!0}function rn(){return!1}function an(e){function t(t,n,r,a,l){for(var o in this._reactName=t,this._targetInst=r,this.type=n,this.nativeEvent=a,this.target=l,this.currentTarget=null,e)e.hasOwnProperty(o)&&(t=e[o],this[o]=t?t(a):a[o]);return this.isDefaultPrevented=(null!=a.defaultPrevented?a.defaultPrevented:!1===a.returnValue)?nn:rn,this.isPropagationStopped=rn,this}return D(t.prototype,{preventDefault:function(){this.defaultPrevented=!0;var e=this.nativeEvent;e&&(e.preventDefault?e.preventDefault():"unknown"!==typeof e.returnValue&&(e.returnValue=!1),this.isDefaultPrevented=nn)},stopPropagation:function(){var e=this.nativeEvent;e&&(e.stopPropagation?e.stopPropagation():"unknown"!==typeof e.cancelBubble&&(e.cancelBubble=!0),this.isPropagationStopped=nn)},persist:function(){},isPersistent:nn}),t}var ln,on,un,sn={eventPhase:0,bubbles:0,cancelable:0,timeStamp:function(e){return e.timeStamp||Date.now()},defaultPrevented:0,isTrusted:0},cn=an(sn),fn=D({},sn,{view:0,detail:0}),dn=an(fn),pn=D({},fn,{screenX:0,screenY:0,clientX:0,clientY:0,pageX:0,pageY:0,ctrlKey:0,shiftKey:0,altKey:0,metaKey:0,getModifierState:Cn,button:0,buttons:0,relatedTarget:function(e){return void 0===e.relatedTarget?e.fromElement===e.srcElement?e.toElement:e.fromElement:e.relatedTarget},movementX:function(e){return"movementX"in e?e.movementX:(e!==un&&(un&&"mousemove"===e.type?(ln=e.screenX-un.screenX,on=e.screenY-un.screenY):on=ln=0,un=e),ln)},movementY:function(e){return"movementY"in e?e.movementY:on}}),hn=an(pn),mn=an(D({},pn,{dataTransfer:0})),vn=an(D({},fn,{relatedTarget:0})),gn=an(D({},sn,{animationName:0,elapsedTime:0,pseudoElement:0})),yn=D({},sn,{clipboardData:function(e){return"clipboardData"in e?e.clipboardData:window.clipboardData}}),bn=an(yn),wn=an(D({},sn,{data:0})),Sn={Esc:"Escape",Spacebar:" ",Left:"ArrowLeft",Up:"ArrowUp",Right:"ArrowRight",Down:"ArrowDown",Del:"Delete",Win:"OS",Menu:"ContextMenu",Apps:"ContextMenu",Scroll:"ScrollLock",MozPrintableKey:"Unidentified"},xn={8:"Backspace",9:"Tab",12:"Clear",13:"Enter",16:"Shift",17:"Control",18:"Alt",19:"Pause",20:"CapsLock",27:"Escape",32:" ",33:"PageUp",34:"PageDown",35:"End",36:"Home",37:"ArrowLeft",38:"ArrowUp",39:"ArrowRight",40:"ArrowDown",45:"Insert",46:"Delete",112:"F1",113:"F2",114:"F3",115:"F4",116:"F5",117:"F6",118:"F7",119:"F8",120:"F9",121:"F10",122:"F11",123:"F12",144:"NumLock",145:"ScrollLock",224:"Meta"},kn={Alt:"altKey",Control:"ctrlKey",Meta:"metaKey",Shift:"shiftKey"};function En(e){var t=this.nativeEvent;return t.getModifierState?t.getModifierState(e):!!(e=kn[e])&&!!t[e]}function Cn(){return En}var _n=D({},fn,{key:function(e){if(e.key){var t=Sn[e.key]||e.key;if("Unidentified"!==t)return t}return"keypress"===e.type?13===(e=tn(e))?"Enter":String.fromCharCode(e):"keydown"===e.type||"keyup"===e.type?xn[e.keyCode]||"Unidentified":""},code:0,location:0,ctrlKey:0,shiftKey:0,altKey:0,metaKey:0,repeat:0,locale:0,getModifierState:Cn,charCode:function(e){return"keypress"===e.type?tn(e):0},keyCode:function(e){return"keydown"===e.type||"keyup"===e.type?e.keyCode:0},which:function(e){return"keypress"===e.type?tn(e):"keydown"===e.type||"keyup"===e.type?e.keyCode:0}}),jn=an(_n),Pn=an(D({},pn,{pointerId:0,width:0,height:0,pressure:0,tangentialPressure:0,tiltX:0,tiltY:0,twist:0,pointerType:0,isPrimary:0})),Nn=an(D({},fn,{touches:0,targetTouches:0,changedTouches:0,altKey:0,metaKey:0,ctrlKey:0,shiftKey:0,getModifierState:Cn})),On=an(D({},sn,{propertyName:0,elapsedTime:0,pseudoElement:0})),zn=D({},pn,{deltaX:function(e){return"deltaX"in e?e.deltaX:"wheelDeltaX"in e?-e.wheelDeltaX:0},deltaY:function(e){return"deltaY"in e?e.deltaY:"wheelDeltaY"in e?-e.wheelDeltaY:"wheelDelta"in e?-e.wheelDelta:0},deltaZ:0,deltaMode:0}),Tn=an(zn),Ln=[9,13,27,32],Rn=c&&"CompositionEvent"in window,An=null;c&&"documentMode"in document&&(An=document.documentMode);var Fn=c&&"TextEvent"in window&&!An,Dn=c&&(!Rn||An&&8<An&&11>=An),Mn=String.fromCharCode(32),In=!1;function Un(e,t){switch(e){case"keyup":return-1!==Ln.indexOf(t.keyCode);case"keydown":return 229!==t.keyCode;case"keypress":case"mousedown":case"focusout":return!0;default:return!1}}function Bn(e){return"object"===typeof(e=e.detail)&&"data"in e?e.data:null}var Vn=!1;var qn={color:!0,date:!0,datetime:!0,"datetime-local":!0,email:!0,month:!0,number:!0,password:!0,range:!0,search:!0,tel:!0,text:!0,time:!0,url:!0,week:!0};function Hn(e){var t=e&&e.nodeName&&e.nodeName.toLowerCase();return"input"===t?!!qn[e.type]:"textarea"===t}function $n(e,t,n,r){_e(r),0<(t=Wr(t,"onChange")).length&&(n=new cn("onChange","change",null,n,r),e.push({event:n,listeners:t}))}var Wn=null,Qn=null;function Kn(e){Mr(e,0)}function Gn(e){if(Q(wa(e)))return e}function Zn(e,t){if("change"===e)return t}var Yn=!1;if(c){var Jn;if(c){var Xn="oninput"in document;if(!Xn){var er=document.createElement("div");er.setAttribute("oninput","return;"),Xn="function"===typeof er.oninput}Jn=Xn}else Jn=!1;Yn=Jn&&(!document.documentMode||9<document.documentMode)}function tr(){Wn&&(Wn.detachEvent("onpropertychange",nr),Qn=Wn=null)}function nr(e){if("value"===e.propertyName&&Gn(Qn)){var t=[];$n(t,Qn,e,Se(e)),ze(Kn,t)}}function rr(e,t,n){"focusin"===e?(tr(),Qn=n,(Wn=t).attachEvent("onpropertychange",nr)):"focusout"===e&&tr()}function ar(e){if("selectionchange"===e||"keyup"===e||"keydown"===e)return Gn(Qn)}function lr(e,t){if("click"===e)return Gn(t)}function or(e,t){if("input"===e||"change"===e)return Gn(t)}var ir="function"===typeof Object.is?Object.is:function(e,t){return e===t&&(0!==e||1/e===1/t)||e!==e&&t!==t};function ur(e,t){if(ir(e,t))return!0;if("object"!==typeof e||null===e||"object"!==typeof t||null===t)return!1;var n=Object.keys(e),r=Object.keys(t);if(n.length!==r.length)return!1;for(r=0;r<n.length;r++){var a=n[r];if(!f.call(t,a)||!ir(e[a],t[a]))return!1}return!0}function sr(e){for(;e&&e.firstChild;)e=e.firstChild;return e}function cr(e,t){var n,r=sr(e);for(e=0;r;){if(3===r.nodeType){if(n=e+r.textContent.length,e<=t&&n>=t)return{node:r,offset:t-e};e=n}e:{for(;r;){if(r.nextSibling){r=r.nextSibling;break e}r=r.parentNode}r=void 0}r=sr(r)}}function fr(e,t){return!(!e||!t)&&(e===t||(!e||3!==e.nodeType)&&(t&&3===t.nodeType?fr(e,t.parentNode):"contains"in e?e.contains(t):!!e.compareDocumentPosition&&!!(16&e.compareDocumentPosition(t))))}function dr(){for(var e=window,t=K();t instanceof e.HTMLIFrameElement;){try{var n="string"===typeof t.contentWindow.location.href}catch(r){n=!1}if(!n)break;t=K((e=t.contentWindow).document)}return t}function pr(e){var t=e&&e.nodeName&&e.nodeName.toLowerCase();return t&&("input"===t&&("text"===e.type||"search"===e.type||"tel"===e.type||"url"===e.type||"password"===e.type)||"textarea"===t||"true"===e.contentEditable)}function hr(e){var t=dr(),n=e.focusedElem,r=e.selectionRange;if(t!==n&&n&&n.ownerDocument&&fr(n.ownerDocument.documentElement,n)){if(null!==r&&pr(n))if(t=r.start,void 0===(e=r.end)&&(e=t),"selectionStart"in n)n.selectionStart=t,n.selectionEnd=Math.min(e,n.value.length);else if((e=(t=n.ownerDocument||document)&&t.defaultView||window).getSelection){e=e.getSelection();var a=n.textContent.length,l=Math.min(r.start,a);r=void 0===r.end?l:Math.min(r.end,a),!e.extend&&l>r&&(a=r,r=l,l=a),a=cr(n,l);var o=cr(n,r);a&&o&&(1!==e.rangeCount||e.anchorNode!==a.node||e.anchorOffset!==a.offset||e.focusNode!==o.node||e.focusOffset!==o.offset)&&((t=t.createRange()).setStart(a.node,a.offset),e.removeAllRanges(),l>r?(e.addRange(t),e.extend(o.node,o.offset)):(t.setEnd(o.node,o.offset),e.addRange(t)))}for(t=[],e=n;e=e.parentNode;)1===e.nodeType&&t.push({element:e,left:e.scrollLeft,top:e.scrollTop});for("function"===typeof n.focus&&n.focus(),n=0;n<t.length;n++)(e=t[n]).element.scrollLeft=e.left,e.element.scrollTop=e.top}}var mr=c&&"documentMode"in document&&11>=document.documentMode,vr=null,gr=null,yr=null,br=!1;function wr(e,t,n){var r=n.window===n?n.document:9===n.nodeType?n:n.ownerDocument;br||null==vr||vr!==K(r)||("selectionStart"in(r=vr)&&pr(r)?r={start:r.selectionStart,end:r.selectionEnd}:r={anchorNode:(r=(r.ownerDocument&&r.ownerDocument.defaultView||window).getSelection()).anchorNode,anchorOffset:r.anchorOffset,focusNode:r.focusNode,focusOffset:r.focusOffset},yr&&ur(yr,r)||(yr=r,0<(r=Wr(gr,"onSelect")).length&&(t=new cn("onSelect","select",null,t,n),e.push({event:t,listeners:r}),t.target=vr)))}function Sr(e,t){var n={};return n[e.toLowerCase()]=t.toLowerCase(),n["Webkit"+e]="webkit"+t,n["Moz"+e]="moz"+t,n}var xr={animationend:Sr("Animation","AnimationEnd"),animationiteration:Sr("Animation","AnimationIteration"),animationstart:Sr("Animation","AnimationStart"),transitionend:Sr("Transition","TransitionEnd")},kr={},Er={};function Cr(e){if(kr[e])return kr[e];if(!xr[e])return e;var t,n=xr[e];for(t in n)if(n.hasOwnProperty(t)&&t in Er)return kr[e]=n[t];return e}c&&(Er=document.createElement("div").style,"AnimationEvent"in window||(delete xr.animationend.animation,delete xr.animationiteration.animation,delete xr.animationstart.animation),"TransitionEvent"in window||delete xr.transitionend.transition);var _r=Cr("animationend"),jr=Cr("animationiteration"),Pr=Cr("animationstart"),Nr=Cr("transitionend"),Or=new Map,zr="abort auxClick cancel canPlay canPlayThrough click close contextMenu copy cut drag dragEnd dragEnter dragExit dragLeave dragOver dragStart drop durationChange emptied encrypted ended error gotPointerCapture input invalid keyDown keyPress keyUp load loadedData loadedMetadata loadStart lostPointerCapture mouseDown mouseMove mouseOut mouseOver mouseUp paste pause play playing pointerCancel pointerDown pointerMove pointerOut pointerOver pointerUp progress rateChange reset resize seeked seeking stalled submit suspend timeUpdate touchCancel touchEnd touchStart volumeChange scroll toggle touchMove waiting wheel".split(" ");function Tr(e,t){Or.set(e,t),u(t,[e])}for(var Lr=0;Lr<zr.length;Lr++){var Rr=zr[Lr];Tr(Rr.toLowerCase(),"on"+(Rr[0].toUpperCase()+Rr.slice(1)))}Tr(_r,"onAnimationEnd"),Tr(jr,"onAnimationIteration"),Tr(Pr,"onAnimationStart"),Tr("dblclick","onDoubleClick"),Tr("focusin","onFocus"),Tr("focusout","onBlur"),Tr(Nr,"onTransitionEnd"),s("onMouseEnter",["mouseout","mouseover"]),s("onMouseLeave",["mouseout","mouseover"]),s("onPointerEnter",["pointerout","pointerover"]),s("onPointerLeave",["pointerout","pointerover"]),u("onChange","change click focusin focusout input keydown keyup selectionchange".split(" ")),u("onSelect","focusout contextmenu dragend focusin keydown keyup mousedown mouseup selectionchange".split(" ")),u("onBeforeInput",["compositionend","keypress","textInput","paste"]),u("onCompositionEnd","compositionend focusout keydown keypress keyup mousedown".split(" ")),u("onCompositionStart","compositionstart focusout keydown keypress keyup mousedown".split(" ")),u("onCompositionUpdate","compositionupdate focusout keydown keypress keyup mousedown".split(" "));var Ar="abort canplay canplaythrough durationchange emptied encrypted ended error loadeddata loadedmetadata loadstart pause play playing progress ratechange resize seeked seeking stalled suspend timeupdate volumechange waiting".split(" "),Fr=new Set("cancel close invalid load scroll toggle".split(" ").concat(Ar));function Dr(e,t,n){var r=e.type||"unknown-event";e.currentTarget=n,function(e,t,n,r,a,o,i,u,s){if(Be.apply(this,arguments),Fe){if(!Fe)throw Error(l(198));var c=De;Fe=!1,De=null,Me||(Me=!0,Ie=c)}}(r,t,void 0,e),e.currentTarget=null}function Mr(e,t){t=0!==(4&t);for(var n=0;n<e.length;n++){var r=e[n],a=r.event;r=r.listeners;e:{var l=void 0;if(t)for(var o=r.length-1;0<=o;o--){var i=r[o],u=i.instance,s=i.currentTarget;if(i=i.listener,u!==l&&a.isPropagationStopped())break e;Dr(a,i,s),l=u}else for(o=0;o<r.length;o++){if(u=(i=r[o]).instance,s=i.currentTarget,i=i.listener,u!==l&&a.isPropagationStopped())break e;Dr(a,i,s),l=u}}}if(Me)throw e=Ie,Me=!1,Ie=null,e}function Ir(e,t){var n=t[ma];void 0===n&&(n=t[ma]=new Set);var r=e+"__bubble";n.has(r)||(qr(t,e,2,!1),n.add(r))}function Ur(e,t,n){var r=0;t&&(r|=4),qr(n,e,r,t)}var Br="_reactListening"+Math.random().toString(36).slice(2);function Vr(e){if(!e[Br]){e[Br]=!0,o.forEach((function(t){"selectionchange"!==t&&(Fr.has(t)||Ur(t,!1,e),Ur(t,!0,e))}));var t=9===e.nodeType?e:e.ownerDocument;null===t||t[Br]||(t[Br]=!0,Ur("selectionchange",!1,t))}}function qr(e,t,n,r){switch(Zt(t)){case 1:var a=$t;break;case 4:a=Wt;break;default:a=Qt}n=a.bind(null,t,n,e),a=void 0,!Le||"touchstart"!==t&&"touchmove"!==t&&"wheel"!==t||(a=!0),r?void 0!==a?e.addEventListener(t,n,{capture:!0,passive:a}):e.addEventListener(t,n,!0):void 0!==a?e.addEventListener(t,n,{passive:a}):e.addEventListener(t,n,!1)}function Hr(e,t,n,r,a){var l=r;if(0===(1&t)&&0===(2&t)&&null!==r)e:for(;;){if(null===r)return;var o=r.tag;if(3===o||4===o){var i=r.stateNode.containerInfo;if(i===a||8===i.nodeType&&i.parentNode===a)break;if(4===o)for(o=r.return;null!==o;){var u=o.tag;if((3===u||4===u)&&((u=o.stateNode.containerInfo)===a||8===u.nodeType&&u.parentNode===a))return;o=o.return}for(;null!==i;){if(null===(o=ya(i)))return;if(5===(u=o.tag)||6===u){r=l=o;continue e}i=i.parentNode}}r=r.return}ze((function(){var r=l,a=Se(n),o=[];e:{var i=Or.get(e);if(void 0!==i){var u=cn,s=e;switch(e){case"keypress":if(0===tn(n))break e;case"keydown":case"keyup":u=jn;break;case"focusin":s="focus",u=vn;break;case"focusout":s="blur",u=vn;break;case"beforeblur":case"afterblur":u=vn;break;case"click":if(2===n.button)break e;case"auxclick":case"dblclick":case"mousedown":case"mousemove":case"mouseup":case"mouseout":case"mouseover":case"contextmenu":u=hn;break;case"drag":case"dragend":case"dragenter":case"dragexit":case"dragleave":case"dragover":case"dragstart":case"drop":u=mn;break;case"touchcancel":case"touchend":case"touchmove":case"touchstart":u=Nn;break;case _r:case jr:case Pr:u=gn;break;case Nr:u=On;break;case"scroll":u=dn;break;case"wheel":u=Tn;break;case"copy":case"cut":case"paste":u=bn;break;case"gotpointercapture":case"lostpointercapture":case"pointercancel":case"pointerdown":case"pointermove":case"pointerout":case"pointerover":case"pointerup":u=Pn}var c=0!==(4&t),f=!c&&"scroll"===e,d=c?null!==i?i+"Capture":null:i;c=[];for(var p,h=r;null!==h;){var m=(p=h).stateNode;if(5===p.tag&&null!==m&&(p=m,null!==d&&(null!=(m=Te(h,d))&&c.push($r(h,m,p)))),f)break;h=h.return}0<c.length&&(i=new u(i,s,null,n,a),o.push({event:i,listeners:c}))}}if(0===(7&t)){if(u="mouseout"===e||"pointerout"===e,(!(i="mouseover"===e||"pointerover"===e)||n===we||!(s=n.relatedTarget||n.fromElement)||!ya(s)&&!s[ha])&&(u||i)&&(i=a.window===a?a:(i=a.ownerDocument)?i.defaultView||i.parentWindow:window,u?(u=r,null!==(s=(s=n.relatedTarget||n.toElement)?ya(s):null)&&(s!==(f=Ve(s))||5!==s.tag&&6!==s.tag)&&(s=null)):(u=null,s=r),u!==s)){if(c=hn,m="onMouseLeave",d="onMouseEnter",h="mouse","pointerout"!==e&&"pointerover"!==e||(c=Pn,m="onPointerLeave",d="onPointerEnter",h="pointer"),f=null==u?i:wa(u),p=null==s?i:wa(s),(i=new c(m,h+"leave",u,n,a)).target=f,i.relatedTarget=p,m=null,ya(a)===r&&((c=new c(d,h+"enter",s,n,a)).target=p,c.relatedTarget=f,m=c),f=m,u&&s)e:{for(d=s,h=0,p=c=u;p;p=Qr(p))h++;for(p=0,m=d;m;m=Qr(m))p++;for(;0<h-p;)c=Qr(c),h--;for(;0<p-h;)d=Qr(d),p--;for(;h--;){if(c===d||null!==d&&c===d.alternate)break e;c=Qr(c),d=Qr(d)}c=null}else c=null;null!==u&&Kr(o,i,u,c,!1),null!==s&&null!==f&&Kr(o,f,s,c,!0)}if("select"===(u=(i=r?wa(r):window).nodeName&&i.nodeName.toLowerCase())||"input"===u&&"file"===i.type)var v=Zn;else if(Hn(i))if(Yn)v=or;else{v=ar;var g=rr}else(u=i.nodeName)&&"input"===u.toLowerCase()&&("checkbox"===i.type||"radio"===i.type)&&(v=lr);switch(v&&(v=v(e,r))?$n(o,v,n,a):(g&&g(e,i,r),"focusout"===e&&(g=i._wrapperState)&&g.controlled&&"number"===i.type&&ee(i,"number",i.value)),g=r?wa(r):window,e){case"focusin":(Hn(g)||"true"===g.contentEditable)&&(vr=g,gr=r,yr=null);break;case"focusout":yr=gr=vr=null;break;case"mousedown":br=!0;break;case"contextmenu":case"mouseup":case"dragend":br=!1,wr(o,n,a);break;case"selectionchange":if(mr)break;case"keydown":case"keyup":wr(o,n,a)}var y;if(Rn)e:{switch(e){case"compositionstart":var b="onCompositionStart";break e;case"compositionend":b="onCompositionEnd";break e;case"compositionupdate":b="onCompositionUpdate";break e}b=void 0}else Vn?Un(e,n)&&(b="onCompositionEnd"):"keydown"===e&&229===n.keyCode&&(b="onCompositionStart");b&&(Dn&&"ko"!==n.locale&&(Vn||"onCompositionStart"!==b?"onCompositionEnd"===b&&Vn&&(y=en()):(Jt="value"in(Yt=a)?Yt.value:Yt.textContent,Vn=!0)),0<(g=Wr(r,b)).length&&(b=new wn(b,e,null,n,a),o.push({event:b,listeners:g}),y?b.data=y:null!==(y=Bn(n))&&(b.data=y))),(y=Fn?function(e,t){switch(e){case"compositionend":return Bn(t);case"keypress":return 32!==t.which?null:(In=!0,Mn);case"textInput":return(e=t.data)===Mn&&In?null:e;default:return null}}(e,n):function(e,t){if(Vn)return"compositionend"===e||!Rn&&Un(e,t)?(e=en(),Xt=Jt=Yt=null,Vn=!1,e):null;switch(e){case"paste":default:return null;case"keypress":if(!(t.ctrlKey||t.altKey||t.metaKey)||t.ctrlKey&&t.altKey){if(t.char&&1<t.char.length)return t.char;if(t.which)return String.fromCharCode(t.which)}return null;case"compositionend":return Dn&&"ko"!==t.locale?null:t.data}}(e,n))&&(0<(r=Wr(r,"onBeforeInput")).length&&(a=new wn("onBeforeInput","beforeinput",null,n,a),o.push({event:a,listeners:r}),a.data=y))}Mr(o,t)}))}function $r(e,t,n){return{instance:e,listener:t,currentTarget:n}}function Wr(e,t){for(var n=t+"Capture",r=[];null!==e;){var a=e,l=a.stateNode;5===a.tag&&null!==l&&(a=l,null!=(l=Te(e,n))&&r.unshift($r(e,l,a)),null!=(l=Te(e,t))&&r.push($r(e,l,a))),e=e.return}return r}function Qr(e){if(null===e)return null;do{e=e.return}while(e&&5!==e.tag);return e||null}function Kr(e,t,n,r,a){for(var l=t._reactName,o=[];null!==n&&n!==r;){var i=n,u=i.alternate,s=i.stateNode;if(null!==u&&u===r)break;5===i.tag&&null!==s&&(i=s,a?null!=(u=Te(n,l))&&o.unshift($r(n,u,i)):a||null!=(u=Te(n,l))&&o.push($r(n,u,i))),n=n.return}0!==o.length&&e.push({event:t,listeners:o})}var Gr=/\r\n?/g,Zr=/\u0000|\uFFFD/g;function Yr(e){return("string"===typeof e?e:""+e).replace(Gr,"\n").replace(Zr,"")}function Jr(e,t,n){if(t=Yr(t),Yr(e)!==t&&n)throw Error(l(425))}function Xr(){}var ea=null,ta=null;function na(e,t){return"textarea"===e||"noscript"===e||"string"===typeof t.children||"number"===typeof t.children||"object"===typeof t.dangerouslySetInnerHTML&&null!==t.dangerouslySetInnerHTML&&null!=t.dangerouslySetInnerHTML.__html}var ra="function"===typeof setTimeout?setTimeout:void 0,aa="function"===typeof clearTimeout?clearTimeout:void 0,la="function"===typeof Promise?Promise:void 0,oa="function"===typeof queueMicrotask?queueMicrotask:"undefined"!==typeof la?function(e){return la.resolve(null).then(e).catch(ia)}:ra;function ia(e){setTimeout((function(){throw e}))}function ua(e,t){var n=t,r=0;do{var a=n.nextSibling;if(e.removeChild(n),a&&8===a.nodeType)if("/$"===(n=a.data)){if(0===r)return e.removeChild(a),void Vt(t);r--}else"$"!==n&&"$?"!==n&&"$!"!==n||r++;n=a}while(n);Vt(t)}function sa(e){for(;null!=e;e=e.nextSibling){var t=e.nodeType;if(1===t||3===t)break;if(8===t){if("$"===(t=e.data)||"$!"===t||"$?"===t)break;if("/$"===t)return null}}return e}function ca(e){e=e.previousSibling;for(var t=0;e;){if(8===e.nodeType){var n=e.data;if("$"===n||"$!"===n||"$?"===n){if(0===t)return e;t--}else"/$"===n&&t++}e=e.previousSibling}return null}var fa=Math.random().toString(36).slice(2),da="__reactFiber$"+fa,pa="__reactProps$"+fa,ha="__reactContainer$"+fa,ma="__reactEvents$"+fa,va="__reactListeners$"+fa,ga="__reactHandles$"+fa;function ya(e){var t=e[da];if(t)return t;for(var n=e.parentNode;n;){if(t=n[ha]||n[da]){if(n=t.alternate,null!==t.child||null!==n&&null!==n.child)for(e=ca(e);null!==e;){if(n=e[da])return n;e=ca(e)}return t}n=(e=n).parentNode}return null}function ba(e){return!(e=e[da]||e[ha])||5!==e.tag&&6!==e.tag&&13!==e.tag&&3!==e.tag?null:e}function wa(e){if(5===e.tag||6===e.tag)return e.stateNode;throw Error(l(33))}function Sa(e){return e[pa]||null}var xa=[],ka=-1;function Ea(e){return{current:e}}function Ca(e){0>ka||(e.current=xa[ka],xa[ka]=null,ka--)}function _a(e,t){ka++,xa[ka]=e.current,e.current=t}var ja={},Pa=Ea(ja),Na=Ea(!1),Oa=ja;function za(e,t){var n=e.type.contextTypes;if(!n)return ja;var r=e.stateNode;if(r&&r.__reactInternalMemoizedUnmaskedChildContext===t)return r.__reactInternalMemoizedMaskedChildContext;var a,l={};for(a in n)l[a]=t[a];return r&&((e=e.stateNode).__reactInternalMemoizedUnmaskedChildContext=t,e.__reactInternalMemoizedMaskedChildContext=l),l}function Ta(e){return null!==(e=e.childContextTypes)&&void 0!==e}function La(){Ca(Na),Ca(Pa)}function Ra(e,t,n){if(Pa.current!==ja)throw Error(l(168));_a(Pa,t),_a(Na,n)}function Aa(e,t,n){var r=e.stateNode;if(t=t.childContextTypes,"function"!==typeof r.getChildContext)return n;for(var a in r=r.getChildContext())if(!(a in t))throw Error(l(108,q(e)||"Unknown",a));return D({},n,r)}function Fa(e){return e=(e=e.stateNode)&&e.__reactInternalMemoizedMergedChildContext||ja,Oa=Pa.current,_a(Pa,e),_a(Na,Na.current),!0}function Da(e,t,n){var r=e.stateNode;if(!r)throw Error(l(169));n?(e=Aa(e,t,Oa),r.__reactInternalMemoizedMergedChildContext=e,Ca(Na),Ca(Pa),_a(Pa,e)):Ca(Na),_a(Na,n)}var Ma=null,Ia=!1,Ua=!1;function Ba(e){null===Ma?Ma=[e]:Ma.push(e)}function Va(){if(!Ua&&null!==Ma){Ua=!0;var e=0,t=bt;try{var n=Ma;for(bt=1;e<n.length;e++){var r=n[e];do{r=r(!0)}while(null!==r)}Ma=null,Ia=!1}catch(a){throw null!==Ma&&(Ma=Ma.slice(e+1)),Qe(Xe,Va),a}finally{bt=t,Ua=!1}}return null}var qa=[],Ha=0,$a=null,Wa=0,Qa=[],Ka=0,Ga=null,Za=1,Ya="";function Ja(e,t){qa[Ha++]=Wa,qa[Ha++]=$a,$a=e,Wa=t}function Xa(e,t,n){Qa[Ka++]=Za,Qa[Ka++]=Ya,Qa[Ka++]=Ga,Ga=e;var r=Za;e=Ya;var a=32-ot(r)-1;r&=~(1<<a),n+=1;var l=32-ot(t)+a;if(30<l){var o=a-a%5;l=(r&(1<<o)-1).toString(32),r>>=o,a-=o,Za=1<<32-ot(t)+a|n<<a|r,Ya=l+e}else Za=1<<l|n<<a|r,Ya=e}function el(e){null!==e.return&&(Ja(e,1),Xa(e,1,0))}function tl(e){for(;e===$a;)$a=qa[--Ha],qa[Ha]=null,Wa=qa[--Ha],qa[Ha]=null;for(;e===Ga;)Ga=Qa[--Ka],Qa[Ka]=null,Ya=Qa[--Ka],Qa[Ka]=null,Za=Qa[--Ka],Qa[Ka]=null}var nl=null,rl=null,al=!1,ll=null;function ol(e,t){var n=Ts(5,null,null,0);n.elementType="DELETED",n.stateNode=t,n.return=e,null===(t=e.deletions)?(e.deletions=[n],e.flags|=16):t.push(n)}function il(e,t){switch(e.tag){case 5:var n=e.type;return null!==(t=1!==t.nodeType||n.toLowerCase()!==t.nodeName.toLowerCase()?null:t)&&(e.stateNode=t,nl=e,rl=sa(t.firstChild),!0);case 6:return null!==(t=""===e.pendingProps||3!==t.nodeType?null:t)&&(e.stateNode=t,nl=e,rl=null,!0);case 13:return null!==(t=8!==t.nodeType?null:t)&&(n=null!==Ga?{id:Za,overflow:Ya}:null,e.memoizedState={dehydrated:t,treeContext:n,retryLane:1073741824},(n=Ts(18,null,null,0)).stateNode=t,n.return=e,e.child=n,nl=e,rl=null,!0);default:return!1}}function ul(e){return 0!==(1&e.mode)&&0===(128&e.flags)}function sl(e){if(al){var t=rl;if(t){var n=t;if(!il(e,t)){if(ul(e))throw Error(l(418));t=sa(n.nextSibling);var r=nl;t&&il(e,t)?ol(r,n):(e.flags=-4097&e.flags|2,al=!1,nl=e)}}else{if(ul(e))throw Error(l(418));e.flags=-4097&e.flags|2,al=!1,nl=e}}}function cl(e){for(e=e.return;null!==e&&5!==e.tag&&3!==e.tag&&13!==e.tag;)e=e.return;nl=e}function fl(e){if(e!==nl)return!1;if(!al)return cl(e),al=!0,!1;var t;if((t=3!==e.tag)&&!(t=5!==e.tag)&&(t="head"!==(t=e.type)&&"body"!==t&&!na(e.type,e.memoizedProps)),t&&(t=rl)){if(ul(e))throw dl(),Error(l(418));for(;t;)ol(e,t),t=sa(t.nextSibling)}if(cl(e),13===e.tag){if(!(e=null!==(e=e.memoizedState)?e.dehydrated:null))throw Error(l(317));e:{for(e=e.nextSibling,t=0;e;){if(8===e.nodeType){var n=e.data;if("/$"===n){if(0===t){rl=sa(e.nextSibling);break e}t--}else"$"!==n&&"$!"!==n&&"$?"!==n||t++}e=e.nextSibling}rl=null}}else rl=nl?sa(e.stateNode.nextSibling):null;return!0}function dl(){for(var e=rl;e;)e=sa(e.nextSibling)}function pl(){rl=nl=null,al=!1}function hl(e){null===ll?ll=[e]:ll.push(e)}var ml=w.ReactCurrentBatchConfig;function vl(e,t){if(e&&e.defaultProps){for(var n in t=D({},t),e=e.defaultProps)void 0===t[n]&&(t[n]=e[n]);return t}return t}var gl=Ea(null),yl=null,bl=null,wl=null;function Sl(){wl=bl=yl=null}function xl(e){var t=gl.current;Ca(gl),e._currentValue=t}function kl(e,t,n){for(;null!==e;){var r=e.alternate;if((e.childLanes&t)!==t?(e.childLanes|=t,null!==r&&(r.childLanes|=t)):null!==r&&(r.childLanes&t)!==t&&(r.childLanes|=t),e===n)break;e=e.return}}function El(e,t){yl=e,wl=bl=null,null!==(e=e.dependencies)&&null!==e.firstContext&&(0!==(e.lanes&t)&&(wi=!0),e.firstContext=null)}function Cl(e){var t=e._currentValue;if(wl!==e)if(e={context:e,memoizedValue:t,next:null},null===bl){if(null===yl)throw Error(l(308));bl=e,yl.dependencies={lanes:0,firstContext:e}}else bl=bl.next=e;return t}var _l=null;function jl(e){null===_l?_l=[e]:_l.push(e)}function Pl(e,t,n,r){var a=t.interleaved;return null===a?(n.next=n,jl(t)):(n.next=a.next,a.next=n),t.interleaved=n,Nl(e,r)}function Nl(e,t){e.lanes|=t;var n=e.alternate;for(null!==n&&(n.lanes|=t),n=e,e=e.return;null!==e;)e.childLanes|=t,null!==(n=e.alternate)&&(n.childLanes|=t),n=e,e=e.return;return 3===n.tag?n.stateNode:null}var Ol=!1;function zl(e){e.updateQueue={baseState:e.memoizedState,firstBaseUpdate:null,lastBaseUpdate:null,shared:{pending:null,interleaved:null,lanes:0},effects:null}}function Tl(e,t){e=e.updateQueue,t.updateQueue===e&&(t.updateQueue={baseState:e.baseState,firstBaseUpdate:e.firstBaseUpdate,lastBaseUpdate:e.lastBaseUpdate,shared:e.shared,effects:e.effects})}function Ll(e,t){return{eventTime:e,lane:t,tag:0,payload:null,callback:null,next:null}}function Rl(e,t,n){var r=e.updateQueue;if(null===r)return null;if(r=r.shared,0!==(2&Nu)){var a=r.pending;return null===a?t.next=t:(t.next=a.next,a.next=t),r.pending=t,Nl(e,n)}return null===(a=r.interleaved)?(t.next=t,jl(r)):(t.next=a.next,a.next=t),r.interleaved=t,Nl(e,n)}function Al(e,t,n){if(null!==(t=t.updateQueue)&&(t=t.shared,0!==(4194240&n))){var r=t.lanes;n|=r&=e.pendingLanes,t.lanes=n,yt(e,n)}}function Fl(e,t){var n=e.updateQueue,r=e.alternate;if(null!==r&&n===(r=r.updateQueue)){var a=null,l=null;if(null!==(n=n.firstBaseUpdate)){do{var o={eventTime:n.eventTime,lane:n.lane,tag:n.tag,payload:n.payload,callback:n.callback,next:null};null===l?a=l=o:l=l.next=o,n=n.next}while(null!==n);null===l?a=l=t:l=l.next=t}else a=l=t;return n={baseState:r.baseState,firstBaseUpdate:a,lastBaseUpdate:l,shared:r.shared,effects:r.effects},void(e.updateQueue=n)}null===(e=n.lastBaseUpdate)?n.firstBaseUpdate=t:e.next=t,n.lastBaseUpdate=t}function Dl(e,t,n,r){var a=e.updateQueue;Ol=!1;var l=a.firstBaseUpdate,o=a.lastBaseUpdate,i=a.shared.pending;if(null!==i){a.shared.pending=null;var u=i,s=u.next;u.next=null,null===o?l=s:o.next=s,o=u;var c=e.alternate;null!==c&&((i=(c=c.updateQueue).lastBaseUpdate)!==o&&(null===i?c.firstBaseUpdate=s:i.next=s,c.lastBaseUpdate=u))}if(null!==l){var f=a.baseState;for(o=0,c=s=u=null,i=l;;){var d=i.lane,p=i.eventTime;if((r&d)===d){null!==c&&(c=c.next={eventTime:p,lane:0,tag:i.tag,payload:i.payload,callback:i.callback,next:null});e:{var h=e,m=i;switch(d=t,p=n,m.tag){case 1:if("function"===typeof(h=m.payload)){f=h.call(p,f,d);break e}f=h;break e;case 3:h.flags=-65537&h.flags|128;case 0:if(null===(d="function"===typeof(h=m.payload)?h.call(p,f,d):h)||void 0===d)break e;f=D({},f,d);break e;case 2:Ol=!0}}null!==i.callback&&0!==i.lane&&(e.flags|=64,null===(d=a.effects)?a.effects=[i]:d.push(i))}else p={eventTime:p,lane:d,tag:i.tag,payload:i.payload,callback:i.callback,next:null},null===c?(s=c=p,u=f):c=c.next=p,o|=d;if(null===(i=i.next)){if(null===(i=a.shared.pending))break;i=(d=i).next,d.next=null,a.lastBaseUpdate=d,a.shared.pending=null}}if(null===c&&(u=f),a.baseState=u,a.firstBaseUpdate=s,a.lastBaseUpdate=c,null!==(t=a.shared.interleaved)){a=t;do{o|=a.lane,a=a.next}while(a!==t)}else null===l&&(a.shared.lanes=0);Du|=o,e.lanes=o,e.memoizedState=f}}function Ml(e,t,n){if(e=t.effects,t.effects=null,null!==e)for(t=0;t<e.length;t++){var r=e[t],a=r.callback;if(null!==a){if(r.callback=null,r=n,"function"!==typeof a)throw Error(l(191,a));a.call(r)}}}var Il=(new r.Component).refs;function Ul(e,t,n,r){n=null===(n=n(r,t=e.memoizedState))||void 0===n?t:D({},t,n),e.memoizedState=n,0===e.lanes&&(e.updateQueue.baseState=n)}var Bl={isMounted:function(e){return!!(e=e._reactInternals)&&Ve(e)===e},enqueueSetState:function(e,t,n){e=e._reactInternals;var r=ts(),a=ns(e),l=Ll(r,a);l.payload=t,void 0!==n&&null!==n&&(l.callback=n),null!==(t=Rl(e,l,a))&&(rs(t,e,a,r),Al(t,e,a))},enqueueReplaceState:function(e,t,n){e=e._reactInternals;var r=ts(),a=ns(e),l=Ll(r,a);l.tag=1,l.payload=t,void 0!==n&&null!==n&&(l.callback=n),null!==(t=Rl(e,l,a))&&(rs(t,e,a,r),Al(t,e,a))},enqueueForceUpdate:function(e,t){e=e._reactInternals;var n=ts(),r=ns(e),a=Ll(n,r);a.tag=2,void 0!==t&&null!==t&&(a.callback=t),null!==(t=Rl(e,a,r))&&(rs(t,e,r,n),Al(t,e,r))}};function Vl(e,t,n,r,a,l,o){return"function"===typeof(e=e.stateNode).shouldComponentUpdate?e.shouldComponentUpdate(r,l,o):!t.prototype||!t.prototype.isPureReactComponent||(!ur(n,r)||!ur(a,l))}function ql(e,t,n){var r=!1,a=ja,l=t.contextType;return"object"===typeof l&&null!==l?l=Cl(l):(a=Ta(t)?Oa:Pa.current,l=(r=null!==(r=t.contextTypes)&&void 0!==r)?za(e,a):ja),t=new t(n,l),e.memoizedState=null!==t.state&&void 0!==t.state?t.state:null,t.updater=Bl,e.stateNode=t,t._reactInternals=e,r&&((e=e.stateNode).__reactInternalMemoizedUnmaskedChildContext=a,e.__reactInternalMemoizedMaskedChildContext=l),t}function Hl(e,t,n,r){e=t.state,"function"===typeof t.componentWillReceiveProps&&t.componentWillReceiveProps(n,r),"function"===typeof t.UNSAFE_componentWillReceiveProps&&t.UNSAFE_componentWillReceiveProps(n,r),t.state!==e&&Bl.enqueueReplaceState(t,t.state,null)}function $l(e,t,n,r){var a=e.stateNode;a.props=n,a.state=e.memoizedState,a.refs=Il,zl(e);var l=t.contextType;"object"===typeof l&&null!==l?a.context=Cl(l):(l=Ta(t)?Oa:Pa.current,a.context=za(e,l)),a.state=e.memoizedState,"function"===typeof(l=t.getDerivedStateFromProps)&&(Ul(e,t,l,n),a.state=e.memoizedState),"function"===typeof t.getDerivedStateFromProps||"function"===typeof a.getSnapshotBeforeUpdate||"function"!==typeof a.UNSAFE_componentWillMount&&"function"!==typeof a.componentWillMount||(t=a.state,"function"===typeof a.componentWillMount&&a.componentWillMount(),"function"===typeof a.UNSAFE_componentWillMount&&a.UNSAFE_componentWillMount(),t!==a.state&&Bl.enqueueReplaceState(a,a.state,null),Dl(e,n,a,r),a.state=e.memoizedState),"function"===typeof a.componentDidMount&&(e.flags|=4194308)}function Wl(e,t,n){if(null!==(e=n.ref)&&"function"!==typeof e&&"object"!==typeof e){if(n._owner){if(n=n._owner){if(1!==n.tag)throw Error(l(309));var r=n.stateNode}if(!r)throw Error(l(147,e));var a=r,o=""+e;return null!==t&&null!==t.ref&&"function"===typeof t.ref&&t.ref._stringRef===o?t.ref:(t=function(e){var t=a.refs;t===Il&&(t=a.refs={}),null===e?delete t[o]:t[o]=e},t._stringRef=o,t)}if("string"!==typeof e)throw Error(l(284));if(!n._owner)throw Error(l(290,e))}return e}function Ql(e,t){throw e=Object.prototype.toString.call(t),Error(l(31,"[object Object]"===e?"object with keys {"+Object.keys(t).join(", ")+"}":e))}function Kl(e){return(0,e._init)(e._payload)}function Gl(e){function t(t,n){if(e){var r=t.deletions;null===r?(t.deletions=[n],t.flags|=16):r.push(n)}}function n(n,r){if(!e)return null;for(;null!==r;)t(n,r),r=r.sibling;return null}function r(e,t){for(e=new Map;null!==t;)null!==t.key?e.set(t.key,t):e.set(t.index,t),t=t.sibling;return e}function a(e,t){return(e=Rs(e,t)).index=0,e.sibling=null,e}function o(t,n,r){return t.index=r,e?null!==(r=t.alternate)?(r=r.index)<n?(t.flags|=2,n):r:(t.flags|=2,n):(t.flags|=1048576,n)}function i(t){return e&&null===t.alternate&&(t.flags|=2),t}function u(e,t,n,r){return null===t||6!==t.tag?((t=Ms(n,e.mode,r)).return=e,t):((t=a(t,n)).return=e,t)}function s(e,t,n,r){var l=n.type;return l===k?f(e,t,n.props.children,r,n.key):null!==t&&(t.elementType===l||"object"===typeof l&&null!==l&&l.$$typeof===T&&Kl(l)===t.type)?((r=a(t,n.props)).ref=Wl(e,t,n),r.return=e,r):((r=As(n.type,n.key,n.props,null,e.mode,r)).ref=Wl(e,t,n),r.return=e,r)}function c(e,t,n,r){return null===t||4!==t.tag||t.stateNode.containerInfo!==n.containerInfo||t.stateNode.implementation!==n.implementation?((t=Is(n,e.mode,r)).return=e,t):((t=a(t,n.children||[])).return=e,t)}function f(e,t,n,r,l){return null===t||7!==t.tag?((t=Fs(n,e.mode,r,l)).return=e,t):((t=a(t,n)).return=e,t)}function d(e,t,n){if("string"===typeof t&&""!==t||"number"===typeof t)return(t=Ms(""+t,e.mode,n)).return=e,t;if("object"===typeof t&&null!==t){switch(t.$$typeof){case S:return(n=As(t.type,t.key,t.props,null,e.mode,n)).ref=Wl(e,null,t),n.return=e,n;case x:return(t=Is(t,e.mode,n)).return=e,t;case T:return d(e,(0,t._init)(t._payload),n)}if(te(t)||A(t))return(t=Fs(t,e.mode,n,null)).return=e,t;Ql(e,t)}return null}function p(e,t,n,r){var a=null!==t?t.key:null;if("string"===typeof n&&""!==n||"number"===typeof n)return null!==a?null:u(e,t,""+n,r);if("object"===typeof n&&null!==n){switch(n.$$typeof){case S:return n.key===a?s(e,t,n,r):null;case x:return n.key===a?c(e,t,n,r):null;case T:return p(e,t,(a=n._init)(n._payload),r)}if(te(n)||A(n))return null!==a?null:f(e,t,n,r,null);Ql(e,n)}return null}function h(e,t,n,r,a){if("string"===typeof r&&""!==r||"number"===typeof r)return u(t,e=e.get(n)||null,""+r,a);if("object"===typeof r&&null!==r){switch(r.$$typeof){case S:return s(t,e=e.get(null===r.key?n:r.key)||null,r,a);case x:return c(t,e=e.get(null===r.key?n:r.key)||null,r,a);case T:return h(e,t,n,(0,r._init)(r._payload),a)}if(te(r)||A(r))return f(t,e=e.get(n)||null,r,a,null);Ql(t,r)}return null}function m(a,l,i,u){for(var s=null,c=null,f=l,m=l=0,v=null;null!==f&&m<i.length;m++){f.index>m?(v=f,f=null):v=f.sibling;var g=p(a,f,i[m],u);if(null===g){null===f&&(f=v);break}e&&f&&null===g.alternate&&t(a,f),l=o(g,l,m),null===c?s=g:c.sibling=g,c=g,f=v}if(m===i.length)return n(a,f),al&&Ja(a,m),s;if(null===f){for(;m<i.length;m++)null!==(f=d(a,i[m],u))&&(l=o(f,l,m),null===c?s=f:c.sibling=f,c=f);return al&&Ja(a,m),s}for(f=r(a,f);m<i.length;m++)null!==(v=h(f,a,m,i[m],u))&&(e&&null!==v.alternate&&f.delete(null===v.key?m:v.key),l=o(v,l,m),null===c?s=v:c.sibling=v,c=v);return e&&f.forEach((function(e){return t(a,e)})),al&&Ja(a,m),s}function v(a,i,u,s){var c=A(u);if("function"!==typeof c)throw Error(l(150));if(null==(u=c.call(u)))throw Error(l(151));for(var f=c=null,m=i,v=i=0,g=null,y=u.next();null!==m&&!y.done;v++,y=u.next()){m.index>v?(g=m,m=null):g=m.sibling;var b=p(a,m,y.value,s);if(null===b){null===m&&(m=g);break}e&&m&&null===b.alternate&&t(a,m),i=o(b,i,v),null===f?c=b:f.sibling=b,f=b,m=g}if(y.done)return n(a,m),al&&Ja(a,v),c;if(null===m){for(;!y.done;v++,y=u.next())null!==(y=d(a,y.value,s))&&(i=o(y,i,v),null===f?c=y:f.sibling=y,f=y);return al&&Ja(a,v),c}for(m=r(a,m);!y.done;v++,y=u.next())null!==(y=h(m,a,v,y.value,s))&&(e&&null!==y.alternate&&m.delete(null===y.key?v:y.key),i=o(y,i,v),null===f?c=y:f.sibling=y,f=y);return e&&m.forEach((function(e){return t(a,e)})),al&&Ja(a,v),c}return function e(r,l,o,u){if("object"===typeof o&&null!==o&&o.type===k&&null===o.key&&(o=o.props.children),"object"===typeof o&&null!==o){switch(o.$$typeof){case S:e:{for(var s=o.key,c=l;null!==c;){if(c.key===s){if((s=o.type)===k){if(7===c.tag){n(r,c.sibling),(l=a(c,o.props.children)).return=r,r=l;break e}}else if(c.elementType===s||"object"===typeof s&&null!==s&&s.$$typeof===T&&Kl(s)===c.type){n(r,c.sibling),(l=a(c,o.props)).ref=Wl(r,c,o),l.return=r,r=l;break e}n(r,c);break}t(r,c),c=c.sibling}o.type===k?((l=Fs(o.props.children,r.mode,u,o.key)).return=r,r=l):((u=As(o.type,o.key,o.props,null,r.mode,u)).ref=Wl(r,l,o),u.return=r,r=u)}return i(r);case x:e:{for(c=o.key;null!==l;){if(l.key===c){if(4===l.tag&&l.stateNode.containerInfo===o.containerInfo&&l.stateNode.implementation===o.implementation){n(r,l.sibling),(l=a(l,o.children||[])).return=r,r=l;break e}n(r,l);break}t(r,l),l=l.sibling}(l=Is(o,r.mode,u)).return=r,r=l}return i(r);case T:return e(r,l,(c=o._init)(o._payload),u)}if(te(o))return m(r,l,o,u);if(A(o))return v(r,l,o,u);Ql(r,o)}return"string"===typeof o&&""!==o||"number"===typeof o?(o=""+o,null!==l&&6===l.tag?(n(r,l.sibling),(l=a(l,o)).return=r,r=l):(n(r,l),(l=Ms(o,r.mode,u)).return=r,r=l),i(r)):n(r,l)}}var Zl=Gl(!0),Yl=Gl(!1),Jl={},Xl=Ea(Jl),eo=Ea(Jl),to=Ea(Jl);function no(e){if(e===Jl)throw Error(l(174));return e}function ro(e,t){switch(_a(to,t),_a(eo,e),_a(Xl,Jl),e=t.nodeType){case 9:case 11:t=(t=t.documentElement)?t.namespaceURI:ue(null,"");break;default:t=ue(t=(e=8===e?t.parentNode:t).namespaceURI||null,e=e.tagName)}Ca(Xl),_a(Xl,t)}function ao(){Ca(Xl),Ca(eo),Ca(to)}function lo(e){no(to.current);var t=no(Xl.current),n=ue(t,e.type);t!==n&&(_a(eo,e),_a(Xl,n))}function oo(e){eo.current===e&&(Ca(Xl),Ca(eo))}var io=Ea(0);function uo(e){for(var t=e;null!==t;){if(13===t.tag){var n=t.memoizedState;if(null!==n&&(null===(n=n.dehydrated)||"$?"===n.data||"$!"===n.data))return t}else if(19===t.tag&&void 0!==t.memoizedProps.revealOrder){if(0!==(128&t.flags))return t}else if(null!==t.child){t.child.return=t,t=t.child;continue}if(t===e)break;for(;null===t.sibling;){if(null===t.return||t.return===e)return null;t=t.return}t.sibling.return=t.return,t=t.sibling}return null}var so=[];function co(){for(var e=0;e<so.length;e++)so[e]._workInProgressVersionPrimary=null;so.length=0}var fo=w.ReactCurrentDispatcher,po=w.ReactCurrentBatchConfig,ho=0,mo=null,vo=null,go=null,yo=!1,bo=!1,wo=0,So=0;function xo(){throw Error(l(321))}function ko(e,t){if(null===t)return!1;for(var n=0;n<t.length&&n<e.length;n++)if(!ir(e[n],t[n]))return!1;return!0}function Eo(e,t,n,r,a,o){if(ho=o,mo=t,t.memoizedState=null,t.updateQueue=null,t.lanes=0,fo.current=null===e||null===e.memoizedState?ii:ui,e=n(r,a),bo){o=0;do{if(bo=!1,wo=0,25<=o)throw Error(l(301));o+=1,go=vo=null,t.updateQueue=null,fo.current=si,e=n(r,a)}while(bo)}if(fo.current=oi,t=null!==vo&&null!==vo.next,ho=0,go=vo=mo=null,yo=!1,t)throw Error(l(300));return e}function Co(){var e=0!==wo;return wo=0,e}function _o(){var e={memoizedState:null,baseState:null,baseQueue:null,queue:null,next:null};return null===go?mo.memoizedState=go=e:go=go.next=e,go}function jo(){if(null===vo){var e=mo.alternate;e=null!==e?e.memoizedState:null}else e=vo.next;var t=null===go?mo.memoizedState:go.next;if(null!==t)go=t,vo=e;else{if(null===e)throw Error(l(310));e={memoizedState:(vo=e).memoizedState,baseState:vo.baseState,baseQueue:vo.baseQueue,queue:vo.queue,next:null},null===go?mo.memoizedState=go=e:go=go.next=e}return go}function Po(e,t){return"function"===typeof t?t(e):t}function No(e){var t=jo(),n=t.queue;if(null===n)throw Error(l(311));n.lastRenderedReducer=e;var r=vo,a=r.baseQueue,o=n.pending;if(null!==o){if(null!==a){var i=a.next;a.next=o.next,o.next=i}r.baseQueue=a=o,n.pending=null}if(null!==a){o=a.next,r=r.baseState;var u=i=null,s=null,c=o;do{var f=c.lane;if((ho&f)===f)null!==s&&(s=s.next={lane:0,action:c.action,hasEagerState:c.hasEagerState,eagerState:c.eagerState,next:null}),r=c.hasEagerState?c.eagerState:e(r,c.action);else{var d={lane:f,action:c.action,hasEagerState:c.hasEagerState,eagerState:c.eagerState,next:null};null===s?(u=s=d,i=r):s=s.next=d,mo.lanes|=f,Du|=f}c=c.next}while(null!==c&&c!==o);null===s?i=r:s.next=u,ir(r,t.memoizedState)||(wi=!0),t.memoizedState=r,t.baseState=i,t.baseQueue=s,n.lastRenderedState=r}if(null!==(e=n.interleaved)){a=e;do{o=a.lane,mo.lanes|=o,Du|=o,a=a.next}while(a!==e)}else null===a&&(n.lanes=0);return[t.memoizedState,n.dispatch]}function Oo(e){var t=jo(),n=t.queue;if(null===n)throw Error(l(311));n.lastRenderedReducer=e;var r=n.dispatch,a=n.pending,o=t.memoizedState;if(null!==a){n.pending=null;var i=a=a.next;do{o=e(o,i.action),i=i.next}while(i!==a);ir(o,t.memoizedState)||(wi=!0),t.memoizedState=o,null===t.baseQueue&&(t.baseState=o),n.lastRenderedState=o}return[o,r]}function zo(){}function To(e,t){var n=mo,r=jo(),a=t(),o=!ir(r.memoizedState,a);if(o&&(r.memoizedState=a,wi=!0),r=r.queue,Ho(Ao.bind(null,n,r,e),[e]),r.getSnapshot!==t||o||null!==go&&1&go.memoizedState.tag){if(n.flags|=2048,Io(9,Ro.bind(null,n,r,a,t),void 0,null),null===Ou)throw Error(l(349));0!==(30&ho)||Lo(n,t,a)}return a}function Lo(e,t,n){e.flags|=16384,e={getSnapshot:t,value:n},null===(t=mo.updateQueue)?(t={lastEffect:null,stores:null},mo.updateQueue=t,t.stores=[e]):null===(n=t.stores)?t.stores=[e]:n.push(e)}function Ro(e,t,n,r){t.value=n,t.getSnapshot=r,Fo(t)&&Do(e)}function Ao(e,t,n){return n((function(){Fo(t)&&Do(e)}))}function Fo(e){var t=e.getSnapshot;e=e.value;try{var n=t();return!ir(e,n)}catch(r){return!0}}function Do(e){var t=Nl(e,1);null!==t&&rs(t,e,1,-1)}function Mo(e){var t=_o();return"function"===typeof e&&(e=e()),t.memoizedState=t.baseState=e,e={pending:null,interleaved:null,lanes:0,dispatch:null,lastRenderedReducer:Po,lastRenderedState:e},t.queue=e,e=e.dispatch=ni.bind(null,mo,e),[t.memoizedState,e]}function Io(e,t,n,r){return e={tag:e,create:t,destroy:n,deps:r,next:null},null===(t=mo.updateQueue)?(t={lastEffect:null,stores:null},mo.updateQueue=t,t.lastEffect=e.next=e):null===(n=t.lastEffect)?t.lastEffect=e.next=e:(r=n.next,n.next=e,e.next=r,t.lastEffect=e),e}function Uo(){return jo().memoizedState}function Bo(e,t,n,r){var a=_o();mo.flags|=e,a.memoizedState=Io(1|t,n,void 0,void 0===r?null:r)}function Vo(e,t,n,r){var a=jo();r=void 0===r?null:r;var l=void 0;if(null!==vo){var o=vo.memoizedState;if(l=o.destroy,null!==r&&ko(r,o.deps))return void(a.memoizedState=Io(t,n,l,r))}mo.flags|=e,a.memoizedState=Io(1|t,n,l,r)}function qo(e,t){return Bo(8390656,8,e,t)}function Ho(e,t){return Vo(2048,8,e,t)}function $o(e,t){return Vo(4,2,e,t)}function Wo(e,t){return Vo(4,4,e,t)}function Qo(e,t){return"function"===typeof t?(e=e(),t(e),function(){t(null)}):null!==t&&void 0!==t?(e=e(),t.current=e,function(){t.current=null}):void 0}function Ko(e,t,n){return n=null!==n&&void 0!==n?n.concat([e]):null,Vo(4,4,Qo.bind(null,t,e),n)}function Go(){}function Zo(e,t){var n=jo();t=void 0===t?null:t;var r=n.memoizedState;return null!==r&&null!==t&&ko(t,r[1])?r[0]:(n.memoizedState=[e,t],e)}function Yo(e,t){var n=jo();t=void 0===t?null:t;var r=n.memoizedState;return null!==r&&null!==t&&ko(t,r[1])?r[0]:(e=e(),n.memoizedState=[e,t],e)}function Jo(e,t,n){return 0===(21&ho)?(e.baseState&&(e.baseState=!1,wi=!0),e.memoizedState=n):(ir(n,t)||(n=mt(),mo.lanes|=n,Du|=n,e.baseState=!0),t)}function Xo(e,t){var n=bt;bt=0!==n&&4>n?n:4,e(!0);var r=po.transition;po.transition={};try{e(!1),t()}finally{bt=n,po.transition=r}}function ei(){return jo().memoizedState}function ti(e,t,n){var r=ns(e);if(n={lane:r,action:n,hasEagerState:!1,eagerState:null,next:null},ri(e))ai(t,n);else if(null!==(n=Pl(e,t,n,r))){rs(n,e,r,ts()),li(n,t,r)}}function ni(e,t,n){var r=ns(e),a={lane:r,action:n,hasEagerState:!1,eagerState:null,next:null};if(ri(e))ai(t,a);else{var l=e.alternate;if(0===e.lanes&&(null===l||0===l.lanes)&&null!==(l=t.lastRenderedReducer))try{var o=t.lastRenderedState,i=l(o,n);if(a.hasEagerState=!0,a.eagerState=i,ir(i,o)){var u=t.interleaved;return null===u?(a.next=a,jl(t)):(a.next=u.next,u.next=a),void(t.interleaved=a)}}catch(s){}null!==(n=Pl(e,t,a,r))&&(rs(n,e,r,a=ts()),li(n,t,r))}}function ri(e){var t=e.alternate;return e===mo||null!==t&&t===mo}function ai(e,t){bo=yo=!0;var n=e.pending;null===n?t.next=t:(t.next=n.next,n.next=t),e.pending=t}function li(e,t,n){if(0!==(4194240&n)){var r=t.lanes;n|=r&=e.pendingLanes,t.lanes=n,yt(e,n)}}var oi={readContext:Cl,useCallback:xo,useContext:xo,useEffect:xo,useImperativeHandle:xo,useInsertionEffect:xo,useLayoutEffect:xo,useMemo:xo,useReducer:xo,useRef:xo,useState:xo,useDebugValue:xo,useDeferredValue:xo,useTransition:xo,useMutableSource:xo,useSyncExternalStore:xo,useId:xo,unstable_isNewReconciler:!1},ii={readContext:Cl,useCallback:function(e,t){return _o().memoizedState=[e,void 0===t?null:t],e},useContext:Cl,useEffect:qo,useImperativeHandle:function(e,t,n){return n=null!==n&&void 0!==n?n.concat([e]):null,Bo(4194308,4,Qo.bind(null,t,e),n)},useLayoutEffect:function(e,t){return Bo(4194308,4,e,t)},useInsertionEffect:function(e,t){return Bo(4,2,e,t)},useMemo:function(e,t){var n=_o();return t=void 0===t?null:t,e=e(),n.memoizedState=[e,t],e},useReducer:function(e,t,n){var r=_o();return t=void 0!==n?n(t):t,r.memoizedState=r.baseState=t,e={pending:null,interleaved:null,lanes:0,dispatch:null,lastRenderedReducer:e,lastRenderedState:t},r.queue=e,e=e.dispatch=ti.bind(null,mo,e),[r.memoizedState,e]},useRef:function(e){return e={current:e},_o().memoizedState=e},useState:Mo,useDebugValue:Go,useDeferredValue:function(e){return _o().memoizedState=e},useTransition:function(){var e=Mo(!1),t=e[0];return e=Xo.bind(null,e[1]),_o().memoizedState=e,[t,e]},useMutableSource:function(){},useSyncExternalStore:function(e,t,n){var r=mo,a=_o();if(al){if(void 0===n)throw Error(l(407));n=n()}else{if(n=t(),null===Ou)throw Error(l(349));0!==(30&ho)||Lo(r,t,n)}a.memoizedState=n;var o={value:n,getSnapshot:t};return a.queue=o,qo(Ao.bind(null,r,o,e),[e]),r.flags|=2048,Io(9,Ro.bind(null,r,o,n,t),void 0,null),n},useId:function(){var e=_o(),t=Ou.identifierPrefix;if(al){var n=Ya;t=":"+t+"R"+(n=(Za&~(1<<32-ot(Za)-1)).toString(32)+n),0<(n=wo++)&&(t+="H"+n.toString(32)),t+=":"}else t=":"+t+"r"+(n=So++).toString(32)+":";return e.memoizedState=t},unstable_isNewReconciler:!1},ui={readContext:Cl,useCallback:Zo,useContext:Cl,useEffect:Ho,useImperativeHandle:Ko,useInsertionEffect:$o,useLayoutEffect:Wo,useMemo:Yo,useReducer:No,useRef:Uo,useState:function(){return No(Po)},useDebugValue:Go,useDeferredValue:function(e){return Jo(jo(),vo.memoizedState,e)},useTransition:function(){return[No(Po)[0],jo().memoizedState]},useMutableSource:zo,useSyncExternalStore:To,useId:ei,unstable_isNewReconciler:!1},si={readContext:Cl,useCallback:Zo,useContext:Cl,useEffect:Ho,useImperativeHandle:Ko,useInsertionEffect:$o,useLayoutEffect:Wo,useMemo:Yo,useReducer:Oo,useRef:Uo,useState:function(){return Oo(Po)},useDebugValue:Go,useDeferredValue:function(e){var t=jo();return null===vo?t.memoizedState=e:Jo(t,vo.memoizedState,e)},useTransition:function(){return[Oo(Po)[0],jo().memoizedState]},useMutableSource:zo,useSyncExternalStore:To,useId:ei,unstable_isNewReconciler:!1};function ci(e,t){try{var n="",r=t;do{n+=B(r),r=r.return}while(r);var a=n}catch(l){a="\nError generating stack: "+l.message+"\n"+l.stack}return{value:e,source:t,stack:a,digest:null}}function fi(e,t,n){return{value:e,source:null,stack:null!=n?n:null,digest:null!=t?t:null}}function di(e,t){try{console.error(t.value)}catch(n){setTimeout((function(){throw n}))}}var pi="function"===typeof WeakMap?WeakMap:Map;function hi(e,t,n){(n=Ll(-1,n)).tag=3,n.payload={element:null};var r=t.value;return n.callback=function(){$u||($u=!0,Wu=r),di(0,t)},n}function mi(e,t,n){(n=Ll(-1,n)).tag=3;var r=e.type.getDerivedStateFromError;if("function"===typeof r){var a=t.value;n.payload=function(){return r(a)},n.callback=function(){di(0,t)}}var l=e.stateNode;return null!==l&&"function"===typeof l.componentDidCatch&&(n.callback=function(){di(0,t),"function"!==typeof r&&(null===Qu?Qu=new Set([this]):Qu.add(this));var e=t.stack;this.componentDidCatch(t.value,{componentStack:null!==e?e:""})}),n}function vi(e,t,n){var r=e.pingCache;if(null===r){r=e.pingCache=new pi;var a=new Set;r.set(t,a)}else void 0===(a=r.get(t))&&(a=new Set,r.set(t,a));a.has(n)||(a.add(n),e=_s.bind(null,e,t,n),t.then(e,e))}function gi(e){do{var t;if((t=13===e.tag)&&(t=null===(t=e.memoizedState)||null!==t.dehydrated),t)return e;e=e.return}while(null!==e);return null}function yi(e,t,n,r,a){return 0===(1&e.mode)?(e===t?e.flags|=65536:(e.flags|=128,n.flags|=131072,n.flags&=-52805,1===n.tag&&(null===n.alternate?n.tag=17:((t=Ll(-1,1)).tag=2,Rl(n,t,1))),n.lanes|=1),e):(e.flags|=65536,e.lanes=a,e)}var bi=w.ReactCurrentOwner,wi=!1;function Si(e,t,n,r){t.child=null===e?Yl(t,null,n,r):Zl(t,e.child,n,r)}function xi(e,t,n,r,a){n=n.render;var l=t.ref;return El(t,a),r=Eo(e,t,n,r,l,a),n=Co(),null===e||wi?(al&&n&&el(t),t.flags|=1,Si(e,t,r,a),t.child):(t.updateQueue=e.updateQueue,t.flags&=-2053,e.lanes&=~a,$i(e,t,a))}function ki(e,t,n,r,a){if(null===e){var l=n.type;return"function"!==typeof l||Ls(l)||void 0!==l.defaultProps||null!==n.compare||void 0!==n.defaultProps?((e=As(n.type,null,r,t,t.mode,a)).ref=t.ref,e.return=t,t.child=e):(t.tag=15,t.type=l,Ei(e,t,l,r,a))}if(l=e.child,0===(e.lanes&a)){var o=l.memoizedProps;if((n=null!==(n=n.compare)?n:ur)(o,r)&&e.ref===t.ref)return $i(e,t,a)}return t.flags|=1,(e=Rs(l,r)).ref=t.ref,e.return=t,t.child=e}function Ei(e,t,n,r,a){if(null!==e){var l=e.memoizedProps;if(ur(l,r)&&e.ref===t.ref){if(wi=!1,t.pendingProps=r=l,0===(e.lanes&a))return t.lanes=e.lanes,$i(e,t,a);0!==(131072&e.flags)&&(wi=!0)}}return ji(e,t,n,r,a)}function Ci(e,t,n){var r=t.pendingProps,a=r.children,l=null!==e?e.memoizedState:null;if("hidden"===r.mode)if(0===(1&t.mode))t.memoizedState={baseLanes:0,cachePool:null,transitions:null},_a(Ru,Lu),Lu|=n;else{if(0===(1073741824&n))return e=null!==l?l.baseLanes|n:n,t.lanes=t.childLanes=1073741824,t.memoizedState={baseLanes:e,cachePool:null,transitions:null},t.updateQueue=null,_a(Ru,Lu),Lu|=e,null;t.memoizedState={baseLanes:0,cachePool:null,transitions:null},r=null!==l?l.baseLanes:n,_a(Ru,Lu),Lu|=r}else null!==l?(r=l.baseLanes|n,t.memoizedState=null):r=n,_a(Ru,Lu),Lu|=r;return Si(e,t,a,n),t.child}function _i(e,t){var n=t.ref;(null===e&&null!==n||null!==e&&e.ref!==n)&&(t.flags|=512,t.flags|=2097152)}function ji(e,t,n,r,a){var l=Ta(n)?Oa:Pa.current;return l=za(t,l),El(t,a),n=Eo(e,t,n,r,l,a),r=Co(),null===e||wi?(al&&r&&el(t),t.flags|=1,Si(e,t,n,a),t.child):(t.updateQueue=e.updateQueue,t.flags&=-2053,e.lanes&=~a,$i(e,t,a))}function Pi(e,t,n,r,a){if(Ta(n)){var l=!0;Fa(t)}else l=!1;if(El(t,a),null===t.stateNode)Hi(e,t),ql(t,n,r),$l(t,n,r,a),r=!0;else if(null===e){var o=t.stateNode,i=t.memoizedProps;o.props=i;var u=o.context,s=n.contextType;"object"===typeof s&&null!==s?s=Cl(s):s=za(t,s=Ta(n)?Oa:Pa.current);var c=n.getDerivedStateFromProps,f="function"===typeof c||"function"===typeof o.getSnapshotBeforeUpdate;f||"function"!==typeof o.UNSAFE_componentWillReceiveProps&&"function"!==typeof o.componentWillReceiveProps||(i!==r||u!==s)&&Hl(t,o,r,s),Ol=!1;var d=t.memoizedState;o.state=d,Dl(t,r,o,a),u=t.memoizedState,i!==r||d!==u||Na.current||Ol?("function"===typeof c&&(Ul(t,n,c,r),u=t.memoizedState),(i=Ol||Vl(t,n,i,r,d,u,s))?(f||"function"!==typeof o.UNSAFE_componentWillMount&&"function"!==typeof o.componentWillMount||("function"===typeof o.componentWillMount&&o.componentWillMount(),"function"===typeof o.UNSAFE_componentWillMount&&o.UNSAFE_componentWillMount()),"function"===typeof o.componentDidMount&&(t.flags|=4194308)):("function"===typeof o.componentDidMount&&(t.flags|=4194308),t.memoizedProps=r,t.memoizedState=u),o.props=r,o.state=u,o.context=s,r=i):("function"===typeof o.componentDidMount&&(t.flags|=4194308),r=!1)}else{o=t.stateNode,Tl(e,t),i=t.memoizedProps,s=t.type===t.elementType?i:vl(t.type,i),o.props=s,f=t.pendingProps,d=o.context,"object"===typeof(u=n.contextType)&&null!==u?u=Cl(u):u=za(t,u=Ta(n)?Oa:Pa.current);var p=n.getDerivedStateFromProps;(c="function"===typeof p||"function"===typeof o.getSnapshotBeforeUpdate)||"function"!==typeof o.UNSAFE_componentWillReceiveProps&&"function"!==typeof o.componentWillReceiveProps||(i!==f||d!==u)&&Hl(t,o,r,u),Ol=!1,d=t.memoizedState,o.state=d,Dl(t,r,o,a);var h=t.memoizedState;i!==f||d!==h||Na.current||Ol?("function"===typeof p&&(Ul(t,n,p,r),h=t.memoizedState),(s=Ol||Vl(t,n,s,r,d,h,u)||!1)?(c||"function"!==typeof o.UNSAFE_componentWillUpdate&&"function"!==typeof o.componentWillUpdate||("function"===typeof o.componentWillUpdate&&o.componentWillUpdate(r,h,u),"function"===typeof o.UNSAFE_componentWillUpdate&&o.UNSAFE_componentWillUpdate(r,h,u)),"function"===typeof o.componentDidUpdate&&(t.flags|=4),"function"===typeof o.getSnapshotBeforeUpdate&&(t.flags|=1024)):("function"!==typeof o.componentDidUpdate||i===e.memoizedProps&&d===e.memoizedState||(t.flags|=4),"function"!==typeof o.getSnapshotBeforeUpdate||i===e.memoizedProps&&d===e.memoizedState||(t.flags|=1024),t.memoizedProps=r,t.memoizedState=h),o.props=r,o.state=h,o.context=u,r=s):("function"!==typeof o.componentDidUpdate||i===e.memoizedProps&&d===e.memoizedState||(t.flags|=4),"function"!==typeof o.getSnapshotBeforeUpdate||i===e.memoizedProps&&d===e.memoizedState||(t.flags|=1024),r=!1)}return Ni(e,t,n,r,l,a)}function Ni(e,t,n,r,a,l){_i(e,t);var o=0!==(128&t.flags);if(!r&&!o)return a&&Da(t,n,!1),$i(e,t,l);r=t.stateNode,bi.current=t;var i=o&&"function"!==typeof n.getDerivedStateFromError?null:r.render();return t.flags|=1,null!==e&&o?(t.child=Zl(t,e.child,null,l),t.child=Zl(t,null,i,l)):Si(e,t,i,l),t.memoizedState=r.state,a&&Da(t,n,!0),t.child}function Oi(e){var t=e.stateNode;t.pendingContext?Ra(0,t.pendingContext,t.pendingContext!==t.context):t.context&&Ra(0,t.context,!1),ro(e,t.containerInfo)}function zi(e,t,n,r,a){return pl(),hl(a),t.flags|=256,Si(e,t,n,r),t.child}var Ti,Li,Ri,Ai,Fi={dehydrated:null,treeContext:null,retryLane:0};function Di(e){return{baseLanes:e,cachePool:null,transitions:null}}function Mi(e,t,n){var r,a=t.pendingProps,o=io.current,i=!1,u=0!==(128&t.flags);if((r=u)||(r=(null===e||null!==e.memoizedState)&&0!==(2&o)),r?(i=!0,t.flags&=-129):null!==e&&null===e.memoizedState||(o|=1),_a(io,1&o),null===e)return sl(t),null!==(e=t.memoizedState)&&null!==(e=e.dehydrated)?(0===(1&t.mode)?t.lanes=1:"$!"===e.data?t.lanes=8:t.lanes=1073741824,null):(u=a.children,e=a.fallback,i?(a=t.mode,i=t.child,u={mode:"hidden",children:u},0===(1&a)&&null!==i?(i.childLanes=0,i.pendingProps=u):i=Ds(u,a,0,null),e=Fs(e,a,n,null),i.return=t,e.return=t,i.sibling=e,t.child=i,t.child.memoizedState=Di(n),t.memoizedState=Fi,e):Ii(t,u));if(null!==(o=e.memoizedState)&&null!==(r=o.dehydrated))return function(e,t,n,r,a,o,i){if(n)return 256&t.flags?(t.flags&=-257,Ui(e,t,i,r=fi(Error(l(422))))):null!==t.memoizedState?(t.child=e.child,t.flags|=128,null):(o=r.fallback,a=t.mode,r=Ds({mode:"visible",children:r.children},a,0,null),(o=Fs(o,a,i,null)).flags|=2,r.return=t,o.return=t,r.sibling=o,t.child=r,0!==(1&t.mode)&&Zl(t,e.child,null,i),t.child.memoizedState=Di(i),t.memoizedState=Fi,o);if(0===(1&t.mode))return Ui(e,t,i,null);if("$!"===a.data){if(r=a.nextSibling&&a.nextSibling.dataset)var u=r.dgst;return r=u,Ui(e,t,i,r=fi(o=Error(l(419)),r,void 0))}if(u=0!==(i&e.childLanes),wi||u){if(null!==(r=Ou)){switch(i&-i){case 4:a=2;break;case 16:a=8;break;case 64:case 128:case 256:case 512:case 1024:case 2048:case 4096:case 8192:case 16384:case 32768:case 65536:case 131072:case 262144:case 524288:case 1048576:case 2097152:case 4194304:case 8388608:case 16777216:case 33554432:case 67108864:a=32;break;case 536870912:a=268435456;break;default:a=0}0!==(a=0!==(a&(r.suspendedLanes|i))?0:a)&&a!==o.retryLane&&(o.retryLane=a,Nl(e,a),rs(r,e,a,-1))}return vs(),Ui(e,t,i,r=fi(Error(l(421))))}return"$?"===a.data?(t.flags|=128,t.child=e.child,t=Ps.bind(null,e),a._reactRetry=t,null):(e=o.treeContext,rl=sa(a.nextSibling),nl=t,al=!0,ll=null,null!==e&&(Qa[Ka++]=Za,Qa[Ka++]=Ya,Qa[Ka++]=Ga,Za=e.id,Ya=e.overflow,Ga=t),t=Ii(t,r.children),t.flags|=4096,t)}(e,t,u,a,r,o,n);if(i){i=a.fallback,u=t.mode,r=(o=e.child).sibling;var s={mode:"hidden",children:a.children};return 0===(1&u)&&t.child!==o?((a=t.child).childLanes=0,a.pendingProps=s,t.deletions=null):(a=Rs(o,s)).subtreeFlags=14680064&o.subtreeFlags,null!==r?i=Rs(r,i):(i=Fs(i,u,n,null)).flags|=2,i.return=t,a.return=t,a.sibling=i,t.child=a,a=i,i=t.child,u=null===(u=e.child.memoizedState)?Di(n):{baseLanes:u.baseLanes|n,cachePool:null,transitions:u.transitions},i.memoizedState=u,i.childLanes=e.childLanes&~n,t.memoizedState=Fi,a}return e=(i=e.child).sibling,a=Rs(i,{mode:"visible",children:a.children}),0===(1&t.mode)&&(a.lanes=n),a.return=t,a.sibling=null,null!==e&&(null===(n=t.deletions)?(t.deletions=[e],t.flags|=16):n.push(e)),t.child=a,t.memoizedState=null,a}function Ii(e,t){return(t=Ds({mode:"visible",children:t},e.mode,0,null)).return=e,e.child=t}function Ui(e,t,n,r){return null!==r&&hl(r),Zl(t,e.child,null,n),(e=Ii(t,t.pendingProps.children)).flags|=2,t.memoizedState=null,e}function Bi(e,t,n){e.lanes|=t;var r=e.alternate;null!==r&&(r.lanes|=t),kl(e.return,t,n)}function Vi(e,t,n,r,a){var l=e.memoizedState;null===l?e.memoizedState={isBackwards:t,rendering:null,renderingStartTime:0,last:r,tail:n,tailMode:a}:(l.isBackwards=t,l.rendering=null,l.renderingStartTime=0,l.last=r,l.tail=n,l.tailMode=a)}function qi(e,t,n){var r=t.pendingProps,a=r.revealOrder,l=r.tail;if(Si(e,t,r.children,n),0!==(2&(r=io.current)))r=1&r|2,t.flags|=128;else{if(null!==e&&0!==(128&e.flags))e:for(e=t.child;null!==e;){if(13===e.tag)null!==e.memoizedState&&Bi(e,n,t);else if(19===e.tag)Bi(e,n,t);else if(null!==e.child){e.child.return=e,e=e.child;continue}if(e===t)break e;for(;null===e.sibling;){if(null===e.return||e.return===t)break e;e=e.return}e.sibling.return=e.return,e=e.sibling}r&=1}if(_a(io,r),0===(1&t.mode))t.memoizedState=null;else switch(a){case"forwards":for(n=t.child,a=null;null!==n;)null!==(e=n.alternate)&&null===uo(e)&&(a=n),n=n.sibling;null===(n=a)?(a=t.child,t.child=null):(a=n.sibling,n.sibling=null),Vi(t,!1,a,n,l);break;case"backwards":for(n=null,a=t.child,t.child=null;null!==a;){if(null!==(e=a.alternate)&&null===uo(e)){t.child=a;break}e=a.sibling,a.sibling=n,n=a,a=e}Vi(t,!0,n,null,l);break;case"together":Vi(t,!1,null,null,void 0);break;default:t.memoizedState=null}return t.child}function Hi(e,t){0===(1&t.mode)&&null!==e&&(e.alternate=null,t.alternate=null,t.flags|=2)}function $i(e,t,n){if(null!==e&&(t.dependencies=e.dependencies),Du|=t.lanes,0===(n&t.childLanes))return null;if(null!==e&&t.child!==e.child)throw Error(l(153));if(null!==t.child){for(n=Rs(e=t.child,e.pendingProps),t.child=n,n.return=t;null!==e.sibling;)e=e.sibling,(n=n.sibling=Rs(e,e.pendingProps)).return=t;n.sibling=null}return t.child}function Wi(e,t){if(!al)switch(e.tailMode){case"hidden":t=e.tail;for(var n=null;null!==t;)null!==t.alternate&&(n=t),t=t.sibling;null===n?e.tail=null:n.sibling=null;break;case"collapsed":n=e.tail;for(var r=null;null!==n;)null!==n.alternate&&(r=n),n=n.sibling;null===r?t||null===e.tail?e.tail=null:e.tail.sibling=null:r.sibling=null}}function Qi(e){var t=null!==e.alternate&&e.alternate.child===e.child,n=0,r=0;if(t)for(var a=e.child;null!==a;)n|=a.lanes|a.childLanes,r|=14680064&a.subtreeFlags,r|=14680064&a.flags,a.return=e,a=a.sibling;else for(a=e.child;null!==a;)n|=a.lanes|a.childLanes,r|=a.subtreeFlags,r|=a.flags,a.return=e,a=a.sibling;return e.subtreeFlags|=r,e.childLanes=n,t}function Ki(e,t,n){var r=t.pendingProps;switch(tl(t),t.tag){case 2:case 16:case 15:case 0:case 11:case 7:case 8:case 12:case 9:case 14:return Qi(t),null;case 1:case 17:return Ta(t.type)&&La(),Qi(t),null;case 3:return r=t.stateNode,ao(),Ca(Na),Ca(Pa),co(),r.pendingContext&&(r.context=r.pendingContext,r.pendingContext=null),null!==e&&null!==e.child||(fl(t)?t.flags|=4:null===e||e.memoizedState.isDehydrated&&0===(256&t.flags)||(t.flags|=1024,null!==ll&&(is(ll),ll=null))),Li(e,t),Qi(t),null;case 5:oo(t);var a=no(to.current);if(n=t.type,null!==e&&null!=t.stateNode)Ri(e,t,n,r,a),e.ref!==t.ref&&(t.flags|=512,t.flags|=2097152);else{if(!r){if(null===t.stateNode)throw Error(l(166));return Qi(t),null}if(e=no(Xl.current),fl(t)){r=t.stateNode,n=t.type;var o=t.memoizedProps;switch(r[da]=t,r[pa]=o,e=0!==(1&t.mode),n){case"dialog":Ir("cancel",r),Ir("close",r);break;case"iframe":case"object":case"embed":Ir("load",r);break;case"video":case"audio":for(a=0;a<Ar.length;a++)Ir(Ar[a],r);break;case"source":Ir("error",r);break;case"img":case"image":case"link":Ir("error",r),Ir("load",r);break;case"details":Ir("toggle",r);break;case"input":Z(r,o),Ir("invalid",r);break;case"select":r._wrapperState={wasMultiple:!!o.multiple},Ir("invalid",r);break;case"textarea":ae(r,o),Ir("invalid",r)}for(var u in ye(n,o),a=null,o)if(o.hasOwnProperty(u)){var s=o[u];"children"===u?"string"===typeof s?r.textContent!==s&&(!0!==o.suppressHydrationWarning&&Jr(r.textContent,s,e),a=["children",s]):"number"===typeof s&&r.textContent!==""+s&&(!0!==o.suppressHydrationWarning&&Jr(r.textContent,s,e),a=["children",""+s]):i.hasOwnProperty(u)&&null!=s&&"onScroll"===u&&Ir("scroll",r)}switch(n){case"input":W(r),X(r,o,!0);break;case"textarea":W(r),oe(r);break;case"select":case"option":break;default:"function"===typeof o.onClick&&(r.onclick=Xr)}r=a,t.updateQueue=r,null!==r&&(t.flags|=4)}else{u=9===a.nodeType?a:a.ownerDocument,"http://www.w3.org/1999/xhtml"===e&&(e=ie(n)),"http://www.w3.org/1999/xhtml"===e?"script"===n?((e=u.createElement("div")).innerHTML="<script><\/script>",e=e.removeChild(e.firstChild)):"string"===typeof r.is?e=u.createElement(n,{is:r.is}):(e=u.createElement(n),"select"===n&&(u=e,r.multiple?u.multiple=!0:r.size&&(u.size=r.size))):e=u.createElementNS(e,n),e[da]=t,e[pa]=r,Ti(e,t,!1,!1),t.stateNode=e;e:{switch(u=be(n,r),n){case"dialog":Ir("cancel",e),Ir("close",e),a=r;break;case"iframe":case"object":case"embed":Ir("load",e),a=r;break;case"video":case"audio":for(a=0;a<Ar.length;a++)Ir(Ar[a],e);a=r;break;case"source":Ir("error",e),a=r;break;case"img":case"image":case"link":Ir("error",e),Ir("load",e),a=r;break;case"details":Ir("toggle",e),a=r;break;case"input":Z(e,r),a=G(e,r),Ir("invalid",e);break;case"option":default:a=r;break;case"select":e._wrapperState={wasMultiple:!!r.multiple},a=D({},r,{value:void 0}),Ir("invalid",e);break;case"textarea":ae(e,r),a=re(e,r),Ir("invalid",e)}for(o in ye(n,a),s=a)if(s.hasOwnProperty(o)){var c=s[o];"style"===o?ve(e,c):"dangerouslySetInnerHTML"===o?null!=(c=c?c.__html:void 0)&&fe(e,c):"children"===o?"string"===typeof c?("textarea"!==n||""!==c)&&de(e,c):"number"===typeof c&&de(e,""+c):"suppressContentEditableWarning"!==o&&"suppressHydrationWarning"!==o&&"autoFocus"!==o&&(i.hasOwnProperty(o)?null!=c&&"onScroll"===o&&Ir("scroll",e):null!=c&&b(e,o,c,u))}switch(n){case"input":W(e),X(e,r,!1);break;case"textarea":W(e),oe(e);break;case"option":null!=r.value&&e.setAttribute("value",""+H(r.value));break;case"select":e.multiple=!!r.multiple,null!=(o=r.value)?ne(e,!!r.multiple,o,!1):null!=r.defaultValue&&ne(e,!!r.multiple,r.defaultValue,!0);break;default:"function"===typeof a.onClick&&(e.onclick=Xr)}switch(n){case"button":case"input":case"select":case"textarea":r=!!r.autoFocus;break e;case"img":r=!0;break e;default:r=!1}}r&&(t.flags|=4)}null!==t.ref&&(t.flags|=512,t.flags|=2097152)}return Qi(t),null;case 6:if(e&&null!=t.stateNode)Ai(e,t,e.memoizedProps,r);else{if("string"!==typeof r&&null===t.stateNode)throw Error(l(166));if(n=no(to.current),no(Xl.current),fl(t)){if(r=t.stateNode,n=t.memoizedProps,r[da]=t,(o=r.nodeValue!==n)&&null!==(e=nl))switch(e.tag){case 3:Jr(r.nodeValue,n,0!==(1&e.mode));break;case 5:!0!==e.memoizedProps.suppressHydrationWarning&&Jr(r.nodeValue,n,0!==(1&e.mode))}o&&(t.flags|=4)}else(r=(9===n.nodeType?n:n.ownerDocument).createTextNode(r))[da]=t,t.stateNode=r}return Qi(t),null;case 13:if(Ca(io),r=t.memoizedState,null===e||null!==e.memoizedState&&null!==e.memoizedState.dehydrated){if(al&&null!==rl&&0!==(1&t.mode)&&0===(128&t.flags))dl(),pl(),t.flags|=98560,o=!1;else if(o=fl(t),null!==r&&null!==r.dehydrated){if(null===e){if(!o)throw Error(l(318));if(!(o=null!==(o=t.memoizedState)?o.dehydrated:null))throw Error(l(317));o[da]=t}else pl(),0===(128&t.flags)&&(t.memoizedState=null),t.flags|=4;Qi(t),o=!1}else null!==ll&&(is(ll),ll=null),o=!0;if(!o)return 65536&t.flags?t:null}return 0!==(128&t.flags)?(t.lanes=n,t):((r=null!==r)!==(null!==e&&null!==e.memoizedState)&&r&&(t.child.flags|=8192,0!==(1&t.mode)&&(null===e||0!==(1&io.current)?0===Au&&(Au=3):vs())),null!==t.updateQueue&&(t.flags|=4),Qi(t),null);case 4:return ao(),Li(e,t),null===e&&Vr(t.stateNode.containerInfo),Qi(t),null;case 10:return xl(t.type._context),Qi(t),null;case 19:if(Ca(io),null===(o=t.memoizedState))return Qi(t),null;if(r=0!==(128&t.flags),null===(u=o.rendering))if(r)Wi(o,!1);else{if(0!==Au||null!==e&&0!==(128&e.flags))for(e=t.child;null!==e;){if(null!==(u=uo(e))){for(t.flags|=128,Wi(o,!1),null!==(r=u.updateQueue)&&(t.updateQueue=r,t.flags|=4),t.subtreeFlags=0,r=n,n=t.child;null!==n;)e=r,(o=n).flags&=14680066,null===(u=o.alternate)?(o.childLanes=0,o.lanes=e,o.child=null,o.subtreeFlags=0,o.memoizedProps=null,o.memoizedState=null,o.updateQueue=null,o.dependencies=null,o.stateNode=null):(o.childLanes=u.childLanes,o.lanes=u.lanes,o.child=u.child,o.subtreeFlags=0,o.deletions=null,o.memoizedProps=u.memoizedProps,o.memoizedState=u.memoizedState,o.updateQueue=u.updateQueue,o.type=u.type,e=u.dependencies,o.dependencies=null===e?null:{lanes:e.lanes,firstContext:e.firstContext}),n=n.sibling;return _a(io,1&io.current|2),t.child}e=e.sibling}null!==o.tail&&Ye()>qu&&(t.flags|=128,r=!0,Wi(o,!1),t.lanes=4194304)}else{if(!r)if(null!==(e=uo(u))){if(t.flags|=128,r=!0,null!==(n=e.updateQueue)&&(t.updateQueue=n,t.flags|=4),Wi(o,!0),null===o.tail&&"hidden"===o.tailMode&&!u.alternate&&!al)return Qi(t),null}else 2*Ye()-o.renderingStartTime>qu&&1073741824!==n&&(t.flags|=128,r=!0,Wi(o,!1),t.lanes=4194304);o.isBackwards?(u.sibling=t.child,t.child=u):(null!==(n=o.last)?n.sibling=u:t.child=u,o.last=u)}return null!==o.tail?(t=o.tail,o.rendering=t,o.tail=t.sibling,o.renderingStartTime=Ye(),t.sibling=null,n=io.current,_a(io,r?1&n|2:1&n),t):(Qi(t),null);case 22:case 23:return ds(),r=null!==t.memoizedState,null!==e&&null!==e.memoizedState!==r&&(t.flags|=8192),r&&0!==(1&t.mode)?0!==(1073741824&Lu)&&(Qi(t),6&t.subtreeFlags&&(t.flags|=8192)):Qi(t),null;case 24:case 25:return null}throw Error(l(156,t.tag))}function Gi(e,t){switch(tl(t),t.tag){case 1:return Ta(t.type)&&La(),65536&(e=t.flags)?(t.flags=-65537&e|128,t):null;case 3:return ao(),Ca(Na),Ca(Pa),co(),0!==(65536&(e=t.flags))&&0===(128&e)?(t.flags=-65537&e|128,t):null;case 5:return oo(t),null;case 13:if(Ca(io),null!==(e=t.memoizedState)&&null!==e.dehydrated){if(null===t.alternate)throw Error(l(340));pl()}return 65536&(e=t.flags)?(t.flags=-65537&e|128,t):null;case 19:return Ca(io),null;case 4:return ao(),null;case 10:return xl(t.type._context),null;case 22:case 23:return ds(),null;default:return null}}Ti=function(e,t){for(var n=t.child;null!==n;){if(5===n.tag||6===n.tag)e.appendChild(n.stateNode);else if(4!==n.tag&&null!==n.child){n.child.return=n,n=n.child;continue}if(n===t)break;for(;null===n.sibling;){if(null===n.return||n.return===t)return;n=n.return}n.sibling.return=n.return,n=n.sibling}},Li=function(){},Ri=function(e,t,n,r){var a=e.memoizedProps;if(a!==r){e=t.stateNode,no(Xl.current);var l,o=null;switch(n){case"input":a=G(e,a),r=G(e,r),o=[];break;case"select":a=D({},a,{value:void 0}),r=D({},r,{value:void 0}),o=[];break;case"textarea":a=re(e,a),r=re(e,r),o=[];break;default:"function"!==typeof a.onClick&&"function"===typeof r.onClick&&(e.onclick=Xr)}for(c in ye(n,r),n=null,a)if(!r.hasOwnProperty(c)&&a.hasOwnProperty(c)&&null!=a[c])if("style"===c){var u=a[c];for(l in u)u.hasOwnProperty(l)&&(n||(n={}),n[l]="")}else"dangerouslySetInnerHTML"!==c&&"children"!==c&&"suppressContentEditableWarning"!==c&&"suppressHydrationWarning"!==c&&"autoFocus"!==c&&(i.hasOwnProperty(c)?o||(o=[]):(o=o||[]).push(c,null));for(c in r){var s=r[c];if(u=null!=a?a[c]:void 0,r.hasOwnProperty(c)&&s!==u&&(null!=s||null!=u))if("style"===c)if(u){for(l in u)!u.hasOwnProperty(l)||s&&s.hasOwnProperty(l)||(n||(n={}),n[l]="");for(l in s)s.hasOwnProperty(l)&&u[l]!==s[l]&&(n||(n={}),n[l]=s[l])}else n||(o||(o=[]),o.push(c,n)),n=s;else"dangerouslySetInnerHTML"===c?(s=s?s.__html:void 0,u=u?u.__html:void 0,null!=s&&u!==s&&(o=o||[]).push(c,s)):"children"===c?"string"!==typeof s&&"number"!==typeof s||(o=o||[]).push(c,""+s):"suppressContentEditableWarning"!==c&&"suppressHydrationWarning"!==c&&(i.hasOwnProperty(c)?(null!=s&&"onScroll"===c&&Ir("scroll",e),o||u===s||(o=[])):(o=o||[]).push(c,s))}n&&(o=o||[]).push("style",n);var c=o;(t.updateQueue=c)&&(t.flags|=4)}},Ai=function(e,t,n,r){n!==r&&(t.flags|=4)};var Zi=!1,Yi=!1,Ji="function"===typeof WeakSet?WeakSet:Set,Xi=null;function eu(e,t){var n=e.ref;if(null!==n)if("function"===typeof n)try{n(null)}catch(r){Cs(e,t,r)}else n.current=null}function tu(e,t,n){try{n()}catch(r){Cs(e,t,r)}}var nu=!1;function ru(e,t,n){var r=t.updateQueue;if(null!==(r=null!==r?r.lastEffect:null)){var a=r=r.next;do{if((a.tag&e)===e){var l=a.destroy;a.destroy=void 0,void 0!==l&&tu(t,n,l)}a=a.next}while(a!==r)}}function au(e,t){if(null!==(t=null!==(t=t.updateQueue)?t.lastEffect:null)){var n=t=t.next;do{if((n.tag&e)===e){var r=n.create;n.destroy=r()}n=n.next}while(n!==t)}}function lu(e){var t=e.ref;if(null!==t){var n=e.stateNode;e.tag,e=n,"function"===typeof t?t(e):t.current=e}}function ou(e){var t=e.alternate;null!==t&&(e.alternate=null,ou(t)),e.child=null,e.deletions=null,e.sibling=null,5===e.tag&&(null!==(t=e.stateNode)&&(delete t[da],delete t[pa],delete t[ma],delete t[va],delete t[ga])),e.stateNode=null,e.return=null,e.dependencies=null,e.memoizedProps=null,e.memoizedState=null,e.pendingProps=null,e.stateNode=null,e.updateQueue=null}function iu(e){return 5===e.tag||3===e.tag||4===e.tag}function uu(e){e:for(;;){for(;null===e.sibling;){if(null===e.return||iu(e.return))return null;e=e.return}for(e.sibling.return=e.return,e=e.sibling;5!==e.tag&&6!==e.tag&&18!==e.tag;){if(2&e.flags)continue e;if(null===e.child||4===e.tag)continue e;e.child.return=e,e=e.child}if(!(2&e.flags))return e.stateNode}}function su(e,t,n){var r=e.tag;if(5===r||6===r)e=e.stateNode,t?8===n.nodeType?n.parentNode.insertBefore(e,t):n.insertBefore(e,t):(8===n.nodeType?(t=n.parentNode).insertBefore(e,n):(t=n).appendChild(e),null!==(n=n._reactRootContainer)&&void 0!==n||null!==t.onclick||(t.onclick=Xr));else if(4!==r&&null!==(e=e.child))for(su(e,t,n),e=e.sibling;null!==e;)su(e,t,n),e=e.sibling}function cu(e,t,n){var r=e.tag;if(5===r||6===r)e=e.stateNode,t?n.insertBefore(e,t):n.appendChild(e);else if(4!==r&&null!==(e=e.child))for(cu(e,t,n),e=e.sibling;null!==e;)cu(e,t,n),e=e.sibling}var fu=null,du=!1;function pu(e,t,n){for(n=n.child;null!==n;)hu(e,t,n),n=n.sibling}function hu(e,t,n){if(lt&&"function"===typeof lt.onCommitFiberUnmount)try{lt.onCommitFiberUnmount(at,n)}catch(i){}switch(n.tag){case 5:Yi||eu(n,t);case 6:var r=fu,a=du;fu=null,pu(e,t,n),du=a,null!==(fu=r)&&(du?(e=fu,n=n.stateNode,8===e.nodeType?e.parentNode.removeChild(n):e.removeChild(n)):fu.removeChild(n.stateNode));break;case 18:null!==fu&&(du?(e=fu,n=n.stateNode,8===e.nodeType?ua(e.parentNode,n):1===e.nodeType&&ua(e,n),Vt(e)):ua(fu,n.stateNode));break;case 4:r=fu,a=du,fu=n.stateNode.containerInfo,du=!0,pu(e,t,n),fu=r,du=a;break;case 0:case 11:case 14:case 15:if(!Yi&&(null!==(r=n.updateQueue)&&null!==(r=r.lastEffect))){a=r=r.next;do{var l=a,o=l.destroy;l=l.tag,void 0!==o&&(0!==(2&l)||0!==(4&l))&&tu(n,t,o),a=a.next}while(a!==r)}pu(e,t,n);break;case 1:if(!Yi&&(eu(n,t),"function"===typeof(r=n.stateNode).componentWillUnmount))try{r.props=n.memoizedProps,r.state=n.memoizedState,r.componentWillUnmount()}catch(i){Cs(n,t,i)}pu(e,t,n);break;case 21:pu(e,t,n);break;case 22:1&n.mode?(Yi=(r=Yi)||null!==n.memoizedState,pu(e,t,n),Yi=r):pu(e,t,n);break;default:pu(e,t,n)}}function mu(e){var t=e.updateQueue;if(null!==t){e.updateQueue=null;var n=e.stateNode;null===n&&(n=e.stateNode=new Ji),t.forEach((function(t){var r=Ns.bind(null,e,t);n.has(t)||(n.add(t),t.then(r,r))}))}}function vu(e,t){var n=t.deletions;if(null!==n)for(var r=0;r<n.length;r++){var a=n[r];try{var o=e,i=t,u=i;e:for(;null!==u;){switch(u.tag){case 5:fu=u.stateNode,du=!1;break e;case 3:case 4:fu=u.stateNode.containerInfo,du=!0;break e}u=u.return}if(null===fu)throw Error(l(160));hu(o,i,a),fu=null,du=!1;var s=a.alternate;null!==s&&(s.return=null),a.return=null}catch(c){Cs(a,t,c)}}if(12854&t.subtreeFlags)for(t=t.child;null!==t;)gu(t,e),t=t.sibling}function gu(e,t){var n=e.alternate,r=e.flags;switch(e.tag){case 0:case 11:case 14:case 15:if(vu(t,e),yu(e),4&r){try{ru(3,e,e.return),au(3,e)}catch(v){Cs(e,e.return,v)}try{ru(5,e,e.return)}catch(v){Cs(e,e.return,v)}}break;case 1:vu(t,e),yu(e),512&r&&null!==n&&eu(n,n.return);break;case 5:if(vu(t,e),yu(e),512&r&&null!==n&&eu(n,n.return),32&e.flags){var a=e.stateNode;try{de(a,"")}catch(v){Cs(e,e.return,v)}}if(4&r&&null!=(a=e.stateNode)){var o=e.memoizedProps,i=null!==n?n.memoizedProps:o,u=e.type,s=e.updateQueue;if(e.updateQueue=null,null!==s)try{"input"===u&&"radio"===o.type&&null!=o.name&&Y(a,o),be(u,i);var c=be(u,o);for(i=0;i<s.length;i+=2){var f=s[i],d=s[i+1];"style"===f?ve(a,d):"dangerouslySetInnerHTML"===f?fe(a,d):"children"===f?de(a,d):b(a,f,d,c)}switch(u){case"input":J(a,o);break;case"textarea":le(a,o);break;case"select":var p=a._wrapperState.wasMultiple;a._wrapperState.wasMultiple=!!o.multiple;var h=o.value;null!=h?ne(a,!!o.multiple,h,!1):p!==!!o.multiple&&(null!=o.defaultValue?ne(a,!!o.multiple,o.defaultValue,!0):ne(a,!!o.multiple,o.multiple?[]:"",!1))}a[pa]=o}catch(v){Cs(e,e.return,v)}}break;case 6:if(vu(t,e),yu(e),4&r){if(null===e.stateNode)throw Error(l(162));a=e.stateNode,o=e.memoizedProps;try{a.nodeValue=o}catch(v){Cs(e,e.return,v)}}break;case 3:if(vu(t,e),yu(e),4&r&&null!==n&&n.memoizedState.isDehydrated)try{Vt(t.containerInfo)}catch(v){Cs(e,e.return,v)}break;case 4:default:vu(t,e),yu(e);break;case 13:vu(t,e),yu(e),8192&(a=e.child).flags&&(o=null!==a.memoizedState,a.stateNode.isHidden=o,!o||null!==a.alternate&&null!==a.alternate.memoizedState||(Vu=Ye())),4&r&&mu(e);break;case 22:if(f=null!==n&&null!==n.memoizedState,1&e.mode?(Yi=(c=Yi)||f,vu(t,e),Yi=c):vu(t,e),yu(e),8192&r){if(c=null!==e.memoizedState,(e.stateNode.isHidden=c)&&!f&&0!==(1&e.mode))for(Xi=e,f=e.child;null!==f;){for(d=Xi=f;null!==Xi;){switch(h=(p=Xi).child,p.tag){case 0:case 11:case 14:case 15:ru(4,p,p.return);break;case 1:eu(p,p.return);var m=p.stateNode;if("function"===typeof m.componentWillUnmount){r=p,n=p.return;try{t=r,m.props=t.memoizedProps,m.state=t.memoizedState,m.componentWillUnmount()}catch(v){Cs(r,n,v)}}break;case 5:eu(p,p.return);break;case 22:if(null!==p.memoizedState){xu(d);continue}}null!==h?(h.return=p,Xi=h):xu(d)}f=f.sibling}e:for(f=null,d=e;;){if(5===d.tag){if(null===f){f=d;try{a=d.stateNode,c?"function"===typeof(o=a.style).setProperty?o.setProperty("display","none","important"):o.display="none":(u=d.stateNode,i=void 0!==(s=d.memoizedProps.style)&&null!==s&&s.hasOwnProperty("display")?s.display:null,u.style.display=me("display",i))}catch(v){Cs(e,e.return,v)}}}else if(6===d.tag){if(null===f)try{d.stateNode.nodeValue=c?"":d.memoizedProps}catch(v){Cs(e,e.return,v)}}else if((22!==d.tag&&23!==d.tag||null===d.memoizedState||d===e)&&null!==d.child){d.child.return=d,d=d.child;continue}if(d===e)break e;for(;null===d.sibling;){if(null===d.return||d.return===e)break e;f===d&&(f=null),d=d.return}f===d&&(f=null),d.sibling.return=d.return,d=d.sibling}}break;case 19:vu(t,e),yu(e),4&r&&mu(e);case 21:}}function yu(e){var t=e.flags;if(2&t){try{e:{for(var n=e.return;null!==n;){if(iu(n)){var r=n;break e}n=n.return}throw Error(l(160))}switch(r.tag){case 5:var a=r.stateNode;32&r.flags&&(de(a,""),r.flags&=-33),cu(e,uu(e),a);break;case 3:case 4:var o=r.stateNode.containerInfo;su(e,uu(e),o);break;default:throw Error(l(161))}}catch(i){Cs(e,e.return,i)}e.flags&=-3}4096&t&&(e.flags&=-4097)}function bu(e,t,n){Xi=e,wu(e,t,n)}function wu(e,t,n){for(var r=0!==(1&e.mode);null!==Xi;){var a=Xi,l=a.child;if(22===a.tag&&r){var o=null!==a.memoizedState||Zi;if(!o){var i=a.alternate,u=null!==i&&null!==i.memoizedState||Yi;i=Zi;var s=Yi;if(Zi=o,(Yi=u)&&!s)for(Xi=a;null!==Xi;)u=(o=Xi).child,22===o.tag&&null!==o.memoizedState?ku(a):null!==u?(u.return=o,Xi=u):ku(a);for(;null!==l;)Xi=l,wu(l,t,n),l=l.sibling;Xi=a,Zi=i,Yi=s}Su(e)}else 0!==(8772&a.subtreeFlags)&&null!==l?(l.return=a,Xi=l):Su(e)}}function Su(e){for(;null!==Xi;){var t=Xi;if(0!==(8772&t.flags)){var n=t.alternate;try{if(0!==(8772&t.flags))switch(t.tag){case 0:case 11:case 15:Yi||au(5,t);break;case 1:var r=t.stateNode;if(4&t.flags&&!Yi)if(null===n)r.componentDidMount();else{var a=t.elementType===t.type?n.memoizedProps:vl(t.type,n.memoizedProps);r.componentDidUpdate(a,n.memoizedState,r.__reactInternalSnapshotBeforeUpdate)}var o=t.updateQueue;null!==o&&Ml(t,o,r);break;case 3:var i=t.updateQueue;if(null!==i){if(n=null,null!==t.child)switch(t.child.tag){case 5:case 1:n=t.child.stateNode}Ml(t,i,n)}break;case 5:var u=t.stateNode;if(null===n&&4&t.flags){n=u;var s=t.memoizedProps;switch(t.type){case"button":case"input":case"select":case"textarea":s.autoFocus&&n.focus();break;case"img":s.src&&(n.src=s.src)}}break;case 6:case 4:case 12:case 19:case 17:case 21:case 22:case 23:case 25:break;case 13:if(null===t.memoizedState){var c=t.alternate;if(null!==c){var f=c.memoizedState;if(null!==f){var d=f.dehydrated;null!==d&&Vt(d)}}}break;default:throw Error(l(163))}Yi||512&t.flags&&lu(t)}catch(p){Cs(t,t.return,p)}}if(t===e){Xi=null;break}if(null!==(n=t.sibling)){n.return=t.return,Xi=n;break}Xi=t.return}}function xu(e){for(;null!==Xi;){var t=Xi;if(t===e){Xi=null;break}var n=t.sibling;if(null!==n){n.return=t.return,Xi=n;break}Xi=t.return}}function ku(e){for(;null!==Xi;){var t=Xi;try{switch(t.tag){case 0:case 11:case 15:var n=t.return;try{au(4,t)}catch(u){Cs(t,n,u)}break;case 1:var r=t.stateNode;if("function"===typeof r.componentDidMount){var a=t.return;try{r.componentDidMount()}catch(u){Cs(t,a,u)}}var l=t.return;try{lu(t)}catch(u){Cs(t,l,u)}break;case 5:var o=t.return;try{lu(t)}catch(u){Cs(t,o,u)}}}catch(u){Cs(t,t.return,u)}if(t===e){Xi=null;break}var i=t.sibling;if(null!==i){i.return=t.return,Xi=i;break}Xi=t.return}}var Eu,Cu=Math.ceil,_u=w.ReactCurrentDispatcher,ju=w.ReactCurrentOwner,Pu=w.ReactCurrentBatchConfig,Nu=0,Ou=null,zu=null,Tu=0,Lu=0,Ru=Ea(0),Au=0,Fu=null,Du=0,Mu=0,Iu=0,Uu=null,Bu=null,Vu=0,qu=1/0,Hu=null,$u=!1,Wu=null,Qu=null,Ku=!1,Gu=null,Zu=0,Yu=0,Ju=null,Xu=-1,es=0;function ts(){return 0!==(6&Nu)?Ye():-1!==Xu?Xu:Xu=Ye()}function ns(e){return 0===(1&e.mode)?1:0!==(2&Nu)&&0!==Tu?Tu&-Tu:null!==ml.transition?(0===es&&(es=mt()),es):0!==(e=bt)?e:e=void 0===(e=window.event)?16:Zt(e.type)}function rs(e,t,n,r){if(50<Yu)throw Yu=0,Ju=null,Error(l(185));gt(e,n,r),0!==(2&Nu)&&e===Ou||(e===Ou&&(0===(2&Nu)&&(Mu|=n),4===Au&&us(e,Tu)),as(e,r),1===n&&0===Nu&&0===(1&t.mode)&&(qu=Ye()+500,Ia&&Va()))}function as(e,t){var n=e.callbackNode;!function(e,t){for(var n=e.suspendedLanes,r=e.pingedLanes,a=e.expirationTimes,l=e.pendingLanes;0<l;){var o=31-ot(l),i=1<<o,u=a[o];-1===u?0!==(i&n)&&0===(i&r)||(a[o]=pt(i,t)):u<=t&&(e.expiredLanes|=i),l&=~i}}(e,t);var r=dt(e,e===Ou?Tu:0);if(0===r)null!==n&&Ke(n),e.callbackNode=null,e.callbackPriority=0;else if(t=r&-r,e.callbackPriority!==t){if(null!=n&&Ke(n),1===t)0===e.tag?function(e){Ia=!0,Ba(e)}(ss.bind(null,e)):Ba(ss.bind(null,e)),oa((function(){0===(6&Nu)&&Va()})),n=null;else{switch(wt(r)){case 1:n=Xe;break;case 4:n=et;break;case 16:default:n=tt;break;case 536870912:n=rt}n=Os(n,ls.bind(null,e))}e.callbackPriority=t,e.callbackNode=n}}function ls(e,t){if(Xu=-1,es=0,0!==(6&Nu))throw Error(l(327));var n=e.callbackNode;if(ks()&&e.callbackNode!==n)return null;var r=dt(e,e===Ou?Tu:0);if(0===r)return null;if(0!==(30&r)||0!==(r&e.expiredLanes)||t)t=gs(e,r);else{t=r;var a=Nu;Nu|=2;var o=ms();for(Ou===e&&Tu===t||(Hu=null,qu=Ye()+500,ps(e,t));;)try{bs();break}catch(u){hs(e,u)}Sl(),_u.current=o,Nu=a,null!==zu?t=0:(Ou=null,Tu=0,t=Au)}if(0!==t){if(2===t&&(0!==(a=ht(e))&&(r=a,t=os(e,a))),1===t)throw n=Fu,ps(e,0),us(e,r),as(e,Ye()),n;if(6===t)us(e,r);else{if(a=e.current.alternate,0===(30&r)&&!function(e){for(var t=e;;){if(16384&t.flags){var n=t.updateQueue;if(null!==n&&null!==(n=n.stores))for(var r=0;r<n.length;r++){var a=n[r],l=a.getSnapshot;a=a.value;try{if(!ir(l(),a))return!1}catch(i){return!1}}}if(n=t.child,16384&t.subtreeFlags&&null!==n)n.return=t,t=n;else{if(t===e)break;for(;null===t.sibling;){if(null===t.return||t.return===e)return!0;t=t.return}t.sibling.return=t.return,t=t.sibling}}return!0}(a)&&(2===(t=gs(e,r))&&(0!==(o=ht(e))&&(r=o,t=os(e,o))),1===t))throw n=Fu,ps(e,0),us(e,r),as(e,Ye()),n;switch(e.finishedWork=a,e.finishedLanes=r,t){case 0:case 1:throw Error(l(345));case 2:case 5:xs(e,Bu,Hu);break;case 3:if(us(e,r),(130023424&r)===r&&10<(t=Vu+500-Ye())){if(0!==dt(e,0))break;if(((a=e.suspendedLanes)&r)!==r){ts(),e.pingedLanes|=e.suspendedLanes&a;break}e.timeoutHandle=ra(xs.bind(null,e,Bu,Hu),t);break}xs(e,Bu,Hu);break;case 4:if(us(e,r),(4194240&r)===r)break;for(t=e.eventTimes,a=-1;0<r;){var i=31-ot(r);o=1<<i,(i=t[i])>a&&(a=i),r&=~o}if(r=a,10<(r=(120>(r=Ye()-r)?120:480>r?480:1080>r?1080:1920>r?1920:3e3>r?3e3:4320>r?4320:1960*Cu(r/1960))-r)){e.timeoutHandle=ra(xs.bind(null,e,Bu,Hu),r);break}xs(e,Bu,Hu);break;default:throw Error(l(329))}}}return as(e,Ye()),e.callbackNode===n?ls.bind(null,e):null}function os(e,t){var n=Uu;return e.current.memoizedState.isDehydrated&&(ps(e,t).flags|=256),2!==(e=gs(e,t))&&(t=Bu,Bu=n,null!==t&&is(t)),e}function is(e){null===Bu?Bu=e:Bu.push.apply(Bu,e)}function us(e,t){for(t&=~Iu,t&=~Mu,e.suspendedLanes|=t,e.pingedLanes&=~t,e=e.expirationTimes;0<t;){var n=31-ot(t),r=1<<n;e[n]=-1,t&=~r}}function ss(e){if(0!==(6&Nu))throw Error(l(327));ks();var t=dt(e,0);if(0===(1&t))return as(e,Ye()),null;var n=gs(e,t);if(0!==e.tag&&2===n){var r=ht(e);0!==r&&(t=r,n=os(e,r))}if(1===n)throw n=Fu,ps(e,0),us(e,t),as(e,Ye()),n;if(6===n)throw Error(l(345));return e.finishedWork=e.current.alternate,e.finishedLanes=t,xs(e,Bu,Hu),as(e,Ye()),null}function cs(e,t){var n=Nu;Nu|=1;try{return e(t)}finally{0===(Nu=n)&&(qu=Ye()+500,Ia&&Va())}}function fs(e){null!==Gu&&0===Gu.tag&&0===(6&Nu)&&ks();var t=Nu;Nu|=1;var n=Pu.transition,r=bt;try{if(Pu.transition=null,bt=1,e)return e()}finally{bt=r,Pu.transition=n,0===(6&(Nu=t))&&Va()}}function ds(){Lu=Ru.current,Ca(Ru)}function ps(e,t){e.finishedWork=null,e.finishedLanes=0;var n=e.timeoutHandle;if(-1!==n&&(e.timeoutHandle=-1,aa(n)),null!==zu)for(n=zu.return;null!==n;){var r=n;switch(tl(r),r.tag){case 1:null!==(r=r.type.childContextTypes)&&void 0!==r&&La();break;case 3:ao(),Ca(Na),Ca(Pa),co();break;case 5:oo(r);break;case 4:ao();break;case 13:case 19:Ca(io);break;case 10:xl(r.type._context);break;case 22:case 23:ds()}n=n.return}if(Ou=e,zu=e=Rs(e.current,null),Tu=Lu=t,Au=0,Fu=null,Iu=Mu=Du=0,Bu=Uu=null,null!==_l){for(t=0;t<_l.length;t++)if(null!==(r=(n=_l[t]).interleaved)){n.interleaved=null;var a=r.next,l=n.pending;if(null!==l){var o=l.next;l.next=a,r.next=o}n.pending=r}_l=null}return e}function hs(e,t){for(;;){var n=zu;try{if(Sl(),fo.current=oi,yo){for(var r=mo.memoizedState;null!==r;){var a=r.queue;null!==a&&(a.pending=null),r=r.next}yo=!1}if(ho=0,go=vo=mo=null,bo=!1,wo=0,ju.current=null,null===n||null===n.return){Au=1,Fu=t,zu=null;break}e:{var o=e,i=n.return,u=n,s=t;if(t=Tu,u.flags|=32768,null!==s&&"object"===typeof s&&"function"===typeof s.then){var c=s,f=u,d=f.tag;if(0===(1&f.mode)&&(0===d||11===d||15===d)){var p=f.alternate;p?(f.updateQueue=p.updateQueue,f.memoizedState=p.memoizedState,f.lanes=p.lanes):(f.updateQueue=null,f.memoizedState=null)}var h=gi(i);if(null!==h){h.flags&=-257,yi(h,i,u,0,t),1&h.mode&&vi(o,c,t),s=c;var m=(t=h).updateQueue;if(null===m){var v=new Set;v.add(s),t.updateQueue=v}else m.add(s);break e}if(0===(1&t)){vi(o,c,t),vs();break e}s=Error(l(426))}else if(al&&1&u.mode){var g=gi(i);if(null!==g){0===(65536&g.flags)&&(g.flags|=256),yi(g,i,u,0,t),hl(ci(s,u));break e}}o=s=ci(s,u),4!==Au&&(Au=2),null===Uu?Uu=[o]:Uu.push(o),o=i;do{switch(o.tag){case 3:o.flags|=65536,t&=-t,o.lanes|=t,Fl(o,hi(0,s,t));break e;case 1:u=s;var y=o.type,b=o.stateNode;if(0===(128&o.flags)&&("function"===typeof y.getDerivedStateFromError||null!==b&&"function"===typeof b.componentDidCatch&&(null===Qu||!Qu.has(b)))){o.flags|=65536,t&=-t,o.lanes|=t,Fl(o,mi(o,u,t));break e}}o=o.return}while(null!==o)}Ss(n)}catch(w){t=w,zu===n&&null!==n&&(zu=n=n.return);continue}break}}function ms(){var e=_u.current;return _u.current=oi,null===e?oi:e}function vs(){0!==Au&&3!==Au&&2!==Au||(Au=4),null===Ou||0===(268435455&Du)&&0===(268435455&Mu)||us(Ou,Tu)}function gs(e,t){var n=Nu;Nu|=2;var r=ms();for(Ou===e&&Tu===t||(Hu=null,ps(e,t));;)try{ys();break}catch(a){hs(e,a)}if(Sl(),Nu=n,_u.current=r,null!==zu)throw Error(l(261));return Ou=null,Tu=0,Au}function ys(){for(;null!==zu;)ws(zu)}function bs(){for(;null!==zu&&!Ge();)ws(zu)}function ws(e){var t=Eu(e.alternate,e,Lu);e.memoizedProps=e.pendingProps,null===t?Ss(e):zu=t,ju.current=null}function Ss(e){var t=e;do{var n=t.alternate;if(e=t.return,0===(32768&t.flags)){if(null!==(n=Ki(n,t,Lu)))return void(zu=n)}else{if(null!==(n=Gi(n,t)))return n.flags&=32767,void(zu=n);if(null===e)return Au=6,void(zu=null);e.flags|=32768,e.subtreeFlags=0,e.deletions=null}if(null!==(t=t.sibling))return void(zu=t);zu=t=e}while(null!==t);0===Au&&(Au=5)}function xs(e,t,n){var r=bt,a=Pu.transition;try{Pu.transition=null,bt=1,function(e,t,n,r){do{ks()}while(null!==Gu);if(0!==(6&Nu))throw Error(l(327));n=e.finishedWork;var a=e.finishedLanes;if(null===n)return null;if(e.finishedWork=null,e.finishedLanes=0,n===e.current)throw Error(l(177));e.callbackNode=null,e.callbackPriority=0;var o=n.lanes|n.childLanes;if(function(e,t){var n=e.pendingLanes&~t;e.pendingLanes=t,e.suspendedLanes=0,e.pingedLanes=0,e.expiredLanes&=t,e.mutableReadLanes&=t,e.entangledLanes&=t,t=e.entanglements;var r=e.eventTimes;for(e=e.expirationTimes;0<n;){var a=31-ot(n),l=1<<a;t[a]=0,r[a]=-1,e[a]=-1,n&=~l}}(e,o),e===Ou&&(zu=Ou=null,Tu=0),0===(2064&n.subtreeFlags)&&0===(2064&n.flags)||Ku||(Ku=!0,Os(tt,(function(){return ks(),null}))),o=0!==(15990&n.flags),0!==(15990&n.subtreeFlags)||o){o=Pu.transition,Pu.transition=null;var i=bt;bt=1;var u=Nu;Nu|=4,ju.current=null,function(e,t){if(ea=Ht,pr(e=dr())){if("selectionStart"in e)var n={start:e.selectionStart,end:e.selectionEnd};else e:{var r=(n=(n=e.ownerDocument)&&n.defaultView||window).getSelection&&n.getSelection();if(r&&0!==r.rangeCount){n=r.anchorNode;var a=r.anchorOffset,o=r.focusNode;r=r.focusOffset;try{n.nodeType,o.nodeType}catch(S){n=null;break e}var i=0,u=-1,s=-1,c=0,f=0,d=e,p=null;t:for(;;){for(var h;d!==n||0!==a&&3!==d.nodeType||(u=i+a),d!==o||0!==r&&3!==d.nodeType||(s=i+r),3===d.nodeType&&(i+=d.nodeValue.length),null!==(h=d.firstChild);)p=d,d=h;for(;;){if(d===e)break t;if(p===n&&++c===a&&(u=i),p===o&&++f===r&&(s=i),null!==(h=d.nextSibling))break;p=(d=p).parentNode}d=h}n=-1===u||-1===s?null:{start:u,end:s}}else n=null}n=n||{start:0,end:0}}else n=null;for(ta={focusedElem:e,selectionRange:n},Ht=!1,Xi=t;null!==Xi;)if(e=(t=Xi).child,0!==(1028&t.subtreeFlags)&&null!==e)e.return=t,Xi=e;else for(;null!==Xi;){t=Xi;try{var m=t.alternate;if(0!==(1024&t.flags))switch(t.tag){case 0:case 11:case 15:case 5:case 6:case 4:case 17:break;case 1:if(null!==m){var v=m.memoizedProps,g=m.memoizedState,y=t.stateNode,b=y.getSnapshotBeforeUpdate(t.elementType===t.type?v:vl(t.type,v),g);y.__reactInternalSnapshotBeforeUpdate=b}break;case 3:var w=t.stateNode.containerInfo;1===w.nodeType?w.textContent="":9===w.nodeType&&w.documentElement&&w.removeChild(w.documentElement);break;default:throw Error(l(163))}}catch(S){Cs(t,t.return,S)}if(null!==(e=t.sibling)){e.return=t.return,Xi=e;break}Xi=t.return}m=nu,nu=!1}(e,n),gu(n,e),hr(ta),Ht=!!ea,ta=ea=null,e.current=n,bu(n,e,a),Ze(),Nu=u,bt=i,Pu.transition=o}else e.current=n;if(Ku&&(Ku=!1,Gu=e,Zu=a),o=e.pendingLanes,0===o&&(Qu=null),function(e){if(lt&&"function"===typeof lt.onCommitFiberRoot)try{lt.onCommitFiberRoot(at,e,void 0,128===(128&e.current.flags))}catch(t){}}(n.stateNode),as(e,Ye()),null!==t)for(r=e.onRecoverableError,n=0;n<t.length;n++)a=t[n],r(a.value,{componentStack:a.stack,digest:a.digest});if($u)throw $u=!1,e=Wu,Wu=null,e;0!==(1&Zu)&&0!==e.tag&&ks(),o=e.pendingLanes,0!==(1&o)?e===Ju?Yu++:(Yu=0,Ju=e):Yu=0,Va()}(e,t,n,r)}finally{Pu.transition=a,bt=r}return null}function ks(){if(null!==Gu){var e=wt(Zu),t=Pu.transition,n=bt;try{if(Pu.transition=null,bt=16>e?16:e,null===Gu)var r=!1;else{if(e=Gu,Gu=null,Zu=0,0!==(6&Nu))throw Error(l(331));var a=Nu;for(Nu|=4,Xi=e.current;null!==Xi;){var o=Xi,i=o.child;if(0!==(16&Xi.flags)){var u=o.deletions;if(null!==u){for(var s=0;s<u.length;s++){var c=u[s];for(Xi=c;null!==Xi;){var f=Xi;switch(f.tag){case 0:case 11:case 15:ru(8,f,o)}var d=f.child;if(null!==d)d.return=f,Xi=d;else for(;null!==Xi;){var p=(f=Xi).sibling,h=f.return;if(ou(f),f===c){Xi=null;break}if(null!==p){p.return=h,Xi=p;break}Xi=h}}}var m=o.alternate;if(null!==m){var v=m.child;if(null!==v){m.child=null;do{var g=v.sibling;v.sibling=null,v=g}while(null!==v)}}Xi=o}}if(0!==(2064&o.subtreeFlags)&&null!==i)i.return=o,Xi=i;else e:for(;null!==Xi;){if(0!==(2048&(o=Xi).flags))switch(o.tag){case 0:case 11:case 15:ru(9,o,o.return)}var y=o.sibling;if(null!==y){y.return=o.return,Xi=y;break e}Xi=o.return}}var b=e.current;for(Xi=b;null!==Xi;){var w=(i=Xi).child;if(0!==(2064&i.subtreeFlags)&&null!==w)w.return=i,Xi=w;else e:for(i=b;null!==Xi;){if(0!==(2048&(u=Xi).flags))try{switch(u.tag){case 0:case 11:case 15:au(9,u)}}catch(x){Cs(u,u.return,x)}if(u===i){Xi=null;break e}var S=u.sibling;if(null!==S){S.return=u.return,Xi=S;break e}Xi=u.return}}if(Nu=a,Va(),lt&&"function"===typeof lt.onPostCommitFiberRoot)try{lt.onPostCommitFiberRoot(at,e)}catch(x){}r=!0}return r}finally{bt=n,Pu.transition=t}}return!1}function Es(e,t,n){e=Rl(e,t=hi(0,t=ci(n,t),1),1),t=ts(),null!==e&&(gt(e,1,t),as(e,t))}function Cs(e,t,n){if(3===e.tag)Es(e,e,n);else for(;null!==t;){if(3===t.tag){Es(t,e,n);break}if(1===t.tag){var r=t.stateNode;if("function"===typeof t.type.getDerivedStateFromError||"function"===typeof r.componentDidCatch&&(null===Qu||!Qu.has(r))){t=Rl(t,e=mi(t,e=ci(n,e),1),1),e=ts(),null!==t&&(gt(t,1,e),as(t,e));break}}t=t.return}}function _s(e,t,n){var r=e.pingCache;null!==r&&r.delete(t),t=ts(),e.pingedLanes|=e.suspendedLanes&n,Ou===e&&(Tu&n)===n&&(4===Au||3===Au&&(130023424&Tu)===Tu&&500>Ye()-Vu?ps(e,0):Iu|=n),as(e,t)}function js(e,t){0===t&&(0===(1&e.mode)?t=1:(t=ct,0===(130023424&(ct<<=1))&&(ct=4194304)));var n=ts();null!==(e=Nl(e,t))&&(gt(e,t,n),as(e,n))}function Ps(e){var t=e.memoizedState,n=0;null!==t&&(n=t.retryLane),js(e,n)}function Ns(e,t){var n=0;switch(e.tag){case 13:var r=e.stateNode,a=e.memoizedState;null!==a&&(n=a.retryLane);break;case 19:r=e.stateNode;break;default:throw Error(l(314))}null!==r&&r.delete(t),js(e,n)}function Os(e,t){return Qe(e,t)}function zs(e,t,n,r){this.tag=e,this.key=n,this.sibling=this.child=this.return=this.stateNode=this.type=this.elementType=null,this.index=0,this.ref=null,this.pendingProps=t,this.dependencies=this.memoizedState=this.updateQueue=this.memoizedProps=null,this.mode=r,this.subtreeFlags=this.flags=0,this.deletions=null,this.childLanes=this.lanes=0,this.alternate=null}function Ts(e,t,n,r){return new zs(e,t,n,r)}function Ls(e){return!(!(e=e.prototype)||!e.isReactComponent)}function Rs(e,t){var n=e.alternate;return null===n?((n=Ts(e.tag,t,e.key,e.mode)).elementType=e.elementType,n.type=e.type,n.stateNode=e.stateNode,n.alternate=e,e.alternate=n):(n.pendingProps=t,n.type=e.type,n.flags=0,n.subtreeFlags=0,n.deletions=null),n.flags=14680064&e.flags,n.childLanes=e.childLanes,n.lanes=e.lanes,n.child=e.child,n.memoizedProps=e.memoizedProps,n.memoizedState=e.memoizedState,n.updateQueue=e.updateQueue,t=e.dependencies,n.dependencies=null===t?null:{lanes:t.lanes,firstContext:t.firstContext},n.sibling=e.sibling,n.index=e.index,n.ref=e.ref,n}function As(e,t,n,r,a,o){var i=2;if(r=e,"function"===typeof e)Ls(e)&&(i=1);else if("string"===typeof e)i=5;else e:switch(e){case k:return Fs(n.children,a,o,t);case E:i=8,a|=8;break;case C:return(e=Ts(12,n,t,2|a)).elementType=C,e.lanes=o,e;case N:return(e=Ts(13,n,t,a)).elementType=N,e.lanes=o,e;case O:return(e=Ts(19,n,t,a)).elementType=O,e.lanes=o,e;case L:return Ds(n,a,o,t);default:if("object"===typeof e&&null!==e)switch(e.$$typeof){case _:i=10;break e;case j:i=9;break e;case P:i=11;break e;case z:i=14;break e;case T:i=16,r=null;break e}throw Error(l(130,null==e?e:typeof e,""))}return(t=Ts(i,n,t,a)).elementType=e,t.type=r,t.lanes=o,t}function Fs(e,t,n,r){return(e=Ts(7,e,r,t)).lanes=n,e}function Ds(e,t,n,r){return(e=Ts(22,e,r,t)).elementType=L,e.lanes=n,e.stateNode={isHidden:!1},e}function Ms(e,t,n){return(e=Ts(6,e,null,t)).lanes=n,e}function Is(e,t,n){return(t=Ts(4,null!==e.children?e.children:[],e.key,t)).lanes=n,t.stateNode={containerInfo:e.containerInfo,pendingChildren:null,implementation:e.implementation},t}function Us(e,t,n,r,a){this.tag=t,this.containerInfo=e,this.finishedWork=this.pingCache=this.current=this.pendingChildren=null,this.timeoutHandle=-1,this.callbackNode=this.pendingContext=this.context=null,this.callbackPriority=0,this.eventTimes=vt(0),this.expirationTimes=vt(-1),this.entangledLanes=this.finishedLanes=this.mutableReadLanes=this.expiredLanes=this.pingedLanes=this.suspendedLanes=this.pendingLanes=0,this.entanglements=vt(0),this.identifierPrefix=r,this.onRecoverableError=a,this.mutableSourceEagerHydrationData=null}function Bs(e,t,n,r,a,l,o,i,u){return e=new Us(e,t,n,i,u),1===t?(t=1,!0===l&&(t|=8)):t=0,l=Ts(3,null,null,t),e.current=l,l.stateNode=e,l.memoizedState={element:r,isDehydrated:n,cache:null,transitions:null,pendingSuspenseBoundaries:null},zl(l),e}function Vs(e){if(!e)return ja;e:{if(Ve(e=e._reactInternals)!==e||1!==e.tag)throw Error(l(170));var t=e;do{switch(t.tag){case 3:t=t.stateNode.context;break e;case 1:if(Ta(t.type)){t=t.stateNode.__reactInternalMemoizedMergedChildContext;break e}}t=t.return}while(null!==t);throw Error(l(171))}if(1===e.tag){var n=e.type;if(Ta(n))return Aa(e,n,t)}return t}function qs(e,t,n,r,a,l,o,i,u){return(e=Bs(n,r,!0,e,0,l,0,i,u)).context=Vs(null),n=e.current,(l=Ll(r=ts(),a=ns(n))).callback=void 0!==t&&null!==t?t:null,Rl(n,l,a),e.current.lanes=a,gt(e,a,r),as(e,r),e}function Hs(e,t,n,r){var a=t.current,l=ts(),o=ns(a);return n=Vs(n),null===t.context?t.context=n:t.pendingContext=n,(t=Ll(l,o)).payload={element:e},null!==(r=void 0===r?null:r)&&(t.callback=r),null!==(e=Rl(a,t,o))&&(rs(e,a,o,l),Al(e,a,o)),o}function $s(e){return(e=e.current).child?(e.child.tag,e.child.stateNode):null}function Ws(e,t){if(null!==(e=e.memoizedState)&&null!==e.dehydrated){var n=e.retryLane;e.retryLane=0!==n&&n<t?n:t}}function Qs(e,t){Ws(e,t),(e=e.alternate)&&Ws(e,t)}Eu=function(e,t,n){if(null!==e)if(e.memoizedProps!==t.pendingProps||Na.current)wi=!0;else{if(0===(e.lanes&n)&&0===(128&t.flags))return wi=!1,function(e,t,n){switch(t.tag){case 3:Oi(t),pl();break;case 5:lo(t);break;case 1:Ta(t.type)&&Fa(t);break;case 4:ro(t,t.stateNode.containerInfo);break;case 10:var r=t.type._context,a=t.memoizedProps.value;_a(gl,r._currentValue),r._currentValue=a;break;case 13:if(null!==(r=t.memoizedState))return null!==r.dehydrated?(_a(io,1&io.current),t.flags|=128,null):0!==(n&t.child.childLanes)?Mi(e,t,n):(_a(io,1&io.current),null!==(e=$i(e,t,n))?e.sibling:null);_a(io,1&io.current);break;case 19:if(r=0!==(n&t.childLanes),0!==(128&e.flags)){if(r)return qi(e,t,n);t.flags|=128}if(null!==(a=t.memoizedState)&&(a.rendering=null,a.tail=null,a.lastEffect=null),_a(io,io.current),r)break;return null;case 22:case 23:return t.lanes=0,Ci(e,t,n)}return $i(e,t,n)}(e,t,n);wi=0!==(131072&e.flags)}else wi=!1,al&&0!==(1048576&t.flags)&&Xa(t,Wa,t.index);switch(t.lanes=0,t.tag){case 2:var r=t.type;Hi(e,t),e=t.pendingProps;var a=za(t,Pa.current);El(t,n),a=Eo(null,t,r,e,a,n);var o=Co();return t.flags|=1,"object"===typeof a&&null!==a&&"function"===typeof a.render&&void 0===a.$$typeof?(t.tag=1,t.memoizedState=null,t.updateQueue=null,Ta(r)?(o=!0,Fa(t)):o=!1,t.memoizedState=null!==a.state&&void 0!==a.state?a.state:null,zl(t),a.updater=Bl,t.stateNode=a,a._reactInternals=t,$l(t,r,e,n),t=Ni(null,t,r,!0,o,n)):(t.tag=0,al&&o&&el(t),Si(null,t,a,n),t=t.child),t;case 16:r=t.elementType;e:{switch(Hi(e,t),e=t.pendingProps,r=(a=r._init)(r._payload),t.type=r,a=t.tag=function(e){if("function"===typeof e)return Ls(e)?1:0;if(void 0!==e&&null!==e){if((e=e.$$typeof)===P)return 11;if(e===z)return 14}return 2}(r),e=vl(r,e),a){case 0:t=ji(null,t,r,e,n);break e;case 1:t=Pi(null,t,r,e,n);break e;case 11:t=xi(null,t,r,e,n);break e;case 14:t=ki(null,t,r,vl(r.type,e),n);break e}throw Error(l(306,r,""))}return t;case 0:return r=t.type,a=t.pendingProps,ji(e,t,r,a=t.elementType===r?a:vl(r,a),n);case 1:return r=t.type,a=t.pendingProps,Pi(e,t,r,a=t.elementType===r?a:vl(r,a),n);case 3:e:{if(Oi(t),null===e)throw Error(l(387));r=t.pendingProps,a=(o=t.memoizedState).element,Tl(e,t),Dl(t,r,null,n);var i=t.memoizedState;if(r=i.element,o.isDehydrated){if(o={element:r,isDehydrated:!1,cache:i.cache,pendingSuspenseBoundaries:i.pendingSuspenseBoundaries,transitions:i.transitions},t.updateQueue.baseState=o,t.memoizedState=o,256&t.flags){t=zi(e,t,r,n,a=ci(Error(l(423)),t));break e}if(r!==a){t=zi(e,t,r,n,a=ci(Error(l(424)),t));break e}for(rl=sa(t.stateNode.containerInfo.firstChild),nl=t,al=!0,ll=null,n=Yl(t,null,r,n),t.child=n;n;)n.flags=-3&n.flags|4096,n=n.sibling}else{if(pl(),r===a){t=$i(e,t,n);break e}Si(e,t,r,n)}t=t.child}return t;case 5:return lo(t),null===e&&sl(t),r=t.type,a=t.pendingProps,o=null!==e?e.memoizedProps:null,i=a.children,na(r,a)?i=null:null!==o&&na(r,o)&&(t.flags|=32),_i(e,t),Si(e,t,i,n),t.child;case 6:return null===e&&sl(t),null;case 13:return Mi(e,t,n);case 4:return ro(t,t.stateNode.containerInfo),r=t.pendingProps,null===e?t.child=Zl(t,null,r,n):Si(e,t,r,n),t.child;case 11:return r=t.type,a=t.pendingProps,xi(e,t,r,a=t.elementType===r?a:vl(r,a),n);case 7:return Si(e,t,t.pendingProps,n),t.child;case 8:case 12:return Si(e,t,t.pendingProps.children,n),t.child;case 10:e:{if(r=t.type._context,a=t.pendingProps,o=t.memoizedProps,i=a.value,_a(gl,r._currentValue),r._currentValue=i,null!==o)if(ir(o.value,i)){if(o.children===a.children&&!Na.current){t=$i(e,t,n);break e}}else for(null!==(o=t.child)&&(o.return=t);null!==o;){var u=o.dependencies;if(null!==u){i=o.child;for(var s=u.firstContext;null!==s;){if(s.context===r){if(1===o.tag){(s=Ll(-1,n&-n)).tag=2;var c=o.updateQueue;if(null!==c){var f=(c=c.shared).pending;null===f?s.next=s:(s.next=f.next,f.next=s),c.pending=s}}o.lanes|=n,null!==(s=o.alternate)&&(s.lanes|=n),kl(o.return,n,t),u.lanes|=n;break}s=s.next}}else if(10===o.tag)i=o.type===t.type?null:o.child;else if(18===o.tag){if(null===(i=o.return))throw Error(l(341));i.lanes|=n,null!==(u=i.alternate)&&(u.lanes|=n),kl(i,n,t),i=o.sibling}else i=o.child;if(null!==i)i.return=o;else for(i=o;null!==i;){if(i===t){i=null;break}if(null!==(o=i.sibling)){o.return=i.return,i=o;break}i=i.return}o=i}Si(e,t,a.children,n),t=t.child}return t;case 9:return a=t.type,r=t.pendingProps.children,El(t,n),r=r(a=Cl(a)),t.flags|=1,Si(e,t,r,n),t.child;case 14:return a=vl(r=t.type,t.pendingProps),ki(e,t,r,a=vl(r.type,a),n);case 15:return Ei(e,t,t.type,t.pendingProps,n);case 17:return r=t.type,a=t.pendingProps,a=t.elementType===r?a:vl(r,a),Hi(e,t),t.tag=1,Ta(r)?(e=!0,Fa(t)):e=!1,El(t,n),ql(t,r,a),$l(t,r,a,n),Ni(null,t,r,!0,e,n);case 19:return qi(e,t,n);case 22:return Ci(e,t,n)}throw Error(l(156,t.tag))};var Ks="function"===typeof reportError?reportError:function(e){console.error(e)};function Gs(e){this._internalRoot=e}function Zs(e){this._internalRoot=e}function Ys(e){return!(!e||1!==e.nodeType&&9!==e.nodeType&&11!==e.nodeType)}function Js(e){return!(!e||1!==e.nodeType&&9!==e.nodeType&&11!==e.nodeType&&(8!==e.nodeType||" react-mount-point-unstable "!==e.nodeValue))}function Xs(){}function ec(e,t,n,r,a){var l=n._reactRootContainer;if(l){var o=l;if("function"===typeof a){var i=a;a=function(){var e=$s(o);i.call(e)}}Hs(t,o,e,a)}else o=function(e,t,n,r,a){if(a){if("function"===typeof r){var l=r;r=function(){var e=$s(o);l.call(e)}}var o=qs(t,r,e,0,null,!1,0,"",Xs);return e._reactRootContainer=o,e[ha]=o.current,Vr(8===e.nodeType?e.parentNode:e),fs(),o}for(;a=e.lastChild;)e.removeChild(a);if("function"===typeof r){var i=r;r=function(){var e=$s(u);i.call(e)}}var u=Bs(e,0,!1,null,0,!1,0,"",Xs);return e._reactRootContainer=u,e[ha]=u.current,Vr(8===e.nodeType?e.parentNode:e),fs((function(){Hs(t,u,n,r)})),u}(n,t,e,a,r);return $s(o)}Zs.prototype.render=Gs.prototype.render=function(e){var t=this._internalRoot;if(null===t)throw Error(l(409));Hs(e,t,null,null)},Zs.prototype.unmount=Gs.prototype.unmount=function(){var e=this._internalRoot;if(null!==e){this._internalRoot=null;var t=e.containerInfo;fs((function(){Hs(null,e,null,null)})),t[ha]=null}},Zs.prototype.unstable_scheduleHydration=function(e){if(e){var t=Et();e={blockedOn:null,target:e,priority:t};for(var n=0;n<Lt.length&&0!==t&&t<Lt[n].priority;n++);Lt.splice(n,0,e),0===n&&Dt(e)}},St=function(e){switch(e.tag){case 3:var t=e.stateNode;if(t.current.memoizedState.isDehydrated){var n=ft(t.pendingLanes);0!==n&&(yt(t,1|n),as(t,Ye()),0===(6&Nu)&&(qu=Ye()+500,Va()))}break;case 13:fs((function(){var t=Nl(e,1);if(null!==t){var n=ts();rs(t,e,1,n)}})),Qs(e,1)}},xt=function(e){if(13===e.tag){var t=Nl(e,134217728);if(null!==t)rs(t,e,134217728,ts());Qs(e,134217728)}},kt=function(e){if(13===e.tag){var t=ns(e),n=Nl(e,t);if(null!==n)rs(n,e,t,ts());Qs(e,t)}},Et=function(){return bt},Ct=function(e,t){var n=bt;try{return bt=e,t()}finally{bt=n}},xe=function(e,t,n){switch(t){case"input":if(J(e,n),t=n.name,"radio"===n.type&&null!=t){for(n=e;n.parentNode;)n=n.parentNode;for(n=n.querySelectorAll("input[name="+JSON.stringify(""+t)+'][type="radio"]'),t=0;t<n.length;t++){var r=n[t];if(r!==e&&r.form===e.form){var a=Sa(r);if(!a)throw Error(l(90));Q(r),J(r,a)}}}break;case"textarea":le(e,n);break;case"select":null!=(t=n.value)&&ne(e,!!n.multiple,t,!1)}},Pe=cs,Ne=fs;var tc={usingClientEntryPoint:!1,Events:[ba,wa,Sa,_e,je,cs]},nc={findFiberByHostInstance:ya,bundleType:0,version:"18.2.0",rendererPackageName:"react-dom"},rc={bundleType:nc.bundleType,version:nc.version,rendererPackageName:nc.rendererPackageName,rendererConfig:nc.rendererConfig,overrideHookState:null,overrideHookStateDeletePath:null,overrideHookStateRenamePath:null,overrideProps:null,overridePropsDeletePath:null,overridePropsRenamePath:null,setErrorHandler:null,setSuspenseHandler:null,scheduleUpdate:null,currentDispatcherRef:w.ReactCurrentDispatcher,findHostInstanceByFiber:function(e){return null===(e=$e(e))?null:e.stateNode},findFiberByHostInstance:nc.findFiberByHostInstance||function(){return null},findHostInstancesForRefresh:null,scheduleRefresh:null,scheduleRoot:null,setRefreshHandler:null,getCurrentFiber:null,reconcilerVersion:"18.2.0-next-9e3b772b8-20220608"};if("undefined"!==typeof __REACT_DEVTOOLS_GLOBAL_HOOK__){var ac=__REACT_DEVTOOLS_GLOBAL_HOOK__;if(!ac.isDisabled&&ac.supportsFiber)try{at=ac.inject(rc),lt=ac}catch(ce){}}t.__SECRET_INTERNALS_DO_NOT_USE_OR_YOU_WILL_BE_FIRED=tc,t.createPortal=function(e,t){var n=2<arguments.length&&void 0!==arguments[2]?arguments[2]:null;if(!Ys(t))throw Error(l(200));return function(e,t,n){var r=3<arguments.length&&void 0!==arguments[3]?arguments[3]:null;return{$$typeof:x,key:null==r?null:""+r,children:e,containerInfo:t,implementation:n}}(e,t,null,n)},t.createRoot=function(e,t){if(!Ys(e))throw Error(l(299));var n=!1,r="",a=Ks;return null!==t&&void 0!==t&&(!0===t.unstable_strictMode&&(n=!0),void 0!==t.identifierPrefix&&(r=t.identifierPrefix),void 0!==t.onRecoverableError&&(a=t.onRecoverableError)),t=Bs(e,1,!1,null,0,n,0,r,a),e[ha]=t.current,Vr(8===e.nodeType?e.parentNode:e),new Gs(t)},t.findDOMNode=function(e){if(null==e)return null;if(1===e.nodeType)return e;var t=e._reactInternals;if(void 0===t){if("function"===typeof e.render)throw Error(l(188));throw e=Object.keys(e).join(","),Error(l(268,e))}return e=null===(e=$e(t))?null:e.stateNode},t.flushSync=function(e){return fs(e)},t.hydrate=function(e,t,n){if(!Js(t))throw Error(l(200));return ec(null,e,t,!0,n)},t.hydrateRoot=function(e,t,n){if(!Ys(e))throw Error(l(405));var r=null!=n&&n.hydratedSources||null,a=!1,o="",i=Ks;if(null!==n&&void 0!==n&&(!0===n.unstable_strictMode&&(a=!0),void 0!==n.identifierPrefix&&(o=n.identifierPrefix),void 0!==n.onRecoverableError&&(i=n.onRecoverableError)),t=qs(t,null,e,1,null!=n?n:null,a,0,o,i),e[ha]=t.current,Vr(e),r)for(e=0;e<r.length;e++)a=(a=(n=r[e])._getVersion)(n._source),null==t.mutableSourceEagerHydrationData?t.mutableSourceEagerHydrationData=[n,a]:t.mutableSourceEagerHydrationData.push(n,a);return new Zs(t)},t.render=function(e,t,n){if(!Js(t))throw Error(l(200));return ec(null,e,t,!1,n)},t.unmountComponentAtNode=function(e){if(!Js(e))throw Error(l(40));return!!e._reactRootContainer&&(fs((function(){ec(null,null,e,!1,(function(){e._reactRootContainer=null,e[ha]=null}))})),!0)},t.unstable_batchedUpdates=cs,t.unstable_renderSubtreeIntoContainer=function(e,t,n,r){if(!Js(n))throw Error(l(200));if(null==e||void 0===e._reactInternals)throw Error(l(38));return ec(e,t,n,!1,r)},t.version="18.2.0-next-9e3b772b8-20220608"},739:function(e,t,n){var r=n(168);t.createRoot=r.createRoot,t.hydrateRoot=r.hydrateRoot},168:function(e,t,n){!function e(){if("undefined"!==typeof __REACT_DEVTOOLS_GLOBAL_HOOK__&&"function"===typeof __REACT_DEVTOOLS_GLOBAL_HOOK__.checkDCE)try{__REACT_DEVTOOLS_GLOBAL_HOOK__.checkDCE(e)}catch(t){console.error(t)}}(),e.exports=n(534)},918:function(e,t,n){var r=n(313),a=Symbol.for("react.element"),l=Symbol.for("react.fragment"),o=Object.prototype.hasOwnProperty,i=r.__SECRET_INTERNALS_DO_NOT_USE_OR_YOU_WILL_BE_FIRED.ReactCurrentOwner,u={key:!0,ref:!0,__self:!0,__source:!0};function s(e,t,n){var r,l={},s=null,c=null;for(r in void 0!==n&&(s=""+n),void 0!==t.key&&(s=""+t.key),void 0!==t.ref&&(c=t.ref),t)o.call(t,r)&&!u.hasOwnProperty(r)&&(l[r]=t[r]);if(e&&e.defaultProps)for(r in t=e.defaultProps)void 0===l[r]&&(l[r]=t[r]);return{$$typeof:a,type:e,key:s,ref:c,props:l,_owner:i.current}}t.jsx=s,t.jsxs=s},306:function(e,t){var n=Symbol.for("react.element"),r=Symbol.for("react.portal"),a=Symbol.for("react.fragment"),l=Symbol.for("react.strict_mode"),o=Symbol.for("react.profiler"),i=Symbol.for("react.provider"),u=Symbol.for("react.context"),s=Symbol.for("react.forward_ref"),c=Symbol.for("react.suspense"),f=Symbol.for("react.memo"),d=Symbol.for("react.lazy"),p=Symbol.iterator;var h={isMounted:function(){return!1},enqueueForceUpdate:function(){},enqueueReplaceState:function(){},enqueueSetState:function(){}},m=Object.assign,v={};function g(e,t,n){this.props=e,this.context=t,this.refs=v,this.updater=n||h}function y(){}function b(e,t,n){this.props=e,this.context=t,this.refs=v,this.updater=n||h}g.prototype.isReactComponent={},g.prototype.setState=function(e,t){if("object"!==typeof e&&"function"!==typeof e&&null!=e)throw Error("setState(...): takes an object of state variables to update or a function which returns an object of state variables.");this.updater.enqueueSetState(this,e,t,"setState")},g.prototype.forceUpdate=function(e){this.updater.enqueueForceUpdate(this,e,"forceUpdate")},y.prototype=g.prototype;var w=b.prototype=new y;w.constructor=b,m(w,g.prototype),w.isPureReactComponent=!0;var S=Array.isArray,x=Object.prototype.hasOwnProperty,k={current:null},E={key:!0,ref:!0,__self:!0,__source:!0};function C(e,t,r){var a,l={},o=null,i=null;if(null!=t)for(a in void 0!==t.ref&&(i=t.ref),void 0!==t.key&&(o=""+t.key),t)x.call(t,a)&&!E.hasOwnProperty(a)&&(l[a]=t[a]);var u=arguments.length-2;if(1===u)l.children=r;else if(1<u){for(var s=Array(u),c=0;c<u;c++)s[c]=arguments[c+2];l.children=s}if(e&&e.defaultProps)for(a in u=e.defaultProps)void 0===l[a]&&(l[a]=u[a]);return{$$typeof:n,type:e,key:o,ref:i,props:l,_owner:k.current}}function _(e){return"object"===typeof e&&null!==e&&e.$$typeof===n}var j=/\/+/g;function P(e,t){return"object"===typeof e&&null!==e&&null!=e.key?function(e){var t={"=":"=0",":":"=2"};return"$"+e.replace(/[=:]/g,(function(e){return t[e]}))}(""+e.key):t.toString(36)}function N(e,t,a,l,o){var i=typeof e;"undefined"!==i&&"boolean"!==i||(e=null);var u=!1;if(null===e)u=!0;else switch(i){case"string":case"number":u=!0;break;case"object":switch(e.$$typeof){case n:case r:u=!0}}if(u)return o=o(u=e),e=""===l?"."+P(u,0):l,S(o)?(a="",null!=e&&(a=e.replace(j,"$&/")+"/"),N(o,t,a,"",(function(e){return e}))):null!=o&&(_(o)&&(o=function(e,t){return{$$typeof:n,type:e.type,key:t,ref:e.ref,props:e.props,_owner:e._owner}}(o,a+(!o.key||u&&u.key===o.key?"":(""+o.key).replace(j,"$&/")+"/")+e)),t.push(o)),1;if(u=0,l=""===l?".":l+":",S(e))for(var s=0;s<e.length;s++){var c=l+P(i=e[s],s);u+=N(i,t,a,c,o)}else if(c=function(e){return null===e||"object"!==typeof e?null:"function"===typeof(e=p&&e[p]||e["@@iterator"])?e:null}(e),"function"===typeof c)for(e=c.call(e),s=0;!(i=e.next()).done;)u+=N(i=i.value,t,a,c=l+P(i,s++),o);else if("object"===i)throw t=String(e),Error("Objects are not valid as a React child (found: "+("[object Object]"===t?"object with keys {"+Object.keys(e).join(", ")+"}":t)+"). If you meant to render a collection of children, use an array instead.");return u}function O(e,t,n){if(null==e)return e;var r=[],a=0;return N(e,r,"","",(function(e){return t.call(n,e,a++)})),r}function z(e){if(-1===e._status){var t=e._result;(t=t()).then((function(t){0!==e._status&&-1!==e._status||(e._status=1,e._result=t)}),(function(t){0!==e._status&&-1!==e._status||(e._status=2,e._result=t)})),-1===e._status&&(e._status=0,e._result=t)}if(1===e._status)return e._result.default;throw e._result}var T={current:null},L={transition:null},R={ReactCurrentDispatcher:T,ReactCurrentBatchConfig:L,ReactCurrentOwner:k};t.Children={map:O,forEach:function(e,t,n){O(e,(function(){t.apply(this,arguments)}),n)},count:function(e){var t=0;return O(e,(function(){t++})),t},toArray:function(e){return O(e,(function(e){return e}))||[]},only:function(e){if(!_(e))throw Error("React.Children.only expected to receive a single React element child.");return e}},t.Component=g,t.Fragment=a,t.Profiler=o,t.PureComponent=b,t.StrictMode=l,t.Suspense=c,t.__SECRET_INTERNALS_DO_NOT_USE_OR_YOU_WILL_BE_FIRED=R,t.cloneElement=function(e,t,r){if(null===e||void 0===e)throw Error("React.cloneElement(...): The argument must be a React element, but you passed "+e+".");var a=m({},e.props),l=e.key,o=e.ref,i=e._owner;if(null!=t){if(void 0!==t.ref&&(o=t.ref,i=k.current),void 0!==t.key&&(l=""+t.key),e.type&&e.type.defaultProps)var u=e.type.defaultProps;for(s in t)x.call(t,s)&&!E.hasOwnProperty(s)&&(a[s]=void 0===t[s]&&void 0!==u?u[s]:t[s])}var s=arguments.length-2;if(1===s)a.children=r;else if(1<s){u=Array(s);for(var c=0;c<s;c++)u[c]=arguments[c+2];a.children=u}return{$$typeof:n,type:e.type,key:l,ref:o,props:a,_owner:i}},t.createContext=function(e){return(e={$$typeof:u,_currentValue:e,_currentValue2:e,_threadCount:0,Provider:null,Consumer:null,_defaultValue:null,_globalName:null}).Provider={$$typeof:i,_context:e},e.Consumer=e},t.createElement=C,t.createFactory=function(e){var t=C.bind(null,e);return t.type=e,t},t.createRef=function(){return{current:null}},t.forwardRef=function(e){return{$$typeof:s,render:e}},t.isValidElement=_,t.lazy=function(e){return{$$typeof:d,_payload:{_status:-1,_result:e},_init:z}},t.memo=function(e,t){return{$$typeof:f,type:e,compare:void 0===t?null:t}},t.startTransition=function(e){var t=L.transition;L.transition={};try{e()}finally{L.transition=t}},t.unstable_act=function(){throw Error("act(...) is not supported in production builds of React.")},t.useCallback=function(e,t){return T.current.useCallback(e,t)},t.useContext=function(e){return T.current.useContext(e)},t.useDebugValue=function(){},t.useDeferredValue=function(e){return T.current.useDeferredValue(e)},t.useEffect=function(e,t){return T.current.useEffect(e,t)},t.useId=function(){return T.current.useId()},t.useImperativeHandle=function(e,t,n){return T.current.useImperativeHandle(e,t,n)},t.useInsertionEffect=function(e,t){return T.current.useInsertionEffect(e,t)},t.useLayoutEffect=function(e,t){return T.current.useLayoutEffect(e,t)},t.useMemo=function(e,t){return T.current.useMemo(e,t)},t.useReducer=function(e,t,n){return T.current.useReducer(e,t,n)},t.useRef=function(e){return T.current.useRef(e)},t.useState=function(e){return T.current.useState(e)},t.useSyncExternalStore=function(e,t,n){return T.current.useSyncExternalStore(e,t,n)},t.useTransition=function(){return T.current.useTransition()},t.version="18.2.0"},313:function(e,t,n){e.exports=n(306)},417:function(e,t,n){e.exports=n(918)},95:function(e,t){function n(e,t){var n=e.length;e.push(t);e:for(;0<n;){var r=n-1>>>1,a=e[r];if(!(0<l(a,t)))break e;e[r]=t,e[n]=a,n=r}}function r(e){return 0===e.length?null:e[0]}function a(e){if(0===e.length)return null;var t=e[0],n=e.pop();if(n!==t){e[0]=n;e:for(var r=0,a=e.length,o=a>>>1;r<o;){var i=2*(r+1)-1,u=e[i],s=i+1,c=e[s];if(0>l(u,n))s<a&&0>l(c,u)?(e[r]=c,e[s]=n,r=s):(e[r]=u,e[i]=n,r=i);else{if(!(s<a&&0>l(c,n)))break e;e[r]=c,e[s]=n,r=s}}}return t}function l(e,t){var n=e.sortIndex-t.sortIndex;return 0!==n?n:e.id-t.id}if("object"===typeof performance&&"function"===typeof performance.now){var o=performance;t.unstable_now=function(){return o.now()}}else{var i=Date,u=i.now();t.unstable_now=function(){return i.now()-u}}var s=[],c=[],f=1,d=null,p=3,h=!1,m=!1,v=!1,g="function"===typeof setTimeout?setTimeout:null,y="function"===typeof clearTimeout?clearTimeout:null,b="undefined"!==typeof setImmediate?setImmediate:null;function w(e){for(var t=r(c);null!==t;){if(null===t.callback)a(c);else{if(!(t.startTime<=e))break;a(c),t.sortIndex=t.expirationTime,n(s,t)}t=r(c)}}function S(e){if(v=!1,w(e),!m)if(null!==r(s))m=!0,L(x);else{var t=r(c);null!==t&&R(S,t.startTime-e)}}function x(e,n){m=!1,v&&(v=!1,y(_),_=-1),h=!0;var l=p;try{for(w(n),d=r(s);null!==d&&(!(d.expirationTime>n)||e&&!N());){var o=d.callback;if("function"===typeof o){d.callback=null,p=d.priorityLevel;var i=o(d.expirationTime<=n);n=t.unstable_now(),"function"===typeof i?d.callback=i:d===r(s)&&a(s),w(n)}else a(s);d=r(s)}if(null!==d)var u=!0;else{var f=r(c);null!==f&&R(S,f.startTime-n),u=!1}return u}finally{d=null,p=l,h=!1}}"undefined"!==typeof navigator&&void 0!==navigator.scheduling&&void 0!==navigator.scheduling.isInputPending&&navigator.scheduling.isInputPending.bind(navigator.scheduling);var k,E=!1,C=null,_=-1,j=5,P=-1;function N(){return!(t.unstable_now()-P<j)}function O(){if(null!==C){var e=t.unstable_now();P=e;var n=!0;try{n=C(!0,e)}finally{n?k():(E=!1,C=null)}}else E=!1}if("function"===typeof b)k=function(){b(O)};else if("undefined"!==typeof MessageChannel){var z=new MessageChannel,T=z.port2;z.port1.onmessage=O,k=function(){T.postMessage(null)}}else k=function(){g(O,0)};function L(e){C=e,E||(E=!0,k())}function R(e,n){_=g((function(){e(t.unstable_now())}),n)}t.unstable_IdlePriority=5,t.unstable_ImmediatePriority=1,t.unstable_LowPriority=4,t.unstable_NormalPriority=3,t.unstable_Profiling=null,t.unstable_UserBlockingPriority=2,t.unstable_cancelCallback=function(e){e.callback=null},t.unstable_continueExecution=function(){m||h||(m=!0,L(x))},t.unstable_forceFrameRate=function(e){0>e||125<e?console.error("forceFrameRate takes a positive int between 0 and 125, forcing frame rates higher than 125 fps is not supported"):j=0<e?Math.floor(1e3/e):5},t.unstable_getCurrentPriorityLevel=function(){return p},t.unstable_getFirstCallbackNode=function(){return r(s)},t.unstable_next=function(e){switch(p){case 1:case 2:case 3:var t=3;break;default:t=p}var n=p;p=t;try{return e()}finally{p=n}},t.unstable_pauseExecution=function(){},t.unstable_requestPaint=function(){},t.unstable_runWithPriority=function(e,t){switch(e){case 1:case 2:case 3:case 4:case 5:break;default:e=3}var n=p;p=e;try{return t()}finally{p=n}},t.unstable_scheduleCallback=function(e,a,l){var o=t.unstable_now();switch("object"===typeof l&&null!==l?l="number"===typeof(l=l.delay)&&0<l?o+l:o:l=o,e){case 1:var i=-1;break;case 2:i=250;break;case 5:i=1073741823;break;case 4:i=1e4;break;default:i=5e3}return e={id:f++,callback:a,priorityLevel:e,startTime:l,expirationTime:i=l+i,sortIndex:-1},l>o?(e.sortIndex=l,n(c,e),null===r(s)&&e===r(c)&&(v?(y(_),_=-1):v=!0,R(S,l-o))):(e.sortIndex=i,n(s,e),m||h||(m=!0,L(x))),e},t.unstable_shouldYield=N,t.unstable_wrapCallback=function(e){var t=p;return function(){var n=p;p=t;try{return e.apply(this,arguments)}finally{p=n}}}},224:function(e,t,n){e.exports=n(95)}},t={};function n(r){var a=t[r];if(void 0!==a)return a.exports;var l=t[r]={exports:{}};return e[r](l,l.exports,n),l.exports}!function(){var e=n(313),t=n(739);function r(e){return r="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(e){return typeof e}:function(e){return e&&"function"==typeof Symbol&&e.constructor===Symbol&&e!==Symbol.prototype?"symbol":typeof e},r(e)}function a(){a=function(){return e};var e={},t=Object.prototype,n=t.hasOwnProperty,l=Object.defineProperty||function(e,t,n){e[t]=n.value},o="function"==typeof Symbol?Symbol:{},i=o.iterator||"@@iterator",u=o.asyncIterator||"@@asyncIterator",s=o.toStringTag||"@@toStringTag";function c(e,t,n){return Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}),e[t]}try{c({},"")}catch(O){c=function(e,t,n){return e[t]=n}}function f(e,t,n,r){var a=t&&t.prototype instanceof h?t:h,o=Object.create(a.prototype),i=new j(r||[]);return l(o,"_invoke",{value:k(e,n,i)}),o}function d(e,t,n){try{return{type:"normal",arg:e.call(t,n)}}catch(O){return{type:"throw",arg:O}}}e.wrap=f;var p={};function h(){}function m(){}function v(){}var g={};c(g,i,(function(){return this}));var y=Object.getPrototypeOf,b=y&&y(y(P([])));b&&b!==t&&n.call(b,i)&&(g=b);var w=v.prototype=h.prototype=Object.create(g);function S(e){["next","throw","return"].forEach((function(t){c(e,t,(function(e){return this._invoke(t,e)}))}))}function x(e,t){function a(l,o,i,u){var s=d(e[l],e,o);if("throw"!==s.type){var c=s.arg,f=c.value;return f&&"object"==r(f)&&n.call(f,"__await")?t.resolve(f.__await).then((function(e){a("next",e,i,u)}),(function(e){a("throw",e,i,u)})):t.resolve(f).then((function(e){c.value=e,i(c)}),(function(e){return a("throw",e,i,u)}))}u(s.arg)}var o;l(this,"_invoke",{value:function(e,n){function r(){return new t((function(t,r){a(e,n,t,r)}))}return o=o?o.then(r,r):r()}})}function k(e,t,n){var r="suspendedStart";return function(a,l){if("executing"===r)throw new Error("Generator is already running");if("completed"===r){if("throw"===a)throw l;return N()}for(n.method=a,n.arg=l;;){var o=n.delegate;if(o){var i=E(o,n);if(i){if(i===p)continue;return i}}if("next"===n.method)n.sent=n._sent=n.arg;else if("throw"===n.method){if("suspendedStart"===r)throw r="completed",n.arg;n.dispatchException(n.arg)}else"return"===n.method&&n.abrupt("return",n.arg);r="executing";var u=d(e,t,n);if("normal"===u.type){if(r=n.done?"completed":"suspendedYield",u.arg===p)continue;return{value:u.arg,done:n.done}}"throw"===u.type&&(r="completed",n.method="throw",n.arg=u.arg)}}}function E(e,t){var n=t.method,r=e.iterator[n];if(void 0===r)return t.delegate=null,"throw"===n&&e.iterator.return&&(t.method="return",t.arg=void 0,E(e,t),"throw"===t.method)||"return"!==n&&(t.method="throw",t.arg=new TypeError("The iterator does not provide a '"+n+"' method")),p;var a=d(r,e.iterator,t.arg);if("throw"===a.type)return t.method="throw",t.arg=a.arg,t.delegate=null,p;var l=a.arg;return l?l.done?(t[e.resultName]=l.value,t.next=e.nextLoc,"return"!==t.method&&(t.method="next",t.arg=void 0),t.delegate=null,p):l:(t.method="throw",t.arg=new TypeError("iterator result is not an object"),t.delegate=null,p)}function C(e){var t={tryLoc:e[0]};1 in e&&(t.catchLoc=e[1]),2 in e&&(t.finallyLoc=e[2],t.afterLoc=e[3]),this.tryEntries.push(t)}function _(e){var t=e.completion||{};t.type="normal",delete t.arg,e.completion=t}function j(e){this.tryEntries=[{tryLoc:"root"}],e.forEach(C,this),this.reset(!0)}function P(e){if(e){var t=e[i];if(t)return t.call(e);if("function"==typeof e.next)return e;if(!isNaN(e.length)){var r=-1,a=function t(){for(;++r<e.length;)if(n.call(e,r))return t.value=e[r],t.done=!1,t;return t.value=void 0,t.done=!0,t};return a.next=a}}return{next:N}}function N(){return{value:void 0,done:!0}}return m.prototype=v,l(w,"constructor",{value:v,configurable:!0}),l(v,"constructor",{value:m,configurable:!0}),m.displayName=c(v,s,"GeneratorFunction"),e.isGeneratorFunction=function(e){var t="function"==typeof e&&e.constructor;return!!t&&(t===m||"GeneratorFunction"===(t.displayName||t.name))},e.mark=function(e){return Object.setPrototypeOf?Object.setPrototypeOf(e,v):(e.__proto__=v,c(e,s,"GeneratorFunction")),e.prototype=Object.create(w),e},e.awrap=function(e){return{__await:e}},S(x.prototype),c(x.prototype,u,(function(){return this})),e.AsyncIterator=x,e.async=function(t,n,r,a,l){void 0===l&&(l=Promise);var o=new x(f(t,n,r,a),l);return e.isGeneratorFunction(n)?o:o.next().then((function(e){return e.done?e.value:o.next()}))},S(w),c(w,s,"Generator"),c(w,i,(function(){return this})),c(w,"toString",(function(){return"[object Generator]"})),e.keys=function(e){var t=Object(e),n=[];for(var r in t)n.push(r);return n.reverse(),function e(){for(;n.length;){var r=n.pop();if(r in t)return e.value=r,e.done=!1,e}return e.done=!0,e}},e.values=P,j.prototype={constructor:j,reset:function(e){if(this.prev=0,this.next=0,this.sent=this._sent=void 0,this.done=!1,this.delegate=null,this.method="next",this.arg=void 0,this.tryEntries.forEach(_),!e)for(var t in this)"t"===t.charAt(0)&&n.call(this,t)&&!isNaN(+t.slice(1))&&(this[t]=void 0)},stop:function(){this.done=!0;var e=this.tryEntries[0].completion;if("throw"===e.type)throw e.arg;return this.rval},dispatchException:function(e){if(this.done)throw e;var t=this;function r(n,r){return o.type="throw",o.arg=e,t.next=n,r&&(t.method="next",t.arg=void 0),!!r}for(var a=this.tryEntries.length-1;a>=0;--a){var l=this.tryEntries[a],o=l.completion;if("root"===l.tryLoc)return r("end");if(l.tryLoc<=this.prev){var i=n.call(l,"catchLoc"),u=n.call(l,"finallyLoc");if(i&&u){if(this.prev<l.catchLoc)return r(l.catchLoc,!0);if(this.prev<l.finallyLoc)return r(l.finallyLoc)}else if(i){if(this.prev<l.catchLoc)return r(l.catchLoc,!0)}else{if(!u)throw new Error("try statement without catch or finally");if(this.prev<l.finallyLoc)return r(l.finallyLoc)}}}},abrupt:function(e,t){for(var r=this.tryEntries.length-1;r>=0;--r){var a=this.tryEntries[r];if(a.tryLoc<=this.prev&&n.call(a,"finallyLoc")&&this.prev<a.finallyLoc){var l=a;break}}l&&("break"===e||"continue"===e)&&l.tryLoc<=t&&t<=l.finallyLoc&&(l=null);var o=l?l.completion:{};return o.type=e,o.arg=t,l?(this.method="next",this.next=l.finallyLoc,p):this.complete(o)},complete:function(e,t){if("throw"===e.type)throw e.arg;return"break"===e.type||"continue"===e.type?this.next=e.arg:"return"===e.type?(this.rval=this.arg=e.arg,this.method="return",this.next="end"):"normal"===e.type&&t&&(this.next=t),p},finish:function(e){for(var t=this.tryEntries.length-1;t>=0;--t){var n=this.tryEntries[t];if(n.finallyLoc===e)return this.complete(n.completion,n.afterLoc),_(n),p}},catch:function(e){for(var t=this.tryEntries.length-1;t>=0;--t){var n=this.tryEntries[t];if(n.tryLoc===e){var r=n.completion;if("throw"===r.type){var a=r.arg;_(n)}return a}}throw new Error("illegal catch attempt")},delegateYield:function(e,t,n){return this.delegate={iterator:P(e),resultName:t,nextLoc:n},"next"===this.method&&(this.arg=void 0),p}},e}function l(e,t,n,r,a,l,o){try{var i=e[l](o),u=i.value}catch(s){return void n(s)}i.done?t(u):Promise.resolve(u).then(r,a)}function o(e){return function(){var t=this,n=arguments;return new Promise((function(r,a){var o=e.apply(t,n);function i(e){l(o,r,a,i,u,"next",e)}function u(e){l(o,r,a,i,u,"throw",e)}i(void 0)}))}}function i(e,t){(null==t||t>e.length)&&(t=e.length);for(var n=0,r=new Array(t);n<t;n++)r[n]=e[n];return r}function u(e,t){return function(e){if(Array.isArray(e))return e}(e)||function(e,t){var n=null==e?null:"undefined"!=typeof Symbol&&e[Symbol.iterator]||e["@@iterator"];if(null!=n){var r,a,l,o,i=[],u=!0,s=!1;try{if(l=(n=n.call(e)).next,0===t){if(Object(n)!==n)return;u=!1}else for(;!(u=(r=l.call(n)).done)&&(i.push(r.value),i.length!==t);u=!0);}catch(c){s=!0,a=c}finally{try{if(!u&&null!=n.return&&(o=n.return(),Object(o)!==o))return}finally{if(s)throw a}}return i}}(e,t)||function(e,t){if(e){if("string"===typeof e)return i(e,t);var n=Object.prototype.toString.call(e).slice(8,-1);return"Object"===n&&e.constructor&&(n=e.constructor.name),"Map"===n||"Set"===n?Array.from(e):"Arguments"===n||/^(?:Ui|I)nt(?:8|16|32)(?:Clamped)?Array$/.test(n)?i(e,t):void 0}}(e,t)||function(){throw new TypeError("Invalid attempt to destructure non-iterable instance.\nIn order to be iterable, non-array objects must have a [Symbol.iterator]() method.")}()}function s(e,t){return function(){return e.apply(t,arguments)}}var c,f=Object.prototype.toString,d=Object.getPrototypeOf,p=(c=Object.create(null),function(e){var t=f.call(e);return c[t]||(c[t]=t.slice(8,-1).toLowerCase())}),h=function(e){return e=e.toLowerCase(),function(t){return p(t)===e}},m=function(e){return function(t){return typeof t===e}},v=Array.isArray,g=m("undefined");var y=h("ArrayBuffer");var b=m("string"),w=m("function"),S=m("number"),x=function(e){return null!==e&&"object"===typeof e},k=function(e){if("object"!==p(e))return!1;var t=d(e);return(null===t||t===Object.prototype||null===Object.getPrototypeOf(t))&&!(Symbol.toStringTag in e)&&!(Symbol.iterator in e)},E=h("Date"),C=h("File"),_=h("Blob"),j=h("FileList"),P=h("URLSearchParams");function N(e,t){var n,r,a=(arguments.length>2&&void 0!==arguments[2]?arguments[2]:{}).allOwnKeys,l=void 0!==a&&a;if(null!==e&&"undefined"!==typeof e)if("object"!==typeof e&&(e=[e]),v(e))for(n=0,r=e.length;n<r;n++)t.call(null,e[n],n,e);else{var o,i=l?Object.getOwnPropertyNames(e):Object.keys(e),u=i.length;for(n=0;n<u;n++)o=i[n],t.call(null,e[o],o,e)}}function O(e,t){t=t.toLowerCase();for(var n,r=Object.keys(e),a=r.length;a-- >0;)if(t===(n=r[a]).toLowerCase())return n;return null}var z="undefined"!==typeof globalThis?globalThis:"undefined"!==typeof self?self:"undefined"!==typeof window?window:global,T=function(e){return!g(e)&&e!==z};var L,R=(L="undefined"!==typeof Uint8Array&&d(Uint8Array),function(e){return L&&e instanceof L}),A=h("HTMLFormElement"),F=function(e){var t=Object.prototype.hasOwnProperty;return function(e,n){return t.call(e,n)}}(),D=h("RegExp"),M=function(e,t){var n=Object.getOwnPropertyDescriptors(e),r={};N(n,(function(n,a){!1!==t(n,a,e)&&(r[a]=n)})),Object.defineProperties(e,r)},I="abcdefghijklmnopqrstuvwxyz",U="0123456789",B={DIGIT:U,ALPHA:I,ALPHA_DIGIT:I+I.toUpperCase()+U};var V={isArray:v,isArrayBuffer:y,isBuffer:function(e){return null!==e&&!g(e)&&null!==e.constructor&&!g(e.constructor)&&w(e.constructor.isBuffer)&&e.constructor.isBuffer(e)},isFormData:function(e){var t="[object FormData]";return e&&("function"===typeof FormData&&e instanceof FormData||f.call(e)===t||w(e.toString)&&e.toString()===t)},isArrayBufferView:function(e){return"undefined"!==typeof ArrayBuffer&&ArrayBuffer.isView?ArrayBuffer.isView(e):e&&e.buffer&&y(e.buffer)},isString:b,isNumber:S,isBoolean:function(e){return!0===e||!1===e},isObject:x,isPlainObject:k,isUndefined:g,isDate:E,isFile:C,isBlob:_,isRegExp:D,isFunction:w,isStream:function(e){return x(e)&&w(e.pipe)},isURLSearchParams:P,isTypedArray:R,isFileList:j,forEach:N,merge:function e(){for(var t=(T(this)&&this||{}).caseless,n={},r=function(r,a){var l=t&&O(n,a)||a;k(n[l])&&k(r)?n[l]=e(n[l],r):k(r)?n[l]=e({},r):v(r)?n[l]=r.slice():n[l]=r},a=0,l=arguments.length;a<l;a++)arguments[a]&&N(arguments[a],r);return n},extend:function(e,t,n){return N(t,(function(t,r){n&&w(t)?e[r]=s(t,n):e[r]=t}),{allOwnKeys:(arguments.length>3&&void 0!==arguments[3]?arguments[3]:{}).allOwnKeys}),e},trim:function(e){return e.trim?e.trim():e.replace(/^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g,"")},stripBOM:function(e){return 65279===e.charCodeAt(0)&&(e=e.slice(1)),e},inherits:function(e,t,n,r){e.prototype=Object.create(t.prototype,r),e.prototype.constructor=e,Object.defineProperty(e,"super",{value:t.prototype}),n&&Object.assign(e.prototype,n)},toFlatObject:function(e,t,n,r){var a,l,o,i={};if(t=t||{},null==e)return t;do{for(l=(a=Object.getOwnPropertyNames(e)).length;l-- >0;)o=a[l],r&&!r(o,e,t)||i[o]||(t[o]=e[o],i[o]=!0);e=!1!==n&&d(e)}while(e&&(!n||n(e,t))&&e!==Object.prototype);return t},kindOf:p,kindOfTest:h,endsWith:function(e,t,n){e=String(e),(void 0===n||n>e.length)&&(n=e.length),n-=t.length;var r=e.indexOf(t,n);return-1!==r&&r===n},toArray:function(e){if(!e)return null;if(v(e))return e;var t=e.length;if(!S(t))return null;for(var n=new Array(t);t-- >0;)n[t]=e[t];return n},forEachEntry:function(e,t){for(var n,r=(e&&e[Symbol.iterator]).call(e);(n=r.next())&&!n.done;){var a=n.value;t.call(e,a[0],a[1])}},matchAll:function(e,t){for(var n,r=[];null!==(n=e.exec(t));)r.push(n);return r},isHTMLForm:A,hasOwnProperty:F,hasOwnProp:F,reduceDescriptors:M,freezeMethods:function(e){M(e,(function(t,n){if(w(e)&&-1!==["arguments","caller","callee"].indexOf(n))return!1;var r=e[n];w(r)&&(t.enumerable=!1,"writable"in t?t.writable=!1:t.set||(t.set=function(){throw Error("Can not rewrite read-only method '"+n+"'")}))}))},toObjectSet:function(e,t){var n={},r=function(e){e.forEach((function(e){n[e]=!0}))};return v(e)?r(e):r(String(e).split(t)),n},toCamelCase:function(e){return e.toLowerCase().replace(/[-_\s]([a-z\d])(\w*)/g,(function(e,t,n){return t.toUpperCase()+n}))},noop:function(){},toFiniteNumber:function(e,t){return e=+e,Number.isFinite(e)?e:t},findKey:O,global:z,isContextDefined:T,ALPHABET:B,generateString:function(){for(var e=arguments.length>0&&void 0!==arguments[0]?arguments[0]:16,t=arguments.length>1&&void 0!==arguments[1]?arguments[1]:B.ALPHA_DIGIT,n="",r=t.length;e--;)n+=t[Math.random()*r|0];return n},isSpecCompliantForm:function(e){return!!(e&&w(e.append)&&"FormData"===e[Symbol.toStringTag]&&e[Symbol.iterator])},toJSONObject:function(e){var t=new Array(10);return function e(n,r){if(x(n)){if(t.indexOf(n)>=0)return;if(!("toJSON"in n)){t[r]=n;var a=v(n)?[]:{};return N(n,(function(t,n){var l=e(t,r+1);!g(l)&&(a[n]=l)})),t[r]=void 0,a}}return n}(e,0)}};function q(e,t){if(!(e instanceof t))throw new TypeError("Cannot call a class as a function")}function H(e){var t=function(e,t){if("object"!==r(e)||null===e)return e;var n=e[Symbol.toPrimitive];if(void 0!==n){var a=n.call(e,t||"default");if("object"!==r(a))return a;throw new TypeError("@@toPrimitive must return a primitive value.")}return("string"===t?String:Number)(e)}(e,"string");return"symbol"===r(t)?t:String(t)}function $(e,t){for(var n=0;n<t.length;n++){var r=t[n];r.enumerable=r.enumerable||!1,r.configurable=!0,"value"in r&&(r.writable=!0),Object.defineProperty(e,H(r.key),r)}}function W(e,t,n){return t&&$(e.prototype,t),n&&$(e,n),Object.defineProperty(e,"prototype",{writable:!1}),e}function Q(e,t,n,r,a){Error.call(this),Error.captureStackTrace?Error.captureStackTrace(this,this.constructor):this.stack=(new Error).stack,this.message=e,this.name="AxiosError",t&&(this.code=t),n&&(this.config=n),r&&(this.request=r),a&&(this.response=a)}V.inherits(Q,Error,{toJSON:function(){return{message:this.message,name:this.name,description:this.description,number:this.number,fileName:this.fileName,lineNumber:this.lineNumber,columnNumber:this.columnNumber,stack:this.stack,config:V.toJSONObject(this.config),code:this.code,status:this.response&&this.response.status?this.response.status:null}}});var K=Q.prototype,G={};["ERR_BAD_OPTION_VALUE","ERR_BAD_OPTION","ECONNABORTED","ETIMEDOUT","ERR_NETWORK","ERR_FR_TOO_MANY_REDIRECTS","ERR_DEPRECATED","ERR_BAD_RESPONSE","ERR_BAD_REQUEST","ERR_CANCELED","ERR_NOT_SUPPORT","ERR_INVALID_URL"].forEach((function(e){G[e]={value:e}})),Object.defineProperties(Q,G),Object.defineProperty(K,"isAxiosError",{value:!0}),Q.from=function(e,t,n,r,a,l){var o=Object.create(K);return V.toFlatObject(e,o,(function(e){return e!==Error.prototype}),(function(e){return"isAxiosError"!==e})),Q.call(o,e.message,t,n,r,a),o.cause=e,o.name=e.name,l&&Object.assign(o,l),o};var Z=Q,Y=null;function J(e){return V.isPlainObject(e)||V.isArray(e)}function X(e){return V.endsWith(e,"[]")?e.slice(0,-2):e}function ee(e,t,n){return e?e.concat(t).map((function(e,t){return e=X(e),!n&&t?"["+e+"]":e})).join(n?".":""):t}var te=V.toFlatObject(V,{},null,(function(e){return/^is[A-Z]/.test(e)}));var ne=function(e,t,n){if(!V.isObject(e))throw new TypeError("target must be an object");t=t||new(Y||FormData);var r=(n=V.toFlatObject(n,{metaTokens:!0,dots:!1,indexes:!1},!1,(function(e,t){return!V.isUndefined(t[e])}))).metaTokens,a=n.visitor||s,l=n.dots,o=n.indexes,i=(n.Blob||"undefined"!==typeof Blob&&Blob)&&V.isSpecCompliantForm(t);if(!V.isFunction(a))throw new TypeError("visitor must be a function");function u(e){if(null===e)return"";if(V.isDate(e))return e.toISOString();if(!i&&V.isBlob(e))throw new Z("Blob is not supported. Use a Buffer instead.");return V.isArrayBuffer(e)||V.isTypedArray(e)?i&&"function"===typeof Blob?new Blob([e]):Buffer.from(e):e}function s(e,n,a){var i=e;if(e&&!a&&"object"===typeof e)if(V.endsWith(n,"{}"))n=r?n:n.slice(0,-2),e=JSON.stringify(e);else if(V.isArray(e)&&function(e){return V.isArray(e)&&!e.some(J)}(e)||(V.isFileList(e)||V.endsWith(n,"[]"))&&(i=V.toArray(e)))return n=X(n),i.forEach((function(e,r){!V.isUndefined(e)&&null!==e&&t.append(!0===o?ee([n],r,l):null===o?n:n+"[]",u(e))})),!1;return!!J(e)||(t.append(ee(a,n,l),u(e)),!1)}var c=[],f=Object.assign(te,{defaultVisitor:s,convertValue:u,isVisitable:J});if(!V.isObject(e))throw new TypeError("data must be an object");return function e(n,r){if(!V.isUndefined(n)){if(-1!==c.indexOf(n))throw Error("Circular reference detected in "+r.join("."));c.push(n),V.forEach(n,(function(n,l){!0===(!(V.isUndefined(n)||null===n)&&a.call(t,n,V.isString(l)?l.trim():l,r,f))&&e(n,r?r.concat(l):[l])})),c.pop()}}(e),t};function re(e){var t={"!":"%21","'":"%27","(":"%28",")":"%29","~":"%7E","%20":"+","%00":"\0"};return encodeURIComponent(e).replace(/[!'()~]|%20|%00/g,(function(e){return t[e]}))}function ae(e,t){this._pairs=[],e&&ne(e,this,t)}var le=ae.prototype;le.append=function(e,t){this._pairs.push([e,t])},le.toString=function(e){var t=e?function(t){return e.call(this,t,re)}:re;return this._pairs.map((function(e){return t(e[0])+"="+t(e[1])}),"").join("&")};var oe=ae;function ie(e){return encodeURIComponent(e).replace(/%3A/gi,":").replace(/%24/g,"$").replace(/%2C/gi,",").replace(/%20/g,"+").replace(/%5B/gi,"[").replace(/%5D/gi,"]")}function ue(e,t,n){if(!t)return e;var r,a=n&&n.encode||ie,l=n&&n.serialize;if(r=l?l(t,n):V.isURLSearchParams(t)?t.toString():new oe(t,n).toString(a)){var o=e.indexOf("#");-1!==o&&(e=e.slice(0,o)),e+=(-1===e.indexOf("?")?"?":"&")+r}return e}var se=function(){function e(){q(this,e),this.handlers=[]}return W(e,[{key:"use",value:function(e,t,n){return this.handlers.push({fulfilled:e,rejected:t,synchronous:!!n&&n.synchronous,runWhen:n?n.runWhen:null}),this.handlers.length-1}},{key:"eject",value:function(e){this.handlers[e]&&(this.handlers[e]=null)}},{key:"clear",value:function(){this.handlers&&(this.handlers=[])}},{key:"forEach",value:function(e){V.forEach(this.handlers,(function(t){null!==t&&e(t)}))}}]),e}(),ce={silentJSONParsing:!0,forcedJSONParsing:!0,clarifyTimeoutError:!1},fe="undefined"!==typeof URLSearchParams?URLSearchParams:oe,de=FormData,pe=function(){var e;return("undefined"===typeof navigator||"ReactNative"!==(e=navigator.product)&&"NativeScript"!==e&&"NS"!==e)&&("undefined"!==typeof window&&"undefined"!==typeof document)}(),he="undefined"!==typeof WorkerGlobalScope&&self instanceof WorkerGlobalScope&&"function"===typeof self.importScripts,me={isBrowser:!0,classes:{URLSearchParams:fe,FormData:de,Blob:Blob},isStandardBrowserEnv:pe,isStandardBrowserWebWorkerEnv:he,protocols:["http","https","file","blob","url","data"]};var ve=function(e){function t(e,n,r,a){var l=e[a++],o=Number.isFinite(+l),i=a>=e.length;return l=!l&&V.isArray(r)?r.length:l,i?(V.hasOwnProp(r,l)?r[l]=[r[l],n]:r[l]=n,!o):(r[l]&&V.isObject(r[l])||(r[l]=[]),t(e,n,r[l],a)&&V.isArray(r[l])&&(r[l]=function(e){var t,n,r={},a=Object.keys(e),l=a.length;for(t=0;t<l;t++)r[n=a[t]]=e[n];return r}(r[l])),!o)}if(V.isFormData(e)&&V.isFunction(e.entries)){var n={};return V.forEachEntry(e,(function(e,r){t(function(e){return V.matchAll(/\w+|\[(\w*)]/g,e).map((function(e){return"[]"===e[0]?"":e[1]||e[0]}))}(e),r,n,0)})),n}return null},ge={"Content-Type":void 0};var ye={transitional:ce,adapter:["xhr","http"],transformRequest:[function(e,t){var n,r=t.getContentType()||"",a=r.indexOf("application/json")>-1,l=V.isObject(e);if(l&&V.isHTMLForm(e)&&(e=new FormData(e)),V.isFormData(e))return a&&a?JSON.stringify(ve(e)):e;if(V.isArrayBuffer(e)||V.isBuffer(e)||V.isStream(e)||V.isFile(e)||V.isBlob(e))return e;if(V.isArrayBufferView(e))return e.buffer;if(V.isURLSearchParams(e))return t.setContentType("application/x-www-form-urlencoded;charset=utf-8",!1),e.toString();if(l){if(r.indexOf("application/x-www-form-urlencoded")>-1)return function(e,t){return ne(e,new me.classes.URLSearchParams,Object.assign({visitor:function(e,t,n,r){return me.isNode&&V.isBuffer(e)?(this.append(t,e.toString("base64")),!1):r.defaultVisitor.apply(this,arguments)}},t))}(e,this.formSerializer).toString();if((n=V.isFileList(e))||r.indexOf("multipart/form-data")>-1){var o=this.env&&this.env.FormData;return ne(n?{"files[]":e}:e,o&&new o,this.formSerializer)}}return l||a?(t.setContentType("application/json",!1),function(e,t,n){if(V.isString(e))try{return(t||JSON.parse)(e),V.trim(e)}catch(r){if("SyntaxError"!==r.name)throw r}return(n||JSON.stringify)(e)}(e)):e}],transformResponse:[function(e){var t=this.transitional||ye.transitional,n=t&&t.forcedJSONParsing,r="json"===this.responseType;if(e&&V.isString(e)&&(n&&!this.responseType||r)){var a=!(t&&t.silentJSONParsing)&&r;try{return JSON.parse(e)}catch(l){if(a){if("SyntaxError"===l.name)throw Z.from(l,Z.ERR_BAD_RESPONSE,this,null,this.response);throw l}}}return e}],timeout:0,xsrfCookieName:"XSRF-TOKEN",xsrfHeaderName:"X-XSRF-TOKEN",maxContentLength:-1,maxBodyLength:-1,env:{FormData:me.classes.FormData,Blob:me.classes.Blob},validateStatus:function(e){return e>=200&&e<300},headers:{common:{Accept:"application/json, text/plain, */*"}}};V.forEach(["delete","get","head"],(function(e){ye.headers[e]={}})),V.forEach(["post","put","patch"],(function(e){ye.headers[e]=V.merge(ge)}));var be=ye,we=V.toObjectSet(["age","authorization","content-length","content-type","etag","expires","from","host","if-modified-since","if-unmodified-since","last-modified","location","max-forwards","proxy-authorization","referer","retry-after","user-agent"]),Se=Symbol("internals");function xe(e){return e&&String(e).trim().toLowerCase()}function ke(e){return!1===e||null==e?e:V.isArray(e)?e.map(ke):String(e)}function Ee(e,t,n,r){return V.isFunction(r)?r.call(this,t,n):V.isString(t)?V.isString(r)?-1!==t.indexOf(r):V.isRegExp(r)?r.test(t):void 0:void 0}var Ce=function(e,t){function n(e){q(this,n),e&&this.set(e)}return W(n,[{key:"set",value:function(e,t,n){var r=this;function a(e,t,n){var a=xe(t);if(!a)throw new Error("header name must be a non-empty string");var l=V.findKey(r,a);(!l||void 0===r[l]||!0===n||void 0===n&&!1!==r[l])&&(r[l||t]=ke(e))}var l=function(e,t){return V.forEach(e,(function(e,n){return a(e,n,t)}))};return V.isPlainObject(e)||e instanceof this.constructor?l(e,t):V.isString(e)&&(e=e.trim())&&!/^[-_a-zA-Z]+$/.test(e.trim())?l(function(e){var t,n,r,a={};return e&&e.split("\n").forEach((function(e){r=e.indexOf(":"),t=e.substring(0,r).trim().toLowerCase(),n=e.substring(r+1).trim(),!t||a[t]&&we[t]||("set-cookie"===t?a[t]?a[t].push(n):a[t]=[n]:a[t]=a[t]?a[t]+", "+n:n)})),a}(e),t):null!=e&&a(t,e,n),this}},{key:"get",value:function(e,t){if(e=xe(e)){var n=V.findKey(this,e);if(n){var r=this[n];if(!t)return r;if(!0===t)return function(e){for(var t,n=Object.create(null),r=/([^\s,;=]+)\s*(?:=\s*([^,;]+))?/g;t=r.exec(e);)n[t[1]]=t[2];return n}(r);if(V.isFunction(t))return t.call(this,r,n);if(V.isRegExp(t))return t.exec(r);throw new TypeError("parser must be boolean|regexp|function")}}}},{key:"has",value:function(e,t){if(e=xe(e)){var n=V.findKey(this,e);return!(!n||void 0===this[n]||t&&!Ee(0,this[n],n,t))}return!1}},{key:"delete",value:function(e,t){var n=this,r=!1;function a(e){if(e=xe(e)){var a=V.findKey(n,e);!a||t&&!Ee(0,n[a],a,t)||(delete n[a],r=!0)}}return V.isArray(e)?e.forEach(a):a(e),r}},{key:"clear",value:function(e){for(var t=Object.keys(this),n=t.length,r=!1;n--;){var a=t[n];e&&!Ee(0,this[a],a,e)||(delete this[a],r=!0)}return r}},{key:"normalize",value:function(e){var t=this,n={};return V.forEach(this,(function(r,a){var l=V.findKey(n,a);if(l)return t[l]=ke(r),void delete t[a];var o=e?function(e){return e.trim().toLowerCase().replace(/([a-z\d])(\w*)/g,(function(e,t,n){return t.toUpperCase()+n}))}(a):String(a).trim();o!==a&&delete t[a],t[o]=ke(r),n[o]=!0})),this}},{key:"concat",value:function(){for(var e,t=arguments.length,n=new Array(t),r=0;r<t;r++)n[r]=arguments[r];return(e=this.constructor).concat.apply(e,[this].concat(n))}},{key:"toJSON",value:function(e){var t=Object.create(null);return V.forEach(this,(function(n,r){null!=n&&!1!==n&&(t[r]=e&&V.isArray(n)?n.join(", "):n)})),t}},{key:Symbol.iterator,value:function(){return Object.entries(this.toJSON())[Symbol.iterator]()}},{key:"toString",value:function(){return Object.entries(this.toJSON()).map((function(e){var t=u(e,2);return t[0]+": "+t[1]})).join("\n")}},{key:Symbol.toStringTag,get:function(){return"AxiosHeaders"}}],[{key:"from",value:function(e){return e instanceof this?e:new this(e)}},{key:"concat",value:function(e){for(var t=new this(e),n=arguments.length,r=new Array(n>1?n-1:0),a=1;a<n;a++)r[a-1]=arguments[a];return r.forEach((function(e){return t.set(e)})),t}},{key:"accessor",value:function(e){var t=(this[Se]=this[Se]={accessors:{}}).accessors,n=this.prototype;function r(e){var r=xe(e);t[r]||(!function(e,t){var n=V.toCamelCase(" "+t);["get","set","has"].forEach((function(r){Object.defineProperty(e,r+n,{value:function(e,n,a){return this[r].call(this,t,e,n,a)},configurable:!0})}))}(n,e),t[r]=!0)}return V.isArray(e)?e.forEach(r):r(e),this}}]),n}();Ce.accessor(["Content-Type","Content-Length","Accept","Accept-Encoding","User-Agent","Authorization"]),V.freezeMethods(Ce.prototype),V.freezeMethods(Ce);var _e=Ce;function je(e,t){var n=this||be,r=t||n,a=_e.from(r.headers),l=r.data;return V.forEach(e,(function(e){l=e.call(n,l,a.normalize(),t?t.status:void 0)})),a.normalize(),l}function Pe(e){return!(!e||!e.__CANCEL__)}function Ne(e,t,n){Z.call(this,null==e?"canceled":e,Z.ERR_CANCELED,t,n),this.name="CanceledError"}V.inherits(Ne,Z,{__CANCEL__:!0});var Oe=Ne;var ze=me.isStandardBrowserEnv?{write:function(e,t,n,r,a,l){var o=[];o.push(e+"="+encodeURIComponent(t)),V.isNumber(n)&&o.push("expires="+new Date(n).toGMTString()),V.isString(r)&&o.push("path="+r),V.isString(a)&&o.push("domain="+a),!0===l&&o.push("secure"),document.cookie=o.join("; ")},read:function(e){var t=document.cookie.match(new RegExp("(^|;\\s*)("+e+")=([^;]*)"));return t?decodeURIComponent(t[3]):null},remove:function(e){this.write(e,"",Date.now()-864e5)}}:{write:function(){},read:function(){return null},remove:function(){}};function Te(e,t){return e&&!/^([a-z][a-z\d+\-.]*:)?\/\//i.test(t)?function(e,t){return t?e.replace(/\/+$/,"")+"/"+t.replace(/^\/+/,""):e}(e,t):t}var Le=me.isStandardBrowserEnv?function(){var e,t=/(msie|trident)/i.test(navigator.userAgent),n=document.createElement("a");function r(e){var r=e;return t&&(n.setAttribute("href",r),r=n.href),n.setAttribute("href",r),{href:n.href,protocol:n.protocol?n.protocol.replace(/:$/,""):"",host:n.host,search:n.search?n.search.replace(/^\?/,""):"",hash:n.hash?n.hash.replace(/^#/,""):"",hostname:n.hostname,port:n.port,pathname:"/"===n.pathname.charAt(0)?n.pathname:"/"+n.pathname}}return e=r(window.location.href),function(t){var n=V.isString(t)?r(t):t;return n.protocol===e.protocol&&n.host===e.host}}():function(){return!0};var Re=function(e,t){e=e||10;var n,r=new Array(e),a=new Array(e),l=0,o=0;return t=void 0!==t?t:1e3,function(i){var u=Date.now(),s=a[o];n||(n=u),r[l]=i,a[l]=u;for(var c=o,f=0;c!==l;)f+=r[c++],c%=e;if((l=(l+1)%e)===o&&(o=(o+1)%e),!(u-n<t)){var d=s&&u-s;return d?Math.round(1e3*f/d):void 0}}};function Ae(e,t){var n=0,r=Re(50,250);return function(a){var l=a.loaded,o=a.lengthComputable?a.total:void 0,i=l-n,u=r(i);n=l;var s={loaded:l,total:o,progress:o?l/o:void 0,bytes:i,rate:u||void 0,estimated:u&&o&&l<=o?(o-l)/u:void 0,event:a};s[t?"download":"upload"]=!0,e(s)}}var Fe="undefined"!==typeof XMLHttpRequest,De={http:Y,xhr:Fe&&function(e){return new Promise((function(t,n){var r,a=e.data,l=_e.from(e.headers).normalize(),o=e.responseType;function i(){e.cancelToken&&e.cancelToken.unsubscribe(r),e.signal&&e.signal.removeEventListener("abort",r)}V.isFormData(a)&&(me.isStandardBrowserEnv||me.isStandardBrowserWebWorkerEnv)&&l.setContentType(!1);var u=new XMLHttpRequest;if(e.auth){var s=e.auth.username||"",c=e.auth.password?unescape(encodeURIComponent(e.auth.password)):"";l.set("Authorization","Basic "+btoa(s+":"+c))}var f=Te(e.baseURL,e.url);function d(){if(u){var r=_e.from("getAllResponseHeaders"in u&&u.getAllResponseHeaders());!function(e,t,n){var r=n.config.validateStatus;n.status&&r&&!r(n.status)?t(new Z("Request failed with status code "+n.status,[Z.ERR_BAD_REQUEST,Z.ERR_BAD_RESPONSE][Math.floor(n.status/100)-4],n.config,n.request,n)):e(n)}((function(e){t(e),i()}),(function(e){n(e),i()}),{data:o&&"text"!==o&&"json"!==o?u.response:u.responseText,status:u.status,statusText:u.statusText,headers:r,config:e,request:u}),u=null}}if(u.open(e.method.toUpperCase(),ue(f,e.params,e.paramsSerializer),!0),u.timeout=e.timeout,"onloadend"in u?u.onloadend=d:u.onreadystatechange=function(){u&&4===u.readyState&&(0!==u.status||u.responseURL&&0===u.responseURL.indexOf("file:"))&&setTimeout(d)},u.onabort=function(){u&&(n(new Z("Request aborted",Z.ECONNABORTED,e,u)),u=null)},u.onerror=function(){n(new Z("Network Error",Z.ERR_NETWORK,e,u)),u=null},u.ontimeout=function(){var t=e.timeout?"timeout of "+e.timeout+"ms exceeded":"timeout exceeded",r=e.transitional||ce;e.timeoutErrorMessage&&(t=e.timeoutErrorMessage),n(new Z(t,r.clarifyTimeoutError?Z.ETIMEDOUT:Z.ECONNABORTED,e,u)),u=null},me.isStandardBrowserEnv){var p=(e.withCredentials||Le(f))&&e.xsrfCookieName&&ze.read(e.xsrfCookieName);p&&l.set(e.xsrfHeaderName,p)}void 0===a&&l.setContentType(null),"setRequestHeader"in u&&V.forEach(l.toJSON(),(function(e,t){u.setRequestHeader(t,e)})),V.isUndefined(e.withCredentials)||(u.withCredentials=!!e.withCredentials),o&&"json"!==o&&(u.responseType=e.responseType),"function"===typeof e.onDownloadProgress&&u.addEventListener("progress",Ae(e.onDownloadProgress,!0)),"function"===typeof e.onUploadProgress&&u.upload&&u.upload.addEventListener("progress",Ae(e.onUploadProgress)),(e.cancelToken||e.signal)&&(r=function(t){u&&(n(!t||t.type?new Oe(null,e,u):t),u.abort(),u=null)},e.cancelToken&&e.cancelToken.subscribe(r),e.signal&&(e.signal.aborted?r():e.signal.addEventListener("abort",r)));var h=function(e){var t=/^([-+\w]{1,25})(:?\/\/|:)/.exec(e);return t&&t[1]||""}(f);h&&-1===me.protocols.indexOf(h)?n(new Z("Unsupported protocol "+h+":",Z.ERR_BAD_REQUEST,e)):u.send(a||null)}))}};V.forEach(De,(function(e,t){if(e){try{Object.defineProperty(e,"name",{value:t})}catch(n){}Object.defineProperty(e,"adapterName",{value:t})}}));var Me={getAdapter:function(e){for(var t,n,r=(e=V.isArray(e)?e:[e]).length,a=0;a<r&&(t=e[a],!(n=V.isString(t)?De[t.toLowerCase()]:t));a++);if(!n){if(!1===n)throw new Z("Adapter ".concat(t," is not supported by the environment"),"ERR_NOT_SUPPORT");throw new Error(V.hasOwnProp(De,t)?"Adapter '".concat(t,"' is not available in the build"):"Unknown adapter '".concat(t,"'"))}if(!V.isFunction(n))throw new TypeError("adapter is not a function");return n},adapters:De};function Ie(e){if(e.cancelToken&&e.cancelToken.throwIfRequested(),e.signal&&e.signal.aborted)throw new Oe(null,e)}function Ue(e){return Ie(e),e.headers=_e.from(e.headers),e.data=je.call(e,e.transformRequest),-1!==["post","put","patch"].indexOf(e.method)&&e.headers.setContentType("application/x-www-form-urlencoded",!1),Me.getAdapter(e.adapter||be.adapter)(e).then((function(t){return Ie(e),t.data=je.call(e,e.transformResponse,t),t.headers=_e.from(t.headers),t}),(function(t){return Pe(t)||(Ie(e),t&&t.response&&(t.response.data=je.call(e,e.transformResponse,t.response),t.response.headers=_e.from(t.response.headers))),Promise.reject(t)}))}var Be=function(e){return e instanceof _e?e.toJSON():e};function Ve(e,t){t=t||{};var n={};function r(e,t,n){return V.isPlainObject(e)&&V.isPlainObject(t)?V.merge.call({caseless:n},e,t):V.isPlainObject(t)?V.merge({},t):V.isArray(t)?t.slice():t}function a(e,t,n){return V.isUndefined(t)?V.isUndefined(e)?void 0:r(void 0,e,n):r(e,t,n)}function l(e,t){if(!V.isUndefined(t))return r(void 0,t)}function o(e,t){return V.isUndefined(t)?V.isUndefined(e)?void 0:r(void 0,e):r(void 0,t)}function i(n,a,l){return l in t?r(n,a):l in e?r(void 0,n):void 0}var u={url:l,method:l,data:l,baseURL:o,transformRequest:o,transformResponse:o,paramsSerializer:o,timeout:o,timeoutMessage:o,withCredentials:o,adapter:o,responseType:o,xsrfCookieName:o,xsrfHeaderName:o,onUploadProgress:o,onDownloadProgress:o,decompress:o,maxContentLength:o,maxBodyLength:o,beforeRedirect:o,transport:o,httpAgent:o,httpsAgent:o,cancelToken:o,socketPath:o,responseEncoding:o,validateStatus:i,headers:function(e,t){return a(Be(e),Be(t),!0)}};return V.forEach(Object.keys(e).concat(Object.keys(t)),(function(r){var l=u[r]||a,o=l(e[r],t[r],r);V.isUndefined(o)&&l!==i||(n[r]=o)})),n}var qe="1.3.2",He={};["object","boolean","number","function","string","symbol"].forEach((function(e,t){He[e]=function(n){return typeof n===e||"a"+(t<1?"n ":" ")+e}}));var $e={};He.transitional=function(e,t,n){function r(e,t){return"[Axios v"+qe+"] Transitional option '"+e+"'"+t+(n?". "+n:"")}return function(n,a,l){if(!1===e)throw new Z(r(a," has been removed"+(t?" in "+t:"")),Z.ERR_DEPRECATED);return t&&!$e[a]&&($e[a]=!0,console.warn(r(a," has been deprecated since v"+t+" and will be removed in the near future"))),!e||e(n,a,l)}};var We={assertOptions:function(e,t,n){if("object"!==typeof e)throw new Z("options must be an object",Z.ERR_BAD_OPTION_VALUE);for(var r=Object.keys(e),a=r.length;a-- >0;){var l=r[a],o=t[l];if(o){var i=e[l],u=void 0===i||o(i,l,e);if(!0!==u)throw new Z("option "+l+" must be "+u,Z.ERR_BAD_OPTION_VALUE)}else if(!0!==n)throw new Z("Unknown option "+l,Z.ERR_BAD_OPTION)}},validators:He},Qe=We.validators,Ke=function(){function e(t){q(this,e),this.defaults=t,this.interceptors={request:new se,response:new se}}return W(e,[{key:"request",value:function(e,t){"string"===typeof e?(t=t||{}).url=e:t=e||{};var n,r=t=Ve(this.defaults,t),a=r.transitional,l=r.paramsSerializer,o=r.headers;void 0!==a&&We.assertOptions(a,{silentJSONParsing:Qe.transitional(Qe.boolean),forcedJSONParsing:Qe.transitional(Qe.boolean),clarifyTimeoutError:Qe.transitional(Qe.boolean)},!1),void 0!==l&&We.assertOptions(l,{encode:Qe.function,serialize:Qe.function},!0),t.method=(t.method||this.defaults.method||"get").toLowerCase(),(n=o&&V.merge(o.common,o[t.method]))&&V.forEach(["delete","get","head","post","put","patch","common"],(function(e){delete o[e]})),t.headers=_e.concat(n,o);var i=[],u=!0;this.interceptors.request.forEach((function(e){"function"===typeof e.runWhen&&!1===e.runWhen(t)||(u=u&&e.synchronous,i.unshift(e.fulfilled,e.rejected))}));var s,c=[];this.interceptors.response.forEach((function(e){c.push(e.fulfilled,e.rejected)}));var f,d=0;if(!u){var p=[Ue.bind(this),void 0];for(p.unshift.apply(p,i),p.push.apply(p,c),f=p.length,s=Promise.resolve(t);d<f;)s=s.then(p[d++],p[d++]);return s}f=i.length;var h=t;for(d=0;d<f;){var m=i[d++],v=i[d++];try{h=m(h)}catch(g){v.call(this,g);break}}try{s=Ue.call(this,h)}catch(g){return Promise.reject(g)}for(d=0,f=c.length;d<f;)s=s.then(c[d++],c[d++]);return s}},{key:"getUri",value:function(e){return ue(Te((e=Ve(this.defaults,e)).baseURL,e.url),e.params,e.paramsSerializer)}}]),e}();V.forEach(["delete","get","head","options"],(function(e){Ke.prototype[e]=function(t,n){return this.request(Ve(n||{},{method:e,url:t,data:(n||{}).data}))}})),V.forEach(["post","put","patch"],(function(e){function t(t){return function(n,r,a){return this.request(Ve(a||{},{method:e,headers:t?{"Content-Type":"multipart/form-data"}:{},url:n,data:r}))}}Ke.prototype[e]=t(),Ke.prototype[e+"Form"]=t(!0)}));var Ge=Ke,Ze=function(){function e(t){if(q(this,e),"function"!==typeof t)throw new TypeError("executor must be a function.");var n;this.promise=new Promise((function(e){n=e}));var r=this;this.promise.then((function(e){if(r._listeners){for(var t=r._listeners.length;t-- >0;)r._listeners[t](e);r._listeners=null}})),this.promise.then=function(e){var t,n=new Promise((function(e){r.subscribe(e),t=e})).then(e);return n.cancel=function(){r.unsubscribe(t)},n},t((function(e,t,a){r.reason||(r.reason=new Oe(e,t,a),n(r.reason))}))}return W(e,[{key:"throwIfRequested",value:function(){if(this.reason)throw this.reason}},{key:"subscribe",value:function(e){this.reason?e(this.reason):this._listeners?this._listeners.push(e):this._listeners=[e]}},{key:"unsubscribe",value:function(e){if(this._listeners){var t=this._listeners.indexOf(e);-1!==t&&this._listeners.splice(t,1)}}}],[{key:"source",value:function(){var t;return{token:new e((function(e){t=e})),cancel:t}}}]),e}();var Ye={Continue:100,SwitchingProtocols:101,Processing:102,EarlyHints:103,Ok:200,Created:201,Accepted:202,NonAuthoritativeInformation:203,NoContent:204,ResetContent:205,PartialContent:206,MultiStatus:207,AlreadyReported:208,ImUsed:226,MultipleChoices:300,MovedPermanently:301,Found:302,SeeOther:303,NotModified:304,UseProxy:305,Unused:306,TemporaryRedirect:307,PermanentRedirect:308,BadRequest:400,Unauthorized:401,PaymentRequired:402,Forbidden:403,NotFound:404,MethodNotAllowed:405,NotAcceptable:406,ProxyAuthenticationRequired:407,RequestTimeout:408,Conflict:409,Gone:410,LengthRequired:411,PreconditionFailed:412,PayloadTooLarge:413,UriTooLong:414,UnsupportedMediaType:415,RangeNotSatisfiable:416,ExpectationFailed:417,ImATeapot:418,MisdirectedRequest:421,UnprocessableEntity:422,Locked:423,FailedDependency:424,TooEarly:425,UpgradeRequired:426,PreconditionRequired:428,TooManyRequests:429,RequestHeaderFieldsTooLarge:431,UnavailableForLegalReasons:451,InternalServerError:500,NotImplemented:501,BadGateway:502,ServiceUnavailable:503,GatewayTimeout:504,HttpVersionNotSupported:505,VariantAlsoNegotiates:506,InsufficientStorage:507,LoopDetected:508,NotExtended:510,NetworkAuthenticationRequired:511};Object.entries(Ye).forEach((function(e){var t=u(e,2),n=t[0],r=t[1];Ye[r]=n}));var Je=Ye;var Xe=function e(t){var n=new Ge(t),r=s(Ge.prototype.request,n);return V.extend(r,Ge.prototype,n,{allOwnKeys:!0}),V.extend(r,n,null,{allOwnKeys:!0}),r.create=function(n){return e(Ve(t,n))},r}(be);Xe.Axios=Ge,Xe.CanceledError=Oe,Xe.CancelToken=Ze,Xe.isCancel=Pe,Xe.VERSION=qe,Xe.toFormData=ne,Xe.AxiosError=Z,Xe.Cancel=Xe.CanceledError,Xe.all=function(e){return Promise.all(e)},Xe.spread=function(e){return function(t){return e.apply(null,t)}},Xe.isAxiosError=function(e){return V.isObject(e)&&!0===e.isAxiosError},Xe.mergeConfig=Ve,Xe.AxiosHeaders=_e,Xe.formToJSON=function(e){return ve(V.isHTMLForm(e)?new FormData(e):e)},Xe.HttpStatusCode=Je,Xe.default=Xe;var et=Xe,tt=n(417);var nt=function(){var t=40.82355,n=(0,e.useRef)(null),r=(0,e.useCallback)((function(e){n.current=e,vt()}),[]),l=(0,e.useRef)(null),i=(0,e.useCallback)((function(e){l.current=e,gt()}),[]),s=(0,e.useRef)(null),c=(0,e.useCallback)((function(e){s.current=e,yt()}),[]),f=u((0,e.useState)(!0),2),d=f[0],p=f[1],h=u((0,e.useState)(""),2),m=h[0],v=h[1],g=u((0,e.useState)(""),2),y=g[0],b=g[1],w=u((0,e.useState)(!1),2),S=w[0],x=w[1],k=u((0,e.useState)(""),2),E=k[0],C=k[1],_=u((0,e.useState)(""),2),j=_[0],P=_[1],N=u((0,e.useState)(""),2),O=N[0],z=N[1],T=u((0,e.useState)(""),2),L=T[0],R=T[1],A=u((0,e.useState)(""),2),F=A[0],D=A[1],M=u((0,e.useState)(""),2),I=M[0],U=M[1],B=u((0,e.useState)(!1),2),V=B[0],q=B[1],H=u((0,e.useState)(""),2),$=H[0],W=H[1],Q=u((0,e.useState)(null),2),K=Q[0],G=Q[1],Z=u((0,e.useState)(""),2),Y=Z[0],J=Z[1],X=u((0,e.useState)(""),2),ee=X[0],te=X[1],ne=u((0,e.useState)(!1),2),re=ne[0],ae=ne[1],le=u((0,e.useState)(""),2),oe=le[0],ie=le[1],ue=u((0,e.useState)(!1),2),se=ue[0],ce=ue[1],fe=u((0,e.useState)(!1),2),de=fe[0],pe=fe[1],he=u((0,e.useState)(""),2),me=he[0],ve=he[1],ge=u((0,e.useState)(null),2),ye=ge[0],be=ge[1],we=u((0,e.useState)(!1),2),Se=we[0],xe=we[1],ke=u((0,e.useState)(""),2),Ee=ke[0],Ce=ke[1],_e=u((0,e.useState)(!1),2),je=_e[0],Pe=_e[1],Ne=u((0,e.useState)(""),2),Oe=Ne[0],ze=Ne[1],Te=u((0,e.useState)(""),2),Le=Te[0],Re=Te[1],Ae=u((0,e.useState)(""),2),Fe=Ae[0],De=Ae[1],Me=u((0,e.useState)(5),2),Ie=Me[0],Ue=Me[1],Be=u((0,e.useState)(""),2),Ve=Be[0],qe=Be[1],He=u((0,e.useState)(!1),2),$e=He[0],We=He[1],Qe=u((0,e.useState)(""),2),Ke=Qe[0],Ge=Qe[1],Ze=u((0,e.useState)(!1),2),Ye=Ze[0],Je=Ze[1],Xe=u((0,e.useState)(!1),2),nt=Xe[0],rt=Xe[1],at=u((0,e.useState)(""),2),lt=at[0],ot=at[1],it=u((0,e.useState)(null),2),ut=it[0],st=it[1],ct=function(e,t){G(e),e.target.value!==t?e.target.setCustomValidity("Password fields must match!"):e.target.setCustomValidity("")},ft=function(e){ve(e),null!=ye&&dt(ye,e)},dt=function(e,t){be(e),e.target.value!==t?e.target.setCustomValidity("Password fields must match!"):e.target.setCustomValidity("")},pt=function(e){ot(e),null!=ut&&ht(ut,e)},ht=function(e,t){st(e),e.target.value!==t?e.target.setCustomValidity("Password fields must match!"):e.target.setCustomValidity("")},mt=function(){var e=o(a().mark((function e(){var t,n;return a().wrap((function(e){for(;;)switch(e.prev=e.next){case 0:return e.prev=0,p(!0),v("Fetching configuration"),b(""),e.next=6,et.get("/config");case 6:t=e.sent,n=t.data,200===t.status?(P(n.module_type),z(n.firmware),R(n.unit_id),D(n.mac_address),U(n.wifi_hostname),J(n.mqtt_server),te(n.mqtt_port),ae(n.mqtt_credentials),ie(n.mqtt_user),ce(n.mqtt_pass_set),xe(n.ssl_use),Ce(n.ssl_root_ca),Pe(n.ssl_validate_client),ze(n.ssl_client_certificate),Re(n.ssl_client_private_key),De(n.ntp),Ue(n.gnss_mode),qe(n.gprs_apn),We(n.gprs_credentials),Ge(n.gprs_user),Je(n.gprs_pass_set),p(!1),v(""),b(""),x(!1),C("")):(p(!1),v(""),b(""),x(!0),C("Fetch of device configuration was not successful!")),e.next=19;break;case 12:e.prev=12,e.t0=e.catch(0),p(!1),v(""),b(""),x(!0),C("Unable to fetch device configuration!");case 19:case"end":return e.stop()}}),e,null,[[0,12]])})));return function(){return e.apply(this,arguments)}}(),vt=function(){null!=n.current&&(n.current.style.fontSize=n.current.scrollWidth/t+"px")},gt=function(){null!=l.current&&(l.current.style.fontSize=l.current.scrollWidth/t+"px")},yt=function(){null!=s.current&&(s.current.style.fontSize=s.current.scrollWidth/t+"px")};(0,e.useEffect)((function(){mt(),window.addEventListener("resize",(function(){vt(),gt(),yt()}))}),[]);var bt=function(){var e=o(a().mark((function e(t){var n;return a().wrap((function(e){for(;;)switch(e.prev=e.next){case 0:return t.preventDefault(),p(!0),v("Saving configuration"),b(""),n={hostname:I,device_password_change:V,device_password:$,mqtt_server:Y,mqtt_port:ee,mqtt_credentials:re,mqtt_username:oe,mqtt_password_change:de,mqtt_password:me,ssl_use:Se,ssl_root_ca:Ee,ssl_validate_client:je,ssl_client_certificate:Oe,ssl_client_private_key:Le,ntp:Fe,gnss_mode:Ie,gprs_apn:Ve,gprs_credentials:$e,gprs_username:Ke,gprs_password_change:nt,gprs_password:lt},e.prev=5,b("Device will be rebooted"),e.next=9,et.post("/config",n);case 9:setTimeout((function(){b("You can close this window"),window.top.close()}),3e3),e.next=19;break;case 12:e.prev=12,e.t0=e.catch(5),p(!1),v(""),b(""),x(!0),C("Error during saving configuration to device!");case 19:case"end":return e.stop()}}),e,null,[[5,12]])})));return function(t){return e.apply(this,arguments)}}();return(0,tt.jsx)("div",{className:"app",children:(0,tt.jsx)("center",{className:"container",children:(0,tt.jsxs)("div",{className:"content",children:[(0,tt.jsx)("h1",{className:"header",children:"STL-CF Setup"}),d?(0,tt.jsxs)(e.Fragment,{children:[(0,tt.jsx)("p",{className:"loader-header",children:m}),(0,tt.jsx)("div",{className:"loader"}),(0,tt.jsx)("p",{className:"loader-message",children:y})]}):S?(0,tt.jsxs)(e.Fragment,{children:[(0,tt.jsx)("p",{className:"error-header",children:"Something went wrong!"}),(0,tt.jsx)("div",{className:"error-sign",children:"\u26a0"}),(0,tt.jsx)("p",{className:"error-message",children:E})]}):(0,tt.jsxs)(e.Fragment,{children:[(0,tt.jsxs)("table",{className:"info-table",children:[(0,tt.jsx)("thead",{children:(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{width:"50%"}),(0,tt.jsx)("td",{width:"50%"})]})}),(0,tt.jsxs)("tbody",{children:[(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"1",children:"Module type:"}),(0,tt.jsx)("td",{align:"right",colSpan:"1",children:(0,tt.jsx)("span",{id:"type",children:j})})]}),(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"1",children:"Firmware version:"}),(0,tt.jsx)("td",{align:"right",colSpan:"1",children:(0,tt.jsx)("span",{id:"firmware",children:O})})]}),(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"1",children:"Unit ID:"}),(0,tt.jsx)("td",{align:"right",colSpan:"1",children:(0,tt.jsx)("span",{id:"unit",children:L})})]}),(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"1",children:"MAC Address:"}),(0,tt.jsx)("td",{align:"right",colSpan:"1",children:(0,tt.jsx)("span",{id:"mac",children:F})})]})]})]}),(0,tt.jsxs)("form",{onSubmit:bt,children:[(0,tt.jsxs)("table",{children:[(0,tt.jsx)("thead",{children:(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"}),(0,tt.jsx)("td",{width:"5%"})]})}),(0,tt.jsxs)("tbody",{children:[(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"9",children:"Hostname"}),(0,tt.jsx)("td",{align:"right",colSpan:"11",children:(0,tt.jsx)("input",{required:!0,size:"1",value:I,type:"text",pattern:"^[a-zA-Z\\d]{0,}[a-zA-Z\\d-]{0,}[a-zA-Z\\d]{1,}$",maxLength:"62",autoCapitalize:"off",onChange:function(e){return U(e.target.value)}})})]}),(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"13",children:"Change Device Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"7",children:(0,tt.jsx)("input",{type:"checkbox",checked:V,onChange:function(e){return q(e.target.checked)}})})]}),V?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"8",children:"Password"}),(0,tt.jsx)("td",{align:"right",colSpan:"11",children:(0,tt.jsx)("input",{required:V,size:"1",value:$,type:"password",autoCapitalize:"off",onChange:function(e){return t=e.target.value,W(t),void(null!=K&&ct(K,t));var t}})})]}):null,V?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"8",children:"Password"}),(0,tt.jsx)("td",{align:"right",colSpan:"11",children:(0,tt.jsx)("input",{required:V,size:"1",type:"password",placeholder:"Retype password",autoCapitalize:"off",onChange:function(e){return ct(e,$)}})})]}):null,(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"9",children:"NTP Server"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:!0,size:"1",value:Fe,type:"text",pattern:"^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$",placeholder:"pool.ntp.org",autoCapitalize:"off",onChange:function(e){return De(e.target.value)}})})]}),(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"9",children:"MQTT Server"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:!0,size:"1",value:Y,type:"text",pattern:"^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$",autoCapitalize:"off",placeholder:"mqtt-server.local",onChange:function(e){return J(e.target.value)}})})]}),(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"9",children:"MQTT Port"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:!0,size:"1",value:ee,type:"text",pattern:"^[0-9]{2,5}$",placeholder:"1883",autoCapitalize:"off",onChange:function(e){return te(e.target.value)}})})]}),(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"13",children:"MQTT Credentials"}),(0,tt.jsx)("td",{align:"center",colSpan:"7",children:(0,tt.jsx)("input",{type:"checkbox",checked:re,onChange:function(e){return ae(e.target.checked)}})})]}),re?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"8",children:"Username"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:re,size:"1",value:oe,type:"text",autoCapitalize:"off",onChange:function(e){return ie(e.target.value)}})})]}):null,re&&!se?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"8",children:"Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:re&&!se,size:"1",value:me,type:"password",autoCapitalize:"off",onChange:function(e){return ft(e.target.value)}})})]}):null,re&&!se?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"8",children:"Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:re&&!se,size:"1",type:"password",placeholder:"Retype password",autoCapitalize:"off",onChange:function(e){return dt(e,me)}})})]}):null,re&&se?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"12",children:"Change Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"7",children:(0,tt.jsx)("input",{type:"checkbox",checked:de,onChange:function(e){return pe(e.target.checked)}})})]}):null,re&&se&&de?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"2"}),(0,tt.jsx)("td",{align:"left",colSpan:"7",children:"Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:re&&se&&de,size:"1",value:me,type:"password",autoCapitalize:"off",onChange:function(e){return ft(e.target.value)}})})]}):null,re&&se&&de?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"2"}),(0,tt.jsx)("td",{align:"left",colSpan:"7",children:"Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:re&&se&&de,size:"1",type:"password",placeholder:"Retype password",autoCapitalize:"off",onChange:function(e){return dt(e,me)}})})]}):null,(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"13",children:"TLS Communication"}),(0,tt.jsx)("td",{align:"center",colSpan:"7",children:(0,tt.jsx)("input",{type:"checkbox",checked:Se,onChange:function(e){return xe(e.target.checked)}})})]}),Se?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"19",className:"label",children:"Root CA Certificate"})]}):null,Se?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"center",colSpan:"19",children:(0,tt.jsx)("textarea",{ref:r,required:Se,value:Ee,autoCapitalize:"off",placeholder:"Paste here content of certificate file/s",onChange:function(e){return Ce(e.target.value)}})})]}):null,Se?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"12",children:"Validate Client"}),(0,tt.jsx)("td",{align:"center",colSpan:"7",children:(0,tt.jsx)("input",{type:"checkbox",checked:je,onChange:function(e){return Pe(e.target.checked)}})})]}):null,Se&&je?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"2"}),(0,tt.jsx)("td",{align:"left",colSpan:"18",className:"label",children:"Client Certificate"})]}):null,Se&&je?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"2"}),(0,tt.jsx)("td",{align:"center",colSpan:"18",children:(0,tt.jsx)("textarea",{ref:i,required:Se&&je,value:Oe,autoCapitalize:"off",placeholder:"Paste here content of client certificate file",onChange:function(e){return ze(e.target.value)}})})]}):null,Se&&je?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"2"}),(0,tt.jsx)("td",{align:"left",colSpan:"18",className:"label",children:"Client Private Key"})]}):null,Se&&je?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"2"}),(0,tt.jsx)("td",{align:"center",colSpan:"18",children:(0,tt.jsx)("textarea",{ref:c,required:Se&&je,value:Le,autoCapitalize:"off",placeholder:"Paste here content of client certificate private key file",onChange:function(e){return Re(e.target.value)}})})]}):null,(0,tt.jsx)("tr",{children:(0,tt.jsx)("td",{align:"left",style:{lineHeight:"27px"},colSpan:"20",children:"GNSS Mode"})}),(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"right",colSpan:"19",children:(0,tt.jsxs)("select",{value:Ie,onChange:function(e){return Ue(e.target.value)},children:[(0,tt.jsx)("option",{value:"1",children:"GLONASS"}),(0,tt.jsx)("option",{value:"2",children:"BEIDOU"}),(0,tt.jsx)("option",{value:"4",children:"GALILEO"}),(0,tt.jsx)("option",{value:"8",children:"QZSS"}),(0,tt.jsx)("option",{value:"3",children:"GLONASS + BEIDOU"}),(0,tt.jsx)("option",{value:"5",children:"GLONASS + GALILEO"}),(0,tt.jsx)("option",{value:"6",children:"BEIDOU + GALILEO"}),(0,tt.jsx)("option",{value:"7",children:"GLONASS + BEIDOU + GALILEO"}),(0,tt.jsx)("option",{value:"15",children:"GLONASS + BEIDOU + GALILEO + QZSS"})]})})]}),(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"9",children:"GPRS APN"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:!0,size:"1",value:Ve,type:"text",pattern:"^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$",autoCapitalize:"off",onChange:function(e){return qe(e.target.value)}})})]}),(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"left",colSpan:"13",children:"GPRS Credentials"}),(0,tt.jsx)("td",{align:"center",colSpan:"7",children:(0,tt.jsx)("input",{type:"checkbox",checked:$e,onChange:function(e){return We(e.target.checked)}})})]}),$e?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"8",children:"Username"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:$e,size:"1",value:Ke,type:"text",autoCapitalize:"off",onChange:function(e){return Ge(e.target.value)}})})]}):null,$e&&!Ye?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"8",children:"Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:$e&&!Ye,size:"1",value:lt,type:"password",autoCapitalize:"off",onChange:function(e){return pt(e.target.value)}})})]}):null,$e&&!Ye?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"8",children:"Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:$e&&!Ye,size:"1",type:"password",placeholder:"Retype password",autoCapitalize:"off",onChange:function(e){return ht(e,lt)}})})]}):null,$e&&Ye?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"1"}),(0,tt.jsx)("td",{align:"left",colSpan:"12",children:"Change Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"7",children:(0,tt.jsx)("input",{type:"checkbox",checked:nt,onChange:function(e){return rt(e.target.checked)}})})]}):null,$e&&Ye&&nt?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"2"}),(0,tt.jsx)("td",{align:"left",colSpan:"7",children:"Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:$e&&Ye&&nt,size:"1",value:lt,type:"password",autoCapitalize:"off",onChange:function(e){return pt(e.target.value)}})})]}):null,$e&&Ye&&nt?(0,tt.jsxs)("tr",{children:[(0,tt.jsx)("td",{align:"center",colSpan:"2"}),(0,tt.jsx)("td",{align:"left",colSpan:"7",children:"Password"}),(0,tt.jsx)("td",{align:"center",colSpan:"11",children:(0,tt.jsx)("input",{required:$e&&Ye&&nt,size:"1",type:"password",placeholder:"Retype password",autoCapitalize:"off",onChange:function(e){return ht(e,lt)}})})]}):null]})]}),(0,tt.jsx)("input",{type:"submit",value:"Save & Reboot"}),(0,tt.jsx)("h2",{className:"info",children:"A reboot is required for the changes to take effect after modifying the configuration"})]})]})]})})})};t.createRoot(document.getElementById("root")).render((0,tt.jsx)(nt,{}))}()}();
)rawliteral";

// ----------------------------------------------------------------------------------------------------------------

sqlite3_stmt *db_res;
const char *db_tail;
int db_rc;
const char *db_path = "/littlefs/database.db";
unsigned long current_millis;

Ticker led_ticker;
bool is_setup_done = DEFAULT_IS_SETUP_DONE;
bool hw_reboot = DEFAULT_HW_REBOOT;
String mac_address_raw;
String mac_address_nag;
String wifi_hostname;
bool is_network_connected = false;      // ?
bool is_gprs_connected = false;         // ?
bool ssl_use = DEFAULT_SSL_USE;
String ssl_root_ca = DEFAULT_SSL_CA_CERTIFICATE;
bool ssl_validate_client = DEFAULT_SSL_VALIDATE_CLIENT; // TODO
String ssl_client_certificate = DEFAULT_SSL_CLIENT_CERTIFICATE; // TODO
String ssl_client_private_key = DEFAULT_SSL_CLIENT_PRIVATE_KEY; // TODO
String device_admin_username = DEFAULT_DEVICE_ADMIN_USERNAME;
String device_admin_password = DEFAULT_DEVICE_ADMIN_PASSWORD;
String mqtt_server = DEFAULT_MQTT_SERVER;
int mqtt_port = DEFAULT_MQTT_PORT;
bool mqtt_credentials = DEFAULT_MQTT_CREDENTIALS;
String mqtt_user = DEFAULT_MQTT_USER;
String mqtt_pass = DEFAULT_MQTT_PASS;
const int mqtt_check_period = 10000;
unsigned long mqtt_check_last_time = 0;
String mqtt_base_topic;
String ntp_server = DEFAULT_NTP_SERVER;
String timezone = DEFAULT_TIMEZONE;
String timezone_name = DEFAULT_TIMEZONE_NAME;
int gnss_mode = DEFAULT_GNSS_MODE;
String gprs_apn = DEFAULT_GPRS_APN;
bool gprs_credentials = DEFAULT_GPRS_CREDENTIALS;
String gprs_user = DEFAULT_GPRS_USER;
String gprs_pass = DEFAULT_GPRS_PASS;
/*const*/ int gps_period = 5000; // 10000
unsigned long gps_last_time = 0;
float gps_last_lat = 0.0;
float gps_last_lon = 0.0;
float gps_last_alt = 0.0;
float gps_last_speed = 0.0;
float gps_last_accuracy = 0.0;
DateTime gps_last_datetime = DateTime(2000, 1, 1, 0, 0, 0);
// int config_button_last_state = 1;
// bool config_button_pressed = false;
// bool config_button_active = false;
// unsigned long config_button_last_time = 0;
// unsigned long config_button_period = 5000;
const int config_button_debounce = 250;
unsigned long config_button_period = 5000;
bool config_button_pressed = false;
bool config_button_active = false;
int config_button_last_state = 0;
unsigned long config_button_last_time = 0;

String driver_card = "";  // ?

// ----------------------------------------------------------------------------------------------------------------

void play_config_entry_beep() { // done
  tone(ESP_BEEPER, 5000, 500);
}

void play_config_save_beep() { // done
  tone(ESP_BEEPER, 7000, 250);
  delay(500);
}

void play_hw_reboot_beep() { // done
  tone(ESP_BEEPER, 7000, 250);
  delay(500);
  tone(ESP_BEEPER, 7000, 250);
  delay(500);
}

void play_on_card_beep() { // done
  tone(ESP_BEEPER, 3000, 1000);
}

String mac_to_string(byte ar[]) { // done
  String s;
  for (byte i = 0; i < 6; ++i) {
    char buf[3];
    sprintf(buf, "%02X", ar[i]); // J-M-L: slight modification, added the 0 in the format for padding
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

String default_hostname() { // done
  uint64_t esp_id = ESP.getEfuseMac();
  String mac_address = mac_to_string((byte*) &esp_id);
  mac_address.replace(":", "");
  String hostname = MODULE_TYPE + String('-') + mac_address;
  return hostname;
}

double round_2(float value) { // done
  return (int)(value * 100 + 0.5) / 100.0;
}

float non_negative(float value) { // done
  // if (value >= 0.0) {
  //   return value;
  // } else {
  //   return 0.00;
  // }
  if (value < 0.0) {
    return 0.00;
  } else {
    return value;
  }
}

double filesystemPercentageUsage() { // done
  float calc = ((float)LittleFS.usedBytes() / (float)LittleFS.totalBytes()) * 100;
  return round_2(calc);
}

String filesystemUsage() { // done
  const size_t capacity = JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> json_doc;
  json_doc["percentage"] = filesystemPercentageUsage();
  json_doc["used"] = LittleFS.usedBytes();
  json_doc["free"] = LittleFS.totalBytes() - LittleFS.usedBytes();
  json_doc["total"] = LittleFS.totalBytes();
  String data_out;
  serializeJson(json_doc, data_out);
  return data_out;
}

void clear_wifi() { // done
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
}

void setup_unit_variables() { // done
  uint64_t esp_id = ESP.getEfuseMac();
  String mac_address = mac_to_string((byte*) &esp_id);
  mac_address_raw = mac_address;
  mac_address.replace(":", "");
  mac_address_nag = mac_address;
  wifi_hostname = default_hostname();
  mqtt_base_topic = "car-fleet/" + mac_address_nag;
}

void setup_pins_esp() { // TODO
  digitalWrite(ESP_RST, HIGH); // off
  pinMode(ESP_RST, OUTPUT);
  digitalWrite(ESP_PWR_LED, LOW); // on // ?
  pinMode(ESP_PWR_LED, OUTPUT); // ?
  digitalWrite(ESP_PWR_CARD_READER, LOW); // LOW/HIGH depends on mosfet type - here should set mosfet to OFF
  pinMode(ESP_PWR_CARD_READER, OUTPUT); // card reader mosfet // TODO
  pinMode(ESP_CONFIG_BUTTON, INPUT_PULLUP); // external pull-down || external pull-up
  pinMode(ESP_LED, OUTPUT);
  digitalWrite(ESP_LED, HIGH); // off
  pinMode(GSM_MODULE_POWER, OUTPUT);
  pinMode(GSM_MODULE_PWR, OUTPUT);
  pinMode(GSM_MODULE_INT, INPUT);
  pinMode(ESP_BEEPER, OUTPUT);
  pinMode(ESP_MPU_INT, INPUT); // TODO - external pull resistor
  pinMode(ESP_CAR_PWR_INT, INPUT); // TODO - external pull resistor

  // attachInterrupt(ESP_MPU_INT, handle_mpu_interrupt_rising, RISING);
  // attachInterrupt(ESP_MPU_INT, handle_mpu_interrupt_falling, FALLING);
  // attachInterrupt(ESP_MPU_INT, handle_mpu_interrupt_change, CHANGE);

  // attachInterrupt(ESP_CAR_PWR_INT, handle_car_pwr_interrupt_rising, RISING);
  // attachInterrupt(ESP_CAR_PWR_INT, handle_car_pwr_interrupt_falling, FALLING);
  // attachInterrupt(ESP_CAR_PWR_INT, handle_car_pwr_interrupt_change, CHANGE);

  SerialMon.println(F("[ESP] Success"));
}

void setup_mpu() { // TODO
  if (!mpu.begin(MPU6050_I2C_ADDRESS)) {
    SerialMon.println(F("[MPU] Error"));
    while (1);
  }

  mpu.setInterruptPinPolarity(true); // interupt is active LOW
  mpu.setInterruptPinLatch(false); // interupt is automaticly cleared after 50 microseconds

  SerialMon.println(F("[MPU] Success"));

  // TODO

  // The acceleration is measured in meters per second square (m/s2)
  // The angular velocity is measured in radians per seconds (rad/s)
  // The temperature is measured in Celsius degrees

  // mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  // mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  // mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  // mpu.setAccelerometerRange(MPU6050_RANGE_16_G);

  // mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  // mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  // mpu.setGyroRange(MPU6050_RANGE_1000_DEG);
  // mpu.setGyroRange(MPU6050_RANGE_2000_DEG);

  // mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  // mpu.setHighPassFilter(MPU6050_HIGHPASS_1_25_HZ);
  // mpu.setHighPassFilter(MPU6050_HIGHPASS_2_5_HZ);
  // mpu.setHighPassFilter(MPU6050_HIGHPASS_5_HZ);

  // mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
  // mpu.setFilterBandwidth(MPU6050_BAND_184_HZ);
  // mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);
  // mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);
  // mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  // mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);
  // mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  // mpu.setMotionDetectionThreshold(1);
  // mpu.setMotionDetectionDuration(20);
  // mpu.setMotionInterrupt(true); // motion detection will trigger interrupt

  // mpu.setTemperatureStandby(true); // disasble temperature sensor
}

// void update_ntp_time() { // done
//   configTzTime(timezone.c_str(), ntp_server.c_str(), NULL, NULL);
//   struct tm timeinfo;
//   if (getLocalTime(&timeinfo)) {
//     SerialMon.print(F("[NTP] Received Time - "));
//     SerialMon.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
//     rtc.adjust(DateTime(timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
//     SerialMon.print(F("[RTC] Time Updated - "));
//     SerialMon.println(rtc.now().timestamp());
//   }
// }

void setup_rtc() { // done
  if (!rtc.begin()) {
    SerialMon.println(F("[RTC] Error"));
    while (1);
  }
  SerialMon.println(F("[RTC] Success"));
  if (rtc.lostPower()) {
    SerialMon.println(F("[RTC] Lost power, setting default date (1/1/2000 00:00:00)"));
    rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
  }
  SerialMon.print(F("[RTC] Temperature is: "));
  SerialMon.print(rtc.getTemperature());
  SerialMon.println(F(" °C"));
}

// ----------------------------------------------------------------------------------------------------------------
int open_db(const char *filename, sqlite3 **db) { // done
  db_rc = sqlite3_open(filename, db);
  if (db_rc) {
    SerialMon.print(F("[DB] Can't open database: "));
    SerialMon.println(sqlite3_errmsg(*db));
    return db_rc;
  }
  return db_rc;
}

const char * db_exec_prep(const char *sql) { // ? - cleanup
  SerialMon.print(F("[DB] ")); // ?
  SerialMon.println(sql); // ?
  db_rc = sqlite3_prepare_v2(fs_db, sql, strlen(sql), &db_res, &db_tail);
  if (db_rc != SQLITE_OK) {
    const char *err = sqlite3_errmsg(fs_db);
    SerialMon.print(F("[DB] Failed preparing sql: "));
    SerialMon.println(err);
    sqlite3_close(fs_db);
    return err;
  }
  if (sqlite3_step(db_res) != SQLITE_DONE) {
    const char *err = sqlite3_errmsg(fs_db);
    SerialMon.print(F("[DB] ERROR executing stmt ("));
    SerialMon.print(sqlite3_step(db_res));
    SerialMon.print(F("): "));
    SerialMon.println(err);
    sqlite3_close(fs_db);
    return err;
  }
  // sqlite3_clear_bindings(db_res); // ?
  // db_rc = sqlite3_reset(db_res); // ?
  // if (db_rc != SQLITE_OK) { // ?
  //   sqlite3_close(fs_db); // ?
  //   return false; // ?
  // } // ?
  sqlite3_finalize(db_res);
  return "OK";
}

// ----------------------------------------------------------------------------------------------------------------

void handle_database() { // TODO
  String sqlArray[] = {

  };

  if (open_db(db_path, &fs_db))
    return;
  for (int i = 0; i < sizeof(sqlArray)/sizeof(sqlArray[0]); i++) {
    db_exec_prep(sqlArray[i].c_str());
  }
  sqlite3_close(fs_db);
  SerialMon.println(F("[CONFIG] Database Loaded"));
}

void handle_config_preferences() { // done
  preferences.begin("module-config", false);
  if (preferences.isKey("wifi_hostname")) {
    wifi_hostname = preferences.getString("wifi_hostname");
  } else {
    preferences.putString("wifi_hostname", default_hostname());
  }
  if (preferences.isKey("device_username")) {
    device_admin_username = preferences.getString("device_username");
  } else {
    preferences.putString("device_username", DEFAULT_DEVICE_ADMIN_USERNAME);
  }
  if (preferences.isKey("device_password")) {
    device_admin_password = preferences.getString("device_password");
  } else {
    preferences.putString("device_password", DEFAULT_DEVICE_ADMIN_PASSWORD);
  }
  if (preferences.isKey("ntp_server")) {
    ntp_server = preferences.getString("ntp_server");
  } else {
    preferences.putString("ntp_server", DEFAULT_NTP_SERVER);
  }
  if (preferences.isKey("time_zone")) {
    timezone = preferences.getString("time_zone");
  } else {
    preferences.putString("time_zone", DEFAULT_TIMEZONE);
  }
  if (preferences.isKey("tz_name")) {
    timezone_name = preferences.getString("tz_name");
  } else {
    preferences.putString("tz_name", DEFAULT_TIMEZONE_NAME);
  }
  if (preferences.isKey("mqtt_server")) {
    mqtt_server = preferences.getString("mqtt_server");
  } else {
    preferences.putString("mqtt_server", DEFAULT_MQTT_SERVER);
  }
  if (preferences.isKey("mqtt_port")) {
    mqtt_port = preferences.getInt("mqtt_port");
  } else {
    preferences.putInt("mqtt_port", DEFAULT_MQTT_PORT);
  }
  if (preferences.isKey("mqtt_cred")) {
    mqtt_credentials = preferences.getBool("mqtt_cred");
  } else {
    preferences.putBool("mqtt_cred", DEFAULT_MQTT_CREDENTIALS);
  }
  if (preferences.isKey("mqtt_user")) {
    mqtt_user = preferences.getString("mqtt_user");
  } else {
    preferences.putString("mqtt_user", DEFAULT_MQTT_USER);
  }
  if (preferences.isKey("mqtt_pass")) {
    mqtt_pass = preferences.getString("mqtt_pass");
  } else {
    preferences.putString("mqtt_pass", DEFAULT_MQTT_PASS);
  }
  if (preferences.isKey("ssl_use")) {
    ssl_use = preferences.getBool("ssl_use");
  } else {
    preferences.putBool("ssl_use", DEFAULT_SSL_USE);
  }
  if (preferences.isKey("ssl_root_ca")) {
    ssl_root_ca = preferences.getString("ssl_root_ca");
  } else {
    preferences.putString("ssl_root_ca", DEFAULT_SSL_CA_CERTIFICATE);
  }
  if (preferences.isKey("ssl_validate_cl")) { // ?
    ssl_validate_client = preferences.getBool("ssl_validate_cl");
  } else {
    preferences.putBool("ssl_validate_cl", DEFAULT_SSL_VALIDATE_CLIENT);
  }
  if (preferences.isKey("ssl_client_cert")) { // ?
    ssl_client_certificate = preferences.getString("ssl_client_cert");
  } else {
    preferences.putString("ssl_client_cert", DEFAULT_SSL_CLIENT_CERTIFICATE);
  }
  if (preferences.isKey("ssl_client_priv")) { // ?
    ssl_client_private_key = preferences.getString("ssl_client_priv");
  } else {
    preferences.putString("ssl_client_priv", DEFAULT_SSL_CLIENT_PRIVATE_KEY);
  }
  if (preferences.isKey("gnss_mode")) {
    gnss_mode = preferences.getInt("gnss_mode");
  } else {
    preferences.putInt("gnss_mode", DEFAULT_GNSS_MODE);
  }
  if (preferences.isKey("gprs_apn")) {
    gprs_apn = preferences.getString("gprs_apn");
  } else {
    preferences.putString("gprs_apn", DEFAULT_GPRS_APN);
  }
  if (preferences.isKey("gprs_cred")) {
    gprs_credentials = preferences.getBool("gprs_cred");
  } else {
    preferences.putBool("gprs_cred", DEFAULT_GPRS_CREDENTIALS);
  }
  if (preferences.isKey("gprs_user")) {
    gprs_user = preferences.getString("gprs_user");
  } else {
    preferences.putString("gprs_user", DEFAULT_GPRS_USER);
  }
  if (preferences.isKey("gprs_pass")) {
    gprs_pass = preferences.getString("gprs_pass");
  } else {
    preferences.putString("gprs_pass", DEFAULT_GPRS_PASS);
  }
  if (preferences.isKey("last_lat")) {
    gps_last_lat = preferences.getFloat("last_lat");
  } else {
    preferences.putFloat("last_lat", 0.0);
  }
  if (preferences.isKey("last_lon")) {
    gps_last_lon = preferences.getFloat("last_lon");
  } else {
    preferences.putFloat("last_lon", 0.0);
  }
  if (preferences.isKey("last_alt")) {
    gps_last_alt = preferences.getFloat("last_alt");
  } else {
    preferences.putFloat("last_alt", 0.0);
  }
  if (preferences.isKey("last_speed")) {
    gps_last_speed = preferences.getFloat("last_speed");
  } else {
    preferences.putFloat("last_speed", 0.0);
  }
  if (preferences.isKey("last_accuracy")) {
    gps_last_accuracy = preferences.getFloat("last_accuracy");
  } else {
    preferences.putFloat("last_accuracy", 0.0);
  }
  if (preferences.isKey("last_datetime")) {
    gps_last_datetime = DateTime(preferences.getString("last_datetime").c_str());
  } else {
    preferences.putString("last_datetime", DateTime(2000,1,1,0,0,0).timestamp());
  }
  if (preferences.isKey("hw_reboot")) {
    hw_reboot = preferences.getBool("hw_reboot");
  } else {
    preferences.putBool("hw_reboot", DEFAULT_HW_REBOOT);
  }
  if (preferences.isKey("setup_done")) {
    is_setup_done = preferences.getBool("setup_done");
  } else {
    preferences.putBool("setup_done", DEFAULT_IS_SETUP_DONE);
  }
  preferences.end();
  SerialMon.println(F("[CONFIG] Preferences Loaded"));
}

void setup_file_system() { // done
  if (!LittleFS.begin(FORMAT_FS_IF_FAILED)) {
    SerialMon.println(F("[FS] File System Mount Failed"));
    while(1);
  } else {
    SerialMon.println(F("[FS] File System Mounted"));
    // LittleFS.remove("/database.db");
    SerialMon.print(F("[FS] Usage: "));
    SerialMon.print(filesystemPercentageUsage());
    SerialMon.print(F(" % ("));
    SerialMon.print(LittleFS.usedBytes());
    SerialMon.print(F(" / "));
    SerialMon.print(LittleFS.totalBytes());
    SerialMon.println(F(" bytes)"));
    handle_config_preferences();
    handle_database();
  }
}

// ----------------------------------------------------------------------------------------------------------------

void handle_sw_reboot() { // done
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  SerialMon.println(F("[BOOT] Restarting"));
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  ESP.restart();
}

void handle_hw_reboot() { // done
  SerialMon.println(F("[BOOT] Performing hardware reboot"));
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  digitalWrite(ESP_RST, LOW);
}

const char *handle_post_json(DynamicJsonDocument json_doc) { // ? - proper validation of certain fields
  preferences.begin("module-config", false);
  if (json_doc.size() > 0) {
    if (json_doc.containsKey("hostname")) { // value is provided
      String json_hostname = json_doc["hostname"]; // default to ""
      json_hostname.trim();
      if (json_hostname.length() > 0) { // value is not default

        // ? - add validation for correct format

        if (json_hostname != wifi_hostname) {
          preferences.putString("wifi_hostname", json_hostname);
        }
      } else { // ?
        preferences.end();
        return "Change of device hostname was requested, but the value of hostname field is invalid!";
      }
    }
    if (json_doc.containsKey("device_password_change")) { // value is provided
      bool json_device_password_change = json_doc["device_password_change"]; // default to false
      if (json_device_password_change) {
        if (json_doc.containsKey("device_password")) { // value is provided
          String json_device_password = json_doc["device_password"]; // default to ""
          json_device_password.trim();
          if (json_device_password.length() > 0) { // value is not default

            // ? - add validation for correct format

            if (json_device_password != device_admin_password) {
              preferences.putString("device_password", json_device_password);
            }
          } else { // ?
            preferences.end();
            return "Device password change was requested, but the value of device password field is invalid!";
          }
        } else { // ?
          preferences.end();
          return "Device password change was requested, but the device password field is missing!";
        }
      }
    }
    if (json_doc.containsKey("mqtt_server")) { // value is provided
      String json_mqtt_server = json_doc["mqtt_server"]; // default to ""
      json_mqtt_server.trim();
      if (json_mqtt_server.length() > 0) { // value is not default
        if (json_mqtt_server != mqtt_server) {
          preferences.putString("mqtt_server", json_mqtt_server);
        }
      } else { // ?
        preferences.end();
        return "Change of mqtt server was requested, but the value of mqtt server field is invalid!";
      }
    }
    if (json_doc.containsKey("mqtt_port")) { // value is provided
      int json_mqtt_port = json_doc["mqtt_port"]; // default to 0
      if (json_mqtt_port > 0) { // value is not default

        // ? - add validation for correct format

        if (json_mqtt_port != mqtt_port) {
          preferences.putInt("mqtt_port", json_mqtt_port);
        }
      } else { // ?
        preferences.end();
        return "Change of mqtt port was requested, but the value of mqtt port field is invalid!";
      }
    }
    if (json_doc.containsKey("mqtt_credentials")) { // value is provided // TODO
      bool json_mqtt_credentials = json_doc["mqtt_credentials"]; // default to false
      if (json_mqtt_credentials) {
        if (json_doc.containsKey("mqtt_username")) {
          String json_mqtt_username = json_doc["mqtt_username"]; // default to ""
          json_mqtt_username.trim();
          if (json_mqtt_username.length() > 0) {
            if (mqtt_pass != DEFAULT_MQTT_PASS && mqtt_pass.length() > 0) { // some password set -> check if password change was requested
              if (json_doc.containsKey("mqtt_password_change")) {
                bool json_mqtt_password_change = json_doc["mqtt_password_change"]; // default to false
                if (json_mqtt_password_change) {
                  if (json_doc.containsKey("mqtt_password")) {
                    String json_mqtt_password = json_doc["mqtt_password"]; // default to ""
                    json_mqtt_password.trim();
                    if (json_mqtt_password.length() > 0) {
                      if (!mqtt_credentials) {
                        preferences.putBool("mqtt_cred", true);
                      }
                      if (json_mqtt_username != mqtt_user) {
                        preferences.putString("mqtt_user", json_mqtt_username);
                      }
                      if (json_mqtt_password != mqtt_pass) {
                        preferences.putString("mqtt_pass", json_mqtt_password);
                      }
                    } else {
                      preferences.end();
                      return "Mqtt password change was requested, but the value of mqtt password field is invalid!";
                    }
                  } else {
                    preferences.end();
                    return "Mqtt password change was requested, but the mqtt password field is missing!";
                  }
                }
              }
            } else { // password not set -> enforce mqtt_pass
              if (json_doc.containsKey("mqtt_password")) {
                String json_mqtt_password = json_doc["mqtt_password"]; // default to ""
                json_mqtt_password.trim();
                if (json_mqtt_password.length() > 0) {
                  if (!mqtt_credentials) {
                    preferences.putBool("mqtt_cred", true);
                  }
                  if (json_mqtt_username != mqtt_user) {
                    preferences.putString("mqtt_user", json_mqtt_username);
                  }
                  if (json_mqtt_password != mqtt_pass) {
                    preferences.putString("mqtt_pass", json_mqtt_password);
                  }
                } else { // ?
                  preferences.end();
                  return "Set mqtt server credentials was requested, but the value of mqtt password field is invalid!";
                }
              } else {
                preferences.end();
                return "Set mqtt server credentials was requested, but the mqtt password field is missing!";
              }
            }
          } else { // ?
            preferences.end();
            return "Set mqtt server credentials was requested, but the value of mqtt username field is invalid!";
          }
        } else {
          preferences.end();
          return "Set mqtt server credentials was requested, but the mqtt username field is missing!";
        }
      } else {
        if (mqtt_credentials) {
          preferences.putBool("mqtt_cred", false);
        }
        if (String(DEFAULT_MQTT_USER) != mqtt_user) {
          preferences.putString("mqtt_user", DEFAULT_MQTT_USER);
        }
        if (String(DEFAULT_MQTT_PASS) != mqtt_pass) {
          preferences.putString("mqtt_pass", DEFAULT_MQTT_PASS);
        }
      }
    }
    if (json_doc.containsKey("ntp")) { // value is provided
      String json_ntp = json_doc["ntp"]; // default to ""
      json_ntp.trim();
      if (json_ntp.length() > 0) { // value is not default
        if (json_ntp != ntp_server) {
          preferences.putString("ntp_server", json_ntp);
        }
      } else { // ?
        preferences.end();
        return "Change of ntp server was requested, but the value is invalid!";
      }
    }
    if (json_doc.containsKey("gnss_mode")) { // value is provided
      int json_gnss_mode = json_doc["gnss_mode"]; // default to 0
      if (json_gnss_mode > 0) { // value is not default

        // ? - add validation for correct format

        if (json_gnss_mode != gnss_mode) {
          preferences.putInt("gnss_mode", json_gnss_mode);
        }
      } else { // ?
        preferences.end();
        return "Change of gnss mode was requested, but the value of mqtt port field is invalid!";
      }
    }
    if (json_doc.containsKey("gprs_apn")) { // value is provided
      String json_gprs_apn = json_doc["gprs_apn"]; // default to ""
      json_gprs_apn.trim();
      if (json_gprs_apn.length() > 0) { // value is not default
        if (json_gprs_apn != gprs_apn) {
          preferences.putString("gprs_apn", json_gprs_apn);
        }
      } else { // ?
        preferences.end();
        return "Change of apn was requested, but the value is invalid!";
      }
    }
    if (json_doc.containsKey("gprs_credentials")) { // value is provided // TODO
      bool json_gprs_credentials = json_doc["gprs_credentials"]; // default to false
      if (json_gprs_credentials) {
        if (json_doc.containsKey("gprs_username")) {
          String json_gprs_username = json_doc["gprs_username"]; // default to ""
          json_gprs_username.trim();
          if (json_gprs_username.length() > 0) {
            if (gprs_pass != DEFAULT_GPRS_PASS && gprs_pass.length() > 0) { // some password set -> check if password change was requested
              if (json_doc.containsKey("gprs_password_change")) {
                bool json_gprs_password_change = json_doc["gprs_password_change"]; // default to false
                if (json_gprs_password_change) {
                  if (json_doc.containsKey("gprs_password")) {
                    String json_gprs_password = json_doc["gprs_password"]; // default to ""
                    json_gprs_password.trim();
                    if (json_gprs_password.length() > 0) {
                      if (!gprs_credentials) {
                        preferences.putBool("gprs_cred", true);
                      }
                      if (json_gprs_username != gprs_user) {
                        preferences.putString("gprs_user", json_gprs_username);
                      }
                      if (json_gprs_password != gprs_pass) {
                        preferences.putString("gprs_pass", json_gprs_password);
                      }
                    } else {
                      preferences.end();
                      return "Gprs password change was requested, but the value of gprs password field is invalid!";
                    }
                  } else {
                    preferences.end();
                    return "Gprs password change was requested, but the gprs password field is missing!";
                  }
                }
              }
            } else { // password not set -> enforce gprs_pass
              if (json_doc.containsKey("gprs_password")) {
                String json_gprs_password = json_doc["gprs_password"]; // default to ""
                json_gprs_password.trim();
                if (json_gprs_password.length() > 0) {
                  if (!gprs_credentials) {
                    preferences.putBool("gprs_cred", true);
                  }
                  if (json_gprs_username != gprs_user) {
                    preferences.putString("gprs_user", json_gprs_username);
                  }
                  if (json_gprs_password != gprs_pass) {
                    preferences.putString("gprs_pass", json_gprs_password);
                  }
                } else { // ?
                  preferences.end();
                  return "Set gprs credentials was requested, but the value of gprs password field is invalid!";
                }
              } else {
                preferences.end();
                return "Set gprs credentials was requested, but the gprs password field is missing!";
              }
            }
          } else {
            preferences.end();
            return "Set gprs credentials was requested, but the value of gprs username field is invalid!";
          }
        } else {
          preferences.end();
          return "Set gprs credentials was requested, but the gprs username field is missing!";
        }
      } else {
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
    }
    if (json_doc.containsKey("ssl_use")) { // value is provided // TODO
      bool json_ssl_use = json_doc["ssl_use"]; // default to false
      if (json_ssl_use) {
        if (json_doc.containsKey("ssl_root_ca")) { // value is provided
          String json_ssl_root_ca = json_doc["ssl_root_ca"]; // default to ""
          json_ssl_root_ca.trim();
          if (json_ssl_root_ca.length() > 0) { // value is not default
            if (json_doc.containsKey("ssl_validate_client")) {
              bool json_ssl_validate_client = json_doc["ssl_validate_client"]; // default to false
              if (json_ssl_validate_client) {
                if (json_doc.containsKey("ssl_client_certificate")) {
                  String json_ssl_client_certificate = json_doc["ssl_client_certificate"]; // default to ""
                  json_ssl_client_certificate.trim();
                  if (json_ssl_client_certificate.length() > 0) { // value is not default
                    if (json_doc.containsKey("ssl_client_private_key")) {
                      String json_ssl_client_private_key = json_doc["ssl_client_private_key"]; // default to ""
                      json_ssl_client_private_key.trim();
                      if (json_ssl_client_private_key.length() > 0) { // value is not default
                        if (!ssl_use) {
                          preferences.putBool("ssl_use", true); // ?
                        }
                        if (json_ssl_root_ca != ssl_root_ca) {
                          preferences.putString("ssl_root_ca", json_ssl_root_ca); // ?
                        }
                        if (!ssl_validate_client) {
                          preferences.putBool("ssl_validate_cl", true); // ?
                        }
                        if (json_ssl_client_certificate != ssl_client_certificate) {
                          preferences.putString("ssl_client_cert", json_ssl_client_certificate); // ?
                        }
                        if (json_ssl_client_private_key != ssl_client_private_key) {
                          preferences.putString("ssl_client_priv", json_ssl_client_private_key); // ?
                        }
                      } else {
                        preferences.end();
                        return "Validation of tls client certificate was requested, but the value of ssl client certificate private key field is invalid!";
                      }
                    } else {
                      preferences.end();
                      return "Validation of tls client certificate was requested, but the ssl client certificate private key field is missing!";
                    }
                  } else {
                    preferences.end();
                    return "Validation of tls client certificate was requested, but the value of ssl client certificate field is invalid!";
                  }
                } else {
                  preferences.end();
                  return "Validation of tls client certificate was requested, but the ssl client certificate field is missing!";
                }
              } else {
                if (!ssl_use) {
                  preferences.putBool("ssl_use", true); // ?
                }
                if (json_ssl_root_ca != ssl_root_ca) {
                  preferences.putString("ssl_root_ca", json_ssl_root_ca); // ?
                }
                if (ssl_validate_client) { // ?
                  preferences.putBool("ssl_validate_cl", false);
                }
                if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != ssl_client_certificate) { // ?
                  preferences.putString("ssl_client_cert", DEFAULT_SSL_CLIENT_CERTIFICATE);
                }
                if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != ssl_client_private_key) { // ?
                  preferences.putString("ssl_client_priv", DEFAULT_SSL_CLIENT_PRIVATE_KEY);
                }
              }
            } else {
              // ? - what if field is missing ? -> enable tls, disable client validation || enable tls, do not modify client validation
              if (!ssl_use) {
                preferences.putBool("ssl_use", true); // ?
              }
              if (json_ssl_root_ca != ssl_root_ca) {
                preferences.putString("ssl_root_ca", json_ssl_root_ca); // ?
              }
              if (ssl_validate_client) { // ?
                preferences.putBool("ssl_validate_cl", false);
              }
              if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != ssl_client_certificate) { // ?
                preferences.putString("ssl_client_cert", DEFAULT_SSL_CLIENT_CERTIFICATE);
              }
              if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != ssl_client_private_key) { // ?
                preferences.putString("ssl_client_priv", DEFAULT_SSL_CLIENT_PRIVATE_KEY);
              }
            }
          } else { // ?
            preferences.end();
            return "Tls was requested, but the value of ssl root certificate field is invalid!";
          }
        } else { // ?
          preferences.end();
          return "Tls was requested, but the ssl root certificate field is missing!";
        }
      } else {
        if (ssl_use) {
          preferences.putBool("ssl_use", false); // ?
        }
        if (String(DEFAULT_SSL_CA_CERTIFICATE) != ssl_root_ca) {
          preferences.putString("ssl_root_ca", DEFAULT_SSL_CA_CERTIFICATE); // ?
        }
        if (ssl_validate_client) { // ?
          preferences.putBool("ssl_validate_cl", false);
        }
        if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != ssl_client_certificate) { // ?
          preferences.putString("ssl_client_cert", DEFAULT_SSL_CLIENT_CERTIFICATE);
        }
        if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != ssl_client_private_key) { // ?
          preferences.putString("ssl_client_priv", DEFAULT_SSL_CLIENT_PRIVATE_KEY);
        }
      }
    }
    if (!is_setup_done) {
      preferences.putBool("setup_done", true);
    }
    preferences.putBool("hw_reboot", true);
    preferences.end();
    return "OK";
  } else {
    preferences.end();
    return "Json with empty body provided!";
  }
}

void handle_web_server_favicon_get() { // done
  web_server.send_P(200, "image/x-icon", favicon, sizeof(favicon));
}

void handle_web_server_css_get() { // done
  web_server.send_P(200, "text/css", main_css);
}

void handle_web_server_js_get() { // done
  web_server.send_P(200, "text/javascript", main_js);
}

void handle_web_server_html_get() { // done
  // if (!web_server.authenticate(device_admin_username.c_str(), device_admin_password.c_str())) return web_server.requestAuthentication(web_server_auth_mode, "Login Required", "Authentication Failed"); // TODO
  // if (!web_server.authenticate(device_admin_username.c_str(), device_admin_password.c_str())) return web_server.requestAuthentication(); // TODO
  web_server.send_P(200, "text/html", main_html);
}

void handle_web_server_config_get() { // TODO
  DynamicJsonDocument json_doc(12288); // 8192
  json_doc["module_type"] = MODULE_TYPE;
  json_doc["firmware"] = MODULE_FIRMWARE_VERSION;
  json_doc["unit_id"] = mac_address_nag;
  json_doc["mac_address"] = mac_address_raw;
  json_doc["wifi_hostname"] = wifi_hostname;
  json_doc["mqtt_server"] = mqtt_server;
  json_doc["mqtt_port"] = mqtt_port;
  json_doc["mqtt_credentials"] = mqtt_credentials;
  json_doc["mqtt_user"] = mqtt_user;
  json_doc["mqtt_pass_set"] = (mqtt_pass != DEFAULT_MQTT_PASS && mqtt_pass.length() > 0);
  json_doc["ssl_use"] = ssl_use;
  json_doc["ssl_root_ca"] = ssl_root_ca;
  json_doc["ssl_validate_client"] = ssl_validate_client; // TODO
  json_doc["ssl_client_certificate"] = ssl_client_certificate; // TODO
  json_doc["ssl_client_private_key"] = ssl_client_private_key; // TODO
  json_doc["ntp"] = ntp_server;
  json_doc["gnss_mode"] = gnss_mode;
  json_doc["gprs_apn"] = gprs_apn;
  json_doc["gprs_credentials"] = gprs_credentials;
  json_doc["gprs_user"] = gprs_user;
  json_doc["gprs_pass_set"] = (gprs_pass != DEFAULT_GPRS_PASS && gprs_pass.length() > 0);
  String data_out;
  serializeJson(json_doc, data_out);
  web_server.send(200, "application/json", data_out);
}

void handle_web_server_config_post() { // done
  if (web_server.hasArg("plain")) {
    String data = web_server.arg("plain");
    DynamicJsonDocument json_doc(12288); // 8192
    DeserializationError json_des_error = deserializeJson(json_doc, data);
    if (!json_des_error) {
      const char * result = handle_post_json(json_doc);
      if (result != "OK") {
        web_server.send(500, "text/plain", result);
      } else {
        web_server.send(200, "text/plain", "Successfully saved! Rebooting!");
        play_config_save_beep(); // TODO
        handle_sw_reboot();
      }
    } else {
      web_server.send(500, "text/plain", json_des_error.c_str());
    }
  } else {
    web_server.send(500, "text/plain", "No payload provided!");
  }
}

void setup_web_server() { // done
  web_server.on("/favicon.ico", HTTP_GET, handle_web_server_favicon_get); // public
  web_server.on("/main.css", HTTP_GET, handle_web_server_css_get); // public
  web_server.on("/main.js", HTTP_GET, handle_web_server_js_get); // public
  web_server.on("/config", HTTP_POST, handle_web_server_config_post); // public
  web_server.on("/config", HTTP_GET, handle_web_server_config_get); // protected
  web_server.on("/", HTTP_GET, handle_web_server_html_get); // protected
  web_server.onNotFound(handle_web_server_html_get);
  web_server.begin();
  SerialMon.println(F("[WEB] Success"));
}

void setup_captive_portal() { // done
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(wifi_ap_ip, wifi_ap_ip, wifi_ap_mask);
  WiFi.softAP(wifi_hostname.c_str(), NULL, 1, false, 1);
  SerialMon.print(F("[AP] Created with IP Gateway "));
  SerialMon.println(WiFi.softAPIP());
  dns_server.setErrorReplyCode(DNSReplyCode::NoError); // ?
  dns_server.start(53, "*", wifi_ap_ip);
  setup_web_server();
}

void handle_captive_portal() { // done
  dns_server.processNextRequest();
  web_server.handleClient();
}

void switch_to_config_mode() { // done
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  SerialMon.println(F("[CONFIG] Switching to config mode"));
  preferences.begin("module-config", false);
  preferences.putBool("setup_done", false);
  preferences.end();
  handle_sw_reboot();
}

void config_button_handler() { // done // TODO - debounce not working !!!
  int config_button_current_state = digitalRead(ESP_CONFIG_BUTTON);
  if (config_button_current_state != config_button_last_state) {
    if ((current_millis - config_button_last_time) >= config_button_debounce) {
      if (config_button_current_state == LOW) {
        if (!config_button_pressed) {
          SerialMon.println(F("[CONFIG] BUTTON PRESS")); // TODO - remove
          config_button_pressed = true;
        }
      } else {
        if (config_button_pressed) {
          SerialMon.println(F("[CONFIG] BUTTON RELEASE")); // TODO - remove
          config_button_active = false;
          config_button_pressed = false;
        }
      }
      config_button_last_state = config_button_current_state;
      config_button_last_time = current_millis;
    }
  } else {
    if (config_button_current_state == LOW) {
      if  (!config_button_active) {
        if (config_button_pressed) {
          if ((current_millis - config_button_last_time) > config_button_period) {
            SerialMon.println(F("[CONFIG] LONG PRESS")); // TODO - remove
            config_button_active = true;
            switch_to_config_mode();
          }
        }
      }
    }
  }
}

void handle_on_boot_config_button() { // TODO
  if (digitalRead(ESP_CONFIG_BUTTON) == LOW) {
    switch_to_config_mode();
  }
}

// ----------------------------------------------------------------------------------------------------------------

void handle_obd_reader() { // TODO - either use for card reader or obd scanner
  if (SerialOBD.available()) { // TODO
    // SerialMon.write(SerialOBD.read());
    driver_card = SerialOBD.readString();
    SerialMon.println("[DEBUG] READER Card: " + driver_card);
    play_on_card_beep();
  }
}

void setup_obd_reader() { // TODO - either use for card reader or obd scanner
  SerialOBD.begin(OBD_BAUD_RATE, SWSERIAL_8N1, ESP_RX_OBD, ESP_TX_OBD); // SerialOBD.begin(OBD_BAUD_RATE, SWSERIAL_8N1, ESP_RX_OBD, ESP_TX_OBD, false);
  if (!SerialOBD) {
    SerialMon.println(F("[OBD] Error"));
  } else {
    SerialMon.println(F("[OBD] Success"));
  }
}

// ----------------------------------------------------------------------------------------------------------------

void on_card_reader_receive() { // TODO
  if (is_setup_done && !hw_reboot) {
    if (SerialCardReader.available()) {
      driver_card = SerialCardReader.readString();
      SerialMon.println("[DEBUG] READER Card: " + driver_card);
      play_on_card_beep();
    }
  }
}

void setup_card_reader() { // done
  SerialCardReader.begin(CARD_READER_BAUD_RATE, SERIAL_8N1, ESP_RX_CARD_READER, ESP_TX_CARD_READER);
  // SerialCardReader.onReceive(on_card_reader_receive);
  // SerialMon.println(F("[READER] Card Reader Success"));
  if (!SerialCardReader) {
    SerialMon.println(F("[READER] Error"));
  } else {
    SerialCardReader.onReceive(on_card_reader_receive);
    // TODO - power on reader -> ESP_PWR_CARD_READER
    SerialMon.println(F("[READER] Success"));
  }
}

void start_ride() { // TODO

  // TODO

  // TODO - power handling of card reader using mosfet - TURN ON

}

void stop_ride() { // TODO

  // TODO

  // TODO - power handling of card reader using mosfet - TURN OFF

  driver_card = "";

}

void handle_tls() { // done
  if (ssl_use) {
    ssl_client.setCACert(ssl_root_ca.c_str());
    SerialMon.println(F("[TLS] SSL is ON - accepting only traffic validated by provided CA certificate/s"));
    if (ssl_validate_client) {
      ssl_client.setCertificate(ssl_client_certificate.c_str());
      ssl_client.setPrivateKey(ssl_client_private_key.c_str());
      SerialMon.println(F("[TLS] Client certificate validation is ON - server will be validating provided client certificate"));
    }
  } else {
    SerialMon.println(F("[TLS] SSL is OFF - disabling TLS"));
  }
}

// void send_sms(String recipient, String message) {
//   bool sms_send = modem.sendSMS(recipient, message);
//   SerialMon.print("[GSM] SMS: ");
//   SerialMon.println(sms_send ? "Sent" : "Failed");
// }

void setup_gsm_module() { // done
  digitalWrite(GSM_MODULE_POWER, HIGH); // POWER_PIN: This pin controls the power supply of the SIM7600
  digitalWrite(GSM_MODULE_PWR, HIGH); // PWR_PIN：This Pin is the PWR-KEY of the SIM7600, The time of active low level impulse of PWRKEY pin to power on module , type 500 ms
  delay(500);
  digitalWrite(GSM_MODULE_PWR, LOW);
  attachInterrupt(GSM_MODULE_INT, []() { // If SIM7600 starts normally, then set the onboard LED to flash once every 1 second
    detachInterrupt(GSM_MODULE_INT);
    led_ticker.attach_ms(1000, []() {
      digitalWrite(ESP_LED, !digitalRead(ESP_LED));
    });
  }, RISING);

  delay(3000); // ?

  SerialAT.begin(GSM_MODULE_UART_BAUD, SERIAL_8N1, GSM_MODULE_RX, GSM_MODULE_TX);

  handle_tls();

  if (digitalRead(GSM_MODULE_INT) == 0) {
    SerialMon.println(F("[GSM] Waiting For Power On..."));
  } else {
    detachInterrupt(GSM_MODULE_INT);
    led_ticker.attach_ms(1000, []() {
      digitalWrite(ESP_LED, !digitalRead(ESP_LED));
    });
  }

  while(digitalRead(GSM_MODULE_INT) == 0) {
    delay(1000); // Waiting for power on
  }

  SerialMon.print(F("[GSM] Initializing modem: "));
  if (modem.init()) {
    SerialMon.println(F("OK"));
  } else {
    SerialMon.println(F("failed"));
  }

  SerialMon.print(F("[GSM] Setting Network Mode: "));
  if (modem.setNetworkMode(DEFAULT_NETWORK_MODE)) {
    SerialMon.println(F("OK"));
  } else {
    SerialMon.println(F("failed"));
  }

  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    SerialMon.print(F("[GSM] Unlocking SIM: "));
    if (modem.simUnlock(GSM_PIN)) {
      SerialMon.println(F("OK"));
    } else {
      SerialMon.println(F("failed"));
    }
  }

  SerialMon.print(F("[GSM] Setting GNSS Mode: "));
  String gnss_res = modem.setGNSSMode(gnss_mode, 1); // String gnss_res = modem.setGNSSMode(gnss_mode, 0);
  if (gnss_res == "OK") {
    SerialMon.println(F("OK"));
  } else {
    SerialMon.println(F("failed"));
  }

  SerialMon.print(F("[GSM] Enabling GPS: "));
  if(modem.enableGPS()) {
    SerialMon.println(F("OK"));
  } else {
    SerialMon.println(F("failed"));
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

  if (!hw_reboot) { // TODO
    SerialMon.print(F("[GSM] Waiting for network: "));
    if (modem.waitForNetwork()) {
      SerialMon.println(F("OK"));
    } else {
      delay(10000); // TODO
      SerialMon.println(F("failed"));
      return; // TODO
    }

    /*bool*/ is_network_connected = modem.isNetworkConnected();
    SerialMon.print(F("[GSM] Network status: "));
    SerialMon.println(F(is_network_connected ? "connected" : "not connected"));

    // modem.NTPServerSync(ntp_server, 0); // ?

    SerialMon.print("[GSM] Starting GPRS (apn: " + gprs_apn);
    SerialMon.print(", user: " + gprs_user);
    SerialMon.print(", pass: " + gprs_pass);
    SerialMon.print("): ");
    if (modem.gprsConnect(gprs_apn.c_str(), gprs_user.c_str(), gprs_pass.c_str())) {
      SerialMon.println(F("OK"));
    } else {
      delay(10000); // TODO
      SerialMon.println(F("failed"));
      return; // TODO
    }

    /*bool*/ is_gprs_connected = modem.isGprsConnected();
    SerialMon.print(F("[GSM] GPRS status: "));
    SerialMon.println(F(is_gprs_connected ? "connected" : "not connected"));

    IPAddress local = modem.localIP();
    SerialMon.println("[GSM] Local IP: " + String(local));
    String imsi = modem.getIMSI();
    SerialMon.println("[GSM] IMSI: " + imsi);
    String ccid = modem.getSimCCID();
    SerialMon.println("[GSM] CCID: " + ccid);

    // sendSMS("+420xxxxxxxxx", String("Hello from ") + imei);

    // String time = modem.getGSMDateTime(DATE_FULL);
    // SerialMon.println("[DEBUG] Current Network Time: " + time);
  }
  SerialMon.println(F("[GSM] Success"));
}

// ----------------------------------------------------------------------------------------------------------------

void publish_mqtt_birth_message() { // done
  if (!mqtt.publish((mqtt_base_topic + "/status").c_str(), MQTT_AVAILABILITY_BIRTH, true)) {
    SerialMon.println(F("[MQTT] Failed to publish birth message"));
  }
}

void publish_mqtt_init_messages() { // ?
  if (mqtt.connected()) {
    mqtt.publish((mqtt_base_topic + "/storage").c_str(), filesystemUsage().c_str(), true /*false*/);

    // ?

  }
}

void handle_mqtt_message(String topic, String payload) { // TODO - subcribed topics handlers
  if (topic == (mqtt_base_topic + "/status")) {
    if (payload != MQTT_AVAILABILITY_BIRTH) { // if lwt incoming with status != online by accident, rebroadcast online status
      publish_mqtt_birth_message();
    }
  }

  // if (topic == (mqtt_base_topic + "/db/card/request")) {
  //   if (payload.length() > 0) {
  //     db_card_handler(payload);
  //   }
  // }

  // TODO

}

void mqtt_callback(char* topic, byte* payload, unsigned int length) { // ? - cleanup
  SerialMon.print(F("[MQTT] Message arrived (")); // ?
  SerialMon.print(topic); // ?
  SerialMon.println(F(")")); // ?
  for (int i = 0; i < length; i++) { // ?
    SerialMon.print((char)payload[i]); // ?
  } // ?
  SerialMon.println(); // ?

  String topic_to_process = topic;
  payload[length] = '\0';
  String payload_to_process = (char*)payload;
  handle_mqtt_message(topic_to_process, payload_to_process);
}

void setup_mqtt() { // done
  if (ssl_use) {
    mqtt.setClient(ssl_client);
  } else {
    mqtt.setClient(client);
  }
  mqtt.setServer(mqtt_server.c_str(), mqtt_port);
  mqtt.setCallback(mqtt_callback);
  mqtt.setBufferSize(12288); // 8192
  SerialMon.println(F("[MQTT] Success"));
}

String get_data_json(bool gps_signal) { // TODO
  DynamicJsonDocument json_doc(1024);
  json_doc["time"] = rtc.now().unixtime();
  JsonObject temp_data = json_doc.createNestedObject("temp");
  temp_data["int"] = round_2(rtc.getTemperature());
  // temp_data["ext"] = round2(20); // ?
  json_doc["gps_signal"] = gps_signal;
  JsonObject gps_data = json_doc.createNestedObject("gps");
  gps_data["lat"] = gps_last_lat; // gps_data["lat"] = serialized(String(gps_last_lat, 6));
  gps_data["lon"] = gps_last_lon; // gps_data["lon"] = serialized(String(gps_last_lon, 6));
  gps_data["alt"] = round_2(gps_last_alt); // gps_data["alt"] = serialized(String(gps_last_alt, 2));
  gps_data["speed"] = round_2(non_negative(gps_last_speed)); // gps_data["speed"] = serialized(String(gps_last_speed, 2));
  gps_data["accuracy"] = round_2(gps_last_accuracy); // gps_data["accuracy"] = serialized(String(gps_last_accuracy, 2));
  gps_data["time"] = gps_last_datetime.unixtime();
  // JsonObject car_data = json_doc.createNestedObject("car");
  // car_data["val1"] = "TODO";
  // car_data["val2"] = "TODO";
  // car_data["val3"] = "TODO";
  String data_out;
  serializeJson(json_doc, data_out);
  return data_out;
}

void publish_realtime_data(String data_out) { // TODO
  if (mqtt.connected()) {
    if (!mqtt.publish((mqtt_base_topic + "/realtime").c_str(), data_out.c_str(), true /*false*/)) {
      return;
    }
  } else {
    // TODO - save to DB
    SerialMon.println(F("[DEBUG] mqtt not connected or failed to publish realtime data"));
    SerialMon.println(data_out.c_str());
  }
}

void connect_mqtt() { // TODO - subscribe to topics
  if (mqtt.connect(mac_address_nag.c_str(), mqtt_user.c_str(), mqtt_pass.c_str(), (mqtt_base_topic + "/status").c_str(), 0, true, MQTT_AVAILABILITY_LWT, MQTT_CLEAN_SESSION)) {
    SerialMon.println(F("[MQTT] Connected"));
    mqtt_check_last_time = 0;
    publish_mqtt_birth_message();
    publish_mqtt_init_messages();
    mqtt.subscribe((mqtt_base_topic + "/status").c_str());

    // TODO

    // TODO - subscribe to needed topics

  } else {
    SerialMon.print(F("[MQTT] Failed to connect ("));
    SerialMon.print(mqtt.state());
    SerialMon.println(F(")"));
  }
}

void handle_mqtt_connection() { // done
  if (!mqtt.connected()) {
    if (current_millis - mqtt_check_last_time >= mqtt_check_period) {
      mqtt_check_last_time = current_millis;
      connect_mqtt();
    }
  } else {
    mqtt.loop();
  }
}

// ----------------------------------------------------------------------------------------------------------------

// void handle_gsm_state() { // TODO
//   // if (!modem.isNetworkConnected()) { // network connection lost
//   // }
//   // if (!modem.isGprsConnected()) { // gprs internet disconnected
//   // }
// }

void get_gps_position() { // TODO
  int year, month, day, hour, minute, second;
  if (modem.getGPS(&gps_last_lat, &gps_last_lon, &gps_last_speed, &gps_last_alt, NULL, NULL, &gps_last_accuracy, &year, &month, &day, &hour, &minute, &second)) {
    gps_last_datetime = DateTime(year, month, day, hour, minute, second);
    preferences.begin("module-config", false);
    preferences.putFloat("last_lat", gps_last_lat);
    preferences.putFloat("last_lon", gps_last_lon);
    preferences.putFloat("last_alt", gps_last_alt);
    preferences.putFloat("last_speed", gps_last_speed);
    preferences.putFloat("last_accuracy", gps_last_accuracy);
    preferences.putString("last_datetime", gps_last_datetime.timestamp());
    preferences.end();
    rtc.adjust(gps_last_datetime); // ?
    publish_realtime_data(get_data_json(true));
    // TODO - publish car monitor data
  } else {
    publish_realtime_data(get_data_json(false));
    // TODO - publish car monitor data
  }
}

void handle_get_gps() { // done
  if (current_millis - gps_last_time >= gps_period) {
    gps_last_time = current_millis;
    get_gps_position();
  }
}

// ----------------------------------------------------------------------------------------------------------------

void core_0_task( void * parameter ) { // This is Core 0 // TODO
  for( ;; ){
    if (is_setup_done) {
      config_button_handler(); // ?
      // if (!hw_reboot) {
      // }
    } else {
      handle_captive_portal();
    }
  }
}

// ----------------------------------------------------------------------------------------------------------------

void setup() { // done
  SerialMon.begin(9600);
  SerialMon.println();
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  SerialMon.println("[Device Unit Type] " + String(MODULE_NAME) + " (" + String(MODULE_TYPE) + ")");
  SerialMon.println("[Firmware Version] " + String(MODULE_FIRMWARE_VERSION));
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  // ----------------------------------------------------------------------------------------------------------------
  setup_pins_esp();
  Wire.setPins(ESP_SDA, ESP_SCL);
  clear_wifi();
  setup_unit_variables();
  setup_file_system();
  sqlite3_initialize();
  setup_rtc();
  setup_mpu(); // TODO
  setup_card_reader();
  setup_obd_reader(); // TODO
  // ----------------------------------------------------------------------------------------------------------------
  if (is_setup_done) {
    handle_on_boot_config_button();
    led_ticker.attach_ms(500, []() {
      digitalWrite(ESP_LED, !digitalRead(ESP_LED));
    });
    setup_gsm_module(); // TODO
    if (!hw_reboot) {
      led_ticker.detach();
      digitalWrite(ESP_LED, LOW); // module LED -> ON
      setup_mqtt();
    }
  } else {
    play_config_entry_beep();
    led_ticker.attach_ms(200, []() {
      digitalWrite(ESP_LED, !digitalRead(ESP_LED));
      digitalWrite(ESP_PWR_LED, !digitalRead(ESP_PWR_LED)); // TODO
    });
    setup_captive_portal();
  }
  // ----------------------------------------------------------------------------------------------------------------
  SerialMon.println(F("[BOOT] Completed"));
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  // ----------------------------------------------------------------------------------------------------------------
  if (hw_reboot) {
    preferences.begin("module-config", false);
    preferences.putBool("hw_reboot", false);
    preferences.end();
    play_hw_reboot_beep(); // TODO
    handle_hw_reboot();
  }
  // ----------------------------------------------------------------------------------------------------------------,
  xTaskCreatePinnedToCore(core_0_task, "core_0_task", 16384, NULL, tskIDLE_PRIORITY, NULL, 0);
  // xTaskCreatePinnedToCore(core_0_task, "core_0_task", 10000, NULL, tskIDLE_PRIORITY, NULL, 0);
  // ----------------------------------------------------------------------------------------------------------------
}

// ----------------------------------------------------------------------------------------------------------------

void loop() { // This is Core 1
  current_millis = millis();
  is_network_connected = modem.isNetworkConnected();
  is_gprs_connected = modem.isGprsConnected();

  if (is_setup_done) {

    // config_button_handler(); // ?

    if (!hw_reboot) {
      // if (modem.isNetworkConnected()) { // network connected
      //   if (modem.isGprsConnected()) { // gprs connected
      if (is_network_connected) { // network connected
        if (is_gprs_connected) { // gprs connected
          // TODO - handle_mqtt_connection();
          handle_mqtt_connection();
        } else { // gprs disconnected
          // TODO - handle_gprs_connection();
          SerialMon.println("[DEBUG] GPRS Disconnected");
        }
      } else { // network not connected
        // TODO - handle_network_connection();
        SerialMon.println("[DEBUG] NETWORK Disconnected");
      }

      /*if (network_connected)*/
      // handle_mqtt_connection();

      handle_get_gps();

      // TODO
      // handle_gsm_state();

      handle_obd_reader(); // TODO

      // modem.maintain(); // TODO

    }
  }
  // else {
  //   handle_captive_portal(); // ?
  // }
}

// ----------------------------------------------------------------------------------------------------------------
