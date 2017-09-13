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

void setup() {
  Serial.begin(115200);
  pinMode(AIA,OUTPUT);
  pinMode(AIB,OUTPUT);
  pinMode(BIA,OUTPUT);
  pinMode(BIB,OUTPUT);

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, STA_AP, AUTH_WPA2_PSK, 6 );
  mesh.onReceive(&receivedCallback);
}

void loop() {
  mesh.update();
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("logServer: Received from %u msg=%s\n", from, msg.c_str());
}

void forward(int lspd, int rspd) {
  digitalWrite(AIA, rspd);       
  digitalWrite(BIA, 0);       
  digitalWrite(AIB, 0);       
  digitalWrite(BIB, lspd);    
}

void backward(int lspd, int rspd) {
  digitalWrite(AIA, 0);       
  digitalWrite(BIA, lspd);       
  digitalWrite(AIB, rspd);       
  digitalWrite(BIB, 0);    
}

