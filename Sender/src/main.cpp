//-- Libraries Included
//--------------------------------------------------------------
#include <Arduino.h>
#include <WiFi.h> // The Basic Function Of The ESP NOD MCU

//------------------------------------------------------------------------------------
// Define I/O Pins
#define LED0 2 // WIFI Module LED

#define INPUT_12 12
#define INPUT_13 13
#define INPUT_14 14
#define INPUT_15 15
#define INPUT_16 16
#define INPUT_4 4

//------------------------------------------------------------------------------------
// Authentication Variables
char *ssid;     // Wifi Name
char *password; // Wifi Password
const String Devicename = "Device_1";

//------------------------------------------------------------------------------------
// WIFI Module Role & Port
IPAddress TCP_Server(192, 168, 4, 1);
IPAddress TCP_Gateway(192, 168, 4, 1);
IPAddress TCP_Subnet(255, 255, 255, 0);

unsigned int TCPPort = 2390;

WiFiClient TCP_Client;

//------------------------------------------------------------------------------------
// Some Variables
char buffer[80];

//====================================================================================

void Check_WiFi_and_Connect_or_Reconnect();
void Tell_Server_we_are_there();
void Send_Request_To_Server();

void setup() {
  // setting the serial port ----------------------------------------------
  Serial.begin(115200);

  // setting the mode of pins ---------------------------------------------
  pinMode(LED0, OUTPUT);    // WIFI OnBoard LED Light
  digitalWrite(LED0, !LOW); // Turn WiFi LED Off

  pinMode(INPUT_12, INPUT_PULLUP);
  pinMode(INPUT_13, INPUT_PULLUP);
  pinMode(INPUT_14, INPUT_PULLUP);
  pinMode(INPUT_15, INPUT_PULLUP);
  pinMode(INPUT_16, INPUT_PULLUP);
  pinMode(INPUT_4, INPUT_PULLUP);

  // WiFi Connect ----------------------------------------------------
  Check_WiFi_and_Connect_or_Reconnect(); // Checking For Connection
}

//====================================================================================

void loop() { Send_Request_To_Server(); }

//====================================================================================

void Send_Request_To_Server() {
  unsigned long tNow;

  tNow = millis();

  uint32_t inputPinState =
      digitalRead(INPUT_12) << 12 | digitalRead(INPUT_13) << 13 |
      digitalRead(INPUT_14) << 14 | digitalRead(INPUT_15) << 15 |
      digitalRead(INPUT_16) << 16 | digitalRead(INPUT_4) << 4;
  char inputPinStateString[11];
  sprintf(buffer, "0x%08x", inputPinState);

  Serial.print("send: ");
  Serial.println(buffer);
  TCP_Client.println(buffer); // Send Data

  while (1) {
    int len = TCP_Client.available(); // Check For Reply
    if (len > 0) {
      if (len > 80) {
        len = 80;
      }
      String line = TCP_Client.readStringUntil('\r'); // if '\r' is found
      Serial.print("received: ");                     // print the content
      Serial.println(line);

      break; // exit
    }
    if ((millis() - tNow) > 1000) { // if more then 1 Second No Reply -> exit
      Serial.println("timeout");
      break; // exit
    }
  }

  Check_WiFi_and_Connect_or_Reconnect();
}

//====================================================================================

void Check_WiFi_and_Connect_or_Reconnect() {
  if (WiFi.status() != WL_CONNECTED) {

    TCP_Client.stop(); // Make Sure Everything Is Reset
    WiFi.disconnect();
    Serial.println("Not Connected...trying to connect...");
    delay(50);
    WiFi.mode(
        WIFI_STA); // station (Client) Only - to avoid broadcasting an SSID ??
    WiFi.begin("DataTransfer"); // the SSID that we want to connect to

    while (WiFi.status() != WL_CONNECTED) {
      for (int i = 0; i < 10; i++) {
        digitalWrite(LED0, !HIGH);
        delay(250);
        digitalWrite(LED0, !LOW);
        delay(250);
        Serial.print(".");
      }
      Serial.println("");
    }
    // stop blinking to indicate if connected -------------------------------
    digitalWrite(LED0, !HIGH);
    Serial.println("!-- Client Device Connected --!");

    // Printing IP Address --------------------------------------------------
    Serial.println("Connected To      : " + String(WiFi.SSID()));
    Serial.println("Signal Strenght   : " + String(WiFi.RSSI()) + " dBm");
    Serial.print("Server IP Address : ");
    Serial.println(TCP_Server);
    Serial.print("Device IP Address : ");
    Serial.println(WiFi.localIP());

    // conecting as a client -------------------------------------
    Tell_Server_we_are_there();
  }
}

//====================================================================================

void Tell_Server_we_are_there() {
  // first make sure you got disconnected
  TCP_Client.stop();

  // if sucessfully connected send connection message
  if (TCP_Client.connect(TCP_Server, TCPPort)) {
    Serial.println("<" + Devicename + "-CONNECTED>");
    TCP_Client.println("<" + Devicename + "-CONNECTED>");
  }
  TCP_Client.setNoDelay(1); // allow fast communication?
}

//====================================================================================
