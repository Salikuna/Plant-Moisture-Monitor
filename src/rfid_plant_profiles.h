#ifndef RFID_PLANT_PROFILES_H
#define RFID_PLANT_PROFILES_H

#include <Arduino.h>
#include <MFRC522.h>

//PIN assignments, using placeholder values as of now
const int RFID_SDA_PIN = 10;
const int RFID_RST_PIN = 9;
// This follows the MFRC522 wiring

//Constructing Plant Profiles for the project:
struct PlantProfile{
    const char* name;
    int moistureMin; //The miniumum threshold for the ideal moisture range (%)
    int moistureMax; //The maximum threshold for the ideal moisture range (%)
    const char* lightNeeds; 
    const char* wateringFreq;
};

//Card ID not found in our lookup table
extern const PlantProfile Unkown_Profile;

//API

void rfidSetup();

//Calling every iteration of the main loop() to check for new card scans
bool rfidLoop();

//gives true if the card gets scanned at least once
bool isProfileLoaded();

//Gives the profile for the most recently scanned card
//Returns the Unkown_Profile if the Card ID is not in our Lookup Table, or no card has been scanned yet. 
const PlantProfile& getLastScannedProfile();

const char* getLastScannedUID();

#endif
