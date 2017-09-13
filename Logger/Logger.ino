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
    inputString += inChar;
    if (inChar == '\n') {
      receivedSerial();
      inputString = "";
    }
  }
}

void receivedSerial() {
  sendMotorCommands(inputString);
}

void sendMotorCommands(String message) {
  int i = message.indexOf("-");
  String lspd = message.substring(0,i);
  String rspd = message.substring(i+1, message.length()-1);

  sendMotorCommands(lspd.toInt(), rspd.toInt());    
}

void sendMotorCommands(int lspd, int rspd) {
  if (lspd > 250) { lspd = 250; }
  if (rspd > 250) { rspd = 250; }
  if (lspd < 0) { lspd = 0; }
  if (rspd < 0) { rspd = 0; }

  String value = String( String(lspd) + "-" + String(rspd) );
  sendBroadcast("MotorCommands", value);  
}

void sendBroadcast(String topic, String value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();
  
  msg["from"] = mesh.getNodeId();
  msg["to"] = "all";
  msg["topic"] = topic;
  msg["value"] = value;

  String str;
  msg.printTo(str);
  logBroadcast(str);
  mesh.sendBroadcast(str);  
}

void logBroadcast(String message) {
  String logMessage = String("Sending: " + message);
  Serial.println(logMessage);  
}


