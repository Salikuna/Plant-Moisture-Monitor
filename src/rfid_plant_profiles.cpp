#include <SPI.h>
#include <MFRC522.h>
#include "rfid_plant_profiles.h"

struct LookupEntry {
    const char* uid;
    PlantProfile profile;
};

static const LookupEntry Plant_Lookup[] ={
    {
    "A3:1F:BB:04",  // <-- REPLACE with real UID from card 1
    {
      "Monstera",
      50, 70,
      "Bright indirect light",
      "Every 7–10 days"
    }
  },
  {
    "B7:2C:4D:19",  // <-- REPLACE with real UID from card 2
    {
      "Succulent",
      10, 30,
      "Full sun or bright direct light",
      "Every 14–21 days"
    }
  },
  {
    "C1:88:A2:F3",  // <-- REPLACE with real UID from card 3
    {
      "Peace Lily",
      60, 80,
      "Low to medium indirect light",
      "Every 5–7 days"
    }
  },
  {
    "D4:55:E6:7A",  // <-- REPLACE with real UID from card 4
    {
      "Cactus",
      5, 20,
      "Full sun",
      "Every 21–30 days"
    }
  },
  {
    "E9:3B:C7:02",  // <-- REPLACE with real UID from card 5
    {
      "Pothos",
      40, 60,
      "Low to bright indirect light",
      "Every 7–10 days"
    }
  }
};

static const int Lookup_Size = sizeof(Plant_Lookup)/sizeof(Plant_Lookup[0]);

const PlantProfile Unknown_Profile = {
    "Unknown Plant",
    0, 100, 
    "Unknown", 
    "Unknown"
};

// Module State : Private
static MFRC522 rfid(RFID_SDA_PIN, RFID_RST_PIN);
static bool ProfileLoaded = false;
static PlantProfile LastProfile = Unknown_Profile;
static char lastUID[30] = "None";

//Helpers
static void buildUIDString(MFRC522::Uid*uid, char* outBuffer, int bufferSize){
    outBuffer[0] = '\0';
    for(byte i = 0; i<uid->size; i++){
        char byteStr[4];
        snprintf(byteStr,sizeof(byteStr), "%02X", uid->uidByte[i]);
        strncat(outBuffer, byteStr, bufferSize - strlen(outBuffer) - 1);
        if (i < uid->size - 1) {
            strncat(outBuffer, ":", bufferSize - strlen(outBuffer) - 1);
        }
    }
}

//We now search the lookup table for a matching UID
//Returning the pointer to the matching PlantProfile or a null pointer (nullptr) if it is not found. 
static const PlantProfile* LookupProfile(const char* uid) {
  for (int i = 0; i < Lookup_Size; i++) {
    if (strcmp(Plant_Lookup[i].uid, uid) == 0) {
      return &Plant_Lookup[i].profile;
    }
  }
  return nullptr;
}

//API Implementation
void rfidSetup() {
  SPI.begin();
  rfid.PCD_Init();

  Serial.println("RFID reader initialised.");
  Serial.print("Lookup table loaded with ");
  Serial.print(Lookup_Size);
  Serial.println(" plant profile(s).");
  Serial.println("Scan a card to identify a plant.");
}

bool rfidLoop() {
  // Check if a new card is present and readable
  if (!rfid.PICC_IsNewCardPresent()) return false;
  if (!rfid.PICC_ReadCardSerial())   return false;

  // Build the UID string
  buildUIDString(&rfid.uid, lastUID, sizeof(lastUID));

  Serial.print("Card scanned. UID: ");
  Serial.println(lastUID);

  // Look up the profile
  const PlantProfile* found = LookupProfile(lastUID);
  if (found != nullptr) {
    LastProfile   = *found;
    ProfileLoaded = true;

    Serial.println("--- Plant Profile Found ---");
    Serial.print("Name:              "); Serial.println(LastProfile.name);
    Serial.print("Ideal moisture:    "); Serial.print(LastProfile.moistureMin);
    Serial.print("% – ");               Serial.print(LastProfile.moistureMax); Serial.println("%");
    Serial.print("Light needs:       "); Serial.println(LastProfile.lightNeeds);
    Serial.print("Watering schedule: "); Serial.println(LastProfile.wateringFreq);
    Serial.println("---------------------------");
  } else {
    LastProfile   = Unkown_Profile;
    ProfileLoaded = false;

    Serial.println("WARNING: Card UID not found in lookup table.");
    Serial.println("To register this card, add its UID to PLANT_LOOKUP in rfid_plant_profiles.cpp");
  }

  // Halt the card and stop encryption to allow the next scan
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  return true;
}

bool isProfileLoaded() {
  return ProfileLoaded;
}

const PlantProfile& getLastScannedProfile() {
  return LastProfile;
}

const char* getLastScannedUID() {
  return lastUID;
}
