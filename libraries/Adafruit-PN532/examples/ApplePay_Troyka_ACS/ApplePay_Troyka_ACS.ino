//Access Control Reader based on ApplePay (EMV) and Troyka cards


#include <Adafruit_PN532.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (10)
#define PN532_MISO (5)

Adafruit_PN532 nfc(PN532_SS);


//EMV card global vars
static const uint8_t aid_marker[] = {0x4F, 0x07};
static const uint8_t pan_marker[] = {0x5A, 0x08};



void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ...");
}

// dumb_memmem
static char* dumb_memmem(const char* haystack, int hlen, const char* needle, int nlen) {
    // naive implementation
    if (nlen > hlen) return 0;
    int i;
    for (i=0; i<hlen-nlen+1; i++) {
        if (memcmp(haystack+i,needle,nlen)==0) {
            return (char*)(haystack+i);
        }
    }
    return NULL;
}


// PAN search
static int pan_search(char *haystack, int hlen, char *outbuff){

if (hlen > 20) {
                      void * searchresult = dumb_memmem(haystack, hlen, pan_marker, 2);
                      if (searchresult) {
                        memcpy(outbuff, searchresult + 2, 8);
                        //Serial.print("Full PAN: ");
                        //nfc.PrintHex(outbuff, 8);
                        return 1;
                      }
                      return 0;

              } // end of if
return 0;
} //end of pan_search



// Get Troyka serial from data block
char * gettroykaserial(char * blockdata)
{

  uint32_t serial_temp;
  char troykaserial[11];


    // Parse Troyka 10 digits serial number (not UID) that printed on card
    // located from 3 byte up to 4 lower bits of 3 byte
    memcpy(&serial_temp, &(blockdata[3]), 4);

    // Covert MSB to LSB byte order
    serial_temp = ((serial_temp>>24)&0xf0) | // move half from byte 3 to byte 0
                    ((serial_temp<<8)&0xff0000) | // move byte 1 to byte 2
                    ((serial_temp>>8)&0xff00) | // move byte 2 to byte 1
                    ((serial_temp<<24)&0xff000000); // byte 0 to byte 3

    // Shift to half byte to the right
    serial_temp = serial_temp >> 4;

    // Add zero's padding from the left if necessary
    sprintf(troykaserial, "%010lu", serial_temp);

    Serial.print("Troyka Serial: ");
    Serial.println(troykaserial);

    return troykaserial;
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)


  // Troyka
  uint8_t TroykaKeyA[6] = { 0xA7, 0x3F, 0x5D, 0xC1, 0xD3, 0x33 }; // Key А from 8 sector Troyka card
  uint8_t KeyTypeA = 0;
  uint8_t KeyTypeB = 1;
  uint8_t TroykaBlockAddr = 32; // Sector 8, block 0 (block number 32)

  // EMV card
  bool apdusuccess;
  uint8_t apdu[255];
  uint8_t berBuffer[255];
  uint8_t berLength = 255;
  uint8_t aid[7];
  void *searchresult;
  uint8_t pan[8] = {0x00};
  uint8_t pan_found = 0;
  char temp[3];
  char * cardID;


  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate length
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    Serial.println("");
    Serial.println("");
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    if (uid)
    {

        if (uidLength == 7)
        {
            // If 7 bytes UID, we probably have a Troyka card
            // TODO: check ATS for detect mifare plus instead of UID length
            Serial.println("Seems to be a Troyka card (7 byte UID). Lets try authenticate it as Mifare Classic");

            success = nfc.mifareclassic_AuthenticateBlock(uid+3, uidLength - 3, TroykaBlockAddr, KeyTypeA, TroykaKeyA);

            if (success)
            {
                 Serial.println("Sector 8 (Blocks 32..35) has been authenticated");
                 uint8_t data[16];

                 // Try to read content of block 32 (sector 8 block 0)
                 success = nfc.mifareclassic_ReadDataBlock(TroykaBlockAddr, data);

                 // Extract Troyka serial from block data
                 cardID =  gettroykaserial(data);
                 Serial.println("-----------------------");
                 Serial.print("Card ID: ");
                 Serial.println(cardID);

                 delay(2000);
            } // end of if (success)

            else
            {
                 // If authentication failed, probably it's not mifare card
                 // tryes to check for EMV card
                 Serial.println("Failed to authenticate card as Mifare. Try to check for EMV card");

                 delay(2000);
            }
        } // end of if (uidLength == 7)

        else if (uidLength == 4)
        {
            // If 4 bytes UID, we probably have a EMV card
            Serial.println("Seems to be a EMV card (4 byte UID). Lets try to read PAN nubmer");

            // Reading PSE file for AID number
            uint8_t apdu[] = {0x00, 0xA4, 0x04, 0x00, 0x0e, 0x32, 0x50, 0x41, 0x59, 0x2e, 0x53, 0x59, 0x53, 0x2e, 0x44, 0x44, 0x46, 0x30, 0x31, 0x00};
            apdusuccess = nfc.inDataExchange(apdu, sizeof(apdu), berBuffer, &berLength);

                 if (apdusuccess)
                 {
                    if (berLength > 10) {
                      searchresult = dumb_memmem(berBuffer, berLength, aid_marker, 2);
                      if (searchresult) {
                        memcpy(&aid, searchresult + 2, 7);
                        //Serial.print("AID: ");
                        //nfc.PrintHex(aid, 7);
                      }
                    }


                     // Now start application on card (AID)
                     uint8_t startAID[] = {0x00, 0xa4, 0x04, 0x00, 0x07, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
                     memcpy(&startAID[5], &aid, 7);
                     //Serial.print("StartAID is: "); nfc.PrintHex(startAID, sizeof(startAID));
                     berLength = 255;
                     apdusuccess = nfc.inDataExchange(startAID, sizeof(startAID), berBuffer, &berLength);


                     memset(pan, 0x00, 8);
                     pan_found = 0;
                     // Looking for PAN
                     //uint8_t apdu1[] = {0x00, 0xb2, 0x01, 0x0c, 0x00};
                     //berLength = 255;
                     //apdusuccess = nfc.inDataExchange(apdu1, sizeof(apdu1), berBuffer, &berLength);
                     //Serial.print("berBuffer1 is: "); nfc.PrintHex(berBuffer, berLength);
                     //pan_found = pan_search(berBuffer, berLength, pan);
                     //if (pan_found) Serial.println("Pan found"); else Serial.println("Pan not found");

                     //uint8_t apdu2[] = {0x00, 0xb2, 0x02, 0x0c, 0x00};
                     //berLength = 255;
                     //apdusuccess = nfc.inDataExchange(apdu2, sizeof(apdu2), berBuffer, &berLength);
                     //Serial.print("berBuffer3 is: "); nfc.PrintHex(berBuffer, berLength);
                     //if (!pan_found)
                      //pan_found = pan_search(berBuffer, berLength, pan);
                     //if (pan_found) Serial.println("Pan found"); else Serial.println("Pan not found");

                     uint8_t apdu3[] = {0x00, 0xb2, 0x01, 0x14, 0x00};
                     berLength = 255;
                     apdusuccess = nfc.inDataExchange(apdu3, sizeof(apdu3), berBuffer, &berLength);
                     //Serial.print("berBuffer4 is: "); nfc.PrintHex(berBuffer, berLength);
                     if (!pan_found)
                      pan_found = pan_search(berBuffer, berLength, pan);
                     //if (pan_found) Serial.println("Pan found"); else Serial.println("Pan not found");

                     uint8_t apdu4[] = {0x00, 0xb2, 0x02, 0x14, 0x00};
                     uint8_t berLength = 255;
                     apdusuccess = nfc.inDataExchange(apdu4, sizeof(apdu4), berBuffer, &berLength);
                     //Serial.print("berBuffer5 is: "); nfc.PrintHex(berBuffer, berLength);
                     if (!pan_found)
                      pan_found = pan_search(berBuffer, berLength, pan);
                     //if (pan_found) Serial.println("Pan found"); else Serial.println("Pan not found");

                     if (pan_found) {
                      Serial.println("-----------------------");
                      Serial.print("Сard ID: ");
                      for (int i = 3; i < 8; i++) {
                        sprintf(temp, "%02x", pan[i]);
                        Serial.print(temp);
                      }
                     }
                 }


        }

        else
        {
                 Serial.print("Abnormal UID Length: ");
                 nfc.PrintHex(uid, uidLength);
        }

    } // end of if (uid)

    else
    {
      // Failed to read card
      Serial.println("Failed to read card. Retry after 2 seconds.");
      delay(2000);
    }
  }
}

