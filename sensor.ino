#include "DHT.h"
#include "WiFi.h"
#include <HTTPClient.h>

#define DHTPIN 4      
#define DHTTYPE DHT22  

DHT dht(DHTPIN, DHTTYPE);

// String sensorCode = "SENSOR_A1";  // ESP32 số 1 -> SENSOR_A1
// String sensorCode = "SENSOR_A2";  // ESP32 số 2 -> SENSOR_A2
String sensorCode = "SENSOR_A3";  // ESP32 số 3 -> SENSOR_A3

const char* ssid = "Chuan thao";
const char* password = "12345678";
const char* serverName = "http://192.168.1.8/Graduation-project/admin/iot/api/iot-sensor.php";

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nKết nối WiFi thành công!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // delay(120000);
  delay(5000);  

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Đọc tín hiệu lỗi!");
    return;
  }

  Serial.printf("%s -> Nhiệt độ: %.2f°C, Độ ẩm: %.2f%%\n", sensorCode.c_str(), t, h);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{";
    jsonData += "\"sensor_code\":\"" + sensorCode + "\",";
    jsonData += "\"temperature\":" + String(t) + ",";
    jsonData += "\"humidity\":" + String(h);
    jsonData += "}";

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("[" + sensorCode + "] Server response: " + response);
    } else {
      Serial.print("[" + sensorCode + "] Lỗi gửi dữ liệu! Mã lỗi: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}
