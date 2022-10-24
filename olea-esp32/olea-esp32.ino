//#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const int rele = 26;
const int pir = 27;

boolean pirTriggered = false;

boolean motorState = false;
unsigned long previousMillis = 0;        
const long intervalOFF = 60000*2;//Intervalo apagado motor en reposo 5'
const long intervalON = 30000;//Intervalo encendido motor en reposo 1'          
unsigned long currentMillis = 0;

//const char *server_url = "http://olea-api.herokuapp.com";// Nodejs application endpoint
const char *server_url = "https://olea-test.herokuapp.com";// Nodejs application endpoint

//const char* ssid     = "MUEVETE3";     // your network SSID (name of wifi network)
//const char* password = "formacion"; // your network password
const char* ssid     = "MiFibra-2FC9";     // your network SSID (name of wifi network)
const char* password = "jtYP6iaj"; // your network password

// Set up the client objet
//WiFiClient client;
WiFiClientSecure client;
HTTPClient http;


void setup() {
  pinMode(rele, OUTPUT);
  pinMode(17, INPUT_PULLUP);
  digitalWrite(rele, HIGH);

  Serial.begin(9600);
  //Serial.setDebugOutput(true);

  Serial.print("Attempting to connect to SSID: \n");
  //Serial.println(ssid);

  delay(4000);   //Delay needed before calling the WiFi.begin
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi..\n");
    // wait 1 second for re-trying
    delay(1000);
  }
  //Serial.println("Connected to the WiFi network");
  //String ip = WiFi.localIP().toString();
  //Serial.printf("[SETUP] WiFi Connected %s\n", ip.c_str());
  Serial.print("Mensage inicial...\n");
  client.setInsecure();//skip verification
}

void loop() {
  if (digitalRead(pir) && !pirTriggered) {
    pirTriggered = true;
    Serial.print("Movement detected!     ");
    Serial.print("Check the pose \n");
    delay(1000);
    digitalWrite(rele, LOW);//Activa el motor
    delay(1000*3);//3 mintutos Tiempo espera de inflado hasta enviar la peticion
    Serial.print("Inflate ...!     ");

    Serial.print("Sending request to server...\n");
    sendPostToServer();//Envia peticion de transaccion
    //testSendGetToServer();
    delay(1000*10);//Time to inflate the sculpture
    digitalWrite(rele, HIGH);
    Serial.print("Deinflating sculpure...\n");
    delay(1000*5);//Time to deinflate the sculpture
    pirTriggered = false;//Listen for Movement again...
    Serial.print("Pir listening again ...\n");
    //Reset interval
     currentMillis = millis();
     previousMillis = currentMillis;
  }
  
  currentMillis = millis();
  if (currentMillis - previousMillis >= (motorState ? intervalOFF : intervalON)) {
    previousMillis = currentMillis;
    motorState = !motorState;
    if (motorState) {
      digitalWrite(rele, HIGH);
    } else {
      digitalWrite(rele, LOW);
    }
  }
}

void sendPostToServer() {
  //Prepare de json Object with the credentials
  DynamicJsonDocument doc(200);
  String h = "706173737730726421773077536f6c316d346e212138";
  doc["pass"] = h;
  String json;

  serializeJson(doc, json);

  http.begin(client, server_url);
  http.setTimeout(20000);//Set 20s wait for response time
  http.addHeader("Content-Type", "application/json");
  //Serial.println("send... ");
  int httpCode = http.POST(json);

  if (httpCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(200);
    deserializeJson(doc, payload);
    Serial.println(payload);

    const char* pass = doc["pass"];
    if (strcmp(pass, "yes") == 0) {
      sendGetToServer();
    } else {
      Serial.print(pass);
    }

  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());

  }
  http.end();

}

void sendGetToServer() {
  boolean wait = true;
  while (wait) {
    // Set up the client objet
    http.begin(server_url);
    int httpCode = http.GET();
    Serial.println("Send get ...");
    if (httpCode > 0) {
      Serial.println("Get Response from GET");
      String payload = http.getString();
      DynamicJsonDocument doc(200);
      deserializeJson(doc, payload);
      //Serial.println(payload);

      const char* transactionhash = doc["transactionhash"];
      const char* id = doc["id"];
      if (strcmp(id, "waiting") != 0) {
        /*********************************************************************************
         * 
         * Mensages del token
         * 
         */
        Serial.print("Token id: ");
        Serial.print(id);
        Serial.print("   hash: ");
        Serial.println(transactionhash);
        Serial.print("\n");
        
        wait = false;
      } else {
        //Serial.print(id);
      }
    }
    http.end();
    delay(2000);
  }
}
void testSendGetToServer() {
  boolean wait = true;
  while (wait) {
    // Set up the client objet
    http.begin(server_url);
    int httpCode = http.GET();
    Serial.println("Send get ...");
    if (httpCode > 0) {
      Serial.println("Get Response from GET");
      String payload = http.getString();
      DynamicJsonDocument doc(200);
      deserializeJson(doc, payload);
      Serial.println(payload);
    }
    http.end();
    delay(2000);
  }
}
