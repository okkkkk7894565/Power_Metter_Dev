///Do not edit
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// wifi config
#include <WiFiManager.h>

// Insert Firebase project API Key
#define API_KEY "AIzaSyDO7QsBc5frdD6xl96FbLiLecF1lrucbY4"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://demometter-default-rtdb.asia-southeast1.firebasedatabase.app/"

//define pin using
#define RsBttForEner D0
#define RsBttForWiFi D1
#define ledRSPre D3
#define ledRS D2
#define LED_PIN1 D8


// declare variable
float volt, ampe, PF, wat, Frequency, Energy;
// int firstStatus, ledss;
float sTimeSend, dur, eTimeSend;
unsigned long sendDataPrevMillis = 0, getButtonData = 0;
bool signupOK = false;
int flagForRsWifi,flagForRsPower,flagSendData;
String Path,espID;
char esp_ID_toChar[100];
int wifi_status=0,countErr;
const char *pass_to_char,*ssid_to_char;

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

//define for WiFiManager object
WiFiManager wifiManager;
