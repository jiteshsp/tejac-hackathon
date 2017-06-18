
#define NEIGHBORS_MAX 10
#define NODES_MAX 20


// A structure to represent an adjacency list node
typedef struct AdjListNode
{
    char name;
    int cost;
    struct AdjListNode* next;
}AdjListNode;
 
// A structure to represent an adjacency list
typedef struct AdjList
{
    char name;
    struct AdjListNode *head;  // pointer to head node of list
} AdjList;
 
// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
struct Graph
{
    int V;
    struct AdjList* array;
};
 
// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(char name, int cost)
{
    struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->name = name;
    newNode->cost = cost;
    newNode->next = NULL;
    return newNode;
}

void addNeighbor( char neighborName, int cost);
void initNeighborData(char name);
void printNeighborInfo();
void initGraph();
void addNodeAdjacency(char src, char dest, int cost);


// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V);
// Adds an edge to an undirected graph
void addEdge(char src, char dest, int cost);


// A utility function to print the adjacenncy list representation of graph
void printGraph(struct Graph* graph);





