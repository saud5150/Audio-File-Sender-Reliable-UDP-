/*
*
*Server Side of a Program which receives a video file from the user and writes it to a new file name.
*Program exits once transfer is complete.
*/
#include<stdio.h> //For printf, file handling, and identifying eof
#include<stdlib.h> // For reading port number and filename arguemnts from user
#include<string.h> //To copy bytes into packets for window to send to server (strcpy())
#include<unistd.h> // In order to set the timeout for when to resend packets sleep() 
#include<sys/socket.h> //To open the datagram socket
#include<netinet/in.h> //To incorporate IP Address, Port number, and AF_INET (IPv4) members
#include<stdbool.h> //Used to label packets as ACK'd or not

int WINDOW_SIZE = 5;

int main(int argc, char *argv[]){
int PORT = atoi(argv[2]);
struct sockaddr_in socketIP, clientIP;
int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

if(serverSocket == -1)
{
printf("\nSocket has not been created.\n");
exit(0);
}

else
{
printf("\nSocket Created Successful.\n");
}

socketIP.sin_family = AF_INET;  //IPv4
socketIP.sin_addr.s_addr = htonl(INADDR_ANY);
socketIP.sin_port = htons(PORT);

if(bind(serverSocket, (struct sockaddr*)&socketIP, sizeof(socketIP)) != 0)
{
printf("\nBinding has failed.\n");
}
else
{
printf("Server Socket Successfully Binded.\n");
}

printf("The Server is Ready to Accept Files.\n");

//Opening the file to write incoming data to
FILE *fileToWrite;
fileToWrite = fopen(argv[1], "wb");
char outgoingACKs[1];
char window[WINDOW_SIZE][500];
char seqNumBuffer[500];
bool reOrder[5]; //Simple boolean to indicate whether packets are in order, or not
int sequenceNumber = 0; //Will be reset after 5 ACKs are sent

socklen_t socketIP_length = sizeof(clientIP);

while(recvfrom(serverSocket, seqNumBuffer, sizeof(seqNumBuffer), 0, (struct sockaddr*)&clientIP, &socketIP_length) > 0)
{
strcpy(window[sequenceNumber], seqNumBuffer);// Incoming bytes are saved
outgoingACKs[0] = seqNumBuffer[0];//Incoming bytes are buffered

sendto(serverSocket, outgoingACKs, sizeof(outgoingACKs), 0, (struct sockaddr *)&clientIP, socketIP_length);
//Acknowledging receipt of Packet to Client Program

if(sequenceNumber != (int)seqNumBuffer[0]) //Out of order Packet received
{
reOrder[sequenceNumber] = false;
memset(&seqNumBuffer, 0, sizeof(seqNumBuffer));//Store in buffer for now, until turn for sequence comes
}

else
{
reOrder[sequenceNumber] = true;//Packet received is in order
fwrite(&seqNumBuffer[1], sizeof(char), sizeof(seqNumBuffer) - 1, fileToWrite);//Correctr data is written in order to file
memset(&seqNumBuffer, 0, sizeof(seqNumBuffer));
}
sequenceNumber++;

if(sequenceNumber == WINDOW_SIZE){
sequenceNumber = 0;//Restart from 0
int leftOverBytes;
for(leftOverBytes = 0; leftOverBytes < WINDOW_SIZE; leftOverBytes++){
if(reOrder[leftOverBytes] == false){
fwrite(&window[leftOverBytes], sizeof(char), sizeof(seqNumBuffer) - 1, fileToWrite);//Packets which were received out of order
//are now written to the file in the corresponding window frame size
}
}
}
}
printf("\nThe file has been received and wrtten to a new file successully\n");

return 0;
}//end main
