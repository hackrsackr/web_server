//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages f/ 404s
//

// Import required libraries
#include <iostream>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "server_config.hpp"

AsyncWebServer server(80);

// HTML web page to handle 4 input fields
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

void notFound(AsyncWebServerRequest *request);
String readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
String processor(const String &var);

void setup() {
  Serial.begin(115200);
  std::cout.precision(3);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    std::cout << "An Error has occurred while mounting SPIFFS" << std::endl;      return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(_SSID, _PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    std::cout << "WiFi Failed!" << std::endl;
    return;
  } 
  std::cout << "IP Address: " << WiFi.localIP() << std::endl;

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    for (uint8_t i=0; i<_NUMBER_OF_SPUNDERS; i++)
    {
      if (request->hasParam(_SETPOINT_NAMES[i])) 
      {
        inputMessage = request->getParam(_SETPOINT_NAMES[i])->value();
        writeFile(SPIFFS, _SETPOINT_FILES[i], inputMessage.c_str());
      } else {
        inputMessage = "No message sent"; 
      }
    }
    std::cout << inputMessage.toFloat() << std::endl;
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  // To access your stored values
  std::array<float, _NUMBER_OF_SPUNDERS> setpoint_vols;
  std::cout << "{";
  for (uint8_t i=0; i<_NUMBER_OF_SPUNDERS; i++)
  {
    setpoint_vols[i] = readFile(SPIFFS, _SETPOINT_FILES[i]).toFloat(); 
    std::cout << "\"" <<_SETPOINT_NAMES[i] << "\": " << setpoint_vols[i];
    if(i != (_NUMBER_OF_SPUNDERS-1))
    {
      std::cout << ", ";
    }
  }
  std::cout << "}" << std::endl;

  delay(5000);
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  //std::cout << "Reading file: " << path << std::endl;
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    std::cout << "- empty file or failed to open file" << std::endl;
    return String();
  }
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  file.close();
  //std::cout << "-read from file: " << fileContent.toFloat() << std::endl;

  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message)
{
  std::cout << "Writing file: " << path << std::endl;
  File file = fs.open(path, "w");
  if(!file){
    std::cout << "- failed to open file for writing" << std::endl;
    return;
  }
  if(file.print(message)){
    std::cout << "- file written" << std::endl;
  } else {
    std::cout << "- write failed" << std::endl;
  }
  file.close();
}

// Replaces placeholder with stored values
String processor(const String& var){
  //std::cout << var << std::endl;
  for (uint8_t i=0; i<_NUMBER_OF_SPUNDERS; i++)
  {
    if(var == _SETPOINT_NAMES[i])
    {
      
      return readFile(SPIFFS, _SETPOINT_FILES[i]);
    } else {}
  }
  return String();
}

