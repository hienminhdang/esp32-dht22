#include "DHT.h"
#include "WiFi.h"
#include <HTTPClient.h>

#define DHTPIN 4     
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Chuan thao";
const char* password = "12345678";
const char* serverName = "http://192.168.1.14/Graduation-project/admin/iot/api/iot-sensor.php";

// IPAddress local_ip(192,168,1,11);
// IPAddress gateway(192,168,1,1);
// IPAddress subnet(255,255,255,0);

void setup() {
  Serial.begin(9600);
  dht.begin();

  // WiFi.config(local_ip, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nKết nối WiFi thành công!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(5000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Đọc tín hiệu lỗi!");
    return;
  }

  Serial.print("Độ ẩm: ");
  Serial.print(h);
  Serial.print("%  Nhiệt độ: ");
  Serial.print(t);
  Serial.print("°C ");

  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;

    String jsonData = "{";
    jsonData += "\"sensor_code\":\"SENSOR_A1\",";
    jsonData += "\"temperature\":" + String(t) + ",";
    jsonData += "\"humidity\":" + String(h);
    jsonData += "}";

    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonData);

    if(httpResponseCode > 0){
      String response = http.getString();
      Serial.println("Server response: " + response);
    }else {
      Serial.print("Lỗi gửi dữ liệu! Mã lỗi: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}

