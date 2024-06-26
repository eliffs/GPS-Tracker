#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

SoftwareSerial ss(1, 0); // RX, TX (NodeMCU D6, D5)

TinyGPSPlus gps;

// WiFi ayarları
const char* ssid = "ELIF";
const char* password = "12345678";

// ThingSpeak ayarları
const char* apiKey = "HYPEJRI3T8KZ3PM4";
const char* server = "api.thingspeak.com";

WiFiClient client;

void setup() {
  Serial.begin(9600); // Seri iletişim için
  ss.begin(9600); // GPS modülü için 
  
  // WiFi'ye bağlan
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  while (ss.available() > 0) {
    gps.encode(ss.read());

    if (gps.location.isUpdated() && gps.altitude.isUpdated()) {
      double latitude = gps.location.lat();
      double longitude = gps.location.lng();
      double altitude = gps.altitude.meters();
      
      Serial.print("Enlem: ");
      Serial.println(latitude, 6);
      Serial.print("Boylam: ");
      Serial.println(longitude, 6);
      Serial.print("Rakım: ");
      Serial.println(altitude, 2);

      // ThingSpeak'e veri gönder
      if (client.connect(server, 80)) {
        String postStr = String("api_key=") + apiKey;
        postStr += "&field1=" + String(latitude, 6);
        postStr += "&field2=" + String(longitude, 6);
        postStr += "&field3=" + String(altitude, 2);
        
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: " + String(postStr.length()) + "\n\n");
        client.print(postStr);

        Serial.println("Data sent to ThingSpeak");
        client.stop();
      } else {
        Serial.println("Connection to ThingSpeak failed");
      }
    }
  }

  delay(20000); // ThingSpeak'e veri gönderme aralığı (20 saniye)
}
