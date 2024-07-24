SoftwareSerial sim800(D7, D8); // RX, TX (D7 là GPIO13, D8 là GPIO15)

void initSIM() {

}

void sendATCommand(String command) {
    sim800.println(command);
    delay(1000); // Đợi module trả lời
    while (sim800.available()) {
        String response = sim800.readString();
        Serial.println("SIM800: " + response);
    }
}

void makeCall(String phoneNumber) {
    sendATCommand("ATD" + phoneNumber + ";");
}

void sendSMS(String phoneNumber, String message) {
    sendATCommand("AT+CMGF=1"); // Chuyển sang chế độ tin nhắn văn bản
    delay(100);
    sim800.print("AT+CMGS=\"");
        delay(100);

    sim800.print(phoneNumber);
        delay(100);

    sim800.println("\"");
    delay(100);
    sim800.print(message);
    delay(100);
    sim800.write(26); // Ký tự kết thúc tin nhắn (Ctrl+Z)
        delay(300);

}

void handleSerialInput(String input) {
    input.trim(); // Loại bỏ khoảng trắng đầu và cuối
    if (input == "123") {
        makeCall("<số điện thoại của bạn>"); // Thay <số điện thoại của bạn> bằng số điện thoại bạn muốn gọi
    } else if (input == "345") {
        sendSMS("<số điện thoại của bạn>", "Hello my friend"); // Thay <số điện thoại của bạn> bằng số điện thoại bạn muốn gửi tin nhắn
    } else {
        // Xử lý các lệnh thông thường khác
        sim800.println(input); // Gửi lệnh khác đến module SIM800
    }
}
