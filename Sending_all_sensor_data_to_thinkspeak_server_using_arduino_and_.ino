/*
[In this project we are sending all the 6 sensor data to the thinkspeak server using arduino and the esp8266 without using AT Command.]

 URL to Check the Data on CLoud
//https://thingspeak.com/channels/227783

 Connect the + on the board to +5V on the Arduino
 Connect the - on the board to GND on the Arduino

 Connect the R on the board to 5 on the Arduino 
 Connect the P on the board to 4 on the Arduino 
 Connect the X on the board to 3 on the Arduino 

 Connect the T on the board to A2 on the Arduino 
 Connect the L on the board to A4 on the Arduino 
 Connect the H on the board to A5 on the Arduino 
*/
#include "DHT.h"
#include <WiFiEsp.h>
#include "SoftwareSerial.h"
#include <Adafruit_Sensor.h>

#define WIFI_AP "AD"
#define WIFI_PASSWORD "adverma200"
#define server "api.thingspeak.com"
#define apiKey "9KH9WPJV0CN3PUUL"
char server[] = "api.thingspeak.com";  // server name
String apiKey ="9KH9WPJV0CN3PUUL";  //Insert your write api here

// DHT
#define DHTPIN A5
#define DHTTYPE DHT11

// Initialize the Ethernet client object
 WiFiEspClient client;


// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial soft(7, 8); // RX, TX

int status = WL_IDLE_STATUS;
unsigned long lastSend;

// server Address of your Cloud
//char server[] = "api.thingspeak.com";  // server name
//String apiKey ="9KH9WPJV0CN3PUUL";  //Insert your write api here

//PIR
int X=3;    

//POWER SWITCH
int P=4;   

//REED SWITCH
int R=5; 



// TEMPERATURE
// A2 has already been defined as Analog Pin 2 in arduino
int T= A2;   

// LIGHT
// A4 has already been defined as Analog Pin 4 in arduino
int L=A4;

// HUMIDITY
// A5 has already been defined as Analog Pin 5 in arduino 
int H=A5;    

// Variable to store the temperature value
float temp;

// Variable to store the humidity value
float hum=0;

// Variable to store the light
float lig=0;


String pwr="0";       // 0 means OFF and 1 means ON
String door="0";      // 0 means CLOSED and 1 means OPEN
String person="0";   // 0 means NO and 1 means YES

void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  dht.begin();
  InitWiFi(); 
  lastSend = 0;
}

void loop()
{
  status = WiFi.status();
  if ( status != WL_CONNECTED)
  {
    while ( status != WL_CONNECTED) 
    {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }
  if ( !client.connected() ) 
  {
    reconnect();
  }
    getAndSendDatas();
}

void getAndSendDatas()
 {
   Serial.println("Collecting  datas.");
   TEMPERATURE();
   HUMIDITY();
   LIGHT();
   POWER();
   REED();
   PIR();

  
      // Create the JSON for the data to be uploaded on the Cloud

      String data;
      
      data += "{";
      data += "\"field1\":\"";              //door_operation either OPEN (0) or CLOSED (1)
      data += door;
      data += "\",";

      data += "\"field2\":\"";              //power_status either OFF or ON
      data += pwr;
      data += "\",";

      data += "\"field3\":\"";              //temperature
      data += String(temp);
      data += "\",";

      data += "\"field4\":\"";              //humidity
      data += String(hum);
      data += "\",";

      data += "\"field5\":\"770\",";        //trip_id replace with your last 3 digits of mobile
      data += "\"field6\":\"Cold Chain\","; //device_type
      data += "\"field7\":\"9529853846\","; // device_no need to put your own mobile number, replace with your own mobile number
      data += "\"field8\":\"transaction\""; //type

      data += "}";


     Serial.println(data);
     client.println("POST /update?api_key=9KH9WPJV0CN3PUUL HTTP/1.1");
     client.println("Host: api.thingspeak.com");
     client.println("Accept: */*");
     client.println("Content-Length: " + String(data.length()));
     client.println("Content-Type: application/json");
     client.println();
     client.println(data);
     
}

void InitWiFi()
{
  // initialize serial for ESP module
  soft.begin(9600);
  // initialize ESP module
  WiFi.init(&soft);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to Thingspeak server ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect(server, 80) ) {
      Serial.println( "Connected" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
     // Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}


void TEMPERATURE()
{    
        // read the value from the Analog variable 
        // and store it to an integer variable
        int value_temp = analogRead(T);   

        // Divide the obtained value with the resolution of the ADC (i.e., 1023.0)
        // multiply the result with reference milli volts. (5V = 5000mV)
        float millivolts_temp = ((value_temp/1023.0)*5000);

        // There will be 1°C change for every 10mV of output
        temp = millivolts_temp/10;

        // Print the output on the Serial Monitor
        Serial.print("Temperature =  ");
        Serial.println(temp);
}

    
void HUMIDITY()
{
      // Define the pin from where the DHT values will be read
      int chk = dht.read(H);

      // Call the humidity function of the DHT11 Library to read the value and 
      // store in a variable
      hum = dht.readHumidity();  

      // Prints the value of the humidity on the Serial Monitor
      Serial.print("Humidity =  ");
      Serial.println(hum);
}


    
void LIGHT()
{
      // read the value from the Analog variable 
      // and store it to an integer variable
      int value_lig=analogRead(L);

      // Divide the obtained value with the resolution of the ADC (i.e., 1023.0)
      // multiply the result with reference milli volts. (5V = 5000mV)
      float millivolts_lig =(value_lig /1023.0)*5000;

      // There will be 1  change for every 10mV of output
      lig=millivolts_lig /10;

      // Prints the value of the Light Intensity on the Serial Monitor
      Serial.print("Light Intensity =  ");
      Serial.println(lig);
}



void POWER()
{
      Serial.print("Power :");
      if(digitalRead(P)==LOW)
      {
      // 0 means OFF and 1 means ON  
      pwr="0";
      Serial.println(" OFF");
      }
      else
      {
      // 0 means OFF and 1 means ON  
      pwr="1";  
      Serial.println(" ON");
      }
} 


void PIR()
{
      Serial.print("Person :");
      if(digitalRead(X)==LOW)
      {
      // 0 means NO and 1 means YES  
      person="1";  
      Serial.println(" YES");
      }
      else
      {
      // 0 means NO and 1 means YES  
      person="0";  
      Serial.println(" NO");
      }
}

void REED()
{
      Serial.print("Door :");
      if(digitalRead(R)==LOW)
      {
      // 0 means CLOSED and 1 means OPEN  
      door="0";  
      Serial.println(" CLOSE");
      }
      else
      {
      // 0 means CLOSED and 1 means OPEN  
      door="1";  
      Serial.println(" OPEN");
      }
}
