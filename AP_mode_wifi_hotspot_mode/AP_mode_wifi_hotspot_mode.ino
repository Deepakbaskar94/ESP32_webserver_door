#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

String qsid = "Door_ESP";  // Enter SSID here
String qpass = "87654321";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

uint8_t LED1pin = 2;
bool LED1status = LOW;

uint8_t LED2pin = 5;
bool LED2status = LOW;

String esid;
String epass = "";

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(LED1pin, OUTPUT);

  
//=====================================================================
//Writing to EEPROM
  Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();

//=====================================================================
  //reading from EEPROM
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
    //Serial.print(esid);
  }
  Serial.println();
  Serial.print("RSSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("RPASS: ");
  Serial.println(epass);
  WiFi.softAP(esid.c_str(), epass.c_str());

//=====================================================================
  
  //WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/dooropen", opendoor);
  server.on("/doorclose", closedoor);
  //server.on("/repeat", repeat);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}




void loop() {
  server.handleClient();
  if(LED1status)
  {digitalWrite(LED1pin, HIGH);}
  else
  {digitalWrite(LED1pin, LOW);}
  
  if(LED2status)
  {digitalWrite(LED2pin, HIGH);}
  else
  {digitalWrite(LED2pin, LOW);}
}


//All API for the led on and led off according to front end input
void handle_OnConnect() {
  LED1status = LOW;
  LED2status = LOW;
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,LED2status)); 
}

void opendoor() {
  LED1status = HIGH;
  Serial.println("GPIO4 Status: ON");
  server.send(200, "text/html", SendHTML(true,LED2status)); 
}

void closedoor() {
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF");
  server.send(200, "text/html", SendHTML(false,LED2status)); 
}

void handle_setting() {
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF");
  server.send(200, "text/html", SendHTML(false,LED2status)); 
}


void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}




//HTML Template for the Door open and close 
String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Probe Plus Door Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} label {color: #444444;margin-bottom: 50px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Probe Plus Door Control</h1>\n";
  //ptr +="<h3>My Wifi Name Pneumatic Door-1</h3>\n";
  
 //if(led1stat)
  //{
  ptr +="<a class=\"button button-on\" href=\"/dooropen\">OPEN</a>";
  ptr +="<a class=\"button button-off\" href=\"/doorclose\">CLOSE</a>\n";
    //}
  //else
  //{
  


  ptr +="<form method='get' action=\"/repeat\">\n";
  ptr +="<label>Repeat:</label>\n";
  ptr +="<input name=\'repeat' length=5>\n";
  ptr +="<input type='submit' value='Start'></form>\n";

//  ptr +="<form method='get' action=\"/led1off\">\n";
//  ptr +="<label>PASSWORD:</label>\n";
//  ptr +="<input name='pass' length=64>\n";
//  ptr +="<input type='submit'></form>\n";
  
//  if(led2stat)
//  {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
//  else
//  {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";

  server.on("/repeat", []() {
      String repeat = server.arg("repeat");
      int repeat1 = repeat.toInt();
      //String qpass = server.arg("pass");
      for (int i = 0; i < repeat1; ++i) {
          digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(1000);                       // wait for a second
          digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
          delay(1000); 
        }
      server.send(200, "text/html", SendHTML(false,LED2status));
        });

  
  return ptr;

  
}
