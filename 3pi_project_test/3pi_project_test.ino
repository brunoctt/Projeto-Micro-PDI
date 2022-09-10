#include <Vector.h>
#include <Pololu3pi.h>
#include <PololuQTRSensors.h>
#include <OrangutanMotors.h>
#include <OrangutanAnalog.h>
#include <OrangutanLEDs.h>
#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanBuzzer.h>

int convert_coordinate(char direction);
char convert_to_turn(int turn);
char get_turn(char direction, int *robot_facing);
Vector<char> create_path_turns(int source, int destination, int *robot_facing);

Pololu3pi robot;
unsigned int sensors[5]; // an array to hold sensor values
const int n_vertices = 11;
// 2d array of vectors is used to store the graph in the form of an adjacency list
Vector<Vector<String>> adj;
int robot_facing = convert_coordinate('N');
int robot_location = 0;


const char welcome[] PROGMEM = ">g32>>c32";

/**
    Inputs relation between two nodes into adjacency matrix
*/
void add_edge(int src, int dest, String fdir, String sdir)
{
  adj[src][dest] = fdir;
  adj[dest][src] = sdir;
}


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

  // Initializing direction robot is facing
	for (int i=0; i<adj.size(); i++){
      String aux[n_vertices];
      Vector<String> v(aux);
      adj.push_back(v);
	}

  // Adding coordinates of nodes to adjacency matrix, mapping directions from source to destination
  add_edge(0, 1, "NE", "WS");
	add_edge(1, 2, "N", "S");
	add_edge(1, 4, "SE", "WN");
	add_edge(1, 5, "E", "W");
	add_edge(2, 3, "NWN", "SES");
	add_edge(2, 6, "E", "W");
	add_edge(2, 10, "W", "E");
	add_edge(3, 7, "EE", "WW");
	add_edge(3, 8, "EN", "SW");
	add_edge(3, 9, "N", "S");

//  int bat = read_battery_millivolts();
//
//  if (!Serial.available()){
//    Serial.println("Battery Level:");
//    Serial.print(bat);
//    Serial.println(" mV");
//    Serial.println("=============================");
//    }
    
  
}

void loop(){

  while (!Serial.available()){}

  unsigned int target = 0;

  if (Serial.available()){
    target = Serial.parseInt();
  }

  Vector<char> path = create_path_turns(robot_location, target, &robot_facing);
  
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

  // Once robot reaches destination, change its location to dest
  robot_location = target;

  Serial.println('0');
  OrangutanMotors::setSpeeds(0, 0);
  delay(500);
}
