#include <Arduino.h>
#include "main.h"
#include "get_power.h"
#include "reset_button.h"
#include "sim.h"

bool wat_err;
int count_mss;
//11/7
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
  digitalWrite(D2, 1);

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
    // digitalWrite(ledRSPre, 0);
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
    // digitalWrite(ledRSPre, 0);
    resetWifiByBtt();
  } else {
    Serial.println("ESP: Break Reset Wifi !!!!!!");
  }
  Serial.println();
  //-------------- Reset Wifi----------------------

  //init sim
  sim800.begin(115200);
  delay(1000);  // Đợi module khởi động
  sendATCommand("AT");
  //init sim


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
  savedSsid = WiFi.SSID();
  pass_to_char = WiFi.psk().c_str();
  ssid_to_char = WiFi.SSID().c_str();

  // nếu đã lưu 1 wifi sẽ kết nối với wifi đó
  countTimeConWifi = 0;
  wifiStatusFlag = 1;

  if (wifi_status == 0) {
    if (wifiManager.autoConnect(esp_ID_toChar, "12345678")) {
      Serial.println("Connect successed:");
    }
  } else {
    WiFi.begin(ssid_to_char, pass_to_char);
    countTimeConWifi = 0;
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      // Serial.println(countTimeConWifi);
      countTimeConWifi++;
      if (countTimeConWifi >= 60) {
        wifiStatusFlag = 0;
        break;
      }
    }
  }
  // -----------------------------------------------infor and connected wifi-----------------------------------------------

  if (wifiStatusFlag != 0) {
    Serial.print("\nConnected with IP: ");
    Serial.println(WiFi.localIP());
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
    // Path = "Systems/" + espID + "/Data";
    Path = "Systems/ESP_3470400_1458270/Data";

    Serial.println("Starting with Wifi");
    // -----------------------------------------------FBDO config -----------------------------------------------
  } else {
    Serial.println("Starting NO Wifi");
  }
  checkWifiFlag = 0;
  ///tesst
  count_mss = 0;
}
void loop() {
  // digitalWrite(ledRSPre, 1);

  if (countErr >= 3) {
    digitalWrite(ledRS, 0);
    delay(3500);
    ESP.reset();
  }
  if (millis() - sendDataPrevMillis > 5000) {
    sendDataPrevMillis = millis();
    // Serial.println("22222222");
    volt = getVol();
    ampe = getAmp();
    PF = getPF();
    wat = getWat();
    Energy = getEnergy();
    Frequency = getFre();
    // wat_err_alert((int)wat);
    showData(volt, ampe, PF, wat, Frequency, Energy);

    switch (wifiStatusFlag) {
      case 0:
        checkWifiFlag++;
        Serial.print("No Wifi +");
        Serial.println(checkWifiFlag);
        if (checkWifiFlag >= 5) {
          if (isSavedNetworkFound(savedSsid)) {
            ESP.restart();
          } else {
            checkWifiFlag = 0;
          }
        }
        break;
      case 1:
        Serial.print("Counnt Err: ");
        Serial.println(countErr);
        // ---------get data and pre-process data-----------------------

        json.set("Vol", volt / 1.0);
        json.set("ampe", ampe);
        json.set("PF", PF);
        json.set("wat", wat);
        json.set("Frequency", Frequency);
        json.set("Energy", Energy);
        json.set("wat_max", wat_max-8);
        delay(500);
        // Serial.println("End Get Data");
        // ---------get data and pre-process data-------------------------
        if (Firebase.ready() && signupOK) {
          delay(100);
          digitalWrite(ledRS, 1);
          Serial.println("Sign up ok");
          sTimeSend = millis();
          sendStartTime = sTimeSend;  // Lưu thời gian bắt đầu gửi
          bool sendSuccess = false;   // Biến để theo dõi trạng thái gửi
          while (millis() - sendStartTime < sendTimeout) {
            if (Firebase.RTDB.setJSON(&fbdo, Path, &json)) {
              sendSuccess = true;
              Serial.println("SEND PASSED");
              Serial.println("PATH: " + fbdo.dataPath());
              Serial.println("TYPE: " + fbdo.dataType());
              flagSendData = 1;
              break;
            } else {
              // Nếu hàm trả về thất bại ngay lập tức, ta có thể thoát khỏi vòng lặp ngay
              if (fbdo.httpCode() != FIREBASE_ERROR_TCP_ERROR_CONNECTION_REFUSED) {
                break;
              }
            }
          }

          if (!sendSuccess) {
            Serial.println("SEND FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
            flagSendData = 0;
            countErr++;
          }

          eTimeSend = millis();
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
          fbErr(flagSendData);
          // Kiểm tra nếu thời gian gửi vượt quá 10 giây
          if (millis() - sendStartTime > sendTimeout) {
            Serial.println("Send timeout, restarting loop.");
          }

        } else {
          Serial.println("Sign up fail");
          countErr = 0;
          digitalWrite(ledRS, 0);
          delay(100);
        }
        Serial.println();
        break;
    }
    wat_err_alert((int)wat);
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
      delay(100);
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

//----------------------------------- isSavedNetworkFound ----------------------------------
bool isSavedNetworkFound(String ssid) {
  Serial.print("Đang tìm:");
  Serial.println(ssid);
  // Lấy số lượng mạng WiFi có sẵn
  int numNetworks = WiFi.scanNetworks();
  // Kiểm tra xem có mạng nào được tìm thấy hay không
  if (numNetworks == 0) {
    Serial.println("Không tìm thấy mạng nào.");
    return false;
  } else {
    for (int j = 0; j < numNetworks; j++) {
      if (ssid.equals(WiFi.SSID(j))) {
        Serial.print("Mạng đã lưu ");
        Serial.print(ssid);
        Serial.println(" được tìm thấy.");
        return true;
      }
    }
    Serial.println("Không tìm thấy mạng đã lưu.");
    return false;
  }
}
//----------------------------------- isSavedNetworkFound ----------------------------------


//----------------------------------- Alert Function -------------------------------------
void wat_err_alert(int current_wat) {
  if (current_wat > wat_max - 8) {
    String send_ms = "Near Overload:" + (String)count_mss + ":" + (String)current_wat + "W";
    sendSMS("0387015635", send_ms);
    // makeCall("0387015635");
    // makeCall("0862657918");
    Serial.println("_______________________SIM:Alert Done(SMS)________________________________");
    Serial.println();
    count_mss++;
    if (current_wat > wat_max) {
      Serial.println("-----------------------------Auto Turn off-----------------------------");
      Serial.println();
      digitalWrite(ledRSPre, 0);
    }
  } else {
    Serial.println("SIM:Nothing to be done");
    Serial.println();
  }
}
//----------------------------------- Alert Function -------------------------------------
