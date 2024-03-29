#include <bits/stdc++.h>
using namespace std;

// no. of vertices
const unsigned int number_of_vertices = 5;

int convert_coordinate(char direction);
char convert_to_turn(int turn);
char get_turn(char direction, int *robot_facing);

// utility function to form edge between two vertices
// source and dest
void add_edge(vector<int> adj[], int src, int dest)
{
	adj[src].push_back(dest);
	adj[dest].push_back(src);
}

// a modified version of BFS that stores predecessor
// of each vertex in array p
// and its distance from source in array d
bool BFS(vector<int> adj[], int src, int dest,
		int pred[], int dist[])
{
	// a queue to maintain queue of vertices whose
	// adjacency list is to be scanned as per normal
	// DFS algorithm
	list<int> queue;

	// boolean array visited[] which stores the
	// information whether ith vertex is reached
	// at least once in the Breadth first search
	bool visited[number_of_vertices];

	// initially all vertices are unvisited
	// so v[i] for all i is false
	// and as no path is yet constructed
	// dist[i] for all i set to infinity
	for (int i = 0; i < number_of_vertices; i++) {
		visited[i] = false;
		dist[i] = INT_MAX;
		pred[i] = -1;
	}

	// now source is first to be visited and
	// distance from source to itself should be 0
	visited[src] = true;
	dist[src] = 0;
	queue.push_back(src);

	// standard BFS algorithm
	while (!queue.empty()) {
		int u = queue.front();
		queue.pop_front();
		for (int i = 0; i < adj[u].size(); i++) {
			if (visited[adj[u][i]] == false) {
				visited[adj[u][i]] = true;
				dist[adj[u][i]] = dist[u] + 1;
				pred[adj[u][i]] = u;
				queue.push_back(adj[u][i]);

				// We stop BFS when we find
				// destination.
				if (adj[u][i] == dest)
					return true;
			}
		}
	}

	return false;
}

// utility function to print the shortest distance
// between source vertex and destination vertex
vector<int> printShortestDistance(vector<int> adj[], int s,
						int dest)
{
	// predecessor[i] array stores predecessor of
	// i and distance array stores distance of i
	// from s
	int pred[number_of_vertices], dist[number_of_vertices];
	vector<int> path;

	if (BFS(adj, s, dest, pred, dist) == false) {
		cout << "Given source and destination"
			<< " are not connected";
		return path;
	}

	// vector path stores the shortest path
// 	vector<int> path;
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

vector<char> createPath(vector<int> path, char (*paths_matrix)[number_of_vertices][3], int *robot_facing)
{
    
    vector<char> turns;
    cout << "\nPath in coordinates: ";
    // cout << paths_matrix[path[2]][path[1]] << " ";
    for (int i = path.size() - 1; i > 0; i--){
	   //cout << paths_matrix[path[i]][path[i-1]] << " ";
        turns.push_back(get_turn(paths_matrix[path[i]][path[i-1]][0], robot_facing));
        if (paths_matrix[path[i]][path[i-1]][1] != '\0')
            turns.push_back(get_turn(paths_matrix[path[i]][path[i-1]][1], robot_facing));
    }
	for (int k = path.size() - 1; k > 0; k--)
	    cout << paths_matrix[path[k]][path[k-1]];
	cout << "\n";
	
	return turns;
}

char get_turn(char direction, int *robot_facing){
    
    int robot_direction = convert_coordinate(*robot_facing);
    
    int conv_dir = convert_coordinate(direction);
    
    // Modulo 4 operation in case turn is negative
    char turn = convert_to_turn((4 + robot_direction - conv_dir) % 4);

    // Updating robot direction after turn
    *robot_facing = conv_dir;
    
    return turn;
}

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
    }
}

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
    }
}

// Driver program to test above functions
int main()
{
	

	// array of vectors is used to store the graph
	// in the form of an adjacency list
	vector<int> adj[number_of_vertices];
	
	int robot_facing = convert_coordinate('N');

    char paths_matrix[number_of_vertices][number_of_vertices][3];
    strcpy(paths_matrix[0][1], "NE");
    strcpy(paths_matrix[1][0], "W");
    strcpy(paths_matrix[1][2], "N");
    strcpy(paths_matrix[1][3], "E");
    strcpy(paths_matrix[1][4], "S");
    strcpy(paths_matrix[2][1], "S");
    strcpy(paths_matrix[3][1], "W");
    strcpy(paths_matrix[4][1], "N");
    

	// Creating graph given in the above diagram.
	// add_edge function takes adjacency list, source
	// and destination vertex as argument and forms
	// an edge between them.
	add_edge(adj, 0, 1);
	add_edge(adj, 1, 2);
	add_edge(adj, 1, 3);
	add_edge(adj, 1, 4);
	
	int source = 0, dest = 2;
	vector<int> path = printShortestDistance(adj, source, dest);
	vector<char> turns = createPath(path, paths_matrix, &robot_facing);
	cout << "Path in turns: ";
	for(int i = 0; i<turns.size(); i++)
	    cout << turns[i];
    cout << "\nRobot Facing: ";
	cout << convert_coordinate(robot_facing);
	return 0;
}
