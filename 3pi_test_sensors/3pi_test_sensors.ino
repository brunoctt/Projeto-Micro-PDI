#include <Pololu3pi.h>
#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanAnalog.h>
#include <OrangutanLEDs.h>
#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanBuzzer.h>
#include <NewPing.h>




Pololu3pi robot;
unsigned int sensors[5]; // an array to hold sensor values
const int PC5_pin = 19;
const int MAX_DISTANCE = 400;

NewPing sonar(PC5_pin, PC5_pin, MAX_DISTANCE);


void setup(){
  unsigned int counter; 
  Serial.begin(9600);
  robot.init(2000);

  
  while (!OrangutanPushbuttons::isPressed(BUTTON_B)){
    }
  
  OrangutanPushbuttons::waitForRelease(BUTTON_B);
  delay(1000);


  for (counter=0; counter<80; counter++)
  {
    if (counter < 20 || counter >= 60)
      OrangutanMotors::setSpeeds(40, -40);
    else
      OrangutanMotors::setSpeeds(-40, 40);

    robot.calibrateLineSensors(IR_EMITTERS_ON);

    delay(20);
  }


  OrangutanMotors::setSpeeds(0, 0);

  if (!Serial.available())
    Serial.write("Saindo do teste");

//  int bat = read_battery_millivolts();
//
//  if (!Serial.available()){
//    Serial.println("Battery Level:");
//    Serial.print(bat);
//    Serial.println(" mV");
//    Serial.println("=============================");
//    }
//    
//   pinMode(PC5_pin, OUTPUT);      // sets the digital pin as output
}


void loop(){
//  robot.readLine(sensors, IR_EMITTERS_ON);
//  if (!Serial.available()){
//      int i;
//      for (i=0; i<5; i++){
//        Serial.println(sensors[i]);
//      }
//      Serial.println("=============================");
//    }
//   delay(2000);
//  Serial.write("Dentro do loop");
  delay(1000);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  Serial.print("Ping: ");
  float distance = sonar.ping_cm();
  Serial.println(distance); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");

//  OrangutanMotors::setSpeeds(40, 40);
//  if distance < 10:
//    OrangutanMotors::setSpeeds(0, 0);


}
