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
char invert_coord(char c);
void add_edge(int src, int dest, String fdir, String sdir);
Vector<char> create_path_turns(int source, int destination, int *robot_facing);
bool final_node(int node);

Pololu3pi robot;
unsigned int sensors[5]; // an array to hold sensor values
const int n_vertices = 13; // maximum number of nodes
const int aux_vertices = 4;
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

void invert_dir(char *dir, int dir_size){
  char aux[dir_size];
  strcpy(aux, dir);
  for (int i = 1; i < dir_size; i++)
    dir[i-1] = invert_coord(aux[dir_size - 1 - i]);
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

  int bat = read_battery_millivolts();
  if (!Serial.available()){
    Serial.println("Battery Level:");
    Serial.print(bat);
    Serial.println(" mV");
    Serial.println("=============================");
    }

    // Nodes amount
    int dest_vert = 9;
    Serial.println("Enter number of destination nodes:");
    while (!Serial.available()){}
    dest_vert = Serial.parseInt();
    Serial.println("Enter number of auxiliary nodes:");
    while (!Serial.available()){}
    const int aux_vertices = Serial.parseInt();

    const int n_vertices = dest_vert + aux_vertices;
    Serial.print("Total of ");
    Serial.print(n_vertices);
    Serial.println(" nodes registered");


    // Registering nodes relations
    bool reg = true;
    Serial.println("Register adjacency matrix:");
    while(reg){
      Serial.println("Enter node relation (node1 node2 direction12):");
      while (!Serial.available()){}
      String input = Serial.readString();
      Serial.println(input);
      int space = input.indexOf(' ');
      int node1 = input.substring(0, space).toInt(); // possibly use trim()
      input = input.substring(space+1);
      space = input.indexOf(' ');
      int node2 = input.substring(0, space).toInt();
      input = input.substring(space+1);
      space = input.length()+1;
      char dir1[space]; 
      input.toCharArray(dir1, space);
      char dir2[space];
      strcpy(dir2, dir1);
      invert_dir(dir2, space);
      
      add_edge(node1, node2, dir1, dir2);

      Serial.println("Add another node relation? (y/n)");
      while (!Serial.available()){}
      char ans = Serial.read();
      if (ans == 'n')
        reg = false;      
      }

//  // Adding coordinates of nodes to adjacency matrix, mapping directions from source to destination
//  // Auxiliary nodes are noted as negative, so they are the last n from n_vertices
//  add_edge(0, 11, "NE", "WS");
//  add_edge(11, 10,"N", "S");
//  add_edge(11, 1, "SE", "WN");
//  add_edge(11, 2, "E", "W");
//  add_edge(10, 9, "NWN", "SES");
//  add_edge(10, 3, "E", "W");
//  add_edge(10, 7, "W", "E");
//  add_edge(9, 6, "N", "S");
//  add_edge(9, 8, "E", "W");
//  add_edge(8, 4, "E", "W");
//  add_edge(8, 5, "N", "S");
  
  Serial.println("Connections list:");
  for (int i =0; i<n_vertices; i++){
    for (int j =0; j<n_vertices; j++){
      if (adj[i][j][0] != '\0'){
        Serial.print(i);
        Serial.print('-');
        Serial.println(j);
        Serial.println(adj[i][j]);
  }}}

  Serial.println("Enter robot location: ");
  while (!Serial.available()){}
  robot_location = validate_input(true, n_vertices, aux_vertices);
  for (int i =0; i<n_vertices; i++){
    if (adj[robot_location][i][0] != '\0'){
      robot_facing = convert_coordinate(adj[robot_location][i][0]);
  }}
}

void loop(){
  Serial.println("\n");
  Serial.print("Current robot location: ");
  Serial.println(robot_location);
  Serial.println("Enter destination:");

  while (!Serial.available()){}
  int target = validate_input(false, n_vertices, aux_vertices);;
  Serial.println("Moving to target:");
  Serial.println(target);

  Vector<char> path = create_path_turns(robot_location, target, &robot_facing);

  // informing path to be followed
  // for (char p: path)
  //    Serial.print(p);
  //  Serial.println();
  //  Serial.println(path.size());

  for (int i=0; i < path.size(); i++){

    Serial.println(path[i]);
    // Since already follows segment, no need to follow first instruction if 'S'
    if (i == 0 && path[i] == 'S')
      continue;
      
    if (i > 0){
      follow_segment();
      if (path[i] == 'S'){
        OrangutanMotors::setSpeeds(50, 50);
        delay(150);
      } else {
        OrangutanMotors::setSpeeds(40, 40);
        delay(200);
      }
    }

    // If destination is auxiliary node, doesnt 180
    if (path[i] == 'B' && !final_node(target)){
      robot_facing = (robot_facing + 2) % 4;
      break;
    }

    // Navigation Debugger
    bool debug = false;
    if (debug){
      Serial.print("Turn ");
      Serial.print(i+1);
      Serial.print(" of ");
      Serial.println(path.size());
      OrangutanMotors::setSpeeds(0, 0);
      delay(2000);
      OrangutanMotors::setSpeeds(40, 40);
    }
      
    // Make a turn according to the instruction stored in
    // path[i].
    turn(path[i]);
//    delay(10);
    if (path[i] == 'B'){
      break;
    }
  }

  // Once robot reaches destination, change its location to dest
  robot_location = target;
  OrangutanMotors::setSpeeds(0, 0);
  delay(1000);
}
