#include<stdio.h>
#include<stdlib.h>
#include "graph.h"


int graph_array_index=0;

struct AdjList* MyNeighborList;
struct Graph* fullGraph;
//to keep track of the nodes which are already entered in the graphs and their indices
char graphNodesIndex[NODES_MAX][2]= {"",""};

void initNeighborData(char name)
{
    MyNeighborList= (struct AdjList*) malloc(sizeof(struct AdjList));
    MyNeighborList->name=name;
    MyNeighborList->head=NULL;
}

void addNeighbor( char neighborName, int cost)
{
    struct AdjListNode* newNode = newAdjListNode(neighborName, cost);
    newNode->next = MyNeighborList->head;
    MyNeighborList->head = newNode;

}

void printNeighborInfo()
{
    if (MyNeighborList)
    {
        printf("\nAdjacency list of %c\n", MyNeighborList->name);
        struct AdjListNode* crawler= MyNeighborList->head;
        while(crawler){
            printf("%c : %d\n", crawler->name,crawler->cost);
            crawler= crawler->next;
        }
        
    }
}

void initGraph()
{
    fullGraph = (struct Graph*) malloc(sizeof(struct Graph));
    fullGraph->V= NODES_MAX;
     // Create an array of adjacency lists.  Size of array will be V
    fullGraph->array = (struct AdjList*) malloc(NODES_MAX * sizeof(struct AdjList));
  // Initialize each adjacency list as empty by making head as NULL
    int i;
    for (i = 0; i < NODES_MAX; ++i)
    {
        fullGraph->array[i].head = NULL;
        fullGraph->array[i].name = '\0';
    }
    // neighbor init has already been called
    // so add the cuurent neighbors to the graph
    if(MyNeighborList->head){
        char currentNode = MyNeighborList->name;
        struct AdjListNode* crawler= MyNeighborList->head;
        while(crawler){
            
            printf("\nUpdating graph with node=%c: cost=%d", crawler->name,crawler->cost);
            char dest = crawler->name;
            int cost = crawler->cost;
            addNodeAdjacency(currentNode, dest, cost);
            crawler= crawler->next;
        }

    }
}
/*
// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;
 
    // Create an array of adjacency lists.  Size of array will be V
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
 
     // Initialize each adjacency list as empty by making head as NULL
    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;
        graph->array[i].name = '\0';
 
    return graph;
}
*/



int indexNodeInGraph(char src)
{
 int ind,j;
    
    for(ind=0; ind<NODES_MAX; ind++)
    {
        
 //       printf("\nsrc -%c,  ind- %d,array- %c, index in graph- %d\n",src, ind, graphNodesIndex[ind][0],graphNodesIndex[ind][1]);
//  printf("\nsrc -%c,  ind- %d,array- %c\n",src, ind, graphNodesIndex[ind][0]);
        if(graphNodesIndex[ind][0])
        {
     
            if(graphNodesIndex[ind][0] == src)
            {
  //              printf("\n--> present\n");
                return (graphNodesIndex[ind][1]); 

            }
        }
  //              printf("\n-->not  present\n");
        // assuming that if sequential any ine of them is null
        //that means no values are present after that, so add next element here
        return 999 + ind;
    }
}
 
// Adds an edge to an undirected graph
void addNodeAdjacency(char src, char dest, int cost)
{
    struct AdjListNode* newNode = newAdjListNode(dest,cost);

    //we need to find whether the neighbor is entered or not
    int isPresent = indexNodeInGraph(src);

    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.  The node is added at the begining
    if( isPresent < 900)
    {

  //              printf("\n----> present\n");
        //crawl through the list to update or add new value
        struct AdjListNode* pCrawl = fullGraph->array[isPresent].head;
 //       printf("\n Node adjacency list of vertex %d head ,node %c\n", isPresent,fullGraph->array[isPresent].name );
        int updateReq=0;
        while (pCrawl)
        {
//printf("\nDBG- dest - %c, src - %c , pcrawl- %c\n",dest, src, pCrawl->name );
            if (dest ==  pCrawl->name){
//printf("\nDBG-matched\n" );
            // entry present
                if(cost == pCrawl->cost){
                    //no need to update
                    updateReq=0;
                    break;
                }
                else
                {   // update the cost
                    updateReq=0;
                    pCrawl->cost=cost;
                }
            
            }
            else{
//printf("\nDBG-node not present , so update\n" );
// node not present , so update
                updateReq=1;
//                printf("-> %c", pCrawl->name);
//                printf("-> %d", pCrawl->cost);
                pCrawl = pCrawl->next;
            }
        }

        if(updateReq){
//            printf("\nUpdating\n");
            newNode->next = fullGraph->array[isPresent].head;
            fullGraph->array[isPresent].head = newNode;
        }
//        printf("\nDBG-->fullGraph->array[isPresent].name=%c: src=%c\n", fullGraph->array[isPresent].name, src);
        if(fullGraph->array[isPresent].name != src)
        {
            printf("\n!!! ERROR !!! array index invalid\n");
        }
            printGraph(fullGraph);
    }
    else
    {

  //              printf("\n--->not present\n");
    //the src is not added in the graph. add at the gobal array offset
        newNode->next = fullGraph->array[graph_array_index].head;
        fullGraph->array[graph_array_index].head = newNode;
        fullGraph->array[graph_array_index].name = src  ;
    //also update the index at whoch the entry as added 
        graphNodesIndex[isPresent-999 ][0] = src;
        graphNodesIndex[isPresent - 999][1] = graph_array_index;
        graph_array_index++;
    }
    // Since graph is undirected, add an edge from dest to src also
//    newNode = newAdjListNode(src,cost);
  //  newNode->next = graph->array[dest].head;
//    graph->array[dest].head = newNode;

}

// A utility function to print the adjacenncy list representation of graph
void printGraph(struct Graph* graph)
{
    int v;
    printf("\nFULL GRAPH IS\n");
    for (v = 0; v < graph_array_index; ++v)
    {
        struct AdjListNode* pCrawl = fullGraph->array[v].head;
        printf("Adjacencies of %d head node \n %c--", v,fullGraph->array[v].name );
        while (pCrawl)
        {
            printf("-> %c", pCrawl->name);
            printf(": %d", pCrawl->cost);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
    printf("--------------------------------\n");
}
/*
int main()
{
    // create the graph given in above fugure
    int V = NEIGHBORS_MAX;
    struct Graph* graph = createGraph(V);
    addEdge(graph, 0, 1);
    addEdge(graph, A,B,1);
    
    addEdge(graph, 0, 4);
    addEdge(graph, 1, 2);
    addEdge(graph, 1, 3);
    addEdge(graph, 1, 4);
    addEdge(graph, 2, 3);
    addEdge(graph, 3, 4);
 
    // print the adjacency list representation of the above graph
    printGraph(graph);
 
    return 0;
}

*/
/*
int main()
{
    initNeighborData('A');
   addNeighbor('B',2);
   addNeighbor('C',2);
printNeighborInfo();    
}
*/
