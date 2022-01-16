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

//const int table1_path_lenght = 5;
//const unsigned char kitchen_table1_path[table1_path_lenght] = "RRLSB";
//const unsigned char table1_kitchen_path[table1_path_lenght] = "RLLSB";
//
//const int table2_path_lenght = 4;
//const unsigned char kitchen_table2_path[table2_path_lenght] = "RSSB";
//const unsigned char table2_kitchen_path[table2_path_lenght] = "SLSB";
//
//const int table3_path_lenght = 5;
//const unsigned char kitchen_table3_path[table3_path_lenght] = "RLRSB";
//const unsigned char table3_kitchen_path[table3_path_lenght] = "LRLSB";
//
//const int table4_path_lenght = 9;
//const unsigned char kitchen_table4_path[table4_path_lenght] = "RLSLRRSSB";
//const unsigned char table4_kitchen_path[table4_path_lenght] = "SLLRSRLSB";
//
//const int table5_path_lenght = 9;
//const unsigned char kitchen_table5_path[table5_path_lenght] = "RLSLRRLSB";
//const unsigned char table5_kitchen_path[table5_path_lenght] = "RLLRSRLSB";
//
//const int table6_path_lenght = 8;
//const unsigned char kitchen_table6_path[table6_path_lenght] = "RLSLRSSB";
//const unsigned char table6_kitchen_path[table6_path_lenght] = "SLRSRLSB";
//
//const int table7_path_lenght = 5;
//const unsigned char kitchen_table7_path[table7_path_lenght] = "RLLSB";
//const unsigned char table7_kitchen_path[table7_path_lenght] = "RRLSB";

//        "RRLSB", //kitchen to table 1
//        "RSSB", //kitchen to table 2
//        "RLRSB", //kitchen to table 3
//        "RLSLRRSSB", //kitchen to table 4
//        "RLSLRRLSB", //kitchen to table 5
//        "RLSLRSSB", //kitchen to table 6
//        "RLLSB", //kitchen to table 7
//        "RRLSB", //table 7 to kitchen
//        "SLRSRLSB", //table 6 to kitchen
//        "RLLRSRLSB", //table 5 to kitchen
//        "SLLRSRLSB", //table 4 to kitchen
//        "LRLSB", //table 3 to kitchen
//        "SLSB", //table 2 to kitchen
//        "RLLSB" //table 1 to kitchen

const unsigned char paths_matrix[][20] = {"",
        "RRLB", //kitchen to table 1
        "RSB", //kitchen to table 2
        "RLRB", //kitchen to table 3
        "RLSLRRSB", //kitchen to table 4
        "RLSLRRLB", //kitchen to table 5
        "RLSLRSB", //kitchen to table 6
        "RLLB", //kitchen to table 7
        "RRLB", //table 7 to kitchen
        "SLRSRLB", //table 6 to kitchen
        "RLLRSRLB", //table 5 to kitchen
        "SLLRSRLB", //table 4 to kitchen
        "LRLB", //table 3 to kitchen
        "SLB", //table 2 to kitchen
        "RLLB" //table 1 to kitchen
    };

const int matrix_size = sizeof(paths_matrix)/sizeof(paths_matrix[0]);


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


  for (counter=0; counter<=80; counter++)
  {
    if (counter < 20 || counter >= 60)
      OrangutanMotors::setSpeeds(40, -40);
    else
      OrangutanMotors::setSpeeds(-40, 40);

    robot.calibrateLineSensors(IR_EMITTERS_ON);

    delay(20);
  }

  OrangutanMotors::setSpeeds(0, 0);

//  int bat = read_battery_millivolts();
//
//  if (!Serial.available()){
//    Serial.println("Battery Level:");
//    Serial.print(bat);
//    Serial.println(" mV");
//    Serial.println("=============================");
//    }
    
  
}

void follow_segment()
{
  int last_proportional = 0;
  long integral=0;

  while(1)
  {
    // Get the position of the line.
    unsigned int position = robot.readLine(sensors);

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

int index(int num, int total_size){
    if (num >= 0)
        return num;
    else
        return  (num % total_size) + (total_size + 1);
    
}

void loop(){

  while (!Serial.available()){}

  unsigned int target = 0;

  if (Serial.available()){
    target = Serial.parseInt();
  }

  

  char path[20] = "";
  int ind = index(target, matrix_size - 1);
  strcpy(path, paths_matrix[ind]);
//  char path[15] = "";
//  int path_length = 0;

//  switch(target){
//    case 1:
//      // Kitchen -> Table 1
//      strcpy(path, kitchen_table1_path);
//      path_length = table1_path_lenght;
//      break;
//
//    case 2:
//      // Kitchen -> Table 2
//      strcpy(path, kitchen_table2_path);
//      path_length = table2_path_lenght;
//      break;
//    
//    case 3:
//      // Kitchen -> Table 3
//      strcpy(path, kitchen_table3_path);
//      path_length = table3_path_lenght;
//      break;
//
//    case 4:
//      // Kitchen -> Table 4
//      strcpy(path, kitchen_table4_path);
//      path_length = table4_path_lenght;
//      break;
//
//    case 5:
//      // Kitchen -> Table 5
//      strcpy(path, kitchen_table5_path);
//      path_length = table5_path_lenght;
//      break;
//
//    case 6:
//      // Kitchen -> Table 6
//      strcpy(path, kitchen_table6_path);
//      path_length = table6_path_lenght;
//      break;
//
//    case 7:
//      // Kitchen -> Table 7
//      strcpy(path, kitchen_table7_path);
//      path_length = table7_path_lenght;
//      break;
//    
//    case -1:
//      // Table 1 -> Kitchen
//      strcpy(path, table1_kitchen_path);
//      path_length = table1_path_lenght;
//      break;
//
//    case -2:
//      // Table 2 -> Kitchen
//      strcpy(path, table2_kitchen_path);
//      path_length = table2_path_lenght;
//      break;
//
//    case -3:
//      // Table 3 -> Kitchen
//      strcpy(path, table3_kitchen_path);
//      path_length = table3_path_lenght;
//      break;
//
//    case -4:
//      // Table 4 -> Kitchen
//      strcpy(path, table4_kitchen_path);
//      path_length = table4_path_lenght;
//      break;
//
//    case -5:
//      // Table 5 -> Kitchen
//      strcpy(path, table5_kitchen_path);
//      path_length = table5_path_lenght;
//      break;
//
//    case -6:
//      // Table 6 -> Kitchen
//      strcpy(path, table6_kitchen_path);
//      path_length = table6_path_lenght;
//      break;
//
//    case -7:
//      // Table 7 -> Kitchen
//      strcpy(path, table7_kitchen_path);
//      path_length = table7_path_lenght;
//      break;
//  }

  
  int i;
  for (i=0; i < sizeof(path); i++){  // sizeof()

    follow_segment();
    
//    if (path[i] != 'B'){  
//
//      // follow_segment();
//      // Drive straight while slowing down, as before.
//      OrangutanMotors::setSpeeds(40, 40);
////      delay(50);
//      delay(200);
////      OrangutanMotors::setSpeeds(40, 40);
////      delay(200);
//    }

    OrangutanMotors::setSpeeds(40, 40);
    delay(220);
      
    // Make a turn according to the instruction stored in
    // path[i].
    turn(path[i]);
    delay(10);
    if (path[i] == 'B'){
      break;
    }
  }

  Serial.println('0');
  OrangutanMotors::setSpeeds(0, 0);
  delay(500);
}
