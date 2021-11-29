![architecture](https://i.imgur.com/zsRpjzq.png)

# What I have done in the assignment
* Designed and implemented a matching system which takes two usernames as input and returns their optimal compatibility
* The system consists of two clients, a main server, and three backend servers
* Each of the two clients takes a command-line argument, and the servers do not take any command-line argument
* The clients communicate with Central server over TCP, and the Central server communicates with the three backend servers over UDP
# What my code files are
## util.cpp and util.h
* Define global constants such as port number, hostname, MAXLINE, etc
* Provide common utilities such as buffered IO, socket opening, error handling, etc
* Provide sys call wrappers which print error messages and exit on sys call failure
* Define related data structures
## client/util.cppp and client/util.h
* Provde utilities for clientA and clientB, such as result processing, on screen message printing
* Define related data structures
## client/clientA.cpp and clientB.cpp
* Use utilities declared in client/utils.h and utils.h to provide clientA and clientB functionalities
## server/central.cpp and server/central.h
* Request and receive data
* Forward data to other nodes
## server/serverT.cpp and server/serverT.h
* Build username -> subgraph mapping from "edgelist.txt", using union find as the underlying data structure
* Support subgraph and related nodes query given username
* Build response messages and send response back to Central
## server/serverS.cpp and server/serverS.h
* Build username -> scores mapping from "scores.txt", using hash map as the underlying data structure
* Support scores query given usernames
* Build response messages and send response back to Central
## server/serverP.cpp and server/serverP.h
* Build source + dest + graph + weights -> optimal path mapping from IO input, using minheap-backed Djikstra algorithm as the underlying algorithm
* Support compatibility scores and optimal path query given graphs and weights
* Build repsonse messages and send response to Central 
## Message format
| Source        | Dest           | Format  | Note    |
| ------------- |:-------------:| -----:| --------:|
| clientA and clientB      | Central | usr/0 |   /0-terminated username |
| Central     | clientA and clientB      |   numHop/0usrA/0usrB/0hop1/0hop2/0.../0gap/0 |  number of hops between userA and userB, userA, userB, hops and the gap; /0 separated and terminated   |
| Central | serverT      |    usrA/0usrB/0 |  usernames; /0 separated and terminated   |
| serverT | Central      |    numNode/0Node/0Node/0 | nodes in the critical subgraph   |
| serverT | Central      |    numNode/0edge1Node1/0edge1Node2/0 | edges in the critical subgraph|
| Central | serverS      |    numNode/0Node1/0Node2/0 |  nodes in the critical subgraph   |
| serverS | Central      |    numNode/0Node1/0Node1Scores/0 | nodes and nodes scores in the critical subgraph |
| Central | serverP      |    A/0usrA/0 |  usrA with "A" prefix |
| Central | serverP      |    B/0usrB/0 |  usrB with "B" prefix |
| Central | serverP      |    0/0numNode/0edge1Node1/0edge1Node2/0 | edges in the critical subgraph with "0" prefix |
| Central | serverP      |    1/0numNode/0Node1/0Node1Scores/0 |  nodes and scores in the critical subgraph, with "1" prefix   |
| serverP | Central      |    numHop/0usrA/0usrB/0hop1/0hop2/0.../0gap/0 |  number of hops between userA and userB, userA, userB, hops and the gap, /0 separated and terminated   |
## Edge cases
* I call recvfrom only once on Central to receive messages from backend servers for simplicity. Thus, if the output of the backend servers are too long (> MAXLINE bytes), some data will be lost
* Since I didn't do CRC, UDP loss will cause trouble
