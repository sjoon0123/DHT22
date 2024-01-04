#include <dht.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define DHTPIN 16  // DHT22 센서의 데이터 핀을 연결한 GPIO 핀 번호
#define DHTTYPE DHT22

const char *ssid = "ASUS_28_2G";      // WiFi SSID
const char *password = "sunday_5946";  // WiFi 비밀번호

const char *mobiusHost = "203.253.128.177";
const int mobiusPort = 7579;
const char *mobiusPath = "/Mobius/sch20201515/DHT22";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(10);

  dht.begin();

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  delay(2000);  // 2초마다 측정

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C, Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  // 업로드할 데이터 생성
  String payload = "{ \"m2m:cin\": { \"con\": \"Temperature: " + String(int(temperature)) + "°C, Humidity: " + String(int(humidity)) + "%\" } }";

  // Mobius 서버로 데이터 업로드
  uploadToMobius(payload);
}

void uploadToMobius(String payload) {
  Serial.print("Connecting to Mobius... ");

  HTTPClient http;

  http.begin("http://" + String(mobiusHost) + ":" + String(mobiusPort) + String(mobiusPath));
  http.addHeader("Accept", "application/json");
  http.addHeader("X-M2M-RI", "12345");
  http.addHeader("X-M2M-Origin", "SKJZkzO42fL");
  http.addHeader("Content-Type", "application/vnd.onem2m-res+json; ty=4");

  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    Serial.println("done");
    Serial.print("HTTP response code: ");
    Serial.println(httpCode);
    Serial.print("Response: ");
    Serial.println(http.getString());
  } else {
    Serial.println("failed");
    Serial.println("HTTP request failed");
  }

  http.end();
}
