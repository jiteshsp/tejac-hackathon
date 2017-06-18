/*
    C socket server example, handles multiple clients using threads
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

 
//the thread function
void *connection_handler(void *);
///
void *thread_print(); 
void *act_as_server(void *);
void *act_as_client();
void *client_thread_per_node();
void * graph_update();
void read_neighbor_info();
void neighbor_init(char* );
extern void initNeighborData(char );
extern void initGraph();

typedef struct AdjListNode
{
    char name;
    int cost;
    struct AdjListNode* next;
}AdjListNode;

typedef struct AdjList
{
    char name;
    struct AdjListNode *head;  // pointer to head node of list
} AdjList;

extern struct AdjList* MyNeighborList;
extern struct Graph* fullGraph;

//to read data from neighbor from server side
char* neighbor_update_data;

pthread_mutex_t mutex_update_graph;
pthread_cond_t cond_update_graph;

struct AdjList* MyNeighborList;


int main(int argc , char *argv[])
{

//printf("\npassed-argv[1]=%s\n",argv[1]);

//the current node info
char node=argv[1][0];
int port=0;
int* port_p= &port;
switch (node) {
    case 'A':
//        printf("\nNode A\n");
        port=8881;
        break;
    case 'B':
//        printf("\nNode B\n");
        port=8882;
        break;
    case 'C':
//        printf("\nNode C\n");
        port=8883;
        break;
    case 'D':
//        printf("\nNode D\n");
        port=8884;
        break;
    case 'E':
//        printf("\nNode E\n");
        port=8885;
        break;
    case 'F':
//        printf("\nNode F\n");
        port=8886;
        break;
    default:
//        printf("\nEnter node initial in Caps\n");
        return 0;
}

    pthread_mutex_init(&mutex_update_graph,NULL);
    pthread_cond_init(&cond_update_graph, NULL);

// init MyNeighborList structure from file
    neighbor_init(argv[1]);
// init the graph 
    initGraph();

//    printf("\n--->starting server thread\n");
    pthread_t server_thread;
     if( pthread_create( &server_thread , NULL ,  act_as_server , (void*)port_p ) < 0)
        {
            perror("could not create server thread");
            return 1;
        }

//    printf("\n--->created server thread\n");
    

     pthread_t graph_update_thread;
//     if( pthread_create( &client_thread , NULL ,  act_as_client ,(void*)argv ) < 0)

     if( pthread_create( &graph_update_thread , NULL ,  graph_update ,(void*)argv ) < 0)
        {
            perror("could not create graph update thread");
            return 1;
        }
//    printf("\n--->created graph thread\n");

     pthread_t client_thread;
//    printf("\n--arguments passed to main %s\n", argv[1]);
    // wait for all the servers to come up .. as the server client file is same
//    printf("\n--->sleeping for 10 secs  before calling client thread\n");

    sleep(10);
//    printf("\n--->creating client thread\n");
     if( pthread_create( &client_thread , NULL ,  act_as_client ,(void*)argv ) < 0)
        {
            perror("could not create client thread");
            return 1;
        }
//    printf("\n--->created client thread\n");

    pthread_join(server_thread,NULL);
    //pthread_join(client_thread,NULL);
    pthread_mutex_destroy(&mutex_update_graph);
    pthread_cond_destroy(&cond_update_graph);
    pthread_exit(NULL);
    return 0;
}

void * graph_update(char * argv[])
{

    while(1){
//        printf("\n----> in graph updaten\n");
    //acquire mutex for graph read 
        pthread_mutex_lock(&mutex_update_graph);
    //this is signalled from server thread
        pthread_cond_wait(&cond_update_graph, &mutex_update_graph);
        read_neighbor_info();
        pthread_mutex_unlock(&mutex_update_graph);
        sleep(3);
    }   
}

// we will get adjacency info in below format
// If B is sending to A , then BA1C2D1 
// if A is sending to D , then AB1C2D1
void read_neighbor_info()
{
//    printf("\n----> Got signat from server ..Reading neighbor info\n");
//    printf("\n-->%s\n",neighbor_update_data);
    int ind=0;
    char dest ;
    int destCost;
    char NodeName = neighbor_update_data[ind];
    ind++;
    while(neighbor_update_data[ind]!='\0'){
        dest= neighbor_update_data[ind];
        ind++;
        destCost=neighbor_update_data[ind] - '0';
        ind++;
        // adding node adjacency to the fullGraph
        addNodeAdjacency(NodeName,dest, destCost);
    }
//    printf("\nMAIN---> the sixe of neighbor details file read: %d\n", ind);
      printNeighborInfo();


      printGraph(fullGraph);

}

// read the neighbor info from file
void neighbor_init(char* nodeName)
{
//    printf("\nMAIN--->Init neighbor info from file:%s\n",nodeName);
    char const* const fileName = nodeName; /* should check that argc > 1 */
    FILE* file = fopen(fileName, "r"); /* should check the result */
    char line[256];

    fscanf(file,"%[^\n]", line);
//    printf("\nMAIN--->File read in init:%s, size is :%d\n", line, (int)sizeof(line));
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */
    fclose(file);

    // Init the global neighbor list
    initNeighborData(nodeName[0]);

    int ind=0;
    char neighborNodeName ;
    int neighborNodeCost;
    while(line[ind]!='\0'){
        neighborNodeName= line[ind];
        ind++;
        neighborNodeCost=line[ind] - '0';
        ind++;
        addNeighbor(neighborNodeName, neighborNodeCost);
    }
//    printf("\nMAIN---> the sixe of neighbor details file read: %d\n", ind);
        printNeighborInfo();  

}


void * act_as_client(char * argv[])
{
/* // should check that argc > 1 
    FILE* file = fopen(fileName, "r"); // should check the result 
    char line[256];

    fscanf(file,"%[^\n]", line);

        printf("\n file read :%s, sixe is :%d\n", line, (int)sizeof(line)); 
    // may check feof here to make a difference between eof and io failure -- network
   //    timeout for instance 

    fclose(file);
    int ind=0;
    while(line[ind]!='\0'){

        ind++;
    }
*/
    int port;


    struct AdjListNode* crawler= MyNeighborList->head;
    int threadIndex =0;
    while(crawler)
    {
//        printf("\n--------node=%c: cost=%d\n", crawler->name,crawler->cost);
            

        switch (crawler->name) {
            case 'A':
                printf("\nNode A\n");
                port=8881;
                break;
            case 'B':
                printf("\nNode B\n");
                port=8882;
                break;
            case 'C':
                printf("\nNode C\n");
                port=8883;
                break;
            case 'D':
                printf("\nNode D\n");
                port=8884;
                break;
            case 'E':
                printf("\nNode E\n");
                port=8885;
                break;
            case 'F':
                printf("\nNode F\n");
                port=8886;
                break;
            default:
                printf("\nError\n");
            return 0;
        }

        int* port_p = &port;

//        printf("\n--->starting individual client thread\n");
        pthread_t connect_to_port[10];
        if( pthread_create( &connect_to_port[threadIndex] , NULL , client_thread_per_node , (void*)port_p ) < 0)
        {
            perror("could not create client thread");
            return 0;
        }
        threadIndex++;
    //    printf("\n--->created client thread to port %d\n",*(int*)port_p);

        crawler= crawler->next;
        sleep(5);
    }

}

void * client_thread_per_node(void* port_p)
{

  int sock;
    int port = *(int*)port_p;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( port );

  //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
 //   puts("JITESH Socket created ");

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 0;
    }
//    puts("Connected to port \n");

    //keep communicating with server
    while(1)
    {
    
//        printf("Enter message : ");
//        scanf("%s" , message);
//   printf("\njite entered message is :%s\n\n", message);
        //Send some data

        struct AdjListNode* crawler= MyNeighborList->head;
        int threadIndex =0;
        char mes_assemble[1000];
        int  i=0, mesInd=0;
                    
        sprintf(message,"%c",MyNeighborList->name);
  //      printf("\n--node=%c: cost=%d, **** mes_assem= %s\n", crawler->name,crawler->cost, mes_assemble);
   //     printf("\n-----------=================-one= %c cost=%d\n", crawler->cost,crawler->cost);
        
  //      mes_assemble[3]=(char)one;
   //     mes_assemble[4]=MyNeighborList->name;
   //     printf("\n\n------lolol------------------------->%s\n\n",mes_assemble);

        while(crawler)
        {
            sprintf(message,"%s%c%d",message, crawler->name,crawler->cost);
//            printf ("\nmes_ass= %s\n", message);
            crawler= crawler->next;

        }
//        printf("\n\n------lolol------------------------->%s\n\n",mes_assemble);
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 0;
        }

        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }

//        puts("Server reply :");
 //       puts(server_reply);
 //       printf("\njit- %s\n\n", server_reply);
 //       sprintf( server_reply, "a" );
 //       printf("\njit- %s\n\n", server_reply);
        sleep(10);
    }
    

    close(sock);

}


//server just reads
void *  act_as_server(void* port_p)
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    int port=*(int*)port_p;
//    printf("\n----->passed port fr server:%d\n",port);
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
//    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 0;
    }
//    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
     
    //Accept and incoming connection
  //  puts("Waiting for incoming connections ...");

    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
//        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 0;
        }
// 
    pthread_t print_thread;
  if( pthread_create( &print_thread , NULL ,  thread_print, NULL) < 0)
        {
            perror("could not create print thread");
            return 0;
        }

        
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
//        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 0;
    }
        
}


void *thread_print()
{

//printf("\n------ print thread started\n");
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    int tid;

typedef struct data {
int one;
int two;
} data_t;

data_t first_data;
first_data.one = 2;
first_data.two = 3;
data_t * data_p= &first_data;

tid= pthread_self();

//printf ("\njitesh= %d\n", tid);

    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Send some messages to the client
    char mes[50];
//printf("\ndat structure one=%d, two=%d, size=%lu\n", first_data.one,first_data.two, sizeof(first_data));
//    printf("\nJitesh enter something\n");
 //   scanf("%s", mes);
//    printf("\nu entered %s\n",mes);

//   write(sock , mes , strlen(mes));
//printf("\nsending data structure\n");
//write(sock,data_p,sizeof(first_data));
//    message = "Greetings! I am your connection handler\n";
//    write(sock , message , strlen(message));
     
//    message = "Now type something and i shall repeat what you type \n";
 //   write(sock , message , strlen(message));
     

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        // data received for client
//        puts(client_message);
        //
        pthread_mutex_lock(&mutex_update_graph);
        neighbor_update_data=client_message;
        // signal the neighbor graph update thread to read the buffer
        pthread_cond_signal(&cond_update_graph);

        pthread_mutex_unlock(&mutex_update_graph);
        //Send the message back to client as acknknowledgement
        write(sock , client_message , strlen(client_message));
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}

