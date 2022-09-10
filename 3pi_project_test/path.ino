#include <Vector.h>
// extern const unsigned int n_vertices;
// extern char adj[n_vertices][n_vertices][3];

/**
    BFS algorithm that stores predecessor of each vertex in array p 
	and its distance from source in array d
*/
bool BFS(int src, int dest, int pred[], int dist[])
{
	// queue to maintain vertices whose adjacency is to be scanned
	Vector<int> queue;
	queue.push_back(src);

	// initially all vertices are unvisited so visited if false for all values
	Vector<bool> visited;
  for (int i = 0; i < n_vertices; i++)
    visited.push_back(false);
	
	// since no path is yet constructed dist is set to infinity for all values
	for (int i = 0; i < n_vertices; i++) {
		dist[i] = 99;
		pred[i] = -1;
	}

	// source is first to be visited and distance to itself is 0
	visited[src] = true;
	dist[src] = 0;
	
	// standard BFS algorithm
	while (!queue.empty()) {
		int u = queue[0];
		queue.remove(0);
		for (int i = 0; i < n_vertices; i++) {
			if (visited[i] == false && adj[u][i][0]) {
				visited[i] = true;
				dist[i] = dist[u] + 1;
				pred[i] = u;
				queue.push_back(i);

				// BFS stops when destination is found.
				if (i == dest)
					return true;
			}
		}
	}

	return false;
}

/**
    Finds shortest path from source to destination, in relation to nodes
*/
Vector<int> create_path_coordinates(int src, int dest)
{
	// Predecessor array stores predecessor of i and distance array stores 
    // distance of i from s
	int pred[n_vertices], dist[n_vertices];
	Vector<int> path;

	// Source and destination are not connected
	if (BFS(src, dest, pred, dist) == false)
		return path;

	// Vector path stores the shortest path
	int crawl = dest;
	path.push_back(crawl);
	while (pred[crawl] != -1) {
		path.push_back(pred[crawl]);
		crawl = pred[crawl];
	}

	// Shortest path length is dist[dest];
		
	return path;
}

/**
    Creates path in turns that robot must follow to go from source node to 
    destination node, given path in nodes
*/
Vector<char> create_path_turns(int source, int destination, int *robot_facing)  // , char (*adj)[n_vertices][3]
{
    
    Vector<char> turns;
    // Gets path in general coordinates to later convert to turns
    Vector<int> path = create_path_coordinates(source, destination);
    
    // Converting path to turns vector
    for (int i = path.size() - 1; i > 0; i--){
        turns.push_back(get_turn(adj[path[i]][path[i-1]][0], robot_facing));
        if (adj[path[i]][path[i-1]][1] != '\0')
            turns.push_back(get_turn(adj[path[i]][path[i-1]][1], robot_facing));
    }

    // At the end of the path, turns robot around
    turns.push_back('B');
    *robot_facing = (*robot_facing + 2) % 4;
	
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
