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
void add_edge(int src, int dest, String fdir, String sdir);
Vector<char> create_path_turns(int source, int destination, int *robot_facing);
bool final_node(int node);

Pololu3pi robot;
unsigned int sensors[5]; // an array to hold sensor values
const int n_vertices = 11;
// 2d array of vectors is used to store the graph in the form of an adjacency list
char adj[n_vertices][n_vertices][4];
int robot_facing = convert_coordinate('N');
int robot_location = 0;

const char welcome[] PROGMEM = ">g32>>c32";

/**
    Inputs relation between two nodes into adjacency matrix
*/
void add_edge(int src, int dest, char fdir[], char sdir[])
{
  strcpy(adj[src][dest], fdir);
  strcpy(adj[dest][src], sdir);
}

void setup(){
  unsigned int counter; 
  Serial.begin(9600);
  //Serial.setTimeout(1);
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

  int bat = read_battery_millivolts();
  if (!Serial.available()){
    Serial.println("Battery Level:");
    Serial.print(bat);
    Serial.println(" mV");
    Serial.println("=============================");
    }

  OrangutanMotors::setSpeeds(0, 0); 
  
  // Adding coordinates of nodes to adjacency matrix, mapping directions from source to destination
  add_edge(0, 1, "NE", "WS");
  add_edge(1, 2,"N", "S");
  add_edge(1, 4, "SE", "WN");
  add_edge(1, 5, "E", "W");
  add_edge(2, 3, "NWN", "SES");
  add_edge(2, 6, "E", "W");
  add_edge(2, 10, "W", "E");
  add_edge(3, 7, "EE", "WW");
  add_edge(3, 8, "EN", "SW");
  add_edge(3, 9, "N", "S");
  Serial.println("Connections list:");
  for (int i =0; i<n_vertices; i++){
    for (int j =0; j<n_vertices; j++){
      if (adj[i][j][0] != '\0'){
        Serial.print(i);
        Serial.print('-');
        Serial.println(j);
        Serial.println(adj[i][j]);
  }}}
}

void loop(){
  Serial.println("\n");
  Serial.print("Current robot location: ");
  Serial.println(robot_location);
  Serial.println("Input destination:");

  while (!Serial.available()){}
  int target = -1;
  if (Serial.available()){
    target = Serial.parseInt();
//    String r = Serial.readString();
//    Serial.println(r);
//    target = r.toInt();
  }

  if (target != 0){
    if (target == -1)
      target = 0;
    Serial.println("Target:");
    Serial.println(target);
    
    if (robot_location == target)
      return;
    Vector<char> path = create_path_turns(robot_location, target, &robot_facing);
  
    for (int i=0; i < sizeof(path); i++){
  
      Serial.println(path[i]);
      if (i == 0 && path[i] == 'S')
        continue;
        
      follow_segment();
  
      OrangutanMotors::setSpeeds(40, 40);
      delay(220);

      if (path[i] == 'B' && !final_node(target)){
        robot_facing = (robot_facing + 2) % 4;
        break;
      }
        
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
    OrangutanMotors::setSpeeds(0, 0);
  }
  delay(2000);
}
