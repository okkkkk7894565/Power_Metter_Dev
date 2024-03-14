// COM10 in lap COM5 in PC
/////update reset 26/12/2023 >> in clone git >> can update
#include <Arduino.h>
#include "main.h"
#include "get_power.h"
#include "reset_button.h"
///adhgfoagfa
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RsBttForWiFi, INPUT);
  pinMode(RsBttForEner, INPUT);
  pinMode(ledRS, OUTPUT);
  pinMode(ledRSPre, OUTPUT);
  delay(300);

  //--------------------Show ESP-INFOR ------------------------------
  Serial.println("ESP:Welcome!!!");
  Serial.println("ESP:Receiver data");
  Serial.println();

  //ESP.getChipId() returns the ESP8266 chip ID as a 32-bit integer.
  espID = "ESP_" + (String)ESP.getChipId() + "_" + (String)ESP.getFlashChipId();
  for (byte len = 1; len <= espID.length() + 1; len++) {
    espID.toCharArray(esp_ID_toChar, len);
    delay(100);
  }

  Serial.print("Esp_Infor:");
  Serial.println(esp_ID_toChar);
  Serial.println();

  //-------------- Reset Power----------------------
  Serial.print("RsBttForEner:");
  Serial.println(digitalRead(RsBttForEner));
  delay(300);

  // get btt reset status >> meet requirment : return 1 ; not meet requirment : return 0
  flagForRsPower = readRsBtt(5000, 1);
  delay(300);

  Serial.print("flagForRsPower:");
  Serial.println(flagForRsPower);
  delay(100);

  if (flagForRsPower == 1) {
    digitalWrite(ledRSPre, 0);
    resetEneryByBtt();
  } else {
    Serial.println("ESP: Break Reset Power !!!!!!");
  }
  Serial.println();
  //-------------- Reset Power----------------------
  //-------------- Reset Wifi----------------------
  Serial.print("RsBttForWiFi:");
  Serial.println(digitalRead(RsBttForWiFi));
  delay(300);
  flagForRsWifi = readRsBttForWifi(4000, 1);
  Serial.print("flagForRsWifi:");
  Serial.println(flagForRsWifi);
  if (flagForRsWifi == 1) {
    digitalWrite(ledRSPre, 0);
    resetWifiByBtt();
  } else {
    Serial.println("ESP: Break Reset Wifi !!!!!!");
  }
  Serial.println();
  //-------------- Reset Wifi----------------------

  // -----------------------------------------------infor and connected wifi-----------------------------------------------
  if (WiFi.SSID() != "" && WiFi.psk() != "") {
    Serial.println("Saved WiFi credentials found:");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    wifi_status = 1;
  } else {
    Serial.println("No saved WiFi credentials found");
    wifi_status = 0;
  }
  pass_to_char = WiFi.psk().c_str();
  ssid_to_char = WiFi.SSID().c_str();

  // nếu đã lưu 1 wifi sẽ kết nối với wifi đó
  int countTimeConWifi = 0;
  if (wifi_status == 0) {
    if (wifiManager.autoConnect(esp_ID_toChar, "12345678")) {
      Serial.println("Connect succes:");
    }
  } else {
    WiFi.begin(ssid_to_char, pass_to_char);
    countTimeConWifi = 0;
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      countTimeConWifi++;
      if (countTimeConWifi > 60) {
        Serial.println("Resarting");
        delay(2000);
        ESP.restart();
      }
    }
  }
  Serial.print("\nConnected with IP: ");
  Serial.println(WiFi.localIP());
  // -----------------------------------------------infor and connected wifi-----------------------------------------------

  // -----------------------------------------------FBDO config -----------------------------------------------
  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Conected to fibase Successfull");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
    delay(1000);
    ESP.reset();
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Path = "METTER/" + espID + "/Data";
  // -----------------------------------------------FBDO config -----------------------------------------------
  Serial.println("Starting");
}
void loop() {
  if (countErr >= 3) {
    digitalWrite(ledRS, 0);
    digitalWrite(ledRSPre, 1);
    delay(3500);
    ESP.reset();
  }
  if (millis() - sendDataPrevMillis > 4000) {
    // Serial.println("---------------Start Get Data---------------------");
    sendDataPrevMillis = millis();
    Serial.print("Counnt Err: ");
    Serial.println(countErr);
    // ---------get data and pre-process data-----------------------
    volt = getVol();
    ampe = getAmp();
    PF = getPF();
    wat = getWat();
    Energy = getEnergy();
    Frequency = getFre();
    showData(volt, ampe, PF, wat, Frequency, Energy);

    json.set("Vol", volt / 1.0);
    json.set("ampe", ampe);
    // json.set("PF", PF);
    json.set("wat", wat);
    // json.set("Frequency", Frequency);
    json.set("Energy", Energy);
    delay(500);
    // Serial.println("End Get Data");
    // ---------get data and pre-process data-------------------------
    if (Firebase.ready() && signupOK) {
      delay(100);
      //connect fbdo success
      digitalWrite(ledRS, 1);
      Serial.println("Sign up ok ");
      sTimeSend = millis();
      if (Firebase.RTDB.setJSON(&fbdo, Path, &json)) {
        // eTimeSend = millis() - sTimeSend;
        // Serial.print("Send Time:");
        // Serial.println(eTimeSend);
        // if (eTimeSend < 1000 && countErr > 0) {
        //   Serial.print("Count--");
        //   countErr--;
        // }
        //  send data after connect success
        Serial.println("SEND PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
        flagSendData = 1;
        //ket thuc gui du lieu
      } else {
        Serial.println("SEND FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
        flagSendData = 0;
        countErr++;
      }
      eTimeSend = millis();
      //----------------------------------------thời gian setData lên fb----------------------------------------
      dur = eTimeSend - sTimeSend;
      Serial.print("Send Time:");
      Serial.print(eTimeSend);
      Serial.print(" - ");
      Serial.print(sTimeSend);
      Serial.print(" = ");
      Serial.println(dur);
      if (dur < 1000 && countErr > 0) {
        Serial.println("Count--");
        countErr--;
      }
      //----------------------------------------thời gian setData lên fb----------------------------------------
      fbErr(flagSendData);
      //done send data to fbdo >> show result of send data process
    } else {
      //  Truyền dữ liệu lên fbdo lỗi (lỗi mạng) >> vẫn in ra màn hình và chớp led báo lỗi mạng
      Serial.println("Sign up fail");
      countErr = 0;
      digitalWrite(ledRS, 0);
      digitalWrite(ledRSPre, 1);
      delay(100);
    }
    digitalWrite(ledRSPre, 0);  // tắt báo led lỗi mạng sau khi hiển thị
    Serial.println();
  }
}


// ------------show result of send data process-----------------------------------------
void fbErr(int getFlag) {
  if (getFlag == 1) {
    delay(200);
    digitalWrite(ledRS, 0);  // báo done truyền dữ  liệu
    delay(100);
  } else {
    // báo kết nối thành công nhưng có lỗi của Fb
    digitalWrite(ledRS, 0);
    delay(200);
    for (int i = 0; i < 3; i++) {
      digitalWrite(ledRSPre, 1);
      delay(100);
      digitalWrite(ledRSPre, 0);
      digitalWrite(ledRS, 1);
      delay(100);
      digitalWrite(ledRS, 0);
    }
  }
}
//----------------------------------- Reset Function -------------------------------------
void resetEneryByBtt() {
  int count = 0;
  while (count <= 5) {
    digitalWrite(ledRS, 1);
    delay(200);
    digitalWrite(ledRS, LOW);
    delay(200);
    count++;
  }
  if (pzem.resetEnergy()) {
    count = 0;
    while (count <= 5) {
      digitalWrite(ledRS, 1);
      delay(200);
      digitalWrite(ledRS, LOW);
      delay(200);
      count++;
    }
    Serial.println("PZEM:Reset Power Succesfull !!!");
  } else {    
    count = 0;
    while (count <= 2) {
      digitalWrite(ledRS, 1);
      delay(1000);
      digitalWrite(ledRS, LOW);
      delay(800);
      count++;
    }
    Serial.println("PZEM:Can not reset power");
    delay(300);
  }
}

void resetWifiByBtt() {
  int count = 0;
  while (count <= 5) {
    digitalWrite(ledRS, 1);
    delay(200);
    digitalWrite(ledRS, LOW);
    delay(200);
    count++;
  }
  Serial.println("ESP:Starting reset WiFi");
  //reset settings - for testing
  wifiManager.resetSettings();
  delay(200);
  wifiManager.autoConnect(esp_ID_toChar, "12345678");

  count = 0;
  while (count <= 5) {
    digitalWrite(ledRS, 1);
    delay(200);
    digitalWrite(ledRS, LOW);
    delay(200);
    count++;
  }
  //exit after config instead of connecting
  wifiManager.setBreakAfterConfig(true);
}
//----------------------------------- Reset Function -------------------------------------
