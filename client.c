/*
**
**The Client Side of a Program Which Sends a Video File to the Server Side
**
*/
#include<stdio.h> //For printf, file handling, and identifying eof
#include<stdlib.h> // For reading port number and filename arguemnts from user
#include<string.h> //To copy bytes into packets for window to send to server (strcpy())
#include<unistd.h> // In order to set the timeout for when to resend packets sleep() 
#include<sys/socket.h> //To open the datagram socket
#include<netinet/in.h> //To incorporate IP Address, Port number, and AF_INET (IPv4) members
#include<stdbool.h> //Used to label packets as ACK'd or not

int WINDOW_SIZE = 5;
int main(int argc, char *argv[])
{
//Creating Socket
int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
if (clientSocket == -1) //Error in making socket
{
	printf("\nSocket Creation Unsuccessful. Try running the program again.\n");
	exit(0); // Program terminates
	}
else
{
	printf("\nClient Socket has been Created Successfully.\n");
	}
struct sockaddr_in socketIP;
socklen_t socketIP_length = sizeof(socketIP);
socketIP.sin_family = AF_INET;
int socketPort = atoi(argv[3]); //Port number provided by user to open socket on
socketIP.sin_port = htons(socketPort);
inet_pton(AF_INET, argv[2], &socketIP.sin_addr);

//Opening the file to be transferred
FILE *fileToSend;
fileToSend = fopen(argv[1], "rb");
char outgoingBytesBuffer[1];
char window[WINDOW_SIZE][500];
char seqNumBuffer[500];
bool ACKs[5]; //Simple boolean to indicate whether packsts are ACK'd or not
int sequenceNumber = 0; //Will be reset after 5 ACKs
int i = 0;
for( i = 0; i < WINDOW_SIZE; i++){
	ACKs[i] = false;
}

while(!feof(fileToSend))
{ //Runs till last byte of file
	outgoingBytesBuffer[0] = sequenceNumber;
	fread(&outgoingBytesBuffer[1], sizeof(char), sizeof(outgoingBytesBuffer) - 1, fileToSend); //Reading the bytes to be Sent
	sendto(clientSocket, outgoingBytesBuffer, sizeof(outgoingBytesBuffer), 0, (struct sockaddr *)&socketIP, socketIP_length); 
//Sending the bytes in form of packets
	strcpy(window[sequenceNumber], outgoingBytesBuffer);//Bytes are saved in accordance to their sequence numbers
	if(recvfrom(clientSocket, seqNumBuffer, sizeof(seqNumBuffer), 0, (struct sockaddr *)&socketIP, socketIP_length) > 0)
	{
		ACKs[(int) seqNumBuffer[0]] = true;//if sender has sent acknowledgement on packet being sent successufuly
		memset(&seqNumBuffer, 0, sizeof(seqNumBuffer));
	}
	memset(&outgoingBytesBuffer, 0, outgoingBytesBuffer);
	sequenceNumber++;//Send next packet
	
	if(sequenceNumber == WINDOW_SIZE)
		{
			sequenceNumber = 0;//Start a new window of 5 packets
			for(int i = 0 ; i < WINDOW_SIZE; i++)
				{
		
					if(ACKs[i] = false)//Server has not acknowledged sent packet
						{
						//Timeout
						sleep(2);
						if(recvfrom(clientSocket, seqNumBuffer, sizeof(seqNumBuffer), 0, (struct sockaddr*)&socketIP, &socketIP_length) > 0)
{
	ACKs[(int) seqNumBuffer[0]] = true;
	memset(&seqNumBuffer, 0, sizeof(seqNumBuffer));
}
if(ACKs[i] = false)//Server does not acknowledge packet which has been sent for additional two seconds
{
sendto(clientSocket, window[i], sizeof(window[i]), 0, (struct sockaddr*)&socketIP, &socketIP_length);//Dispatching packet to receiver side again
sleep(2);
if(recvfrom(clientSocket, seqNumBuffer, sizeof(seqNumBuffer), 0, (struct sockadrr*)&socketIP, &socketIP_length) > 0)
{
ACKs[(int) seqNumBuffer[0]] = true;
memset(&seqNumBuffer, 0, sizeof(seqNumBuffer));
}

}
						}		
				
				}
ACKs[i] = false;
			
	}
for(int i = 0; i < WINDOW_SIZE; i++)
{
memset(&window[i], 0, sizeof(window[i]));
}
				}//end while loop
printf("\nThe video has been sent, and received at the server side.\n");
close(clientSocket);
return 0;
}
