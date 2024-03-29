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
  const int turn_intensity = 40;
    
  switch(dir)
  {
  case 'L':
    // Turn left.
    OrangutanMotors::setSpeeds(-turn_intensity, turn_intensity);
    break;
  case 'R':
    // Turn right.
    OrangutanMotors::setSpeeds(turn_intensity, -turn_intensity);
    break;
  case 'B':
    // Turn around.
    OrangutanMotors::setSpeeds(turn_intensity, -turn_intensity);
    break;
  case 'S':
    // Keep going.
    OrangutanMotors::setSpeeds(50, 50);
    return;
  }
  delay(100);
  do{
    robot.readLine(sensors);
  } while(sensors[2] <= 950);
}
