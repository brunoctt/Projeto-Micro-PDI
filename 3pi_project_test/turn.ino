#include <Pololu3pi.h>
#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanAnalog.h>
#include <OrangutanLEDs.h>
#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanBuzzer.h>


void turn(unsigned char dir)
{
  const int turn_intensity = 55;
    
  switch(dir)
  {
  case 'L':
    // Turn left.
    OrangutanMotors::setSpeeds(-turn_intensity, turn_intensity);
//    OrangutanMotors::setSpeeds(-turn_intensity, turn_intensity);
//    delay(350);
    break;
  case 'R':
    // Turn right.
    OrangutanMotors::setSpeeds(turn_intensity, -turn_intensity);
//    OrangutanMotors::setSpeeds(turn_intensity, -turn_intensity);
//    delay(350);
    break;
  case 'B':
    // Turn around.
    OrangutanMotors::setSpeeds(turn_intensity, -turn_intensity);
//    delay(680);
    break;
  case 'S':
    // Don't do anything!
    return;
  }
//  if (dir != 'B'){
  delay(80);
  do{
    robot.readLine(sensors);
  //      } while(position != 2000);
  } while(sensors[2] <= 950);
//  }
//  if (dir == 'B'){
//    delay(80);
//    do{
//    robot.readLine(sensors);
//  } while(sensors[2] <= 950);
//  }
}