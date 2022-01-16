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

  
  int i;
  for (i=0; i < sizeof(path); i++){  // sizeof()

    follow_segment();

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
