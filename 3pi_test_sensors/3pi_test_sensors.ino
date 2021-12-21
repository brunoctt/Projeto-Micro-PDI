#include <Pololu3pi.h>
#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanAnalog.h>
#include <OrangutanLEDs.h>
#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanBuzzer.h>




Pololu3pi robot;
unsigned int sensors[5]; // an array to hold sensor values


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

  int bat = read_battery_millivolts();

  if (!Serial.available()){
    Serial.println("Battery Level:");
    Serial.print(bat);
    Serial.println(" mV");
    Serial.println("=============================");
    }
    
  
}


void loop(){
  robot.readLine(sensors, IR_EMITTERS_ON);
  if (!Serial.available()){
      int i;
      for (i=0; i<5; i++){
        Serial.println(sensors[i]);
      }
      Serial.println("=============================");
    }
   delay(2000);
}
