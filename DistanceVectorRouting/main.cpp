#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <ctype.h>


// Some standard Defines used in the program
//#define DEBUG
#define DEBUG1
#define MAXVECTORS 200
#define MAXNEIGHBORS 100
#define STDIN 0
#define INITIAL_VALUE -1
#define MAXVISITED  10

// This is the structre that stores information about this Node's Neighbors
// I need the IP address and port number to send to the neighbor
// I need the nodeNumber so I know which node they are.
struct neighbor_struct{
    int nodeNum;
    char ipaddress[15];
    int portNumber;
};

// This is my vector table. It stores the fromNode (int), toNode (int) and
// the cost fromNode-toNode. It also keeps track of 'changed', which tells
// me if i received new information about this cost. If i do get new info
// I will want to send this info to other nodes. That is how DV algorithms work

struct new_vector_struct{
    int fromNode;
    int toNode;
    int cost;
    int changed;
    int previousNode;
};

// Declare all the functions I will use
int readDVFromNeighbor(int sd, struct neighbor_struct neighbors[], int myNode, struct new_vector_struct DV[], int numberOfNodes);
int initNeighbors(struct neighbor_struct neighbors[]);
int printNeighbors(struct neighbor_struct neighbors[]);
int readNeighborsFile(struct neighbor_struct neighbors[], char *filename, int myNode);
int readDVFile(struct new_vector_struct DV[], char *filename, int myNode);

int printDV(struct new_vector_struct vectors[], int numberOfNodes);
int getInputFromUser(struct neighbor_struct neighbors[], int myNode, struct new_vector_struct DV[], int numberOfNodes);
int createListeningSocket(int portNumber, int *socket);
int initDV(struct new_vector_struct DV[], int numberOfNodes, int myNode);
int sendDVToNeighbor(struct neighbor_struct neighbors[], int myNode,struct new_vector_struct DV[], int numberOfNodes );


int main(int argc, char *argv[] ){
    struct neighbor_struct neighbors[MAXNEIGHBORS];// struct for neighbors
    int myNode = -1; // who am i
    char neighborFile[100], vectorFile[100];
    int listeningSocket;
    // stuff for the select() statement is here
    fd_set socketFDS;
    //  int maxSD;
    int rc;
    struct timeval timeout;
    struct new_vector_struct *DVTABLE;

    if (argc < 6){
        printf ("Usage is DV <neighborFile> <vectorFile> <nodeNumber> <Port#> <# Of Nodes>\n");
        exit (1);
    }
    // need to create the DV table.
    int numberOfNodes = atoi(argv[5]); // know the number of nodes in graph
    //DVTABLE = malloc (sizeof (struct new_vector_struct)*numberOfNodes+1);
    int size = sizeof(struct new_vector_struct);
    size *= (numberOfNodes+1);
    DVTABLE = (struct new_vector_struct *) malloc(size);


    myNode = atoi(argv[3]); //myNode should be the second parameter
    strcpy (neighborFile, argv[1]); // Copy the parms into variables
    strcpy (vectorFile, argv[2]);

    initDV(DVTABLE, numberOfNodes, myNode); // Initial the DV table


    initNeighbors(neighbors); // Initialize the neighbors structure
    readNeighborsFile(neighbors, neighborFile, myNode); // read in the neighbor file into structure

    readDVFile(DVTABLE, vectorFile, myNode); // Read in the vector file.
#ifdef DEBUG
    printDV(DVTABLE, numberOfNodes);
#endif


    // At this point, my data structures should be initialized.
    // I have NOT contacted any neighbors
    // but my vector table should be good. So i will print it out.
    //DMO don't need to print  printNeighbors(neighbors);

    // Next, i need to share my DV table with my neighbors.
    // recall all my neighbors may not be online yet
    // and that is ok. i will eventually send things when i receive updates.

    createListeningSocket(atoi(argv[4]), &listeningSocket);

    // now i have a socket created. I wait for things to happen. I will either
    //  1) get input from the command terminal (user) or
    //  2) get something from the network.

#ifdef DEBUG
    printf("Main: sending to neighbors\n");
#endif
    sendDVToNeighbor(neighbors, myNode, DVTABLE, numberOfNodes );
#ifdef DEBUG
    printf("Main: sent to neighbors completed\n");
#endif

    // loop forever
    int counter = 0;
    for (;;){
        // one of two things will happen. I will get input from the keyboard OR
        // i will get input from the network. I just have to figure out which
        // one popped and do the appropriate action


        // The following is not optimal, but it works well enough.
        printf ("Update format should be: <YourNode> <Destination Node> <Cost>\n");

        // ************ ADDED THIS LINE TO NOTIFY USER OF ROUTE FORMAT ************
        printf ("Route format should be: ROUTE <StartingNode>\n");

        // this is all setup for the select statement. I will wait on input
        // from the user or the network or both!
        FD_ZERO(&socketFDS);
        FD_SET(listeningSocket, &socketFDS);
        FD_SET(STDIN, &socketFDS); // by default STDIN is fd 0
        timeout.tv_sec = 30; // wake every 30 seconds to send updates
        timeout.tv_usec = 0;

        // select block until something arrives on STDIN/socket or timeout
#ifdef DEBUG1
        printf ("Main: Please enter a command(print, route)\n");
#endif

        rc = select (listeningSocket+1, &socketFDS, NULL, NULL, &timeout);
#ifdef DEBUG1
        printf ("Main: Select popped, counter is %d\n", ++counter);
#endif
        // If I get here it means the select popped, and i have an event
        // form the keyboard or the network...or a timeout
        // i have to figure out which one.
        if (rc == 0){ // means the timout hit
            /* send my vectors to my neighbors */
            sendDVToNeighbor(neighbors, myNode, DVTABLE, numberOfNodes);
        }
        if (FD_ISSET(STDIN, &socketFDS)){
            // means input from stdin- meaning the user types something in
            getInputFromUser(neighbors, myNode, DVTABLE, numberOfNodes);
            // call function that handles user input
        }

        // i could have input from the keyboard AND the network,
        // which is why this is not an else if
        if (FD_ISSET(listeningSocket, &socketFDS)){ // NEW
#ifdef DEBUG
            printf ("Main: received something on listening socket\n");
#endif
            readDVFromNeighbor(listeningSocket, neighbors, myNode, DVTABLE,  numberOfNodes);
        }

    } // end of the for(;;) loop

    return 0;
}

// this function should print the neighbor table only printing valid entries
int printNeighbors(struct neighbor_struct neighbors[]){
    int i = 0;
    ////////print the neighbors table
    //int numNodes = 5;

    while (i < 1){
        printf("%1d ", neighbors[i].nodeNum);
        i++;
    }
    return 0;

}

// this function should print the vector table
int printDV(struct new_vector_struct DV[], int numberOfNodes){
    int i;
    printf("\n");
    /////// print the current distance vector table
    // ONLY PRINT VALID VECTORS, COST = -1, DON'T PRINT
    for (i = 1; i <= numberOfNodes; i++){
        if (DV[i].cost != -1)
            printf("The cost from node %d to node %d is %d via %d\n", DV[i].fromNode, DV[i].toNode, DV[i].cost,
                   DV[i].previousNode);
    }
    printf("\n");
    return 0;
}

// This function reads in the neighbors file and creates entries
// in the neighbors table.
int readNeighborsFile(struct neighbor_struct neighbors[], char *filename, int myNode){
    FILE *fileptr;
    int maxNeighbors = 0;
    int nodeNumber, portNumber;
    char ipaddress [15];

    fileptr = fopen (filename, "r");
    if(fileptr == NULL){
        printf("\nError reading neighbor files! Please verify the files are valid & in the correct folder.\n");
        exit(1);
    }

    //// read in ONLY the entries that match your node
    int i = 0;
    // FSCANF WILL RETURN # OF ELEMENTS, WHICH IS WHY WE COMPARE TO 3 B/C WE WANT TO ENSURE WE GOT 3 ELEMENTS
    while(fscanf(fileptr, "%d" "%s" "%d", &nodeNumber, ipaddress, &portNumber) == 3){
        neighbors[i].nodeNum = nodeNumber;
        strcpy(neighbors[i].ipaddress, ipaddress);
        neighbors[i].portNumber = portNumber;
        i++;
    }
    return 0;
}


// This routine reads in the vector file into the vector structure
// ONLY add vectors where myNode == fromNode. that allows us to
// initialize all the programs with a single
// vectors files. In real life I would ONLY ever know my vectors,
// so this emulates that
int readDVFile(struct new_vector_struct DV[], char *filename, int myNode){
    FILE *fileptr;
    int fromNode, toNode, cost;

    //// read in the DV table. ONLY read vectors that are yours
    fileptr = fopen(filename, "r");

    if(fileptr == NULL){
        printf("\nError reading DV file. Please verify the DV file is valid & in the correct folder.\n");
        exit(1);
    }

    int i = 0;
    while(fscanf(fileptr, "%d" "%d" "%d", &fromNode, &toNode, &cost) == 3){
        if (myNode == fromNode){
            DV[toNode].fromNode = fromNode;
            DV[toNode].toNode = toNode;
            DV[toNode].cost = cost;
            DV[fromNode].previousNode = fromNode;
            DV[myNode].cost = 0;
        }
        i++;
    }
    return 0;
}



int getInputFromUser(struct neighbor_struct neighbors[], int myNode, struct new_vector_struct DV[], int numberOfNodes){
    // This function will be called when the user enters a command.
    //Valid commands are:
    //     print - this prints the current vectors table
    //     refresh - tells node to send changes to the neighbors.
    //               there should never be unsent changes
    //               but I included this function as a failsafe.
    //     update - the format for this is Update fromNode toNode cost.
    //              The code validates that fromNod is myNode
    //              before it does any sending of data.

    int fromNode, toNode, cost;
    int rc;
    char buffer [20];
    char command[8];
    // assumes we have already written the screen asking for input

    rc = read(STDIN, buffer, 20);
    if (rc < 0) // error handling
    {
        perror ("Problem reading from stdin\n");
        exit (1);
    }

    rc = sscanf(buffer, "%s %d %d %d ", command, &fromNode, &toNode, &cost);

    // Converting commands to lowercase in case of uppercase use
    for(int i = 0; i < sizeof(command); i++){
        command[i] = tolower(command[i]);
    }

    // Note that I may only scan one item, C handles that.
    // the other variables not scanned will be
#ifdef DEBUG
    printf ("getInputFromUser: command '%s' fromnode %d, tonode %d, cost %d, mynode %d\n",
	  command, fromNode, toNode, cost, myNode);
#endif
    ///// NOW FIGURE OUT WHAT TO DO WITH EACH COMMAND ////////

    if (strcmp (command, "print") == 0){
        printDV(DV, numberOfNodes);
        return 0;
    }

    else if (strcmp (command, "refresh") == 0){
        // HERE WE WILL NEED TO UPDATE NEIGHBORS COST FOR OUR CURRENT NODE FOR FROM-NODE TO TO-NODE
        sendDVToNeighbor(neighbors, myNode, DV, numberOfNodes);
        return 0;
    }

    // Route Option - 1st Input = FromNode which is actually representing toNode, we want returned cost
    // Because of this, we need this checked prior to checking if fromNode != myNode -> exit out of processing
    else if (strcmp(command, "route") == 0){
        // Scans User-input by order: command, fromNode, toNode, cost
        // Saves our desired toNode as //fromNode cause of the way our buffer is
        toNode = fromNode;      // Changing to toNode so psuedocode of Names/Logic make sense
        fromNode = myNode;
        printf("\nThe lowest cost from node %d to node %d is: %d\nThe 1st node to travel to is: node %d\n\n",
               myNode, toNode, DV[toNode].cost, DV[toNode].previousNode);
    }

    // Optimizing here, if the fromNode is not me, i can't process anything
    else if (fromNode != myNode)
        return -1;

    // again, optimization here. If it is not an update, return
    else if (strcmp(command, "update") != 0)
        return -1;

    else if (rc < 4) // error, i need 4 parameters if this is an update
        return -1;

    // NEED TO SET THE NEW COST AND TELL MY NEIGHBORS
    else if (strcmp(command, "update") == 0){
        // If User's input fromNode matches myNode, we want to update that fromNode
        if (fromNode == myNode) {
            for (int i = 0; i < numberOfNodes; i++) {
                for (int j = 0; j < numberOfNodes; j++) {
                // Checking that    1. From Node and Desire To Node exists
                //                  2. Cost is not Zero because we can't change the cost if FromNode = ToNode
                //                  3. That our toNode of index i, exists as neighbor in that node's neighbor table
                //                  4. That if all above = true, our new cost < old cost = update cost, otherwise no
                if (DV[i].fromNode == fromNode && DV[i].toNode == toNode && DV[i].cost != 0 & DV[i].toNode == neighbors[j].nodeNum) {
                    // Checking if the new cost is greater than our old cost
                    // If yes = new cost overwrites old cost between FromNode and ToNode
                    if (DV[i].cost > cost){
                        DV[i].cost = cost;
                        printf("\nFrom node %d to node %d, the cost has been updated to: %d\n\n",
                               DV[i].fromNode, neighbors[j].nodeNum, DV[i].cost);
                        sendDVToNeighbor(neighbors, myNode, DV, numberOfNodes); // Updating neighbors of new cost
                    }
                    else
                        printf("\nThe new cost between node %d and node %d is greater than its previous cost.\n"
                               "Due to this, the cost will not be updated.\n\n", DV[i].fromNode, DV[i].toNode);
                    }
                }
            }
        }
    }
    else
        printf("\nError: Not a valid command. Please try again.\n");

    return 0;
}


int createListeningSocket(int portNumber, int *sd){
    // function used to create a server-type socket
    struct sockaddr_in server_address;
    int rc = 0;

    *sd = socket (AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(portNumber);
    server_address.sin_addr.s_addr = INADDR_ANY;

    rc = bind (*sd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (rc <0)
    {
        // I didn't show you this in lab1. Without this, if the bind fails,
        // your server doesn't realize it!!
        printf ("Bind failed, do a netstat -an | grep <portnumber> to see if you are in a weird state\n");
        perror ("bind");
        exit (1);
    }
    listen (*sd, 5);
    return 0;
}

// Just set the neighbor structure to INITIAL_VALUE
int initNeighbors(struct neighbor_struct neighbors[]){
    int i;
    for (i=0;i<MAXNEIGHBORS;i++){
        neighbors[i].nodeNum = INITIAL_VALUE;
        neighbors[i].portNumber = INITIAL_VALUE;
#ifdef DEBUG
        printf("port %x, node %x\n",
	   neighbors[i].portNumber,neighbors[i].nodeNum);
#endif
    }
    return 0;
}

int initDV(struct new_vector_struct DV[], int numberOfNodes, int myNode)
{
    int i;

    for (i=0;i<=numberOfNodes; i++)
    {
        DV[i].changed = 0;
        DV[i].fromNode = myNode;
        DV[i].toNode = i;
        DV[i].cost = INITIAL_VALUE;
        DV[i].previousNode = myNode;
    }
    return 0;
}


// this is the function where i loop through all my neighbors and send my DV
// whether it changed or not
int sendDVToNeighbor(struct neighbor_struct neighbors[], int myNode,
                     struct new_vector_struct DV[], int numberOfNodes ){
    int sd; //socket description
    struct sockaddr_in server_address;
    char serverIP[29];
    int rc = 0;
    int i=0;
    int index = 0;
    int portNumber;
    char vectorToSend[26];// hack here
    char tempStr[5];
    int length;

    sprintf(vectorToSend, "%d ", myNode);

    for (i=1;i<=numberOfNodes;i++){
        sprintf (tempStr, "%d ", DV[i].cost);
        strcat(vectorToSend, tempStr);
    }
#ifdef DEBUG1
    printf ("sendDVToNeighbor: vectorToSend is '%s'\n", vectorToSend);
#endif
    length = 25 - strlen(vectorToSend);
#ifdef DEBUG1
    printf ("sendDVToNeighbor: length is %d\n", length);
#endif
    if (length > 0){
        for (i=0;i<length;i++){
            strcat(vectorToSend, " ");
        }
    }
    printf ("vector to send is '%s' \n", vectorToSend);

    // probably not the neatest/most cost effective way to do this looping,
    //but good enough for this lab
    for (index =0; index<MAXNEIGHBORS; index++){
        if (neighbors[index].portNumber == INITIAL_VALUE){
            continue; // skip this one. again not efficient but ok for now
        }
#ifdef DEBUG1
        printf ("sendToNeighbor: created socket\n");
#endif
        sd = socket (AF_INET, SOCK_STREAM, 0);

        // Now i need to fill in the structure for sending to this neighbor
        portNumber = neighbors[index].portNumber;
        strcpy(serverIP, neighbors[index].ipaddress);
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(portNumber);
        server_address.sin_addr.s_addr = inet_addr(serverIP);
        rc = connect(sd, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
#ifdef DEBUG1
        //printf ("sendToNeighbors: server:port %s:%d\n", serverIP, portNumber);    // I COMMENTED THIS OUT
#endif
        if (rc < 0) {
            // couldn't connect for whatever reason. server could not be up yet!
            close (sd);
            perror ("sendToNeighbor:error connecting to stream socket\nmaybe neighbor is down? ");
            index ++;
#ifdef DEBUG1
            printf ("sendToNeighbors: no connection for index %s\n", serverIP);
            printf ("sendTodNeighbors: ipaddres of partner is %s, port is %d\n",
                    serverIP, portNumber);
#endif
            continue;
        }
#ifdef DEBUG1
        printf ("sendToNeighbors: FROMNODE = %x\n", DV[i].fromNode);
#endif

        rc = write(sd, vectorToSend, strlen(vectorToSend));
        if (rc < 0){
            perror("Send to ");
            printf("Sent %d bytes\n", rc);
        }
        close (sd);
    } // end of the neighbors
    for (i=0;i<numberOfNodes;i++)
        DV[i].changed = 0;

#ifdef DEBUG1
    printf ("returning from sendNeighbor routine\n");
#endif
    return 0;
}
// i am passed in a listening socket in sd. I am only called when SELECT
// realizes a partner is trying
// to connect to that listening socket. I will accept() and create a
// new socket, do my processing (send stuff)
// then i will disconnect the accepted socket.
// I will loop receiving 25 bytes of data at a time from the other
// side. If the data is "CLOSE", then, by protocol defition, i stop receiving data.

int readDVFromNeighbor(int sd, struct neighbor_struct neighbors[],
                       int myNode, struct new_vector_struct DV[], int numberOfNodes){
    int connected_sd;
    int rc;
    struct sockaddr_in from_address;
    socklen_t fromLength;
    char buffer[26];
    int fromNode, cost;
    int i = 0;
    int changed =0;

    // RECOMMENDED BY OGLE TO ADD THIS STATEMENT BEFORE THE ACCEPT COMMAND
    fromLength = sizeof(struct sockaddr_in);

    connected_sd = accept(sd, (struct sockaddr *) &from_address, &fromLength);
#ifdef DEBUG
    printf ("readFromNeighbor: after accept, new socket is %d\n", connected_sd);
#endif

    memset (buffer, 0, 26); // by PROTOCOL definition, the other side is sending 25 bytes

    rc = read(connected_sd, &buffer, 25);

#ifdef DEBUG
    printf ("readFromNeighbor: read '%s'\n", buffer);
#endif

    char* token = strtok(buffer, " ");
    sscanf (token, "%d ", &fromNode);

    // Keep printing tokens while one of the
    // delimiters present in str[].
    i = 0;
    do{
        i++;
        token = strtok(NULL, " ");
        if (token == NULL)
            continue;
        sscanf(token, "%d ", &cost);
        printf("info from node: %d Going to node %d: cost is  %d\n", fromNode, i, cost);
        //    sleep(2);

        // now figure out if this one is lower cost. 'i' is the toNode position in the DV table
        if (i==fromNode) // CHeck to see if this is wrong DMODMODMO
            continue; // skip this if from/to are the same node
        if (cost == INITIAL_VALUE)
            continue; // skip this is there is no cost
        int totalCost = cost + DV[fromNode].cost; // cost to get to fromNode + cost
        if ((DV[i].cost == INITIAL_VALUE)|| (DV[i].cost > totalCost)){ // then store this in the table
            DV[i].cost = totalCost;
            DV[i].previousNode = fromNode;
            changed =1;
#ifdef DEBUG
            printf ("readDVFromNeighbors: cost decrease\n");
#endif
        }
        else if((DV[i].previousNode == fromNode) &&
                (DV[i].cost < totalCost)){
            /* the case where cost went up? */
            DV[i].cost = totalCost;
            changed =1;
#ifdef DEBUG
            printf ("readDVFromNeighbors: cost increased\n");
#endif
        }

    }  while (token != NULL); // end of the while() loop
    if (changed ==1){
        sendDVToNeighbor(neighbors, myNode, DV,  numberOfNodes );
#ifdef DEBUG
        printf ("readDVFromNeighbors: just updated my neighbors\n");
#endif
    }

    close (connected_sd);
    return 0;
}
