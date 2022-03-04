//Project to Pub/Sub to the MQTT Broker CloudMQTT
//The device WeMos D1 (ESP8266) will report the RSSI measurment every 20 sec along other parameters


// ****************************** Include Libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>


// ****************************** WiFi Netwrok Parameters
const char* ssid = "Alvaro"; //Change this to your network SSID (name).
const char* pass = "alvaro123"; //Change this your network password


// ****************************** MQTT Broker Parameters
const char* mqttServer = "m16.cloudmqtt.com"; //Server MQTT
const char* mqttUserName = "rblczunz"; //Username provided by the MQTT Broker
const char* mqttPass = "_k4NiXYWRDDg"; //Password provided by the MQTT Broker
const char* mqttClientID = "WeMosClient"; //ClientID provided by the MQTT Broker
const int mqttPort = 18475; //Port MQTT

char EstadoSaida = '0'; //DELET THIS UNNECESARY VARIABLE!!!!!!!!!!!!!!!


// ****************************** Refrigerator Parameters
int ref_PIN = 6; //What pin is the refrigerator's relay command connected to
int temp_CORR = 0; //Correction value for temperature read from the DHT11 sensor
int des_temp = 11; //Desired temperature, wich will define the aim temperature of the system
int delta_temp = 2; //Detla tempertaure allowed, which will establish upper and lower thresholds for relay activation/deactivation
int upper_temp = des_temp + delta_temp; //Upper temperature allowed, once reached, turn ON the refrigerator
int lower_temp = des_temp - delta_temp; //Lower temperature desired, once reached, turn OFF the refrigerator
float temp_VALUE = 0; //Will store the temperature value comming from DHT


// ****************************** Humidifier Parameters
int hum_PIN = 7; //What pin is the humidifier's relay command connected to
int hum_CORR = 0; //Correction value for humidity read from the DHT11 sensor
int des_hum = 11; //Desired humidity, wich will define the aim humidity of the system
int delta_hum = 2; //Detla humidity allowed, which will establish upper and lower thresholds for relay activation/deactivation
int upper_hum = des_hum + delta_hum; //Upper humidity desired, once reached, turn OFF the humidifier
int lower_hum = des_hum - delta_hum; //Lower humidity allowed, once reached, turn ON the humidifier
float hum_VALUE = 0; //Will store the humidity value comming from DHT


// ****************************** Miscellaneous Parameters
int correction = 0; //Correction value sent by reference to serial_print auxiliar funcion
float value = 0; //Value of the parameter to be printed sent by reference to serial_print auxiliar funcion
char measure = 'u'; //Code for printing (t=temperarutr or h=humidity) sent to serial_print axiliar funcion
float rssi_VALUE = 0; //Will store the RSSI measuremente form ESP8266


// ******************************  Define DHTPIN and DHTTYPE correspondence for ESP8266
#define DHTPIN 13
#define DHTTYPE DHT11

// ******************************  Define PINOUT correspondence for ESP8266
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1


// ****************************** Initialize Libraies
WiFiClient client; //Initialize the Wifi client library
PubSubClient mqttClient(client); //Initialize the PuBSubClient library
DHT dht(DHTPIN, DHTTYPE); //Initialize DHT library


// ****************************** Functions Prototypes
void setup_serial();
void setup_pinout();
void setup_wifi();
void setup_mqtt();
void setup_dht();
void reconnect_mqtt();
void callback(char* topic, byte* payload, unsigned int length);


// ****************************** Setup main functions, units and variables
void setup()
{
  setup_serial();
  setup_pinout();
  setup_dht();
  setup_wifi();
  setup_mqtt();

  mqttClient.publish("WeMos", "Online");
  mqttClient.subscribe("WeMos");
}


// ****************************** Loop function - Code runs overa and over again
void loop()
{
  if (!mqttClient.connected()) //Reconnect if MQTT client is not connected
    reconnect_mqtt();

  mqttClient.loop(); //Call the loop continuously to establish connection to the server (keepalive)

  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  // Temp parameters
  temp_VALUE = dht.readTemperature();
  JSONencoder["TEMP"] = String (temp_VALUE);
  JSONencoder["TEMPD"] = String (des_temp);
  JSONencoder["DTEMP"] = String (delta_temp);
  // Hum parameters
  hum_VALUE = dht.readTemperature();
  JSONencoder["HUM"] = String (hum_VALUE);
  JSONencoder["HUMD"] = String (des_hum);
  JSONencoder["DHUM"] = String (delta_hum);
  // Misc parameters
  rssi_VALUE=Wifi.RSSI();
  JSONencoder["RSSI"] = String (rssi_VALUE);
  JSONencoder["PIN"] = "L5";

  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Sending message to MQTTto pic...");
  Serial.println(JSONmessageBuffer);
 
  if (mqttClient.publish("WeMos", JSONmessageBuffer) == true) {
    Serial.println("Success sending message");
  } else {
    Serial.println("Error sending message");
  }
  
  delay (10000);
  // mqttClient.publish("WeMos", "JSON_DATA" );
  // mqttClient.subscribe("LED");
}




// ****************************** AUX FUNCTIONS
void reconnect_mqtt()
{
  Serial.print("Trying to reconnect to MTQQ Server: ");
  Serial.print(mqttServer);
  Serial.print(", Port: ");
  Serial.print(mqttPort);
  Serial.print(", using Username: ");
  Serial.println(mqttUserName);
  Serial.println();
  while (!mqttClient.connected()) //Loop until reconnected
  {
    if (mqttClient.connect(mqttClientID, mqttUserName, mqttPass)) // Connect to the MQTT broker
    {
      Serial.print("Connected to MTQQ Server: ");
      Serial.print(mqttServer);
      Serial.print(", Port: ");
      Serial.print(mqttPort);
      Serial.print(", using Username:  ");
      Serial.println(mqttUserName);
      Serial.println();
    }
    else
    {
      // If connection fails, print the ErrorCode in order to know what happened
      // Error Code reference https://pubsubclient.knolleary.net/api.html#state for the failure code explanation
      Serial.print("Connection failed, ErrorCode=");
      Serial.print(mqttClient.state());
      Serial.println("Try again in 5 seconds...");
      delay(5000);
    }
  }
}


// ****************************** AUX FUNCTIONS
void setup_serial()
{
  Serial.begin(115200); //Initialize the serial port (USB Commm)
}


// ****************************** AUX FUNCTIONS
void setup_dht()
{
  dht.begin(); //Initialize DHT to send HUM&TEMP readings
}


// ****************************** AUX FUNCTIONS
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to WiFi Network... ");
  Serial.println(ssid);

  //Initialize WiFi and try to connect the WiFi Network
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Network connected");
  Serial.println("");
}


// ****************************** AUX FUNCTIONS
void setup_pinout()
{
  //Initialize DIGITAL PIN as I/O and set an initial level
  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW); 
}


// ****************************** AUX FUNCTIONS
void setup_mqtt()
{
  //Set the MQTT broker parameters and try to connect the MQTT Broker
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);

  Serial.print("Trying to connect to MTQQ Server: ");
  Serial.print(mqttServer);
  Serial.print(", Port: ");
  Serial.print(mqttPort);
  Serial.print(", using Username: ");
  Serial.println(mqttUserName);
  Serial.println();
  while (!mqttClient.connected())
  {
    if (mqttClient.connect(mqttClientID, mqttUserName, mqttPass)) // Connect to the MQTT broker
    {
      Serial.print("Connected to MTQQ Server: ");
      Serial.print(mqttServer);
      Serial.print(", Port: ");
      Serial.print(mqttPort);
      Serial.print(", using Username: ");
      Serial.println(mqttUserName);
      Serial.println();
    }
    else
    {
      // If connection fails, print the ErrorCode in order to know what happened
      // Error Code reference https://pubsubclient.knolleary.net/api.html#state for the failure code explanation
      Serial.print("Connection failed, ErrorCode=");
      Serial.print(mqttClient.state());
      Serial.println("Trying to connect again in 5 seconds...");
      delay(5000);
    }
  }
}

// ****************************** AUX FUNCTIONS
void callback(char* topic, byte* payload, unsigned int length)
{
  String msg;
  char c='n';

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (int i = 0; i < length; i++)
  {
    c = (char)payload[i];
    msg += c;
  }
  Serial.print(msg);


  //LED inverted logic: Write HIGH on D5 turns the LED OFF
  if (msg.equals("L5"))
  {
    digitalWrite(D5, HIGH);
    EstadoSaida = '0';
  }
  if (msg.equals("D5"))
  {
    digitalWrite(D5, LOW);
    EstadoSaida = '1';
  }
  Serial.println();
  Serial.println("--------");
}
