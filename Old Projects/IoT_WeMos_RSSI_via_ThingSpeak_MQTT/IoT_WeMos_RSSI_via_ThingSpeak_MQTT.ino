//Project to Pub/Sub to the MQTT Broker of ThingSpeak
//The device WeMos D1 (ESP8266) will repor the RSSI measurment every 20 sec

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Alvaro"; //Change this to your network SSID (name).
const char* pass = "alvaro123"; //Change this your network password
const char* mqttUserName = "WeMosD1MQTT"; //Can be any name.
const char* mqttPass = "1W1MP1PUTVZPLXK1"; //Change this your MQTT APIK ey from Account > MyProfile.
const char* writeAPIKey = "4GIQRNCDXILMMCEL"; //Change to your channel Write API Key.
const char* mqttServer = "mqtt.thingspeak.com"; //Server MQTT ThingSpeak 
const int mqttPort = 1883; //Port MQTT ThingSpeak 
const long channelID = 695700;
unsigned long lastConnectionTime = 0; //Timer for last connection
const unsigned long postingInterval = 20L * 1000L; //Post data every 20 seconds.

static const char* alphanum ="0123456789"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz"; //For random generation of client ID.

WiFiClient client; //Initialize the Wifi client library.
PubSubClient mqttClient(client); //Initialize the PuBSubClient library.

void setup()
{
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  mqttClient.setServer(mqttServer, mqttPort); //Set the MQTT broker details
}

void loop()
{
  if (!mqttClient.connected()) //Reconnect if MQTT client is not connected
    reconnect();

  mqttClient.loop(); //Call the loop continuously to establish connection to the server.

  if (millis() - lastConnectionTime > postingInterval) //If interval time has passed since the last connection, Publish data to ThingSpeak
    mqttpublish();
}




////////////////////////////////////////////////// AUX FUNCTIONS


void reconnect() 
{
  char clientID[9];

  while (!mqttClient.connected()) //Loop until reconnected
  {
    Serial.print("Attempting MQTT connection...");
    for (int i = 0; i < 8; i++) //Generate ClientID
        clientID[i] = alphanum[random(51)];
    
    clientID[8]='\0';
    
    if (mqttClient.connect(clientID,mqttUserName,mqttPass)) // Connect to the MQTT broker 
    {
      Serial.print("Connected with Client ID:  ");
      Serial.print(String(clientID));
      Serial.print(", Username: ");
      Serial.print(mqttUserName);
      Serial.print(" , Passwword: ");
      Serial.println(mqttPass);
    } else 
    {
      Serial.print("failed, rc=");
      // Print to know why the connection failed.
      // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void mqttpublish()
{
  float rssi = WiFi.RSSI();

  // Create data string to send to ThingSpeak
  String data = String("field1=" + String(rssi));
  int length = data.length();
  char msgBuffer[length];
  data.toCharArray(msgBuffer,length+1);
  Serial.println(msgBuffer);
  
  // Create a topic string and publish data to ThingSpeak channel feed. 
  String topicString ="channels/" + String( channelID ) + "/publish/"+String(writeAPIKey);
  length=topicString.length();
  char topicBuffer[length];
  topicString.toCharArray(topicBuffer,length+1);
 
  mqttClient.publish( topicBuffer, msgBuffer );

  lastConnectionTime = millis();
}
