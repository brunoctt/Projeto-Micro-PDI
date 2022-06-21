#include <bits/stdc++.h>
using namespace std;

// no. of vertices
const unsigned int n_vertices = 5;

// 2d array of vectors is used to store the graph
// in the form of an adjacency list
char adj[n_vertices][n_vertices][3];

int convert_coordinate(char direction);
char convert_to_turn(int turn);
char get_turn(char direction, int *robot_facing);

// utility function to form edge between two vertices
// source and dest
void add_edge(int src, int dest, char fdir[], char sdir[])
{
	strcpy(adj[src][dest], fdir);
	strcpy(adj[dest][src], sdir);
}

// a modified version of BFS that stores predecessor
// of each vertex in array p
// and its distance from source in array d
bool BFS(int src, int dest, int pred[], int dist[])
{
	// a queue to maintain queue of vertices whose
	// adjacency list is to be scanned as per normal
	// DFS algorithm
	vector<int> queue;
	queue.push_back(src);

	// boolean array visited[] which stores the
	// information whether ith vertex is reached
	// at least once in the Breadth first search
	vector<bool> visited(n_vertices, false);

	// initially all vertices are unvisited
	// so v[i] for all i is false
	// and as no path is yet constructed
	// dist[i] for all i set to infinity
	for (int i = 0; i < n_vertices; i++) {
// 		for (int k = 0; k < n_vertices; k++)
// 		    visited[i][k] = false;
		dist[i] = INT_MAX;
		pred[i] = -1;
	}

	// now source is first to be visited and
	// distance from source to itself should be 0
	visited[src] = true;
	dist[src] = 0;
	
	// standard BFS algorithm
	while (!queue.empty()) {
		int u = queue[0];
		queue.erase(queue.begin());
// 		queue.pop_front();
		for (int i = 0; i < n_vertices; i++) {
			if (visited[i] == false && adj[u][i][0]) {
			 //   cout << u << "->" << i;
			 //   cout << "\n";
				visited[i] = true;
				dist[i] = dist[u] + 1;
				pred[i] = u;
				queue.push_back(i);

				// We stop BFS when we find
				// destination.
				if (i == dest)
					return true;
			}
		}
	}

	return false;
}

/**
    Finds shortest path from source to destination
*/
vector<int> create_path_coordinates(int s, int dest)
{
	// predecessor[i] array stores predecessor of i and distance array stores 
    // distance of i from s
	int pred[n_vertices], dist[n_vertices];
	vector<int> path;

	if (BFS(s, dest, pred, dist) == false) {
		cout << "Given source and destination"
			<< " are not connected";
		return path;
	}

	// vector path stores the shortest path
	int crawl = dest;
	path.push_back(crawl);
	while (pred[crawl] != -1) {
		path.push_back(pred[crawl]);
		crawl = pred[crawl];
	}

	// distance from source is in distance array
	cout << "Shortest path length is : "
		<< dist[dest];

	// printing path from source to destination
	cout << "\nPath is:\n";
	for (int i = path.size() - 1; i >= 0; i--)
		cout << path[i] << " ";
		
	return path;
}

/**
    Creates path in turns that robot must follow to go from source node to 
    destination node
*/
vector<char> create_path_turns(int source, int destination, int *robot_facing)  // , char (*adj)[n_vertices][3]
{
    
    vector<char> turns;
    // Gets path in general coordinates to later convert to turns
    vector<int> path = create_path_coordinates(source, destination);
    
    // Converting path to turns vector
    cout << "\nPath in coordinates: ";
    for (int i = path.size() - 1; i > 0; i--){
        turns.push_back(get_turn(adj[path[i]][path[i-1]][0], robot_facing));
        if (adj[path[i]][path[i-1]][1] != '\0')
            turns.push_back(get_turn(adj[path[i]][path[i-1]][1], robot_facing));
    }
	for (int k = path.size() - 1; k > 0; k--)
	    cout << adj[path[k]][path[k-1]];
	cout << "\n";
	
	return turns;
}

/**
    Calculates next turn given direction robot must go and direction its facing
    using a modulo 4 operation and int equivalent of coordinate
*/
char get_turn(char direction, int *robot_facing){
    
    int robot_direction = *robot_facing;
    
    int conv_dir = convert_coordinate(direction);
    
    // Modulo 4 operation in case turn is negative
    char turn = convert_to_turn((4 + robot_direction - conv_dir) % 4);
    
    // Updating robot direction after turn
    *robot_facing = conv_dir;
    
    return turn;
}

/**
    Converts int coordinate to turn
*/
char convert_to_turn(int turn){
    switch (turn){
        case 0:
            return 'S';
        case 1:
            return 'L';
        case 2:
            return 'B';
        case 3:
            return 'R'; 
        default:
            return 'S';
    }
}

/**
    Converts coordinates to corresponding int values
*/
int convert_coordinate(char direction){
    switch (direction){
        case 'N':
            return 0;
        case 'E':
            return 1;
        case 'S':
            return 2;
        case 'W':
            return 3;  
        default:
            return 0;
    }
}


int main()
{
	
	// Initializing direction robot is facing
	int robot_facing = convert_coordinate('N');
	// Choosing start and end node    
	int source = 0, dest = 4;

	// add_edge function adds coordinates to given source and destination
	// (source, destination, coord source-destination, coord destination-source)
	add_edge(0, 1, "NE", "W");
	add_edge(1, 2, "N", "S");
	add_edge(1, 3, "E", "W");
	add_edge(1, 4, "S", "N");
	
	// Getting path given source, destination and direction robot is facing
	vector<char> turns = create_path_turns(source, dest, &robot_facing);
	
	// Printing turns robot must take and its final direction
	cout << "Path in turns: ";
	for(int i = 0; i<turns.size(); i++)
	    cout << turns[i];
    cout << "\nRobot Facing: ";
	cout << convert_coordinate(robot_facing);
	
	return 0;
}
