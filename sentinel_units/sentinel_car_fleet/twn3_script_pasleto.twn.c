#include <sys.twn.h>

const byte MAXIDBYTES = 8;
const byte MAXIDBITS = MAXIDBYTES*8;

byte ID[MAXIDBYTES];
byte IDBitCnt;
byte TagType;

byte LastID[MAXIDBYTES];
byte LastIDBitCnt;
byte LastTagType;

void main() {
    SetVolume(0);
    // Beep(BEEPNONE);
    Beep(BEEPSUCCESS); // Make some noise at startup at minimum volume
    // SetVolume(4); // Set maximum volume
    // if (GetConnection() == V24) { // A V24 device is sending the version at startup
    //     HostSendVersion();
    //     HostSendChar('\r');
    // }
    LEDSet(GREEN,ON); // Turn on green LED
    LEDSet(RED,OFF); // Turn off red LED
    LastTagType = TAGTYPE_NONE;// No transponder found up to now
    while (TRUE) {
        if (TagSearch(ID,IDBitCnt,TagType)) { // Search a transponder
            if (TagType != LastTagType || IDBitCnt != LastIDBitCnt || !CompBits(ID,LastID,MAXIDBITS)) { // Is this transponder new to us?
                CopyBits(LastID,0,ID,0,MAXIDBITS); // Save this as known ID, before modifying the ID for proper output format
                LastIDBitCnt = IDBitCnt;
                LastTagType = TagType;
                Beep(BEEPHIGH); // Yes! Sound a beep
                // Beep(BEEPNONE);
                LEDSet(GREEN,OFF); // Turn off the green LED
                LEDSet(RED,ON); // Let the red one blink
                if (TagType == TAGTYPE_HIDPROX) { // Send the ID in our standard format
                    if (IDBitCnt < 45){ // Send HID ID in decimal format
                        if (IDBitCnt > 32) {
                            CopyBits(ID,0,ID,IDBitCnt-32,31); // Show ID without the paritys at start
                            HostSendDec(ID,31,0);
                        } else {
                            IDBitCnt -= 2; // Show ID without the paritys at start and end
                            CopyBits(ID,0,ID,1,IDBitCnt);
                            HostSendDec(ID,IDBitCnt,0);
                        }
                    } else { 
                        HostSendDec(ID,IDBitCnt,0); // Show ID in plain long format
                    }
                } else {
                    HostSendHex(ID,IDBitCnt,(IDBitCnt+7)/8*2); // Send ID with appropriate number of digits
                }
                HostSendChar('\r');
            }
            StartTimer(0,10); // Start a timeout of one seconds
        }
        if (TestTimer(0)) {
            LEDSet(GREEN,ON);
            LEDSet(RED,OFF);
            LastTagType = TAGTYPE_NONE;
        }
    }
}
