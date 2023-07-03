#include <ThingSpeak.h>
#include <Boards.h>

#include <FirmataConstants.h>

#include <FirmataDefines.h>

#include <FirmataMarshaller.h>

#include <FirmataParser.h>

#include <KiddeeExpress.h>
#include <FirebaseArduino.h>



#include <AirGradient.h>

#include <ESP8266WiFi.h>

#include <Wire.h>

#include <Adafruit_Sensor.h>

#include <Arduino.h>

#define LENG 31   //0x42 + 31 bytes equal to 32 bytes

unsigned char buf[LENG];

 

int PM01Value=1;          //define PM1.0 value of the air detector module

int PM2_5Value=2;         //define PM2.5 value of the air detector module

int PM10Value=3;         //define PM10 value of the air detector module

float h, t, p, pin, dp;





String apiKey = "3B2FJYBRMUNUGDU0";

// replace with your routers SSID

const char* ssid = "JioFiber-CJ3be";

// replace with your routers password

const char* password = "bablisuman1012";

 

const char* server = "api.thingspeak.com";

#define FIREBASE_HOST "air-mon-de389-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "HpFaUuKoF0zNZl47JaijC2JoCvgiN69eMCtKK3yj"



WiFiClient client;

 

void setup()

{

  Serial.begin(9600);   

  delay(10);

  Serial.println();

  Serial.print("Connecting to ");

  Serial.println(ssid);  

  WiFi.begin(ssid, password);

  

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");

  }

  Serial.println("");

  Serial.println("WiFi connected");

  

    // Printing the ESP IP address

  Serial.println(WiFi.localIP());  

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

 

  

}

 

void loop()

{

  if(Serial.find(0x42)){    //start to read when detect 0x42

    Serial.readBytes(buf,LENG);

 

    if(buf[0] == 0x4d){

      if(checkValue(buf,LENG)){

        PM01Value=transmitPM01(buf); //count PM1.0 value of the air detector module

        PM2_5Value=transmitPM2_5(buf);//count PM2.5 value of the air detector module

        PM10Value=transmitPM10(buf); //count PM10 value of the air detector module 

      }           

    } 

  }

 

  static unsigned long OledTimer=millis();  

    if (millis() - OledTimer >=8000) 

    {

      OledTimer=millis(); 

      

      Serial.print("PM1.0: ");  

      Serial.print(PM01Value);

      Serial.println("  ug/m3");            

    

      Serial.print("PM2.5: ");  

      Serial.print(PM2_5Value);

      Serial.println("  ug/m3");     

      

      Serial.print("PM10 : ");  

      Serial.print(PM10Value);

      Serial.println("  ug/m3");   

      Serial.println();

     

 

      Serial.println("...............................................");

 

      if (client.connect(server,80))  // "184.106.153.149" or api.thingspeak.com

    {

        String postStr = apiKey;

        postStr +="&field1=";

        postStr += String(PM01Value);

        postStr +="&field2=";

        postStr += String(PM2_5Value);

        postStr +="&field3=";

        postStr += String(PM10Value);

        postStr +="&field4=";

        

        

        client.print("POST /update HTTP/1.1\n");

        client.print("Host: api.thingspeak.com\n");

        client.print("Connection: close\n");

        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");

        client.print("Content-Type: application/x-www-form-urlencoded\n");

        client.print("Content-Length: ");

        client.print(postStr.length());

        client.print("\n\n");

        client.print(postStr); 
        Serial.println("Data send to Thingspeak");
    }   
  Firebase.setInt("PM 1.0",PM01Value);
  Firebase.setInt("MP 2.5",PM2_5Value);
  Firebase.setInt("PM 10",PM10Value);
    client.stop();  
    Serial.println("Waiting......");
    delay(2000);
    

    

 

    }

 

  }

char checkValue(unsigned char *thebuf, char leng)

{  

  char receiveflag=0;

  int receiveSum=0;

 

  for(int i=0; i<(leng-2); i++){

  receiveSum=receiveSum+thebuf[i];

  }

  receiveSum=receiveSum + 0x42;

 

  if(receiveSum == ((thebuf[leng-2]<<8)+thebuf[leng-1]))  //check the serial data 

  {

    receiveSum = 0;

    receiveflag = 1;

  }

  return receiveflag;

}

int transmitPM01(unsigned char *thebuf)

{

  int PM01Val;

  PM01Val=((thebuf[3]<<8) + thebuf[4]); //count PM1.0 value of the air detector module

  return PM01Val;

}

//transmit PM Value to PC

int transmitPM2_5(unsigned char *thebuf)

{

  int PM2_5Val;

  PM2_5Val=((thebuf[5]<<8) + thebuf[6]);//count PM2.5 value of the air detector module

  return PM2_5Val;

  }

//transmit PM Value to PC

int transmitPM10(unsigned char *thebuf)

{

  int PM10Val;

  PM10Val=((thebuf[7]<<8) + thebuf[8]); //count PM10 value of the air detector module  

  return PM10Val;

}
