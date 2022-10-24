#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid     = "MiFibra-2FC9";     // your network SSID (name of wifi network)
const char* password = "jtYP6iaj"; // your network password
WiFiClient client;
HTTPClient http;

void setup()
{
    Serial.begin(9600);
    delay(10);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    delay(4000);
}


void loop()
{
  http.useHTTP10(true);
  //http.begin(client, "http://arduinojson.org/example.json");
  http.begin(client, "http://olea-test.herokuapp.com");
  
  http.GET();

  String payload = http.getString();

  Serial.println(payload);

  // Disconnect
  http.end();
  delay(10000);
}
