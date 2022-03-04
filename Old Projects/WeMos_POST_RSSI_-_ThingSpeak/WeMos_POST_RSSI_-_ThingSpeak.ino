//Project to POST the RSSI value measured at the ESP8266 antenna.

#include <ESP8266WiFi.h>

// Network information.
const char* ssid = "Alvaro"; //SSID of WiFi newtork connecting to
const char* password = "alvaro123"; //Password

// ThingSpeak information.
#define NUM_FIELDS 1                                // To update more fields, increase this number and add a field label below.
#define RSSI_FIELD 1                                // ThingSpeak field for RSSI measurement.
#define TIMEOUT 5000                                // Timeout for server response.
#define THING_SPEAK_ADDRESS "api.thingspeak.com"
String writeAPIKey = "3VU7GZ2UY0U5LYRQ";            // Change this to your channel Write API key.

// Global variables.

WiFiClient client;

void setup()
{
  Serial.begin( 115200 );   // You may need to adjust the speed depending on your hardware.
  connectWifi();
}

void loop()
{
  // You can fill fieldData with up to 8 values to write to successive fields in your channel.
  String fieldData[ NUM_FIELDS ];

  // You can write to multiple fields by storing data in the fieldData[] array, and changing numFields.
  fieldData[ RSSI_FIELD ] = String( WiFi.RSSI( ) );   // Write the RSSI data to field 1.
  Serial.print( " RSSI measurement = " );
  Serial.println( fieldData[ RSSI_FIELD ] );

//  HTTPPost( NUM_FIELDS , fieldData );

  if (client.connect(THING_SPEAK_ADDRESS,80))
  {
    String postStr = writeAPIKey;
    postStr +="&field1=";
    postStr += fieldData[ RSSI_FIELD ];
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }

  delay( 20000 ); // Add delay so you don't post to ThingSpeak too often (limit is 15 sec).
}



// Connect to the local WiFi network
int connectWifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
}



// This function builds the data string for posting to ThingSpeak
// and provides the correct format for the wifi client to communicate with ThingSpeak.
// It will post "numFields" worth of data entries, and takes the
// data from the fieldData parameter passed to it.
int HTTPPost( int numFields , String fieldData[] )
{
  if (client.connect( THING_SPEAK_ADDRESS , 80 ))
  {
    // Build the Posting data string. If you have multiple fields, make sure the sting does not exceed 1440 characters.
    String postData = "api_key=" + writeAPIKey;
    for ( int fieldNumber = 1; fieldNumber < numFields + 1; fieldNumber++ )
    {
      String fieldName = "field" + String( fieldNumber );
      postData += "&" + fieldName + "=" + fieldData[ fieldNumber ];
    }

    // POST data via HTTP
    Serial.println( "Connecting to ThingSpeak for update..." );
    Serial.println();

    client.println( "POST /update HTTP/1.1" );
    client.println( "Host: api.thingspeak.com" );
    client.println( "Connection: close" );
    client.println( "Content-Type: application/x-www-form-urlencoded" );
    client.println( "Content-Length: " + String( postData.length() ) );
    client.println();
    client.println( postData );

    Serial.println( postData );

    String answer = getResponse();
    Serial.println( answer );
  }
  else
  {
    Serial.println ( "Connection Failed" );
  }
}




// Wait for a response from the server to be available,
//and then collect the response and build it into a string.
String getResponse()
{
  String response;
  long startTime = millis();
  delay( 200 );
  while ( client.available() < 1 && (( millis() - startTime ) < TIMEOUT ) )
    delay( 5 );

  if ( client.available() > 0 ) // Get response from server
  {
    char charIn;
    do
    {
      charIn = client.read(); // Read a char from the buffer.
      response += charIn;     // Append the char to the string response.
    }
    while ( client.available() > 0 );
  }
  client.stop();

  return response;
}
