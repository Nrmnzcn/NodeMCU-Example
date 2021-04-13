/* 0- Reset Board */
    
    int countRST = 0;                                                             /* Counting variable for reset function to trigger */
    int ResetPin = 16;                                                            // Have to unpluged the cable to RST pin during uploading code !!! 

   
    /* 1- WSP8266 WiFi connection */

    #include <ESP8266WiFi.h>                                                      // library for wifi configuration codes. Need to pre-install the Nodemcu board via board manager
                                                                                  // Kindly check instruction to install Nodemcu board at Solarduino website 
    const char* ssid = "********************";                                     // key in your wifi-router name (can use handphone to check your wifi full name)
    const char* password =  "**********";                                           // key in your wifi-router password
    WiFiClient client;                                                            /* define the Node Mcu variable for wifi ISP communication*/

   
    /* 2- Read URI API from ThingSpeak ThingHTTP to NodeMCU */
     
    #include <ESP8266HTTPClient.h>                                                // library for HTTP communication codes. Will automatically pre-installed from NoveMCU board manager
    const char* host = "api.thingspeak.com";                                      // The main server HTTP URL where you read the website value. If using ThingTHHP app, this value remains
    const int httpPortRead = 80;                                                  /* Port number of the HTTP URL */
    const char* url = "*****************************************************";    // Get the API link from ThingSpeak (ThingHTTP App), have to exclude the main HTTP URL
    //const char* url2 = ".......";                                               /* 1 value or 1 url can only read for 1 channel*/
                                                                                  /* If you need to read more than 1 value, you have to add more variables: url2,url3....*/                                                                               
    HTTPClient http;                                                              /* define the Node MCU variable for HTTP communication*/
    //HTTPClient http2;                                                           // 1 value or 1 http can only read for 1 channel. 
                                                                                  // If you need to read more than 1 value, you have to add more variables: http1,http2....
    float f1 ;                                                                    // If you need to read more than 1 value, you have to add more variables: f1, f2...
    float f2 ;
    float f3 ;
    float f1Convert;                                                              /* Variable use to convert Fahrenheit to celcuit for f1 value*/  
    int trimPosition;                                                             /* Variable use for character position*/

    /* 3- Write NodeMCU to ThingSpeak channel */
    
    String apiKey = "****************";                                           // Enter your Write API key from ThingSpeak channel. Each channel will have different Write Api Key
    const char* server = "api.thingspeak.com";                                    /* In this project we are using Web Server broker: thingspeak.com. Kindly remain unchanged*/
    const int httpPortWrite = 80;                                                 /* Port number of the server*/

 
void setup() 
{
    Serial.begin(9600);                                                           /* initiate Serial monitor with baud rate 9600 */
    
    /* 0- Reset Board */
          
    pinMode(ResetPin, OUTPUT);                                                    /* to define pin as output function */
    digitalWrite(ResetPin, HIGH);                                                 /* set pin as high output */


    /* 1- WSP8266 WiFi connection */
    
    WiFi.disconnect();                                                            /* disconnects any previous Wifi connection*/
    delay(1000);                                                                  /* wait for 1 second */
    WiFi.begin(ssid, password);                                                   /* initiate Wifi connection with your router */
    while (WiFi.status() != WL_CONNECTED)                                         /* if connection not completed, keep run the codes */
      {   
        delay(1000); Serial.println("Connecting to WiFi.."); 
            
        /* 0- Reset Board */ 
        
        countRST = countRST + 1;                                                  /* add counting time, if waited too long (by default 1 minute), reset the board */
        if (countRST == 60)
        { digitalWrite(ResetPin, LOW); countRST = 0; }  
      }
    Serial.println("Connected to the WiFi network");                              /* once connection completed, continue do the following */              
    Serial.println(WiFi.localIP());                                               /* print IP address of Node Mcu on Serial Monitor */
  
}

 
void loop()                                                                       /* These codes continue to run forever*/
{
  
   countRST = 0;                                                                  /* use to trigger reset board if connection takes too long*/
   
    /* 1- WSP8266 WiFi connection */
          
    while (WiFi.status() != WL_CONNECTED)                                         /* if wifi connection not completed or disconnection */
      { 
        if(countRST ==0)                                                          /* for the first time if disconnection detected*/
        {
        WiFi.disconnect();                                                        /* disconnect any previous connection*/
        delay(1000);                                                              /* wait for 1 second*/
        }
        WiFi.begin(ssid, password);                                               /* initiate Wifi connection with your router */
        Serial.println("Reconnecting to WiFi..");       
        delay(20000);                                                             /* let it has 20 second time for processing */
            
        /* 0- Reset Board */
            
        countRST = countRST + 1;                                                  /* record how many times keep trying to connect*/ 
        if (countRST == 15)                                                       /* if waited too long (by default 5 minutes has reached)*/ 
        { digitalWrite(ResetPin, LOW); countRST = 0; }                            /* reset the board */
      }


    /* 2- Read URI from ThingSpeak ThingHTTP to NodeMCU */
    
    while(!client.connect(host,httpPortRead))                                     /* if failed to connect to the web server*/
      {
        client.connect(host, httpPortRead);                                       /* tries to connect to server*/
        countRST = countRST + 1;                                                  /* record how many tries*/
        delay(10000);                                                             /* wait for 10 seconds*/
        if (countRST == 30)                                                       /* if waited too long (by default 5 minutes has reached)*/
          { 
            WiFi.disconnect();                                                    /* disconnect wifi*/
            delay(2000);                                                          /* wait 2 seconds*/
            WiFi.begin(ssid, password);                                           /* start connect wifi again*/
            countRST = 0;                                                         /*reset the counting time*/
          }
      }
    
    if( http.begin(host,httpPortRead,url))                                              // If the connection is ok for first value, for more values this set of code need to duplicate
      {
        int httpCode = http.GET();                                                      /* Check HTTP return code*/
        if (httpCode > 0)                                                               /* If return code is good*/
        {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
            {
              
              // To find first value (total cases) from return value : 75,779 2,13016,917  
              
              String payload = http.getString();                                        /* get the return value in string*/
                                                                                        // For example the return value : 75,779 2,13016,917 (it actually consists of 3 seperate values inside)
                                                                                        // These numbers consists of 3 values: 75779 (cases), 2130 (deaths), and 16917 (recovered)
              String payload2 = payload ;                                               // make a copy of this return string value to calculate 2nd value and to prevent mix up value and calculation
              Serial.println(payload);                                                  /* display the raw values get from API link */
              payload.replace(","  ,  "");                                              /* replace comma with nothing, means delete the comma so that can be convert to float value*/
              f1 = payload.toFloat();                                                   /* convert string to float value. All values after a blank space will automatically deleted */
              Serial.println(f1,0);                                                     /* display the final values which to be displayed on ThingSpeak channel later*/
                                                                                        // defined f1 as total cases of coronavirus
              
              // Text : 3,849,011 33,9393,331,411          
                             
              int emptySpace = payload2.indexOf(" ");                                   /* find the empty white space location, which is character number 6, starting value is 0 */
              payload2.remove(0,emptySpace);                                            /* remove all the characters until the white empty space, result : 2,13016,917 */
              
              String payload3 = payload2 ;                                              // make a copy of previous return string value to calculate third value and to prevent mix up value and calculation with second value
                                               
              int firstComma = payload2.indexOf(',');                                   // Get fisrt comma position
              int secondComma = payload2.indexOf(',', firstComma + 1);                  // Get second comma position
              int thirdComma = payload2.indexOf(',', firstComma + 2);                   // Get third comma position
              int fourthComma = payload2.indexOf(',', firstComma + 3);                  // Get third comma position
              if(secondComma-firstComma > 4 )                                           // Determine the position to seperate second value and third value
              { trimPosition = firstComma+4 ; }
              if(thirdComma-secondComma > 4 )
              { trimPosition = secondComma+4 ; }
              if(fourthComma-thirdComma > 4 )
              { trimPosition = thirdComma+4 ; }
              payload2.remove(trimPosition);                                            // to determine second value, trim the value starting the 4th value after a comma
              payload2.replace(","  ,  "");                                             // replace comma with nothing, means delete the comma so that can be convert to float value*/
              f2 = payload2.toFloat();                                                  /* convert string to float value.*/
              Serial.println(f2,0);                                                     /* display the final values which to be displayed on ThingSpeak channel later*/
                                                                                        // defined f2 as total deaths of coronavirus

              // To find third value (total recovered) from result : 2,13016,917 
              
              payload3.remove(0,trimPosition);                                          /* since previously has determined the seperation location (which is character number 4 after a comma), trim all values from starting, result = 16,917*/
              payload3.replace(","  ,  "");                                             // replace comma with nothing, means delete the comma so that can be convert to float value*/
              f3 = payload3.toFloat();                                                  /* convert string to float value.*/
              Serial.println(f3,0);                                                     /* display the final values which to be displayed on ThingSpeak channel later*/
                                                                                        // defined f3 as total recovered of coronavirus
            }
        }
        else 
        {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        
        /* 3- Write NodeMCU to ThingSpeak channel */

        if (client.connect(server,httpPortWrite))                                     /* if connection to ThingSpeak server is ok. "184.106.153.149" or api.thingspeak.com*/
          {  
            String postStr = apiKey;                                                // set the reat API key
            postStr +="&field1=";                                                   // Put the value on field1 of the channel (channel based on the write API Key)
            postStr += String(f1);                                                  // Set the calculated value f1 (previous code) to write to channel 1
            postStr +="&field2=";                                                   
            postStr += String(f2);                                                  // Set the calculated value f2 (previous code) to write to channel 2
            postStr +="&field3=";                                                   
            postStr += String(f3);                                                  // Set the calculated value f3 (previous code) to write to channel 3
            postStr += "\r\n\r\n";                                                  // proper code that need to send to ThinkSpeak via REST API protocol. Do not need to edit anything here
            client.print("POST /update HTTP/1.1\n");                             
            client.print("Host: api.thingspeak.com\n");
            client.print("Connection: close\n");
            client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
            client.print("Content-Type: application/x-www-form-urlencoded\n");
            client.print("Content-Length: ");
            client.print(postStr.length());
            client.print("\n\n");
            client.print(postStr);
            Serial.println("Values have sent to ThingSpeak ");                      /* to confirm that values are sent to channel successfully*/
          }

        http.end();                                                                 /* after Read and Write codes are completed, close the http connection*/
      } 
    else 
      {
        Serial.printf("[HTTP} Unable to connect\n");
      }
    delay(60000);                                                                 // update every 10 minutes      
}
