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

#define ledRSPre D2
#define ledRS D3
#define LED_PIN1 D8


// declare variable
float volt, ampe, PF, wat, Frequency, Energy;
float sTimeSend, dur, eTimeSend;
unsigned long sendDataPrevMillis = 0, getButtonData = 0,sendStartTime;
bool signupOK = false;
unsigned int flagForRsWifi,flagForRsPower,flagSendData,wifiStatusFlag,checkWifiFlag;
String Path,espID,savedSsid;
char esp_ID_toChar[100];
unsigned int wifi_status=0,countErr,countTimeConWifi,wat_max=50;
const char *pass_to_char,*ssid_to_char;
const unsigned long sendTimeout = 5000;  // 10 seconds



//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

//define for WiFiManager object
WiFiManager wifiManager;
