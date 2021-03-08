This is a translation done from the original documentation with the help of DeepL. May contain translation errors.

Features
===============

In the following section, a summary of the main characteristics of the project is presented. Later on, these characteristics will be developed in more detail in their corresponding sections.

Lightweight code
-------------

In order to be able to run the nodes on devices with low performance, such as IoT devices, as well as to be able to run multiple nodes on a single machine to be able to perform large-scale simulations, we tried to develop a code as lightweight as possible. To this end, the following considerations were taken into account:

-   **Written in C**, since this will allow us to have a low-level control of how we want the data structures to behave and the final compiled code will be faster than in any other language. In addition, this language can be used to program microcontrollers and other types of devices that can be found in IoT.

-   **Stateless communication**, i.e., each message sent between nodes contains enough information to generate a response, without the need to maintain any kind of conversation state. This saves a large amount of resources, speeding up code execution.

-   **Single threaded**, this will allow on the one hand to run large-scale simulations on a single computer without loss of performance and, in addition, facilitates the implementation of the code in devices with a single processing core such as microcontrollers. This feature is made possible by stateless communication.

-   **UDP Communication**, following the paradigm of not maintaining states in the communication channels. This saves the computational overhead of using the TCP protocol for large-scale simulations. In addition, it also facilitates the use of the code in microcontrollers and other embedded systems, since they more often have ways of communicating via UDP than via TCP, due to the simplicity of the former protocol with respect to the latter.

-   **Uses static memory** during normal node operation. Dynamic memory is only used when the node is initialized, the rest of the time static memory data structures are used. Dynamic memory reservation is one of the slowest system calls made by programs and leads to large performance losses and is not available on embedded devices without an operating system. For these reasons, variants of the "list" structure are used that make use of static memory only.

Overlay network
---------------

For the search and distribution of information between nodes, a network has been developed that is superimposed on UDP/IP and implements the "randomcast" and "broadcast" communication methods that the former lacks.

Through this network, a new node that joins can learn about the other participating nodes, and file fragments can be distributed as they are uploaded. It implements three forms of communication:
during normal node operation. Dynamic memory is only used when the node is initialized, the rest of the time static memory data structures are used. Dynamic memory reservation is one of the slowest system calls made by programs and leads to large performance losses and is not available on embedded devices without an operating system. For these reasons, variants of the "list" structure are used that make use of static memory only.

-   **Singlecast**: classic point-to-point communication in a similar way as UDP/IP does.

-   **Randomcast**: a packet is routed from node to node by selecting a random neighbor from the list of known nodes a certain number of times. The content is processed only by the last node to receive the message.

-   **Broadcast**: a received packet is sent to all known neighbors, with a maximum number of forwardings. This avoids uncontrolled packet transmission in very large networks.

Distributed file system
-------------------------------

The nodes interconnect with each other to form a distributed file system, which allows both uploading and downloading **fragments** of files. Each fragment occupies exactly **8192 bytes**, and is accompanied by a **hash** that identifies it. These fragments will be stored by those nodes whose identifier modulo 16 is equal to modulo 16 of the first 64 bits of the hash. The system also has an **index** structure for uploading larger files, so that the file is divided into 8192-byte fragments and the index fragment keeps track of the hashes of the fragments that compose it. Each index allows 255 hashes to be stored, allowing files up to **2088960 bytes** to be uploaded. These indexes could be recursive to allow storage of even larger files, but this is not currently implemented.

Files can be uploaded to this file system with the 'dhtsend' tool, which will return the hash of the index file. Each stored file fragment can be downloaded by requesting the information associated with a hash. Using the 'dhtrecv' tool, the hash of an index file can be entered, and the tool will download all the associated fragments and join them together.

Test scenario
===================

The following describes how to deploy a test scenario in order to test the operation of the application.

Step 1. Compiling
-------------------

In the source code folder, there is a **Makefile** file to help with the compilation of the code. It includes commands to compile each tool independently, as well as all at once.

<img src=".//media/image3.png" width="431" height="161" />

Illustration 1. Source code

If the command ‘make all’ is run, all tools will be compiled.

<img src=".//media/image4.png" width="566" height="84" />

Illustration 2. Compiling

The script MAKE will store the compiled binaries in the folder ‘bin’ on the root of the source code.

<img src=".//media/image5.png" width="566" height="104" />

Illustration 3. Compiled code

Step 2. Node deployment
-------------------------------

In the root directory is the script 'deploy\_test\_scene.sh'. This script will deploy 256 nodes on the local machine assigning them port range 8000-8256, starting the first one as the root server and connecting the rest to random nodes already deployed. Wait 0.1 seconds between each node to allow time for the previous node to start.

<img src=".//media/image6.png" width="250" height="209" />

Illustration 4. Script 'deploy\_test\_scene.sh'

We execute it to start the deployment process. It will take 25 seconds.

<img src=".//media/image7.png" width="314" height="166" />

Illustration 5. Test scenario deployment

Step 3. Sending and receiving a file
---------------------------------------

Now we are going to try to upload a file to the system and download it. To do this, we have prepared an environment with two folders, each with a terminal open. In one of them, there are a series of test pictures and the **dhtsend** tool that we have just compiled. In the other, there is only the **dhtrecv** tool.

<img src=".//media/image8.png" width="565" height="306" />

Illustration 6. Test environment

In file A, we are going to choose one of the photos and send it to the platform. To do this, using the **'dhtsend'** tool, we specify the file we want to send, the address and port of one of the system nodes, and the port through which we are going to listen.

The tool will proceed to send the fragments. Each fragment will be represented by the symbol '\#' in the terminal. Finally, the hash of the index fragment will be printed.

<img src=".//media/image9.png" width="566" height="309" />

Illustration 7. Sending a file


Next, we are going to run the 'dhtrecv' tool on file B to download the image. To do this, we will specify the hash of the index, the address and port of one of the nodes in the system, and the port through which we are going to listen.

In file A, we are going to choose one of the photos and send it to the platform. To do this, using the **'dhtsend'** tool, we specify the file we want to send, the address and port of one of the system nodes, and the port through which we are going to listen.

<img src=".//media/image10.png" width="566" height="307" />

Illustration 8. Receiving a file

As you can see, the tool will request the fragment and analyze the index. Then it will ask to download each fragment, represented with a '#' in the terminal.

Once the simulation is finished, the nodes can be shut down by executing 'killall dhtnode'.

Test scene storage
--------------------------------------

The storage of the test scenario nodes is done in the 'test\_scene' directory in the root of the source code. Inside is a directory for each node, inside which the fragments are located.

<img src=".//media/image11.png" width="566" height="325" />

Illustration 9. Node directories

<img src=".//media/image12.png" width="566" height="325" />

Illustration 10. Node fragments

Technical development
==================

In this section we will present in more detail the characteristics of the developed system, as well as explanations of the code and justifications in the decision making process during its development.

Overlay network
---------------

In order to facilitate node discovery, a P2P overlay network over UDP/IP has been developed. This network has been given useful functionalities for the development of the distributed system that do not exist in UDP/IP.

### Overlay network packets

Messages exchanged between nodes are wrapped in a packet structure used to communicate over this overlay network.

<img src=".//media/image13.png" width="566" height="66" />

Illustration 11. Packet structure

-   **Header Type:** header type, which determines the packet transmission method.

-   **Header:** header content. It is defined as a ‘union’, so it will occupy the same physical space, that is, they are exclusive in a packet.

    -   **Randomcast**

        -   **Hops:** number of hops until the packet is processed.

    -   **Broadcast**

        -   **Breadth:** maximum reach of broadcast (should be depth, instead of breadth).

        -   **UID:** packet UID, to avoid duplicates.

    -   **Singlecast:** the header is empty, but its structure is conserved in order to be able to be extended in future versions.

-   **Source:** packet source, specified in ‘peer\_addr.h’

    -   **ID:** source node identifier

    -   **IP Address:** TCP/IP address of the source node

-   **Payload:** packet payload

    -   **Type:** payload content type

    -   **Length:** payload length

    -   **Content:** payload. The types of packets that can be transmited will be specified in the following sections.

The specification of the packets in the source code can be found in the header ‘packets.h’.

### Packet transmission

Transmission can be made in three ways:

-   **Singlecast:** point-to-point

-   **Randomcast:** sending the message to a random node. The packet is routed ‘hops’ times to a random neighbor.

-   **Broadcast:** the message is sent to every known neighbor, with a maximum distance ‘breadth’. It contains an UID field that each node remembers when they receive a broadcast message. If a message with a previously seen UID is received, it is ignored.

#### Virtual network level processing by nodes

Each time a node receives a packet, the function ‘handlenetl’ is called in the file ‘messages.c’, that decides what to do with the packet and returns 1 if the contained message should be processed, or 0 otherwise.

-   If a singlecast packet is received, it just returns 1.

-   If a randomcast is received, the remaining number of hops is checked. If there are remaining hops, it is sent to a random neighbor and the content is ignored. Else, the message is processed.

-   If a broadcast is received, first its UID is checked against the list of known UIDs. If it is already there, it is ignored. Otherwise, the content is processed and, if the value of the field breadth is greater than 0, it is relayed to all its neighbors.

    -   The list of known UIDs is implemented as a circular list in static memory, which has a limit of remembered values.

    -   There is an exception in the case that a broadcast packet is received with content ‘NEWFILE’, which delegates the packet processing to the message processing function.

#### Packet sending functions

The functions that fill the packet fields and sends them through the virtual network can be found implemented in the file ‘networking.c’.

### System messages

The system defines a series of messages that nodes use to communicate. The approach of these messages is focused on being totally independent, and that it is not necessary to maintain a context or conversational state in order to be able to process a message correctly. The types of messages exchanged are defined in file ‘msgdefs.h’.

When the ‘handlenetl’ function of a node decides that it must process the content of a message, the packet is redirected to the ‘procmsg’ function, which, depending on the value of the Content Type field of the packet, will choose the function corresponding to the message content to process it. This logic can be found in the file ‘messages.c’.

#### Message list

Following is the list of messages defined on the project:

-   **JOIN:** sent by a node when it joins the network

-   **PEERS:** response to a JOIN message, contains a list of network nodes.

-   **ADDME:** the message receiver must add the message sender to its neighbor list.

-   **PING:** the receiver has to return the message to the sender.

-   **FILEFRAG:** contains a file fragment.

-   **FILEGET:** file fragment request.

-   **FIND:** similar to PING, but only the nodes whose identifier modulo equals the one specified in the message should answer.

-   **NEWFILE:** transmited by the nodes that store a new fragment.

### Network joining protocol

When ‘dhtnode’ is executed, a listening port should be specified and, optionally, the address of a seed node. If left unspecified, the node is deployed as a root node. If specified, the node will begin by executing the network joining protocol.

1.  First, the node sends a JOIN message to the seed node, in which it specifies how many neighbors of the seed node it wants to receive.

2.  The seed node answer with a PEERS message that contains the addresses of the requested nodes, or those of every known node in case of not being able to reach the requested value. The new node is added to the list of known peers.

3.  The new node adds the seed node to its known nodes list and those nodes contained in the PEERS message. Then, it sends a list of ADDME messages with its address through randomcast, so random nodes add it to their neighbor list.

Data structures
--------------------

This section presents the data structures used by the system.

### Static list

It consists of a list implemented in static memory. You can add new elements, remove arbitrary elements and check if an element is in the list. The stored element can be of any type. The structure **does not preserve element order**. It is used to keep the neighbor list.

### Circular static list

Circular list implemented in static memory, specialized for 8-byte integers. It stores a number N of elements, so that as new elements are added, the old ones are deleted. It is used to maintain the UIDs of broadcast packets.

### Hash

In this element it is more important the algorithm than the structure per se. It is used to calculate a 32 byte integer that uniquely identifies a set of data. Being *x*, *y* two data sets and *H(x)* the hash function over a data set *x*, the hash function tries to fulfill two premises:

-   Id *x'* is a slightly modified version of *x*, then *H(x)* will be completelly different to *H(x′)*.

-   It is practically impossible to find a data set *y ≠ x* such as *H(x)* = *H(y)*

File handling
------------------

This section specifies everything related to the handling of files by the platform nodes.

### Fragments

The distributed file system deals with files of a fixed length of **8192 bytes**. The choice of this value is due to the fact that we wanted a size considerably larger than the size used for the hashes, since the system would be highly inefficient if both values were similar. On the other hand, we also wanted a size small enough to fit in a single UDP packet (limited to just under 65536 bytes). This left 4096, 8192 and 16384, with 8192 being the value furthest from both limits. Each fragment can be uniquely referenced by its **hash** value, and the nodes containing it can be obtained by calculating the **module 16 of the hash**. This is because a number of distinct nodes "shards" of 16 has been decided. This number was chosen because it was high enough to distribute the fragments over several nodes and that when launching the 256 nodes of the simulation there would be enough of each type.

### Index fragment

Index shards are ordinary shards that have a certain structure. This structure is used to keep track of multiple hashes, so that larger files can be uploaded to the system.

<img src=".//media/image14.png" width="335" height="97" />

Illustration 12. Index fragment structure

Each index fragment has 29 bytes to store the file name, 1 byte to indicate the number of fragments that make up the file, the number of bytes of the last fragment that are part of the file and 255 hashes of the fragments that make up the file. In total, files of up to **2088960 bytes** can be stored with an index fragment.

These indexes could be nested in order to store larger files, but this is not implemented in the current version.

This structure is specified in the file ‘index\_file.h’.

### Fragment messages

To send fragments between nodes, they are wrapped in a message of type **FILEFRAG**, together with their hash. They are sent in a single UDP packet.

### File utilities

In order to check if a fragment already exists in a node storage and make it easier to read and write fragments stored in the packets to a file, the utilities **fm\_exists**, **fm\_store** y **fm\_read** are offered, which are implemented in the file ‘file\_manager.c’.

Internal node state
-----------------------

Each node must maintain its internal state, i.e. information regarding the port used for communication, the list of nodes it knows, the list of broadcast packets received, its own address... To do so the structure ‘internal\_state’ is defined, which is passed by reference to most functions that a node uses, as it contains all the persistent information it may require. Usually it is passed through an argument called ‘self’.

File transfer
=======================

This section explains the procedure followed in order to send and receive files to and from the distributed sytstem through the tools ‘dhtsend’ and ‘dhtrecv’, and the communication generated in consequence between the nodes.

File sending
-----------------

First, lets develop the file sending procedure, using the tool **dhtsend**.

### Initialization

When using this tool, you must specify the file to be uploaded, the address of the node to be used as seed and the port to be used for listening. The tool will check if the specified file exceeds the upload limit, issuing an error message as a consequence.

Both the upload and download tools use the logic of the distributed system nodes, so they need to maintain internal state. Initially they will know only one node, the seed node.

The next thing to do is to prepare a structure that allows nodes to be stored according to their identifier module, called "peerlist". A PING message will be sent with "breadth" 3, and the nodes that respond will be stored in the peerlist structure.

### Sending

Then the upload process begins. Fragments of the file to be uploaded will be taken in blocks of 8192 bytes, which will be stored in a message of type FILEFRAG, its hash will be calculated, and it will be sent to a random node from the list of known nodes whose identifier module corresponds to the module of the fragment's hash. For each fragment sent, a '\#' will be printed on the screen. In case no node with such identifier modulus is known, a FIND message will be sent to search for one.

There is a delay of 0.1 seconds after each fragment to avoid collapsing the nodes as a consequence of a massive sending of data packets. During the development phases of the system, before including this delay, the combined traffic generated between the 256 nodes during the simulation was able to fill the memory buffer of the UDP sockets on the Linux for Windows subsystem on which the tests were performed. In case the network is deployed between different devices we assume that this would not be a problem, but the delay would be equally advisable to avoid saturating the network if there were multiple users.

Finally, the index fragment structure is packaged into a FILEFRAG message, hashed and sent to a node. The hash of this fragment will be printed on the screen so that it can be referenced and used to download the file.

### Node traffic

When a node receives a FILEFRAG message, it first checks whether its hash corresponds to that node storing the fragment. If so, it checks that it has not already stored the fragment. If it has not already stored it, it stores it, and generates a **NEWFILE** packet that it sends in broadcast with UID equal to the first 8 bytes of the file hash. With this packet the node announces to other nodes that it has just received a fragment, and that other nodes with the same identifier module can send it a FILEGET message to download it.

When a node receives a NEWFILE packet by broadcast, it generates an exception in the normal behavior of handling broadcast packets at the virtual network level and delegates to the message controller to process it at this level.

It first checks if the hash contained in the message corresponds to its identifier module and, if not, if it still has hops left to be retransmitted, it retransmits it and ignores it. If it is addressed to nodes with its module, it requests the fragment with a FILEGET message from the sending node. When it receives the reply FILEFRAG, it sends a NEWFILE packet again by broadcast, setting itself as the download peer.

#### Improvements from initial version

Initially, when a packet was sent to the system, a FILEFRAG message was sent in broadcast that was distributed throughout the network. As a result, in the 256-node simulation, a minimum of 3,208,642,560 bytes ≈ 3.2 Gb were exchanged in total if a file with the maximum number of fragments was uploaded assuming that each node knows 6 neighbors (512 \* 8192 bytes of the file \* 6 sends per node \* 256 neighbors). With the new approach, where a fragment request message is required to travel through the virtual network, traffic is estimated to be less than 600 Mb.

File reception
---------------------

The 'dhtrecv' tool will ask for a hash of an index fragment as input. It will then initialize its state and its list of known nodes in a similar way as the 'dhtrecv' tool does. It will ask one of the nodes to return the fragment via a FILEGET message and extract the list of fragments that make up the file. It will make successive requests for each fragment and store them at the end of the same file with the name retrieved from the index fragment.

Debug tools
==========================

Along tools ‘dhtnode’, ‘dhtsend’ and ‘dhtrecv’ other two tools used during the project development are included, ‘dhthash’ and ‘dhtping’.

-   **dhthash:** calculates a file hash.

-   **dhtping:** makes it easy to send PING messages through singlecast, multicast and broadcast to check the network connectivity.

Appendix: notes, observaions and possible improvements
=================================================

This section presents notes and observations on the practice carried out, as well as possible improvements to the system that have not been implemented due to the difficulty involved or lack of time.

- It has been assumed a secure environment and that all nodes seek the common good, therefore, no security system has been proposed, which could pose great difficulties.

- It would be necessary to include timeouts in the joining of a new node to the network and in the downloading of fragments with the dhtrecv tool.

- The dhtsend tool could check that the file has been sent correctly, by trying to download it and comparing its hash with that of the original file.

- A system to check if a node is still active or not would be missing. It could be implemented as an event that is executed after X packets processed, with a certain probability after each packet or by checking the time that has passed since the last execution after processing a packet. In this event, a number N of neighbors proportional to the total number of known neighbors would be deleted, and a special message would be sent to them, to which the deleted nodes would respond with an ADDME message to be added back if they were still available.

- Index shards could be nested to upload larger files.

- The functionality to specify the IP address of the node so that it can be accessed from the outside would still need to be implemented. Currently it is specified to 'localhost' (internal\_state.c, line 13).

- When sending a set of peers in response to JOIN, they should be chosen randomly.

- The file dissemination strategy using the NEWFILE message was added at the end of the development of the practice, for this reason it is put as an exception in the 'handlenetl' function instead of being integrated following the rest of the architecture.

- A debug logging system is missing.

- The configuration constants are not grouped in a single file, instead of being scattered in different headers.

- The broadcast UIDs history is set to 256 since multiple nodes will be running on the same computer. In reality, this number could be increased to reduce the possibility of repeated packets.

