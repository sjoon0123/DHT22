#include <DHTStable.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define DHT22_PIN 16  // DHT22 센서의 데이터 핀을 연결한 GPIO 핀 번호
#define DHTTYPE DHT22

const char *ssid = "ASUS_28_2G";      // WiFi SSID
const char *password = "sunday_5946";  // WiFi 비밀번호

const char *mobiusHost = "203.253.128.177";
const int mobiusPort = 7579;
const char *mobiusPath = "/Mobius/sch20201515/DHT22";

DHTStable DHT;

struct
{
    uint32_t total;
    uint32_t ok;
    uint32_t crc_error;
    uint32_t time_out;
    uint32_t connect;
    uint32_t ack_l;
    uint32_t ack_h;
    uint32_t unknown;
} counter = { 0,0,0,0,0,0,0,0};

void setup() {
  Serial.begin(115200);
  delay(10);

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
  Serial.print(getTime());
  Serial.print(",\t");
    uint32_t start = micros();
    int chk = DHT.read22(DHT22_PIN);
    uint32_t stop = micros();

    counter.total++;
    switch (chk)
    {
    case DHTLIB_OK:
        counter.ok++;
        //Serial.print("OK,\t");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        counter.crc_error++;
        Serial.print("Checksum error,\t");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        counter.time_out++;
        Serial.print("Time out error,\t");
        break;
    default:
        counter.unknown++;
        Serial.print("Unknown error,\t");
        break;
    }

    // DISPLAY DATA
    Serial.print(DHT.getHumidity(), 1);
    Serial.print(",\t");
    Serial.print(DHT.getTemperature(), 1);
    Serial.println(",\t");

  // 업로드할 데이터 생성
  String payload = "{ \"m2m:cin\": { \"con\": \"Time: " + String(getTime()) + ", Temperature: " + String(int(DHT.getHumidity())) + "°C, Humidity: " + String(int(DHT.getTemperature())) + "%\" } }";

  // Mobius 서버로 데이터 업로드
  uploadToMobius(payload);
}
String getTime() {
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) { //  google.com 연결 안되면, !!! 또는 ! 는 부울
  Serial.println("connection failed, retrying..."); 
  }
  
  client.print("HEAD / HTTP/1.1\r\n\r\n"); // google.com 에 접속해서 헤더파일 받아오는 명령어?

  while(!!!client.available()) {} // !!! 또는 ! 는 부울

  while(client.available()){
    if (client.read() == '\n') {   
      if (client.read() == 'D') {   
        if (client.read() == 'a') {   
          if (client.read() == 't') {   
            if (client.read() == 'e') {   
              if (client.read() == ':') {   
                client.read();
                String theDate = client.readStringUntil('\r'); 
                client.stop();
                return theDate;  // Date: 까지 읽고, 맞으면 \r 까지 읽어오기.
              }
            }
          }
        }
      }
    }
  }
}

void uploadToMobius(String payload) {
  //Serial.print("Connecting to Mobius... ");

  HTTPClient http;

  http.begin("http://" + String(mobiusHost) + ":" + String(mobiusPort) + String(mobiusPath));
  http.addHeader("Accept", "application/json");
  http.addHeader("X-M2M-RI", "12345");
  http.addHeader("X-M2M-Origin", "SKJZkzO42fL");
  http.addHeader("Content-Type", "application/vnd.onem2m-res+json; ty=4");

  int httpCode = http.POST(payload);

  /*if (httpCode > 0) {
    Serial.println("done");
    Serial.print("HTTP response code: ");
    Serial.println(httpCode);
    Serial.print("Response: ");
    Serial.println(http.getString());
  } else {
    Serial.println("failed");
    Serial.println("HTTP request failed");
  }*/

  http.end();
}
