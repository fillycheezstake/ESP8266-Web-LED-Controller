#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WS2812FX.h>
#include <FS.h>
#include <ArduinoJson.h>

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

#define LED_PIN D6
#define LED_COUNT 112

#define WIFI_TIMEOUT 30000
#define HTTP_PORT 80

#define DEFAULT_COLOR 0xFF5900
#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_SPEED 1000
#define DEFAULT_MODE FX_MODE_STATIC

#define API_KEY "FOO"

unsigned long last_wifi_check_time = 0;

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(HTTP_PORT);

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("Starting...");

  Serial.println("WS2812FX setup");
  ws2812fx.init();
  ws2812fx.setMode(DEFAULT_MODE);
  ws2812fx.setColor(DEFAULT_COLOR);
  ws2812fx.setSpeed(DEFAULT_SPEED);
  ws2812fx.setBrightness(DEFAULT_BRIGHTNESS);
  ws2812fx.start();

  Serial.println("Wifi setup");
  wifi_setup();
 
  Serial.println("HTTP server setup");

  SPIFFS.begin();

  server.on("/set", srv_handle_set);
  server.on("/modes", srv_handle_modes);
  server.on("/status", srv_handle_status);
  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  server.begin();
  Serial.println("HTTP server started.");

  Serial.println("ready!");

  
}


void loop() {
  unsigned long now = millis();

  server.handleClient();
  ws2812fx.service();

  if(now - last_wifi_check_time > WIFI_TIMEOUT) {
    Serial.print("Checking WiFi... ");
    if(WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi connection lost. Reconnecting...");
      wifi_setup();
    } else {
      Serial.println("OK");
    }
    last_wifi_check_time = now;
  }
}


void wifi_setup() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.mode(WIFI_STA);

  while(WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

//-------WEBSERVER HANDLES -----

//Handle /modes
//sends list of modes in JSON array
void srv_handle_modes() {

  uint8_t num_modes = ws2812fx.getModeCount();
  const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(num_modes*2);
  DynamicJsonDocument doc(capacity);

  doc["modeCount"] = num_modes;
  JsonArray Modes = doc.createNestedArray("Modes");
  
  for(int i=0; i < num_modes; i++) {
    Modes.add(ws2812fx.getModeName(i));
  }

  serializeJson(doc, Serial);
  String jsonString = "";
  serializeJson(doc,jsonString);
  
  server.send(200,"application/json", jsonString);
}


//handle /status
//sends JSON
void srv_handle_status() {

  const size_t capacity = JSON_OBJECT_SIZE(8);
  DynamicJsonDocument doc(capacity);

  
  doc["currentColor"] = ws2812fx.getColor();
  doc["currentModeName"] = ws2812fx.getModeName(ws2812fx.getMode());
  doc["currentModeIndex"] = ws2812fx.getMode();
  doc["modeCount"] =  ws2812fx.getModeCount();
  doc["currentSpeed"] = ws2812fx.getSpeed();
  doc["currentBrightness"] = ws2812fx.getBrightness();
  doc["currentState"] = ws2812fx.isRunning();

  serializeJson(doc, Serial);
  String jsonString = "";
  serializeJson(doc,jsonString);
  
  server.send(200,"application/json", jsonString);
}

//handles /set
//uses GET request - arguments
void srv_handle_set() {
  
  if (server.hasArg("API_KEY")){
    Serial.println("API_KEY found");
    
    if (server.arg("API_KEY") == API_KEY) {
      Serial.println("API_KEY verified");


      for (uint8_t i=0; i < server.args(); i++){
        
      if(server.argName(i) == "c") {
        Serial.print("Raw arg: ");
        Serial.println(server.arg(i));
        
        if(server.arg(i).equalsIgnoreCase("RED")) {
          ws2812fx.setColor(RED);
          Serial.print("Color Set to: #"); 
          Serial.println(RED,HEX);
        }
        else if(server.arg(i).equalsIgnoreCase("GREEN")) {
          ws2812fx.setColor(GREEN);
          Serial.print("Color Set to: #"); 
          Serial.println(GREEN,HEX);
        }
        else if(server.arg(i).equalsIgnoreCase("BLUE")) {
          ws2812fx.setColor(BLUE);
          Serial.print("Color Set to: #"); 
          Serial.println(BLUE,HEX);
        }
        else if(server.arg(i).equalsIgnoreCase("WHITE")) {
          ws2812fx.setColor(WHITE);
          Serial.print("Color Set to: #"); 
          Serial.println(WHITE,HEX);
        }
        else if(server.arg(i).equalsIgnoreCase("YELLOW")) {
          ws2812fx.setColor(YELLOW);
          Serial.print("Color Set to: #"); 
          Serial.println(YELLOW,HEX);
        }
        else if(server.arg(i).equalsIgnoreCase("PURPLE")) {
          ws2812fx.setColor(PURPLE);
          Serial.print("Color Set to: #"); 
          Serial.println(PURPLE,HEX);
        }
        else if(server.arg(i).equalsIgnoreCase("ORANGE")) {
          ws2812fx.setColor(ORANGE);
          Serial.print("Color Set to: #"); 
          Serial.println(ORANGE,HEX);
        }
        else if(server.arg(i).equalsIgnoreCase("ORANGE")) {
          ws2812fx.setColor(ORANGE);
          Serial.print("Color Set to: #"); 
          Serial.println(ORANGE,HEX);
        }
        else if(server.arg(i).equalsIgnoreCase("MAGENTA")) {
          ws2812fx.setColor(MAGENTA);
          Serial.print("Color Set to: #"); 
          Serial.println(MAGENTA,HEX);
        }
  
        else {
          uint32_t tmp = (uint32_t) strtol(server.arg(i).c_str(), NULL, 16);
          if(tmp >= 0x000000 && tmp <= 0xFFFFFF) {
            ws2812fx.setColor(tmp);
            Serial.print("Color Set to: #"); 
            Serial.println(tmp,HEX);
          }        
        }
  
      }
  
      if(server.argName(i) == "m") {
        uint8_t tmp = (uint8_t) strtol(server.arg(i).c_str(), NULL, 10);
        ws2812fx.setMode(tmp % ws2812fx.getModeCount());
        Serial.print("mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
      }
  
      if(server.argName(i) == "b") {
        if(server.arg(i)[0] == '-') {
          ws2812fx.setBrightness(ws2812fx.getBrightness() * 0.8);
        } else if(server.arg(i)[0] == ' ') {
          ws2812fx.setBrightness(_min(_max(ws2812fx.getBrightness(), 5) * 1.2, 255));
        } else { // set brightness directly
          uint8_t tmp = (uint8_t) strtol(server.arg(i).c_str(), NULL, 10);
          ws2812fx.setBrightness(tmp);
        }
        Serial.print("brightness is "); Serial.println(ws2812fx.getBrightness());
      }
  
      if(server.argName(i) == "s") {
        if(server.arg(i)[0] == '-') {
          Serial.println("minus speed");
          ws2812fx.setSpeed(_max(ws2812fx.getSpeed(), 5) * 1.2);
        } else {
          Serial.println("plus speed");
          ws2812fx.setSpeed(ws2812fx.getSpeed() * 0.8);
        }
        Serial.print("speed is "); Serial.println(ws2812fx.getSpeed());
      }
  
      if(server.argName(i) == "p") {
        if(server.arg(i)[0] == '1') {
          ws2812fx.start();
          Serial.println("Set On");
        }
        else if(server.arg(i)[0] == '0') {
          ws2812fx.stop();
          Serial.println("Set Off");
        }
      }

      }
      server.send(200, "text/plain", "OK");
    }
    else {
      Serial.println("api_key not verified");
      server.send(200, "text/plain", "BAD API KEY");
    }
  }
}


//reads files out of ESP8266 SPIFFS
bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}

//LOOKUP for correct content type string
String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
