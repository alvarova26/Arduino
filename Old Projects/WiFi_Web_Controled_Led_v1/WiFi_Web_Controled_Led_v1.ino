/*
  WiFi Web Controled LED using WeMos D1 (ESP8266) - Test program just for turn a led via webpage
*/

//Notes

//Libraries
#include <ESP8266WiFi.h>
 
//Global Constants

//Global Variables
const char* ssid = "Alvaro"; //SSID
const char* password = "alvaro123"; //Password
int ledPin = D5; //Digital PIN the the LED is connected to

WiFiServer server(80); //Declares the objet server of class WiFiServer on port TCP:80

void setup() //Routine "setup" runs once after reset
{
  Serial.begin(115200); //Initialize Serial Comm (over USB port) - Calls function begin of object Serial
  delay(10);

  pinMode(ledPin, OUTPUT); //Initialize the DIGITAL PIN as output
  digitalWrite(ledPin, LOW); //Write LOW (0V) on the DIGITAL PIN

  WiFi.mode(WIFI_STA); //Initialize WiFi as "station" - Access the member fuction mode of object WiFi

  Serial.println(); //Print auxiliar text to Serial Comm (USB)
  Serial.println(); //Print auxiliar text to Serial Comm (USB)
  Serial.print("Trying to connect to Network: "); //Print auxiliar text to Serial Comm (USB)
  Serial.println(ssid); //Print auxiliar text to Serial Comm (USB)
 
  WiFi.begin(ssid, password); //Connect to WiFi network using the SSID and PASSWORD parameters

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("."); //Print auxiliar text to Serial Comm (USB)
  }
  Serial.println(""); //Print auxiliar text to Serial Comm (USB)
  Serial.print("WiFi connected to Network :"); //Print auxiliar text to Serial Comm (USB)
  Serial.println(WiFi.SSID()); //Print auxiliar text to Serial Comm (USB)

  server.begin(); //Start the server - Calls the member begin of the object server (class WiFiServer)

  Serial.println("Server started"); //Print auxiliar text to Serial Comm (USB)
  Serial.println(""); //Print auxiliar text to Serial Comm (USB)
  Serial.print("URL to access the Web Server : "); //Print auxiliar text to Serial Comm (USB)
  Serial.print("http://"); //Print auxiliar text to Serial Comm (USB)
  Serial.print(WiFi.localIP());
  Serial.println("/"); //Print auxiliar text to Serial Comm (USB)
  Serial.println(""); //Print auxiliar text to Serial Comm (USB)
}

void loop() //Routine "loop" runs over and over again
{
  WiFiClient client = server.available(); //Defines client objet class WiFiClient - Calls available funcion of object server

  if (!client)
    return;

  Serial.println("New data from a client detected"); //Print auxiliar text to Serial Comm (USB)

  while(!client.available()) //Wait until the client sends data
    delay(1);
 
  String request = client.readStringUntil('\r'); //Defines request variable type string and reads data until "\r" with the funcion member readStringUntil of the object client
  Serial.println(request); //Print auxiliar text to Serial Comm (USB)
  client.flush(); //Flushes the TCPClient object buffer - calls the member flush of the objet client class WiFiClient
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)
  {
    digitalWrite(ledPin, HIGH); //Write HIGH (5V) on the DIGITAL PIN
    value = HIGH;
  } 
  if (request.indexOf("/LED=OFF") != -1)
  {
    digitalWrite(ledPin, LOW); //Write LOW (0V) on the DIGITAL PIN
    value = LOW;
  }
 
  delay(1);
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println(""); // New line
  client.print("Led pin is now: ");
  if(value == HIGH)
    client.print("On");  
  else
    client.print("Off");
  client.println(""); // New line
  client.println("<br><br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 5 ON<br>");
  client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 5 OFF<br>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}
