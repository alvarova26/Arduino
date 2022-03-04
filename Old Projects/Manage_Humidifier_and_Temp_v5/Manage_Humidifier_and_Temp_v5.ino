/*
  Manage Humidifier and Refrigerator - Test program to run 2 relays connected to an Humidifier
  and a Refrigerator following a upper/lower logic with hysteresis
*/

//******************* Sent to WeMOS 01/02/2019 *******************//

//Note: The humidifier is connected to the relay's NO (normal opened) pins
// - If the relay is deactivated, then the humidifier is turned OFF
// - If the relay is activated, then the humidifier is turned ON
//Note: The refrigerator is connected to the relay's NC (normal closed) pins
// - If the relay is deactivated, then the refrigerator is turned ON
// - If the relay is activated, then the refrigerator is turned OFF

//Realy behavior
//If digitalWrite(ref_PIN, HIGH) => Writes 5V to Relay => Deactivated => Relay's LED off.
//If digitalWrite(ref_PIN, LOW) => Writes 0V to Relay => Activated => Relay's LED on.

//Libraries
#include "DHT.h"
#include <ESP8266WiFi.h>

//Global Constants
#define DHTTYPE DHT11   // DHT 11
const int DHTPin = 5;
DHT dht(DHTPin, DHTTYPE); // Initialize DHT sensor.

//Global Variables
int ref_PIN = 12; //What pin is the refrigerator's relay command connected to
int hum_PIN = 13; //What pin is the humidifier's relay command connected to
int hum_CORR = 0; //Correction value for humidity read from the DHT11 sensor
int temp_CORR = 0; //Correction value for temperature read from the DHT11 sensor
int correction = 0; //Correction value sent by reference to serial_print auxiliar funcion
float upper_temp = 24; //Upper temperature allowed, once reached, turn ON the refrigerator
float lower_temp = 22; //Lower temperature desired, once reached, turn OFF the refrigerator
float upper_hum = 75; //Upper humidity desired, once reached, turn OFF the humidifier
float lower_hum = 65; //Lower humidity allowed, once reached, turn ON the humidifier
float temp_VALUE = 0; //Will store the temperature value comming from DHT
float hum_VALUE = 0; //Will store the humidity value comming from DHT
float value = 0; //Value of the parameter to be printed sent by reference to serial_print auxiliar funcion
char measure = 'u'; //Code for printing (t=temperarutr or h=humidity) sent to serial_print axiliar funcion

//DHT dht(DHTPIN, DHTTYPE); //Initialize DHT sensor for normal 16mhz Arduino


void setup() //Routine "setup" runs once after reset
{
  Serial.begin(115200); //Initialize Serial Comm (over USB port)
  Serial.println("Ready."); //Print the auxiliar text: "Ready." to Serial Comm (USB)
  dht.begin(); //Initialize DHT to send HUM&TEMP readings
  pinMode(ref_PIN, OUTPUT); //Initialize the DIGITAL PIN as output
  pinMode(hum_PIN, OUTPUT); //Initialize the DIGITAL PIN as output
  digitalWrite(ref_PIN, HIGH); //Write HIGH (5V) on the DIGITAL PIN (realy deactivated) - Refrigerator ON
  digitalWrite(hum_PIN, HIGH); //Write HIGH (5V) on the DIGITAL PIN (realy deactivated) - Humidifier OFF
}

void loop()
{
  temp_VALUE = dht.readTemperature(); //Read the temperature value form the DHT
  hum_VALUE = dht.readHumidity(); //Read the humidity value form the DHT

  digitalWrite(ref_PIN, HIGH); //Write HIGH (5V) on the DIGITAL PIN (realy deactivated) - Refrigerator ON
  delay (3000);
  digitalWrite(ref_PIN, LOW); //Write LOW (0V) on the DIGITAL PIN (realy activated) - Refrigerator OFF

 Serial.print("Humidity (actual): "); //Print the auxiliar text: "Humidity (actual): " to Serial Comm (USB)
 Serial.print(hum_VALUE); //Print the current value of the hum_VALUE variable to Serial Comm (USB)
 Serial.println(" %"); //Print the auxiliar text: " %" to Serial Comm (USB)
 Serial.print("Humidity (corrected): "); //Print the auxiliar text: "Humidity (actual): " to Serial Comm (USB)
 Serial.print(hum_VALUE-hum_CORR); //Print the corrected value of humidity to Serial Comm (USB)
 Serial.println(" %"); //Print the auxiliar text: " %" to Serial Comm (USB)

 Serial.print("Temperature (actual): "); //Print the auxiliar text: "Temperature (actual): " to Serial Comm (USB)
 Serial.print(temp_VALUE); //Print the current value of the temp_VALUE variable to Serial Comm (USB)
 Serial.println(" C"); //Print the auxiliar text: " C" to Serial Comm (USB)
 Serial.print("Temperature (corrected): "); //Print the auxiliar text: "Temperature (actual): " to Serial Comm (USB)
 Serial.print(temp_VALUE-temp_CORR); //Print the corrected value of Temperature to Serial Comm (USB)
 Serial.println(" C"); //Print the auxiliar text: " C" to Serial Comm (USB)


  //************************* Logic for Temperature control - START ********************************//
//  serial_print(temp_VALUE,temp_CORR,'t' ); //Call serial_print () => Print Temperature values
  if ((temp_VALUE-temp_CORR) < lower_temp) //Corrected temperature below lower defined threshold
    digitalWrite(ref_PIN, LOW); //Write LOW (0V) on the DIGITAL PIN (realy activated) - Refrigerator OFF
  if ((temp_VALUE-temp_CORR) >= upper_temp) //Corrected temperature above upper defined threshold
    digitalWrite(ref_PIN, HIGH); //Write HIGH (5V) on the DIGITAL PIN (realy deactivated) - Refrigerator ON
  //************************* Logic for Temperature control - END **********************************//


  //************************* Logic for Humidity control - START ***********************************//
//  serial_print(hum_VALUE,hum_CORR,'h' ); //Call serial_print () => Print Humidity values
  if ((hum_VALUE-hum_CORR) < lower_hum) //Corrected Humidity below lower defined threshold
    digitalWrite(hum_PIN, LOW); //Write LOW (0V) on the DIGITAL PIN (realy activated) - Humidifier ON
  if ((hum_VALUE-hum_CORR) >= upper_hum) //Corrected Humidity above upper defined threshold
    digitalWrite(hum_PIN, HIGH); //Write HIGH (5V) on the DIGITAL PIN (realy deactivated) - Humidifier OFF
  //************************* Logic for Humidity control - END *************************************//

  delay (5000);
}


// Auxiliar Printing Function
void serial_print(float value,int correction, char measure)
{
  switch (measure)
  {
    case 'h':
      Serial.print("Humidity (actual): "); //Print the auxiliar text: "Humidity (actual): " to Serial Comm (USB)
      Serial.print(value); //Print the current value of the hum_VALUE variable to Serial Comm (USB)
      Serial.println(" %"); //Print the auxiliar text: " %" to Serial Comm (USB)
      Serial.print("Humidity (corrected): "); //Print the auxiliar text: "Humidity (actual): " to Serial Comm (USB)
      Serial.print(value-correction); //Print the corrected value of humidity to Serial Comm (USB)
      Serial.println(" %"); //Print the auxiliar text: " %" to Serial Comm (USB)
    break;
    case 't':
      Serial.print("Temperature (actual): "); //Print the auxiliar text: "Temperature (actual): " to Serial Comm (USB)
      Serial.print(value); //Print the current value of the temp_VALUE variable to Serial Comm (USB)
      Serial.println(" C"); //Print the auxiliar text: " C" to Serial Comm (USB)
      Serial.print("Temperature (corrected): "); //Print the auxiliar text: "Temperature (actual): " to Serial Comm (USB)
      Serial.print(value-correction); //Print the corrected value of Temperature to Serial Comm (USB)
      Serial.println(" C"); //Print the auxiliar text: " C" to Serial Comm (USB)
    break;
    defualt:
      Serial.println("Something went wrong. Check the program."); //Print the auxiliar text to Serial Comm (USB)
    break;
  }
}
