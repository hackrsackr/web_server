//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "server_config.hpp"

AsyncWebServer server(80);

const char* ssid = "NETGEAR-2.4-G";
const char* password = "Finance08";

//const char* PARAM_STRING = "inputString";
//const char* PARAM_INT = "inputInt";
const char* INPUT_NAME_1 = "setpoint-1";
const char* INPUT_NAME_2 = "setpoint-2";
const char* INPUT_NAME_3 = "setpoint-3";
const char* INPUT_NAME_4 = "setpoint-4";


// HTML web page to handle 3 input fields (inputString, inputInt, inputFloat)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      //alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script></head><body>
  <form action="/get" target="hidden-form">
    Setpoint-1 <input type="text" name="setpoint-1">
    <input type="submit" value="Submit" onclick="submitMessage()">
    Current value: %setpoint-1%
  </form><br>
  <form action="/get" target="hidden-form">
    Setpoint-2 <input type="text" name="setpoint-2">
    <input type="submit" value="Submit" onclick="submitMessage()">
    Current value: %setpoint-2%
  </form><br>
  <form action="/get" target="hidden-form">
    Setpoint-3 <input type="text" name="setpoint-3">
    <input type="submit" value="Submit" onclick="submitMessage()">
    Current value: %setpoint-3%
  </form><br>
  <form action="/get" target="hidden-form">
    Setpoint-4 <input type="text" name="setpoint-4">
    <input type="submit" value="Submit" onclick="submitMessage()">
    Current value: %setpoint-4%
  </form>
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  file.close();
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

// Replaces placeholder with stored values
String processor(const String& var){
  //Serial.println(var);
  if(var == "setpoint-1"){
    return readFile(SPIFFS, "/setpoint-1.txt");
  }
  else if(var == "setpoint-2"){
    return readFile(SPIFFS, "/setpoint-2.txt");
  }
  else if(var == "setpoint-3"){
    return readFile(SPIFFS, "/setpoint-3.txt");
  }
  else if(var == "setpoint-4"){
    return readFile(SPIFFS, "/setpoint-4.txt");
  }
  return String();
}

void setup() {
  Serial.begin(115200);
  // Initialize SPIFFS
  #ifdef ESP32
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #else
    if(!SPIFFS.begin()){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(INPUT_NAME_1)) {
      inputMessage = request->getParam(INPUT_NAME_1)->value();
      writeFile(SPIFFS, "/setpoint-1.txt", inputMessage.c_str());
    }
    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    else if (request->hasParam(INPUT_NAME_2)) {
      inputMessage = request->getParam(INPUT_NAME_2)->value();
      writeFile(SPIFFS, "/setpoint-2.txt", inputMessage.c_str());
    }
    // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
    else if (request->hasParam(INPUT_NAME_3)) {
      inputMessage = request->getParam(INPUT_NAME_3)->value();
      writeFile(SPIFFS, "/setpoint-3.txt", inputMessage.c_str());
    }
    // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
    else if (request->hasParam(INPUT_NAME_4)) {
      inputMessage = request->getParam(INPUT_NAME_4)->value();
      writeFile(SPIFFS, "/setpoint-4.txt", inputMessage.c_str());
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  // To access your stored values on inputString, inputInt, inputFloat
  float setpoint_1 = readFile(SPIFFS, "/setpoint-1.txt").toFloat();
  Serial.print("setpoint-1: ");
  Serial.println(setpoint_1);
  
  float setpoint_2 = readFile(SPIFFS, "/setpoint-2.txt").toFloat();
  Serial.print("setpoint-2: ");
  Serial.println(setpoint_2);
  
  float setpoint_3 = readFile(SPIFFS, "/setpoint-3.txt").toFloat();
  Serial.print("setpoint-3: ");
  Serial.println(setpoint_3);
  
  float setpoint_4 = readFile(SPIFFS, "/setpoint-4.txt").toFloat();
  Serial.print("setpoint-4: ");
  Serial.println(setpoint_4);
  delay(5000);
}