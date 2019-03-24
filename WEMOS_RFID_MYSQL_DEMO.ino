
/* 
 *  
 *  Connection between RFID reader and Wemos D1

RST/Reset=>D3

SPI SS=>D8

SPI MOSI=>D7

SPI MISO=>D6

SPI SCK=>D5
 */

#include <Wire.h>
#include <MFRC522.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define RST_PIN D3 // RST-PIN for RC522 - RFID - SPI - Module GPIO15 
#define SS_PIN  D8  // SDA-PIN for RC522 - RFID - SPI - Module GPIO2
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance


#define BlueLed D1
#define GreenLed D2
#define RedLed D0

//Wireless name and password
const char* ssid     = ""; // replace with you wireless network name
const char* password = ""; //replace with you wireless network password

// Remote site information
const char* host = "TUODOMINIO"; // IP address of your local server or web domain
String url = ""; // folder location of the txt file with the RFID cards identification, p.e. "/test.txt" if on the root of the server

int time_buffer = 5000; // amount of time in miliseconds that the relay will remain open

void setup() {

  pinMode(BlueLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
  pinMode(RedLed, OUTPUT);
   digitalWrite(GreenLed, 1);
  Serial.begin(9600);    // Initialize serial communications
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522

  // We start by connecting to a WiFi network

  Serial.println("Connecting to ");
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
  leds_off();
  delay(3000);
}

void leds_off() {
  digitalWrite(BlueLed, 0);   // turn the LED off
  digitalWrite(GreenLed, 0);   // turn the LED off
  digitalWrite(RedLed, 0);   // turn the LED off
}

void reject() {
  digitalWrite(RedLed, 1);   // turn the Red LED on
  delay(2000);
  leds_off(); 
}
void connectweb() {
  digitalWrite(BlueLed, 1);   // turn the Red LED on
  delay(2000);
  leds_off(); 
}
void authorize() {
  digitalWrite(GreenLed, 1);   // turn the Green LED on
 // digitalWrite(Relay,1);
  delay(time_buffer);              // wait for a second 
 // digitalWrite(Relay,0);
  leds_off(); 
}

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void loop() {
  int authorized_flag = 0;
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {   
    delay(50);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {   
    delay(50);
    return;
  }

////-------------------------------------------------RFID----------------------------------------------


  // Shows the card ID on the serial console
  String content= "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  Serial.println("Cart read:" + content);

////-------------------------------------------------SERVER----------------------------------------------

  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
   if (client.connect(host, httpPort)) {
  url = "http://www.piersoft.it/myarduino/lettura.php";
   url += "?card="+String(content);
   Serial.println("Connected with "+url);
  // This will send the request to the server
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
connectweb();
 delay(1000);

  // Read all the lines of the reply from server and print them to Serial

  while(client.available()){
     String line = client.readStringUntil('\r');
    // Serial.print(line); //DEBUG
     if(line.indexOf("SI") > 0){ 
      authorized_flag=1;
     }
     
    if(line==content){
      authorized_flag=1;
    }
  }
  
  if(authorized_flag==1){
    Serial.println("AUTHORIZED");
    authorize();
  }
  else{
    Serial.println("NOT AUTHORIZED");
    reject();
  }
   }
}
