SWARM NOTES

== HARDWARE ==

--  Wemos Lolin V3 NodeMcu V1.0 (black one)
Add board manage url: http://arduino.esp8266.com/stable/package_esp8266com_index.json (Preferences -> Additional Boards Manager URLs)
Install ESP8266: Tools -> Board: XXX -> Board Manager... -> Select latest version of esp8266 and install
Select NodeMCU 1.0 from Tools -> Board: XXXX -> NodeMCU 1.0 (ESP-12E Module)
Set Tools -> Frequency to 80MHz
Set Tools -> Upload Speed to 115200 (on board is written 9600 - either should be fine I think)

-- Wemos D1 mini
Basically same as before just select different board

== MPU6050 ==

-- Wemos Lolin V3 NodeMcu V1.0 (black one) 
Change the following code
#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
into 
#define INTERRUPT_PIN D6  // use pin 2 on Arduino Uno & most boards

Connections:
Wemos	->	MPU6050
3V		-> 	VCC
G		->	GND
D1		->	SCL
D2		->	SDA
D6		->	INT

-- Wemos D1 mini
Basically same as before just select different voltage (5V)



== SONAR ==
I have tried using the PWM with the sonar, because I read on several blogs that it returned better results. However, I have noticed that this a slow process - like seriously analog is hundreds times faster - and I couldn't get better results (I might have done something wrong here).
There is a nice library I have found called Maxbotic (funny enough I read that same page like 20 times but managed to miss the library link).

link: https://github.com/Diaoul/arduino-Maxbotix
sample code:

#include "Maxbotix.h"
Maxbotix rangeSensorAD(A0, Maxbotix::AN, Maxbotix::LV, Maxbotix::BEST, 9); // Set the Maxbotix to read Analog from pin 0 with a mode filter sample sized to 9
void setup()
{
  Serial.begin(9600);
  rangeSensorAD.setADSampleDelay(10); // Set the delay between AD readings to 10ms
}

void loop() {
  start = millis();
  Serial.print("AD (BEST): ");
  Serial.print(rangeSensorAD.getRange());
  Serial.print("cm - ");
  Serial.print(millis() - start);
  Serial.print("ms - ");
  printArray(rangeSensorAD.getSample(), rangeSensorAD.getSampleSize());
  Serial.print(" - Highest Mode: ");
  Serial.print(rangeSensorAD.getSampleMode(true));
  Serial.print(" - Lowest Mode: ");
  Serial.print(rangeSensorAD.getSampleMode(false));
  Serial.print(" - Median: ");
  Serial.print(rangeSensorAD.getSampleMedian());
  Serial.print(" - Best: ");
  Serial.print(rangeSensorAD.getSampleBest());
  Serial.println();
}

void printArray(float* array, uint8_t array_size) {
  Serial.print("[");
  for (int i = 0; i < array_size; i++) {
    Serial.print(array[i]);
    if (i != array_size - 1) {
      Serial.print(", ");
    }
  }
  Serial.print("]");
}

== SWARM CONNECTION ==

link: https://gitlab.com/blackedder/painlessmesh

important parts:

#include "painlessMesh.h" // this is the library we are using

// this kind of speaks for it self
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

painlessMesh mesh; // this is the main mesh object where most operations are on
// the src for it is here: https://gitlab.com/BlackEdder/painlessMesh/blob/master/src/painlessMesh.cpp


// setting up it up 
void setup() {

	// debug message types (there are several of these if you run into bigger problems)
	mesh.setDebugMsgTypes( ERROR | STARTUP );
	
	// basically 
	mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
	mesh.onReceive(&receivedCallback);
	mesh.onNewConnection(&newConnectionCallback);
	mesh.onChangedConnections(&changedConnectionCallback);
	mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

	// if you have specific tasks to run	
	mesh.scheduler.addTask( taskSendMessage );
	taskSendMessage.enable() ;
}

// message that can be send (tends to be in his own function)

//plain string
void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// json object converted to string after
DynamicJsonBuffer jsonBuffer;
    JsonObject& msg = jsonBuffer.createObject();
    msg["topic"] = "sensor";
    msg["value"] = random(0, 180);

    String str;
    msg.printTo(str);
    if (XXXXXXX) // if you want to send a general broadcast
        mesh.sendBroadcast(str);
    else // if you want to send to a specific unit
        mesh.sendSingle(logServerId, str);
}