#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

painlessMesh  mesh;

// Receiving Serial Commands
String inputString = "";         // a string to hold incoming data

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("logServer: Received from %u msg=%s\n", from, msg.c_str());
}

void setup() {
  Serial.begin(115200);

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, STA_AP, AUTH_WPA2_PSK, 6 );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection([](size_t nodeId) { Serial.printf("New Connection %u\n", nodeId); });
  mesh.onDroppedConnection([](size_t nodeId){ Serial.printf("Dropped Connection %u\n", nodeId);  });

  inputString.reserve(200);
}

void loop() {
  mesh.update();
  while(Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      sendBroadcast(inputString);
      inputString = "";
    } else {
      inputString += inChar;      
    }
  }
}

void sendBroadcast(String message) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();
  
  msg["to"] = "all";
  msg["topic"] = "LoggerBroadCast";
  msg["msg"] = message;

  String str;
  msg.printTo(str);
  String logMessage = String("Sending: " + str);
  Serial.println(logMessage); 
  mesh.sendBroadcast(str);  
}


