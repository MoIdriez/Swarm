#include "painlessMesh.h"

// network information
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

// motor information
#define AIA D4 
#define AIB D3 
#define BIA D2
#define BIB D1 

// mesh object
painlessMesh  mesh;
unsigned long lastMotorCommand;

void setup() {
  Serial.begin(115200);
  pinMode(AIA,OUTPUT);
  pinMode(AIB,OUTPUT);
  pinMode(BIA,OUTPUT);
  pinMode(BIB,OUTPUT);

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, STA_AP, AUTH_WPA2_PSK, 6 );
  mesh.onReceive(&receivedCallback);

  lastMotorCommand = millis();
}

void loop() {
  mesh.update();

  if (millis() - lastMotorCommand > 3000) {
    resetMotors();
  }
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("logServer: Received from %u msg=%s\n", from, msg.c_str());  

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(msg.c_str());
  
  String message = String(root["msg"].as<const char*>());  

  // Assuming MOTOR-forward,10,11
  int i = message.indexOf("-");
  String command = message.substring(0,i);
  String value = message.substring(i+1);

  int iv = value.indexOf(",");
  int iv1 = value.indexOf(",", iv+1);
  
  bool isForward = value.substring(0, iv).equals("forward");
  int lspd = value.substring(iv +1, iv1).toInt();
  int rspd = value.substring(iv1 +1).toInt();

  /*
  Serial.println("-----------");
  Serial.println(command);
  Serial.println(value);
  Serial.println(isForward);
  Serial.println(lspd);
  Serial.println(rspd);
  Serial.println("-----------"); 
  */
  if (command.equals("MOTOR")) {
    receivedMotorCommands(isForward, lspd, rspd);
  }
}

void receivedMotorCommands(bool isFoward, int lspd, int rspd) {
  if (lspd > 250) { lspd = 250; }
  if (rspd > 250) { rspd = 250; }
  if (lspd < 0) { lspd = 0; }
  if (rspd < 0) { rspd = 0; }

  lastMotorCommand = millis();
  if (isFoward) {
    forward(lspd, rspd);
  } else {
    backward(lspd, rspd);
  }  
}

void forward(int lspd, int rspd) {
  digitalWrite(AIA, 0);       
  digitalWrite(BIA, rspd);       
  digitalWrite(AIB, lspd);       
  digitalWrite(BIB, 0);   
   
}

void backward(int lspd, int rspd) {
  digitalWrite(AIA, lspd);       
  digitalWrite(BIA, 0);       
  digitalWrite(AIB, 0);       
  digitalWrite(BIB, rspd);    
}

void resetMotors() {
  digitalWrite(AIA, 0);       
  digitalWrite(BIA, 0);       
  digitalWrite(AIB, 0);       
  digitalWrite(BIB, 0); 
}

