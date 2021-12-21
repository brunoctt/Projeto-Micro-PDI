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
const int table1_path_lenght = 5;
const unsigned char kitchen_table1_path[table1_path_lenght] = "RRLSB";
const unsigned char table1_kitchen_path[table1_path_lenght] = "RLLSB";

const int table2_path_lenght = 4;
const unsigned char kitchen_table2_path[table2_path_lenght] = "RSSB";
const unsigned char table2_kitchen_path[table2_path_lenght] = "SLSB";

const int table3_path_lenght = 5;
const unsigned char kitchen_table3_path[table3_path_lenght] = "RLRSB";
const unsigned char table3_kitchen_path[table3_path_lenght] = "LRLSB";

const int table4_path_lenght = 9;
const unsigned char kitchen_table4_path[table4_path_lenght] = "RLSLRRSSB";
const unsigned char table4_kitchen_path[table4_path_lenght] = "SLLRSRLSB";



const char welcome[] PROGMEM = ">g32>>c32";

void setup(){
  unsigned int counter; 
  Serial.begin(9600);
  robot.init(2000);

  OrangutanBuzzer::playFromProgramSpace(welcome);
  delay(1000);
  
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

void follow_segment()
{
  int last_proportional = 0;
  long integral=0;

  while(1)
  {
    // Get the position of the line.
    unsigned int position = robot.readLine(sensors, IR_EMITTERS_ON);

    // The "proportional" term should be 0 when we are on the line.
    int proportional = ((int)position) - 2000;

    // Compute the derivative (change) and integral (sum) of the
    // position.
    int derivative = proportional - last_proportional;
    integral += proportional;

    // Remember the last position.
    last_proportional = proportional;
    int power_difference = proportional/20 + integral/10000 + derivative*3/2;

    // Compute the actual motor settings.  We never set either motor
    // to a negative value.
    const int maximum = 50; // the maximum speed
    if (power_difference > maximum)
      power_difference = maximum;
    if (power_difference < -maximum)
      power_difference = -maximum;

    if (power_difference < 0)
      OrangutanMotors::setSpeeds(maximum + power_difference, maximum);
    else
      OrangutanMotors::setSpeeds(maximum, maximum - power_difference);

    if (sensors[1] < 100 && sensors[2] < 100 && sensors[3] < 100)
    {
      // There is no line visible ahead, and we didn't see any
      // intersection.  Must be a dead end.
      return;
    }
    else if (sensors[0] > 500 || sensors[4] > 500)
    {
      // Detected an intersection
      return;
    }

  }
}

void turn(unsigned char dir)
{
  const int turn_intensity = 55;
    
  switch(dir)
  {
  case 'L':
    // Turn left.
    OrangutanMotors::setSpeeds(-turn_intensity, turn_intensity);
    delay(350);
    break;
  case 'R':
    // Turn right.
    OrangutanMotors::setSpeeds(turn_intensity, -turn_intensity);
    delay(350);
    break;
  case 'B':
    // Turn around.
    OrangutanMotors::setSpeeds(turn_intensity, -turn_intensity);
    delay(700);
    break;
  case 'S':
    // Don't do anything!
    break;
  }
}

void loop(){

  while (!Serial.available()){}

  unsigned int target = 0;

  if (Serial.available()){
    target = Serial.parseInt();
    Serial.println(target);
  }
  
  char path[15] = "";
  int path_length = 0;

  switch(target){
    case 1:
      // Kitchen -> Table 1
      strcpy(path, kitchen_table1_path);
      path_length = table1_path_lenght;
      break;

    case 2:
      // Kitchen -> Table 2
      strcpy(path, kitchen_table2_path);
      path_length = table2_path_lenght;
      break;
    
    case 3:
      // Kitchen -> Table 3
      strcpy(path, kitchen_table3_path);
      path_length = table3_path_lenght;
      break;

    case 4:
      // Kitchen -> Table 4
      strcpy(path, kitchen_table4_path);
      path_length = table4_path_lenght;
      break;

    case -1:
      // Table 1 -> Kitchen
      strcpy(path, table1_kitchen_path);
      path_length = table1_path_lenght;
      break;

    case -2:
      // Table 2 -> Kitchen
      strcpy(path, table2_kitchen_path);
      path_length = table2_path_lenght;
      break;

    case -3:
      // Table 3 -> Kitchen
      strcpy(path, table3_kitchen_path);
      path_length = table3_path_lenght;
      break;

    case -4:
      // Table 4 -> Kitchen
//      Serial.println(path);
      strcpy(path, table4_kitchen_path);
//      Serial.println(path);
      path_length = table4_path_lenght;
      break;
  }

  Serial.println(path);
  
  int i;
  for (i=0; i < path_length; i++){

    if (path[i] != 'B'){
      follow_segment();

      // Drive straight while slowing down, as before.
      OrangutanMotors::setSpeeds(50, 50);
      delay(50);
      OrangutanMotors::setSpeeds(40, 40);
      delay(200);
      }
    
      
    // Make a turn according to the instruction stored in
    // path[i].
    turn(path[i]);
    delay(10);
    }

  Serial.println("Arrived");
  OrangutanMotors::setSpeeds(0, 0);
  delay(500);
}
