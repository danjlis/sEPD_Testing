//////////////////////////////////////////////////////////////////////////
// Stepper Code for Column Scans
//
// Author:		Sebastian Seeds
// Date:		2.24.15
// Use:			For use with Zaber stepper motors for transport and
//				positioning of a radioactive source above static
//				scintillating tiles. Includes sample ZABER code for manual
//				repositioning. Made for the SPHENIX HCal upgrade for use
//				with QA testing.
//
//////////////////////////////////////////////////////////////////////////

// Sample Code Documentation
/*------------------------------------------------------------------------
Module:        MAIN.C
Author:        ZABER
Project:
State:
Creation Date: 27 June 2001
Description:   Example program for communicating with
Teckmo devices on a serial port
Language : C
Platform : Win32
Serial   : Polled Mode Operation
------------------------------------------------------------------------*/
//FOR BASIC OPERATION
#define WIN32_LEAN_AND_MEAN
#include <cstdio>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "\users\daq_asus\daq\archive\pserial.cpp"  // Polled Serial API
//#include "\Users\DAQ_ASUS\daq\Archive\pserial.c"  // Polled Serial API

//FOR SCOPE CODE
#include "sicl.h" // made this into quotes
//#include "\users\daq_asus\daq\archive\stepper2_freeze20200306\sicl.h" // made this into quotes
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <tchar.h>
//OPTION INCLUDES
//#include <TROOT.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TVirtualStreamerInfo.h>
//#include <TPluginManager.h>

//FOR STEPPER
static unsigned char Unit;
static unsigned char Command;
static long Data;

//FOR SOCKET

#include <ws2tcpip.h>
#include <winsock2.h>
#include <string.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")
#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dmoguids.lib")
#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")
#pragma comment(lib, "msdmo.lib")
#pragma comment(lib, "strmiids.lib")

#define DEFAULT_PORT "23"
#define DEFAULT_BUFLEN 512

//FOR STL STUFF...

using std::cout;
using std::endl;
using std::ofstream;

/*
INST oscillo;

unsigned long ReadWord (short *buffer, unsigned long BytesToRead)
{
	unsigned long BytesRead;
	int reason;

	iread(oscillo, (char *) buffer, BytesToRead, &reason, &BytesRead);

	return BytesRead;

}

unsigned long ReadByte (char *buffer, unsigned long BytesToRead)
{
	unsigned long BytesRead;
	int reason;

	iread(oscillo,buffer,BytesToRead,&reason,&BytesRead);

	return BytesRead;
}

void WriteIO(char *buffer)
{
	unsigned long actualcnt;
	unsigned long BytesToWrite;
	int send_end=1;
	char temp[50];

	BytesToWrite = strlen(buffer)+1;
	strcpy_s(temp, buffer);
	strcat_s(temp, "\n");
	iwrite(oscillo, temp, BytesToWrite, send_end, &actualcnt);

}
void ReadIO(char *buffer)
{
	unsigned long actualcnt;
	unsigned long BytesToWrite;
	int send_end;
	char temp[50];

	BytesToWrite = strlen(buffer)+1;
	strcpy_s(temp, buffer);
	strcat_s(temp, "\n");
	iread(oscillo, temp, BytesToWrite, &send_end, &actualcnt);

}

void ReadDouble(double *buffer)
{
	iscanf(oscillo, "%lf", buffer);
}
*/
int MoveStepperMotor(double x, double y){
	// Reports total number of available microsteps for each drive.
	bool debug = true;
	double xmicrosteptot = 8062992;
	double ymicrosteptot = 4031496;
	double stepspercm = xmicrosteptot/100;
	// Sets origin for scan./////////////////////////////////////////////////
	if (debug) std::cout<<"Moving the source: "<<std::endl;

	////////////////////////////////////////////////////////////////////////////
	/////// Adjust values from this line!!
	//for background and cosmics position test
	// 0 to 100 on x and 0 to 50 on y
	double xorigincm = x;
	double yorigincm = y;
	// Adjust for step length across both x and y by cm...
	double xsteplengthcm = 0.0;//20220114,18 sEPD test v3, v6
	double ysteplengthcm = 0.0;//20220114,18 sEPD test v3, v6

	// Adjust for number of steps across x and y ////////////////////////////
	int xsteps = 1;// 20220118 sEPD test v6 for dark current test
	int ysteps = 1;//20220118 sEPD test v6 for dark current test

	/////// Adjust values up to this line!!
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////


	double xorigin = stepspercm*xorigincm;
	double yorigin = stepspercm*yorigincm;

	double xgeneralstep = stepspercm*xsteplengthcm;
	double ygeneralstep = stepspercm*ysteplengthcm;

	// Variable for recording current positions of drives. Used to calibrate
	// wait times. Takes stage 860ms to go 8062992 steps.
	double xcurrentpos = 0.0;
	double ycurrentpos = 0.0;
	double sleepzero = 0.0;
	double sleeptime = 0.0;
	double sleeptime2 = 0.0;


	//double xtotscanlengthcm = xorigincm + xsteplengthcm*xsteps;
	//double ytotscanlengthcm = yorigincm + ysteplengthcm*ysteps;
	double ytotscanlengthcm = 47.0;
	double yskiplengthcm = 15.0;
	double xskiplengthcm = 55.0;



	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////


	// Checks to see if parameters are within physics ranges of the stages
	if (xsteps*xgeneralstep+xorigin > xmicrosteptot || ysteps*ygeneralstep+yorigin > ymicrosteptot)
	{
		cout << "Settings take the source beyond the end of the drive. Please readjust." << endl;
		Sleep(10000);
		return 0;
	}

	// Variables for PSERIAL_Receive command.
	unsigned char Unit;
	unsigned char Unit2;
	unsigned char Command;
	unsigned char Command2;
	long Data;
	long Data2;


	cout << "Each step in X is " << xgeneralstep << "." << endl;
	cout << "Each step in Y is " << ygeneralstep << "." << endl;


	// Initializes the stepper motors to interpret instructions.
	PSERIAL_Initialize();

	// Rezero drives ---------------------------------------------------------
	PSERIAL_Open("com3");
	PSERIAL_Send( 0,2,0);
	Sleep(1500);
	//PSERIAL_Send( 1,1,64 );
	//PSERIAL_Send( 2,1,64 );
	// Waiting for drives by current position
	// Returns current position
	PSERIAL_Send( 1,60,64 );
	//if (debug) std::cout<<__LINE__<<": Looking for a blip..."<<std::endl;
	while (1)
	{
		if (PSERIAL_Receive (&Unit,&Command,&Data) && Unit == 1 && Command == 60)
		{
			xcurrentpos = Data;
			break;
		}
	}
	//if (debug) std::cout<<__LINE__<<": Found blip."<<std::endl;
	Sleep(1500);
	PSERIAL_Send( 2,60,64 );
	while (1)
	{
		if (PSERIAL_Receive (&Unit,&Command,&Data2) && Unit == 2 && Command == 60)
		{
			ycurrentpos = Data2;
			break;
		}
	}
	cout << "The position of the X stepper is " << xcurrentpos << "." << endl;
	cout << "The position of the Y stepper is " << ycurrentpos << "." << endl;
	
	if(xcurrentpos > ycurrentpos)
	{
		//Sleeptime if x > y
		sleepzero = 100*1000*(xcurrentpos/xmicrosteptot); //(length of drive in cm)*(ms/cm)*(fractional drive position)
	}
	else
	{
		sleepzero = 50*1000*(ycurrentpos/ymicrosteptot); //(length of drive in cm)*(ms/cm)*(fractional drive position)
	}
	//cout << sleepzero << endl;

	PSERIAL_Send( 1,20,0 );
	PSERIAL_Send( 2,20,0 );
	//Sleep(20000);
	Sleep(sleepzero);
	
	printf("\n Moving to (x, y) : (%f, %f) \n", xorigincm, yorigincm);

	PSERIAL_Send( 1,20, xorigin );


	PSERIAL_Send( 2,60,64 );
	while (1)
	{
		if (PSERIAL_Receive (&Unit2,&Command,&Data2) && Unit2 == 2 && Command == 60)
		{
			ycurrentpos = Data2;
			break;
		}
	}
	sleeptime = 50*1000*((ycurrentpos-yorigin)/ymicrosteptot); //(length of drive in cm)*(ms/cm)*(fractional drive position)

	if(xorigincm > yorigincm)
	{
		sleeptime = 1000*xorigincm; //(ms/cm)*(origin)
	}
	else
	{
		sleeptime = 1000*yorigincm; //(ms/cm)*(origin)
	}

	PSERIAL_Send( 2,20,yorigin);

	//Sleep(sleeptime);


	printf(" \n Moved to new position! \n");

	return 1;
}

int ReadDarkCurrent(int n, char* name){
	// Read the dartk current with the source at x,y

	bool debug = true;
	cout << "Starting to Read the dark current: " << endl;
	cout << "------------------------------------------ " << endl;

	// initialize the timing variables
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	char timestamp[100];
	char daypart[100];
	char timepart[100];

	if(tm.tm_hour>9 && tm.tm_min>9) sprintf(timepart,"%d%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour>9 && tm.tm_min<=9) sprintf(timepart,"%d0%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour<=9 && tm.tm_min>9) sprintf(timepart,"0%d%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour<=9 && tm.tm_min<=9) sprintf(timepart,"0%d0%d", tm.tm_hour, tm.tm_min);

	if(tm.tm_mon>8 && tm.tm_mday>9) sprintf(daypart,"%d%d%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon>8 && tm.tm_mday<=9) sprintf(daypart,"%d%d0%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon<=8 && tm.tm_mday>9) sprintf(daypart,"%d0%d%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon<=8 && tm.tm_mday<=9) sprintf(daypart,"%d0%d0%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

	sprintf(timestamp,"%s-%s",daypart,timepart);


	// Creating and opening a text file for pulse averaging method
	ofstream file_1;

	char filename1[100];
	char *disc = "dark";
	sprintf(filename1,"%s_%s_%s.txt",timestamp, disc, name);

	file_1.open(filename1);
	file_1 << "Test Sector:" << endl;
	file_1 << "Device ID, Channel, Tile, Trial, x, y, IMON, RMON, VCOMP" <<endl;

	// Commencing the socket
	if (debug) std::cout<<"Commence the socketing to TUFF box..."<<std::endl;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;

	int iResult;
	int itt;

	char *sendbuf = "ls \n";
	char *lastbuf;
	char *recvbuf2 = new char[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// IP address of the tuff box that is telneted into
	char *argv = "192.168.140.81";

	// initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		file_1 << "WSAStartup failed." <<endl;
		return 1;
	}

	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory( &hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(argv, DEFAULT_PORT, &hints, &result);
	if ( iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		file_1 << "getaddrinfo failed with error." <<endl;
		WSACleanup();
		return 1;
	}

	//attempt to connect
	for (ptr = result; ptr != NULL ; ptr=ptr->ai_next) {
		// create socket for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error :%ld\n", WSAGetLastError());
			file_1 << "socket failed with error." <<endl;
			WSACleanup();
			return 1;
		}
		// connect to server
		iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("unable to connect to server. \n");
		file_1 << "unable to connect to server." <<endl;
		WSACleanup();
		return 1;
	}

	// Receive until peer closes the connection
	iResult = 1;

	char a;
	char *aa;
	while (iResult > 0) {

		iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
			recvbuf2[iResult] = '\0';
			a = *(recvbuf2 + iResult - 2);
			aa = &a;
			//printf("%s \n", recvbuf2);
			if(aa[0] == '#') break;
		}
		else if (iResult == 0) printf("Connection closed\n");
		else printf("recv function failed with error: %d\n", WSAGetLastError());
	}

	sendbuf = "cd home \n";
	// send initial buffer
	iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	//printf("Bytes Sent: %ld\n", iResult);
	//printf("%s \n", sendbuf);

	while (iResult > 0) {

		iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
			recvbuf2[iResult] = '\0';
			a = *(recvbuf2 + iResult - 2);
			aa = &a;
			//printf("%s \n", recvbuf2);
			if(aa[0] == '#') break;
		}
		else if (iResult == 0) printf("Connection closed\n");
		else printf("recv function failed with error: %d\n", WSAGetLastError());
	}

	sendbuf = "ls \n";
	// send initial buffer
	iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	//printf("Bytes Sent: %ld\n", iResult);
	//printf("%s \n", sendbuf);

	while (iResult > 0) {

		iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
			recvbuf2[iResult] = '\0';
			a = *(recvbuf2 + iResult - 2);
			aa = &a;
			//printf("%s \n", recvbuf2);
			if(aa[0] == '#') break;
		}
		else if (iResult == 0) printf("Connection closed\n");
		else printf("recv function failed with error: %d\n", WSAGetLastError());
	}

	if (debug) std::cout<<"Done with socketing to TUFF box..."<<std::endl;


	// Reports total number of available microsteps for each drive.


	int nRepeat = n;
	/////// Adjust values up to this line!!
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	if (debug) std::cout << "Reading through the socket:" << endl;

			///////////////////////////////////////////////////////////////////////////
	int DCBiasVoltage = 67;

			// Getting the data
	printf("Setting voltages to the two boards: %d Volts \n", DCBiasVoltage);
	for(int id = 0; id < 2; id++){
		for(int kk = 0; kk < 16; kk++){
			char buf[50];
			sprintf(buf, "./set_epd -b 0 -d %d -c %d -V %d -P 0 -C 0 \n", id, kk, DCBiasVoltage);
			sendbuf = &buf[0];
			// send initial buffer
			iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d \n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}
		}//channel loop
	}//device loop
			///////////////////////////////////////////////////////////////////////////
	for(int irepeat = 0; irepeat < nRepeat; irepeat++){
		printf("Progress: %f ... \n", (float)irepeat/(float)nRepeat);
		printf("-------------------------------------------- \n");
		
		for(int id = 0; id < 2; id++){
			for(int kk = 0; kk < 16; kk++){
				char buf[50];
				int tile_number = kk*2 + id;
				sprintf(buf, "./set_epd -b 0 -d %d -c %d \n", id, kk);
				sendbuf = &buf[0];
				
				printf("giving command: ./set_epd -b 0 -d %d -c %d \n", id, kk);
				printf("%d, %d , %d, %f , %f , ", id, kk, tile_number, 0, 0);
				
				file_1 << id << ", " << kk<<", "<< tile_number << ", "<<irepeat<<", "<< 0<< ", " << 0 <<", " ;
				// send initial buffer
				iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}

				//printf("Bytes Sent: %ld\n", iResult);
				//printf("%s \n", sendbuf);

				char word_configure[] = "aaaaaaaaa";
				char word_reading[] = "aaaa";
				char corr_configure[] = "configure";

				char imon[] = "IMON";
				char curr;
				int i;
				int j;

				char *line_read = new char[100];
				while (iResult > 0) {
					iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
					if (iResult > 0) {
						//printf("Bytes received: %d\n", iResult);
						recvbuf2[iResult] = '\0';
						a = *(recvbuf2 + iResult - 2);
						aa = &a;

						for (i = 0; i < iResult; i++){
							curr = *(recvbuf2 + i);
							memmove(word_configure, &(word_configure[1]), strlen(&(word_configure[1])));
							memmove(word_reading, &(word_reading[1]), strlen(&(word_reading[1])));
							//memmove(word_vcomp, &(word_vcomp[1]), strlen(&(word_vcomp[1])));
							word_configure[strlen(word_configure) - 1] = curr;
							word_reading[strlen(word_reading) - 1] = curr;
							//word_vcomp[strlen(word_vcomp) - 1] = curr;

							if(0 == strcmp(word_reading,imon)) {
								//printf("FOUND IT: %s \n", word_reading);
								strncpy(line_read, recvbuf2 + i + 2, 8);
								line_read[8] = '\0';
								for ( j = 0; j < 8; j++){
									if (line_read[j] == ',') break;
								}
								line_read[j] = '\0';
								printf("%s \n", line_read);

								file_1 << line_read << " \n";
								iResult = 0;
							}

						}
						//printf("%s \n", recvbuf2);
						if(aa[0] == '#') break;
					}
					else if (iResult == 0) printf("Connection closed\n");
					else printf("recv function failed with error: %d\n", WSAGetLastError());
				}
			}//channel loop
		}//device loop
	}//repeat loop
					/////////////////////////////////////////////////////////////////////

	printf("Data Collected \n");
	Sleep(2000);



	// Closing and rezeroing.
	file_1.close();


	////////////////////////////////////////////////////////////////////////////
	// ENABLE IF HODOSCOPE IS ATTACHED! ----------------------------------------

	// -------------------------------------------------------------------------
	////////////////////////////////////////////////////////////////////////////

	if (debug) std::cout<<"Closing socket"<<std::endl;
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ConnectSocket);
	WSACleanup();

	// Exiting program.
	return 0;


}

int DrawALine(char* filename, int n_rep = 5, double dx = 0.5 ,double xi =4 , double yi = 24, double xf = 88, double yf = 46, int DCViasVoltage = 67){
	
	bool debug = true;
	cout << "Starting Middle Scan!" << endl; 

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	char timestamp[100];
	char daypart[100];
	char timepart[100];

	if(tm.tm_hour>9 && tm.tm_min>9) sprintf(timepart,"%d%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour>9 && tm.tm_min<=9) sprintf(timepart,"%d0%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour<=9 && tm.tm_min>9) sprintf(timepart,"0%d%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour<=9 && tm.tm_min<=9) sprintf(timepart,"0%d0%d", tm.tm_hour, tm.tm_min);

	if(tm.tm_mon>8 && tm.tm_mday>9) sprintf(daypart,"%d%d%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon>8 && tm.tm_mday<=9) sprintf(daypart,"%d%d0%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon<=8 && tm.tm_mday>9) sprintf(daypart,"%d0%d%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon<=8 && tm.tm_mday<=9) sprintf(daypart,"%d0%d0%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

	sprintf(timestamp,"%s-%s",daypart,timepart);


	// Creating and opening a text file for pulse averaging method
	ofstream file_1;

	char filename1[100];
	char *disc = "middle_scan";
	sprintf(filename1,"%s_%s_n_%s.txt",timestamp, disc, filename);

	file_1.open(filename1);
	// Commencing the socket
	if (debug) std::cout<<"Commence the socketing to TUFF box..."<<std::endl;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	file_1 << "Test Sector:" << filename << endl;
	file_1 << "n_rep, dx, xi, yi, xf, yf, DCViasVoltage = " << n_rep <<", "<<dx<<", "<<xi<<", "<<yi<<", "<<xf<<", "<<yf<<", " << DCViasVoltage<<endl;
	file_1 << "Device ID, Channel, Tile ID, trial, x, y, IMON, RMON, VCOMP" <<endl;
	int iResult;
	int itt;

	char *sendbuf = "ls \n";
	char *lastbuf;
	char *recvbuf2 = new char[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	char *argv = "192.168.140.81";
	
	// initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		file_1 << "WSAStartup failed." <<endl;
		return 1;
	}
	
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	
	ZeroMemory( &hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(argv, DEFAULT_PORT, &hints, &result);
	if ( iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		file_1 << "getaddrinfo failed with error." <<endl;
		WSACleanup();
		return 1;
	}

	//attempt to connect
	for (ptr = result; ptr != NULL ; ptr=ptr->ai_next) {
		// create socket for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error :%ld\n", WSAGetLastError());
			file_1 << "socket failed with error." <<endl;
			WSACleanup();
			return 1;
		}
		// connect to server
		iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	
	freeaddrinfo(result);
	
	if (ConnectSocket == INVALID_SOCKET) {
		printf("unable to connect to server. \n");
		file_1 << "unable to connect to server." <<endl;
		WSACleanup();
		return 1;
	}
	
	// Receive until peer closes the connection
	iResult = 1;

	char a;
	char *aa;
	while (iResult > 0) {
		
		iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
			recvbuf2[iResult] = '\0';
			a = *(recvbuf2 + iResult - 2);
			aa = &a;
			//printf("%s \n", recvbuf2);
			if(aa[0] == '#') break;
		}
		else if (iResult == 0) printf("Connection closed\n");
		else printf("recv function failed with error: %d\n", WSAGetLastError());
	}

	sendbuf = "cd home \n";
	// send initial buffer
	iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}



	while (iResult > 0) {
		
		iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
		if (iResult > 0) {
			recvbuf2[iResult] = '\0';
			a = *(recvbuf2 + iResult - 2);
			aa = &a;
			if(aa[0] == '#') break;
		}
		else if (iResult == 0) printf("Connection closed\n");
		else printf("recv function failed with error: %d\n", WSAGetLastError());
	}

	sendbuf = "ls \n";
	// send initial buffer
	iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}


	while (iResult > 0) {
		
		iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
		if (iResult > 0) {
			recvbuf2[iResult] = '\0';
			a = *(recvbuf2 + iResult - 2);
			aa = &a;
			if(aa[0] == '#') break;
		}
		else if (iResult == 0) printf("Connection closed\n");
		else printf("recv function failed with error: %d\n", WSAGetLastError());
	}

	
	// for each channel set the voltage before hand
	for(int ii = 0; ii < 16; ii++){
		char buf[50];
		sprintf(buf, "./set_epd -b 0 -d 0 -c %d -V %d -P 0 -C 0 \n", ii, DCViasVoltage);
		sendbuf = &buf[0];
		iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		sprintf(buf, "./set_epd -b 0 -d 1 -c %d -V %d -P 0 -C 0 \n", ii, DCViasVoltage);
		sendbuf = &buf[0];
		iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
	}
	if (debug) std::cout<<"Done with socketing to TUFF box..."<<std::endl;


	// Reports total number of available microsteps for each drive.

	double xmicrosteptot = 8062992;
	double ymicrosteptot = 4031496;
	double stepspercm = xmicrosteptot/100;
	// Sets origin for scan./////////////////////////////////////////////////
	if (debug) std::cout<<"Setting Origin"<<std::endl;

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	/////// Adjust values from this line!!
	// 2022 Jan. 26 Yeonju Go
	// Set the first and last (x, y) positions corresponding to xorigincm, yorigincm and xfinalcm, yfinalcm. 
	// The stepper motor will move along the line connected to these two points 
	// Set y positions of the two points higher than the ymiddlecm (middle position of the sector in y axis)
	// After scanning the half of the sector along the line you set, 
	// the steppor moder will filp the line in terms of the y=ymiddlecm and scan the other half of the sector
	double xorigincm = xi;
	double yorigincm = yi;
	double xfinalcm = xf;
	double yfinalcm = yf;
	double ymiddlecm = 23.0;
	double xsteplengthcm = dx;
	double ysteplengthcm = xsteplengthcm*(yfinalcm-yorigincm)/(xfinalcm-xorigincm);
	int xsteps = floor((xfinalcm-xorigincm)/xsteplengthcm);
	int ysteps = xsteps;
	std::cout<<"xfinalcm, xorigincm, xsteplengthcm, xsteps = " << xfinalcm << ", " << xorigincm << ", " << xsteplengthcm << ", " << xsteps << std::endl;
	int nRepeat = n_rep;
	/////// Adjust values up to this line!! 
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////


	double xorigin = stepspercm*xorigincm;
	double yorigin = stepspercm*yorigincm;
	double ymiddle = stepspercm*ymiddlecm;
	
	double xgeneralstep = stepspercm*xsteplengthcm;
	double ygeneralstep = stepspercm*ysteplengthcm;

	// Variable for recording current positions of drives. Used to calibrate
	// wait times. Takes stage 860ms to go 8062992 steps.
	double xcurrentpos = 0.0;
	double ycurrentpos = 0.0;
	double sleepzero = 0.0;
	double sleeptime = 0.0;
	double sleeptime2 = 0.0;
	
	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	// Checks to see if parameters are within physics ranges of the stages
	if (xsteps*xgeneralstep+xorigin > xmicrosteptot || ysteps*ygeneralstep+yorigin > ymicrosteptot)
	{
		cout << "Settings take the source beyond the end of the drive. Please readjust." << endl;
		Sleep(10000);
		return 0;
	}

	// Variables for PSERIAL_Receive command.
	unsigned char Unit;
	unsigned char Unit2;
	unsigned char Command;
	unsigned char Command2;
	long Data;
	long Data2;

	// Initializes the stepper motors to interpret instructions.
	PSERIAL_Initialize();

	// Rezero drives ---------------------------------------------------------
	PSERIAL_Open("com3");
	PSERIAL_Send( 0,2,0);
	Sleep(1500);
	//PSERIAL_Send( 1,1,64 );
	//PSERIAL_Send( 2,1,64 );
	// Waiting for drives by current position
	// Returns current position
	PSERIAL_Send( 1,60,64 );
	if (debug) std::cout<<__LINE__<<": Looking for a blip..."<<std::endl;
	while (1)
	{
		if (PSERIAL_Receive (&Unit,&Command,&Data) && Unit == 1 && Command == 60)
		{
			xcurrentpos = Data;
			break;
		}
	}
	if (debug) std::cout<<__LINE__<<": Found blip."<<std::endl;
	Sleep(1500);
	PSERIAL_Send( 2,60,64 );
	while (1)
	{
		if (PSERIAL_Receive (&Unit,&Command,&Data2) && Unit == 2 && Command == 60)
		{
			ycurrentpos = Data2;
			break;
		}
	}
	cout << "The position of the X stepper is " << xcurrentpos << "." << endl;
	cout << "The position of the Y stepper is " << ycurrentpos << "." << endl;

	if(xcurrentpos > ycurrentpos)
	{
		//Sleeptime if x > y
		sleepzero = 100*1000*(xcurrentpos/xmicrosteptot); //(length of drive in cm)*(ms/cm)*(fractional drive position)
	}
	else
	{
		sleepzero = 50*1000*(ycurrentpos/ymicrosteptot); //(length of drive in cm)*(ms/cm)*(fractional drive position)
	}

	PSERIAL_Send( 1,20,0 );
	PSERIAL_Send( 2,20,0 );
	Sleep(sleepzero);

	cout << "now starting the scan..." << endl;
	
	// Begin Scan
	double ylastpositioncm = 0;
	for (int ieo = 0; ieo < 2; ieo++){
	for (int ii = 0; ii < xsteps; ii++)
	{
		int jj=1;
		if((ieo==0 && ii==0) || (ieo==0 && ii==xsteps-1)) jj=0;
		for (jj; jj < 2; jj++)
		{
			
			/*
			//Setting the sleeptime for y to return
			if (jj == 0 && ii != 0){
				PSERIAL_Send( 2,60,64 );
				while (1)
				{
					if (PSERIAL_Receive (&Unit2,&Command,&Data2) && Unit2 == 2 && Command == 60)
					{
						ycurrentpos = Data2;
						break;
					}
				}
				sleeptime = 50*1000*((ycurrentpos-yorigin)/ymicrosteptot); //(length of drive in cm)*(ms/cm)*(fractional drive position)
			}*/
			
			//wait time to send to the origin from zero
			if(jj==0 && ii ==0)
			{ 
				if(xorigincm > yorigincm)
				{
					sleeptime = 1000*xorigincm; //(ms/cm)*(origin)
				}
				else
				{
					sleeptime = 1000*yorigincm; //(ms/cm)*(origin)
				}
			}

			
			//move the motor
			double xstepposition = (xorigin + ii*xgeneralstep);
			double ystepposition = (yorigin + ii*ygeneralstep);
			double xpositioncm = xorigincm + ((double)ii)*xsteplengthcm;
			double ypositioncm = yorigincm + ((double)ii)*ysteplengthcm; 
			if(ieo==0 && (ii==0 || ii==xsteps-1) && jj==0){// for dark current
				xstepposition = 0;
				ystepposition = 0;
				xpositioncm = 0;
				ypositioncm = 0;
			}

			if(ieo==1){
				ystepposition = ymiddle - ((yorigin + ii*ygeneralstep) -ymiddle);
				ypositioncm = ymiddlecm - ((yorigincm + ((double)ii)*ysteplengthcm) - ymiddlecm);
			}

			// move to the position
			printf("\nMoving to column %i, row %i : (x, y) = (%f, %f) cm\n", ii, jj, xpositioncm, ypositioncm);
			PSERIAL_Send( 1,20, xstepposition );
			PSERIAL_Send( 2,20, ystepposition );
			

			if (jj == 0)
			{
				Sleep(sleeptime);
			}
			else
			{
				Sleep(1000*ysteplengthcm); // (ms/cm)*(cm/step)
			}	
		
			if (debug) std::cout << "Reading through the socket:" << endl;

			///////////////////////////////////////////////////////////////////////////
			
			
			// Getting the data
			for(int irepeat = 0; irepeat < nRepeat; irepeat++){
			for(int kk = 0; kk < 16; kk++){
			for(int id = 0; id < 2; id++){
		
				char buf[50];
				sprintf(buf, "./set_epd -b 0 -d %d -c %d \n", id, kk);
				sendbuf = &buf[0];
				if(irepeat==0 && kk==0 && id==0){
					printf("giving command: ./set_epd -b 0 -d (0 to 1) -c (0 to 16) \n", id, kk);
					if(id==0)
						printf("%d, %d, %d, %d, %f, %f, \n", id, kk, kk*2, irepeat, xpositioncm, ypositioncm);
					else if(id==1)
						printf("%d, %d, %d, %d, %f, %f, \n", id, kk, kk*2+1, irepeat, xpositioncm, ypositioncm);
				}
				if(id==0)
					file_1 << id << ", " << kk<<", "<< kk*2<<", "<< irepeat <<", " << xpositioncm << ", " << ypositioncm <<", " ;
				else if(id==1)
					file_1 << id << ", " << kk<<", "<< kk*2+1<<", "<< irepeat <<", " << xpositioncm << ", " << ypositioncm <<", " ;

				// send initial buffer
				iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}

				char word_configure[] = "aaaaaaaaa";
				char word_reading[] = "aaaa";
				char corr_configure[] = "configure";

				char imon[] = "IMON";
				char rmon[] = "RMON";
				char vcomp[] = "OMP=";
				char curr;
				int i;
				int j;

				char *line_read = new char[100];
				while (iResult > 0) {
					iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
					if (iResult > 0) {
						//printf("Bytes received: %d\n", iResult);
						recvbuf2[iResult] = '\0';
						a = *(recvbuf2 + iResult - 2);
						aa = &a;
								
						for (i = 0; i < iResult; i++){
							curr = *(recvbuf2 + i);
							memmove(word_configure, &(word_configure[1]), strlen(&(word_configure[1])));
							memmove(word_reading, &(word_reading[1]), strlen(&(word_reading[1])));			
							//memmove(word_vcomp, &(word_vcomp[1]), strlen(&(word_vcomp[1])));			
							word_configure[strlen(word_configure) - 1] = curr;
							word_reading[strlen(word_reading) - 1] = curr;
							//word_vcomp[strlen(word_vcomp) - 1] = curr;
					
							// Finding IMON
							if(0 == strcmp(word_reading,imon)) {
								//printf("FOUND IT: %s \n", word_reading);
								strncpy(line_read, recvbuf2 + i + 2, 8);
								line_read[8] = '\0';
								for ( j = 0; j < 8; j++){
									if (line_read[j] == ',') break;
								}
								line_read[j] = '\0';
								//printf("%s , ", line_read);
								file_1 << line_read << ", ";
							}
							// Finding RMON
							if(0 == strcmp(word_reading,rmon)) {
								//printf("FOUND IT: %s \n", word_reading);
								strncpy(line_read, recvbuf2 + i + 2, 8);
								line_read[8] = '\0';
								for ( j = 0; j < 8; j++){
									if (line_read[j] == ',') break;
								}
								line_read[j] = '\0';
								//printf("%s, ", line_read);
								file_1 << line_read << ", ";
							}
							// Finding VCOMP
							if(0 == strcmp(word_reading, vcomp)) {
								cout<< "line: "<<__LINE__<<endl;
								printf("FOUND IT: %s \n", word_reading);
								cout<< "line: "<<__LINE__<<endl;
								strncpy(line_read, recvbuf2 + i + 1, 8);
								cout<< "line: "<<__LINE__<<endl;
								line_read[8] = '\0';
								cout<< "line: "<<__LINE__<<endl;
								for ( j = 0; j < 8; j++){
									if (line_read[j] == '/') break;
								}
								cout<< "line: "<<__LINE__<<endl;
								line_read[j] = '\0';
								printf("%s \n ", line_read);
								file_1 << line_read << endl;
								cout<< "line: "<<__LINE__<<endl;
							}
						}
						//printf("%s \n", recvbuf2);
						if(aa[0] == '#') break;
					}
					else if (iResult == 0) printf("Connection closed\n");
					else printf("recv function failed with error: %d\n", WSAGetLastError());
				}
			}//channel loop
			}//device loop
			}//repeat loop
			///////////////////////////////////////////////////////////////////////////

			

			printf("Data Collected \n");
			Sleep(2000);

		}//y step loop
	}//x step loop
	}//even/odd tile loop

	// Closing and rezeroing.
	file_1.close();
	/*
	printf("Returning to home positions");
	PSERIAL_Send( 1,1,64 );
	PSERIAL_Send( 2,1,64 );

	////////////////////////////////////////////////////////////////////////////
	// ENABLE IF HODOSCOPE IS ATTACHED! ----------------------------------------
	
	Sleep(100000);
	PSERIAL_Send( 1,36,0 );
	PSERIAL_Send( 2,36,0 );
	*/
	// -------------------------------------------------------------------------
	////////////////////////////////////////////////////////////////////////////
	
	if (debug) std::cout<<__LINE__<<"Closing socket"<<std::endl;
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ConnectSocket);
	WSACleanup();
	
	Sleep(100000);
	// Exiting program.
	return 1;
}

int FullSectorScan(char *filename, int n_rep = 5){

	return 1;
}
/*
int FullDarkCurrent(double x, double y, double dx, double dy, int nx, int ny, int n){

	bool debug = true;
	cout << "Starting!" << endl;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	char timestamp[100];
	char daypart[100];
	char timepart[100];

	if(tm.tm_hour>9 && tm.tm_min>9) sprintf(timepart,"%d%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour>9 && tm.tm_min<=9) sprintf(timepart,"%d0%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour<=9 && tm.tm_min>9) sprintf(timepart,"0%d%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour<=9 && tm.tm_min<=9) sprintf(timepart,"0%d0%d", tm.tm_hour, tm.tm_min);

	if(tm.tm_mon>8 && tm.tm_mday>9) sprintf(daypart,"%d%d%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon>8 && tm.tm_mday<=9) sprintf(daypart,"%d%d0%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon<=8 && tm.tm_mday>9) sprintf(daypart,"%d0%d%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon<=8 && tm.tm_mday<=9) sprintf(daypart,"%d0%d0%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

	sprintf(timestamp,"%s-%s",daypart,timepart);


	// Creating and opening a text file for pulse averaging method
	ofstream file_1;

	char filename1[100];
	char *disc = "TEST";
	sprintf(filename1,"%s_%s_OUTPUT.txt",timestamp, disc);

	file_1.open(filename1);
	// Commencing the socket
	if (debug) std::cout<<"Commence the socketing to TUFF box..."<<std::endl;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	file_1 << "Test Sector:" << endl;
	file_1 << "Device ID, Channel, x, y, IMON, RMON, VCOMP" <<endl;
	int iResult;
	int itt;

	char *sendbuf = "ls \n";
	char *lastbuf;
	char *recvbuf2 = new char[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	char *argv = "192.168.140.81";

	// initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		file_1 << "WSAStartup failed." <<endl;
		return 1;
	}

	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory( &hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(argv, DEFAULT_PORT, &hints, &result);
	if ( iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		file_1 << "getaddrinfo failed with error." <<endl;
		WSACleanup();
		return 1;
	}

	//attempt to connect
	for (ptr = result; ptr != NULL ; ptr=ptr->ai_next) {
		// create socket for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error :%ld\n", WSAGetLastError());
			file_1 << "socket failed with error." <<endl;
			WSACleanup();
			return 1;
		}
		// connect to server
		iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("unable to connect to server. \n");
		file_1 << "unable to connect to server." <<endl;
		WSACleanup();
		return 1;
	}

	// Receive until peer closes the connection
	iResult = 1;

	char a;
	char *aa;
	while (iResult > 0) {

		iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
			recvbuf2[iResult] = '\0';
			a = *(recvbuf2 + iResult - 2);
			aa = &a;
			//printf("%s \n", recvbuf2);
			if(aa[0] == '#') break;
		}
		else if (iResult == 0) printf("Connection closed\n");
		else printf("recv function failed with error: %d\n", WSAGetLastError());
	}

	sendbuf = "cd home \n";
	// send initial buffer
	iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	//printf("Bytes Sent: %ld\n", iResult);
	//printf("%s \n", sendbuf);

	while (iResult > 0) {

		iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
			recvbuf2[iResult] = '\0';
			a = *(recvbuf2 + iResult - 2);
			aa = &a;
			//printf("%s \n", recvbuf2);
			if(aa[0] == '#') break;
		}
		else if (iResult == 0) printf("Connection closed\n");
		else printf("recv function failed with error: %d\n", WSAGetLastError());
	}

	sendbuf = "ls \n";
	// send initial buffer
	iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	//printf("Bytes Sent: %ld\n", iResult);
	//printf("%s \n", sendbuf);

	while (iResult > 0) {

		iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
			recvbuf2[iResult] = '\0';
			a = *(recvbuf2 + iResult - 2);
			aa = &a;
			//printf("%s \n", recvbuf2);
			if(aa[0] == '#') break;
		}
		else if (iResult == 0) printf("Connection closed\n");
		else printf("recv function failed with error: %d\n", WSAGetLastError());
	}

	if (debug) std::cout<<"Done with socketing to TUFF box..."<<std::endl;


	// Reports total number of available microsteps for each drive.

	double xmicrosteptot = 8062992;
	double ymicrosteptot = 4031496;
	double stepspercm = xmicrosteptot/100;
	// Sets origin for scan./////////////////////////////////////////////////
	if (debug) std::cout<<"Setting Origin"<<std::endl;

	////////////////////////////////////////////////////////////////////////////
	/////// Adjust values from this line!!
	//for background and cosmics position test
	// 0 to 100 on x and 0 to 50 on y
	double xorigincm = x;
	double yorigincm = y;
	// Adjust for step length across both x and y by cm...
	double xsteplengthcm = dx;//20220114,18 sEPD test v3, v6
	double ysteplengthcm = dy;//20220114,18 sEPD test v3, v6

	// Adjust for number of steps across x and y ////////////////////////////
	int xsteps = nx;// 20220118 sEPD test v6 for dark current test
	int ysteps = ny;//20220118 sEPD test v6 for dark current test

	int nRepeat = n;
	/////// Adjust values up to this line!!
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////


	double xorigin = stepspercm*xorigincm;
	double yorigin = stepspercm*yorigincm;

	double xgeneralstep = stepspercm*xsteplengthcm;
	double ygeneralstep = stepspercm*ysteplengthcm;

	// Variable for recording current positions of drives. Used to calibrate
	// wait times. Takes stage 860ms to go 8062992 steps.
	double xcurrentpos = 0.0;
	double ycurrentpos = 0.0;
	double sleepzero = 0.0;
	double sleeptime = 0.0;
	double sleeptime2 = 0.0;


	//double xtotscanlengthcm = xorigincm + xsteplengthcm*xsteps;
	//double ytotscanlengthcm = yorigincm + ysteplengthcm*ysteps;
	double ytotscanlengthcm = 47.0;
	double yskiplengthcm = 15.0;
	double xskiplengthcm = 55.0;



	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////


	// Checks to see if parameters are within physics ranges of the stages
	if (xsteps*xgeneralstep+xorigin > xmicrosteptot || ysteps*ygeneralstep+yorigin > ymicrosteptot)
	{
		cout << "Settings take the source beyond the end of the drive. Please readjust." << endl;
		Sleep(10000);
		return 0;
	}

	// Variables for PSERIAL_Receive command.
	unsigned char Unit;
	unsigned char Unit2;
	unsigned char Command;
	unsigned char Command2;
	long Data;
	long Data2;

	// Warns if step settings distort scan grid.
	if ( xgeneralstep > ygeneralstep + 1000 || xgeneralstep < ygeneralstep - 1000)
	{
		cout << "Warning: Check Aspect Ratio." << endl;
		cout << "Each step in X is " << xgeneralstep << "." << endl;
		cout << "Each step in Y is " << ygeneralstep << "." << endl;
		cout << "It might be desired to adjust xaxis and yaxis parameters to produce similar values" << endl;
		Sleep(10000);
	}

	// Initializes the stepper motors to interpret instructions.
	PSERIAL_Initialize();

	// Rezero drives ---------------------------------------------------------
	PSERIAL_Open("com3");
	PSERIAL_Send( 0,2,0);
	Sleep(1500);
	//PSERIAL_Send( 1,1,64 );
	//PSERIAL_Send( 2,1,64 );
	// Waiting for drives by current position
	// Returns current position
	PSERIAL_Send( 1,60,64 );
	if (debug) std::cout<<__LINE__<<": Looking for a blip..."<<std::endl;
	while (1)
	{
		if (PSERIAL_Receive (&Unit,&Command,&Data) && Unit == 1 && Command == 60)
		{
			xcurrentpos = Data;
			break;
		}
	}
	if (debug) std::cout<<__LINE__<<": Found blip."<<std::endl;
	Sleep(1500);
	PSERIAL_Send( 2,60,64 );
	while (1)
	{
		if (PSERIAL_Receive (&Unit,&Command,&Data2) && Unit == 2 && Command == 60)
		{
			ycurrentpos = Data2;
			break;
		}
	}
	cout << "The position of the X stepper is " << xcurrentpos << "." << endl;
	cout << "The position of the Y stepper is " << ycurrentpos << "." << endl;

	if(xcurrentpos > ycurrentpos)
	{
		//Sleeptime if x > y
		sleepzero = 100*1000*(xcurrentpos/xmicrosteptot); //(length of drive in cm)*(ms/cm)*(fractional drive position)
	}
	else
	{
		sleepzero = 50*1000*(ycurrentpos/ymicrosteptot); //(length of drive in cm)*(ms/cm)*(fractional drive position)
	}
	//cout << sleepzero << endl;

	PSERIAL_Send( 1,20,0 );
	PSERIAL_Send( 2,20,0 );
	//Sleep(20000);
	Sleep(sleepzero);

	//cout << "now starting the scan..." << endl;

	// Begin Scan
	for (int ii = 0; ii < xsteps; ii++)
	{
		PSERIAL_Send( 1,20, (xorigin + ii*xgeneralstep) );

		for (int jj = 0; jj < ysteps; jj++)
		{
			//Setting the sleeptime for y to return
			if (jj == 0 && ii != 0){
				PSERIAL_Send( 2,60,64 );
				while (1)
				{
					if (PSERIAL_Receive (&Unit2,&Command,&Data2) && Unit2 == 2 && Command == 60)
					{
						ycurrentpos = Data2;
						break;
					}
				}
				sleeptime = 50*1000*((ycurrentpos-yorigin)/ymicrosteptot); //(length of drive in cm)*(ms/cm)*(fractional drive position)
			}
			//wait time to send to the origin from zero
			if(jj==0 && ii ==0)
			{
				if(xorigincm > yorigincm)
				{
					sleeptime = 1000*xorigincm; //(ms/cm)*(origin)
				}
				else
				{
					sleeptime = 1000*yorigincm; //(ms/cm)*(origin)
				}
			}


			//move the motor
			printf("\nMoving to column %i, row %i\n", ii, jj);
			PSERIAL_Send( 2,20,(yorigin + jj*ygeneralstep) );

			if (jj == 0)
			{
				Sleep(sleeptime);
			}
			else
			{
				Sleep(1000*ysteplengthcm); // (ms/cm)*(cm/step)
			}

			//skip the area where the sector does not exist
			double skipoffsetcm = yskiplengthcm*(xskiplengthcm-(xorigincm+ii*xsteplengthcm))/xskiplengthcm;
			bool doskip = abs(ytotscanlengthcm/2. - (yorigincm+jj*ysteplengthcm)) > (ytotscanlengthcm/2. - skipoffsetcm);
			if((xorigincm+ii*xsteplengthcm) == 0 && (yorigincm+jj*ysteplengthcm) == 0){
				printf("\nDo not skip the data taking when the source is at (0,0) to get the dark current (column %i, row %i)\n", ii, jj);
			} else{
				if(doskip){
					printf("\nSkipping column %i, row %i\n", ii, jj);
					continue;
				}
			}
			if (debug) std::cout << "Reading through the socket:" << endl;

			///////////////////////////////////////////////////////////////////////////
			int DCViasVoltage = 67;

			// Getting the data

			printf("Setting voltages to the two boards: %d V", DCBiasVoltage);
			for(int id = 0; id < 2; id++){
				for(int kk = 0; kk < 16; kk++){
					char buf[50];
					sprintf(buf, "./set_epd -b 0 -d %d -c %d -V %d -P 0 -C 0\n", id, kk, DCBiasVoltage);
					sendbuf = &buf[0];
					// send initial buffer
					iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
					if (iResult == SOCKET_ERROR) {
						printf("send failed with error: %d\n", WSAGetLastError());
						closesocket(ConnectSocket);
						WSACleanup();
						return 1;
					}
				}//channel loop
			}//device loop
					///////////////////////////////////////////////////////////////////////////
			for(int irepeat = 0; irepeat < nRepeat; irepeat++){
				if(floor(irepeat%((float)nRepeat/100.0))==0 ){
					printf("Progress: %f ... \n", (float)irepeat/(float)nRepeat);
					printf("-------------------------------------------- \n");
				}
				for(int id = 0; id < 2; id++){
					for(int kk = 0; kk < 16; kk++){
						char buf[50];
						int tile_number = kk*2 + id;
						sprintf(buf, "./set_epd -b 0 -d %d -c %d\n", id, kk, DCViasVoltage);
						sendbuf = &buf[0];
						if(floor(irepeat%((float)nRepeat/100.0))==0 ){
							printf("giving command: ./set_epd -b 0 -d %d -c %d -V %d -P 0 -C 0\n", id, kk, DCViasVoltage);
							printf("%d, %d , %f , %f , ", id, kk, xorigincm + ((double)ii)*xsteplengthcm, yorigincm + ((double)jj)*ysteplengthcm);
						}
						file_1 << id << ", " << kk<<", "<< tile_number << ", "<<irepeat<<", "<< xorigincm + ((double) ii)*xsteplengthcm << ", " << yorigincm + ((double) jj)*ysteplengthcm <<", " ;
						// send initial buffer
						iResult = send( ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
						if (iResult == SOCKET_ERROR) {
							printf("send failed with error: %d\n", WSAGetLastError());
							closesocket(ConnectSocket);
							WSACleanup();
							return 1;
						}

						//printf("Bytes Sent: %ld\n", iResult);
						//printf("%s \n", sendbuf);

						char word_configure[] = "aaaaaaaaa";
						char word_reading[] = "aaaa";
						char corr_configure[] = "configure";

						char imon[] = "IMON";
						char rmon[] = "RMON";
						char vcomp[] = "OMP=";
						char curr;
						int i;
						int j;

						char *line_read = new char[100];
						while (iResult > 0) {
							iResult = recv(ConnectSocket, recvbuf2, recvbuflen, 0);
							if (iResult > 0) {
								//printf("Bytes received: %d\n", iResult);
								recvbuf2[iResult] = '\0';
								a = *(recvbuf2 + iResult - 2);
								aa = &a;

								for (i = 0; i < iResult; i++){
									curr = *(recvbuf2 + i);
									memmove(word_configure, &(word_configure[1]), strlen(&(word_configure[1])));
									memmove(word_reading, &(word_reading[1]), strlen(&(word_reading[1])));
									//memmove(word_vcomp, &(word_vcomp[1]), strlen(&(word_vcomp[1])));
									word_configure[strlen(word_configure) - 1] = curr;
									word_reading[strlen(word_reading) - 1] = curr;
									//word_vcomp[strlen(word_vcomp) - 1] = curr;

									if(0 == strcmp(word_reading,imon)) {
										//printf("FOUND IT: %s \n", word_reading);
										strncpy(line_read, recvbuf2 + i + 2, 8);
										line_read[8] = '\0';
										for ( j = 0; j < 8; j++){
											if (line_read[j] == ',') break;
										}
										line_read[j] = '\0';
										if(floor(irepeat%((float)nRepeat/100.0))==0 ){
											printf("%s, ", line_read);
										}

										file_1 << line_read << ", ";
									}
									if(0 == strcmp(word_reading,rmon)) {
										//printf("FOUND IT: %s \n", word_reading);
										strncpy(line_read, recvbuf2 + i + 2, 8);
										line_read[8] = '\0';
										for ( j = 0; j < 8; j++){
											if (line_read[j] == ',') break;
										}
										line_read[j] = '\0';
										if(floor(irepeat%((float)nRepeat/100.0))==0 ){
											printf("%s, ", line_read);
										}
										file_1 << line_read << ", ";
									}
									if(0 == strcmp(word_reading,vcomp)) {
										//printf("FOUND IT: %s \n", word_reading);
										strncpy(line_read, recvbuf2 + i + 1, 8);
										line_read[8] = '\0';
										for ( j = 0; j < 8; j++){
											if (line_read[j] == '/') break;
										}
										line_read[j] = '\0';
										if(floor(irepeat%((float)nRepeat/100.0))==0 ){
											printf("%s \n", line_read);
										}
										file_1 << line_read << endl;
									}
								}
								//printf("%s \n", recvbuf2);
								if(aa[0] == '#') break;
							}
							else if (iResult == 0) printf("Connection closed\n");
							else printf("recv function failed with error: %d\n", WSAGetLastError());
						}
					}//channel loop
				}//device loop
			}//repeat loop


			printf("Data Collected \n");
			Sleep(2000);

		}

	}

	// Closing and rezeroing.
	file_1.close();

	printf("Returning to home positions");
	PSERIAL_Send( 1,1,64 );
	PSERIAL_Send( 2,1,64 );

	////////////////////////////////////////////////////////////////////////////
	// ENABLE IF HODOSCOPE IS ATTACHED! ----------------------------------------

	Sleep(100000);
	PSERIAL_Send( 1,36,0 );
	PSERIAL_Send( 2,36,0 );

	// -------------------------------------------------------------------------
	////////////////////////////////////////////////////////////////////////////

	if (debug) std::cout<<__LINE__<<"Closing socket"<<std::endl;
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ConnectSocket);
	WSACleanup();

	Sleep(100000);
	// Exiting program.
	return 0;

}

int SingleCosmic(int n, int ch){

	bool debug = true;
	cout << "Starting!" << endl;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	char timestamp[100];
	char daypart[100];
	char timepart[100];

	if(tm.tm_hour>9 && tm.tm_min>9) sprintf(timepart,"%d%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour>9 && tm.tm_min<=9) sprintf(timepart,"%d0%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour<=9 && tm.tm_min>9) sprintf(timepart,"0%d%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour<=9 && tm.tm_min<=9) sprintf(timepart,"0%d0%d", tm.tm_hour, tm.tm_min);

	if(tm.tm_mon>8 && tm.tm_mday>9) sprintf(daypart,"%d%d%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon>8 && tm.tm_mday<=9) sprintf(daypart,"%d%d0%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon<=8 && tm.tm_mday>9) sprintf(daypart,"%d0%d%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon<=8 && tm.tm_mday<=9) sprintf(daypart,"%d0%d0%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

	sprintf(timestamp,"%s-%s",daypart,timepart);


	// Creating and opening a text file for pulse averaging method
	ofstream file_1;
	ofstream file_2;
	ofstream file_3;
	ofstream file_4;
	ofstream file_5;
	ofstream file_6;
	ofstream file_7;

	char filename1[100];
	char filename2[100];
	char filename3[100];
	char filename4[100];
	char filename5[100];
	char filename6[100];
	char filename7[100];

	int tile = ch;
	sprintf(filename1,"%s_%d_TIME.txt",timestamp, tile);
	sprintf(filename2,"%s_%d_AREA_SIPM1.txt",timestamp, tile);
	sprintf(filename3,"%s_%d_VMIN_SIPM1.txt",timestamp, tile);
	sprintf(filename4,"%s_%d_Unaveraged_VMin1.txt",timestamp, tile);

	file_1.open(filename1);
	file_2.open(filename2);
	file_3.open(filename3);
	file_4.open(filename4);

	// Reports total number of available microsteps for each drive.
	double xo = 0.0;
	double yo = 0.0;
	MoveStepperMotor(xo, yo);

	//cout << "now starting the scan..." << endl;

	// Begin Scan
	for (int ii = 0; ii < n; ii++)
	{
		oscillo = iopen("gpib1,7");
		double area1 = 0.0; //New
		double vmin1 = 10.0;
		time_t rawtime;
		clock_t t;
		struct tm * timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		//double hour = 0.0;
		double minute = 0.0;
		double second = 0.0;
		itimeout(oscillo, 2000000);


		//cout << "now setting up the display for DAQ" << endl;

		// FOR SOURCE TEST, CHECK EVERY TIME
		WriteIO(":CDISPLAY");
		//cout << "view channel1?" << endl;
		WriteIO(":VIEW CHANNEL1");
		//cout << "view channel2?" << endl;
		WriteIO(":VIEW CHANNEL2");
		
		WriteIO(":TIMEBASE:SCALE 20E-9");
		//WriteIO(":TIMEBASE:POSITION -40E-9"); // source, cosmics
		WriteIO(":TIMEBASE:POSITION 130E-9"); // LED
		// New LED 375 nm
		WriteIO(":CHANNEL1:SCALE 200E-3");
		WriteIO(":CHANNEL2:SCALE 200E-3");
		WriteIO(":CHANNEL1:OFFSET -650E-3");
		WriteIO(":CHANNEL2:OFFSET -650E-3");
		//LED 361 nm
		
		//cout << "getting the clock" << endl;
		t = clock();
		//cout << "telling the scope to start " << endl;
		WriteIO(":RUN");
		WriteIO(":MEASURE:SENDVALID ON");

		// --- it's necessary to delay between unaveraged readouts so the scope doesn't choke and give duplicates
		// --- 100 (msec) is safe for source on panel, lower may also be possible
		// --- 200 is needed for off panel
		// --- 5000 is good for cosmics...

		int sleep = 100;

		// --- see below, method to prevent data loss during crashes...
		// --- this isn't necessary in linux, but it seems to be necessary in windows...
		file_1.close();
		file_4.close();
		file_5.close();
		file_6.close();
		file_7.close();

		//cout << "gonna start doing stuff now " << endl << endl;
		if(xsteps == 10 && ysteps == 10)
		{
			WriteIO(":ACQUIRE:AVERAGE OFF");
			iclose(oscillo);
			int VMin_Count = 0;
			int VMin_Stats = 3;
			//int VMin_Stats = 5000;
			double vminp = 0;
			while(VMin_Count < VMin_Stats)
			{
				vminp=vmin1;
				oscillo = iopen("gpib1,7");
				WriteIO(":MEASURE:SOURCE CHANNEL1");
				WriteIO(":MEASURE:VMIN");
				WriteIO(":MEASURE:VMIN?");
				ReadDouble(&vmin1);
				cout <<"VMin 1 "<< vmin1 << endl;
				iclose(oscillo);
				oscillo = iopen("gpib1,7");
				WriteIO(":MEASURE:SOURCE CHANNEL2");
				WriteIO(":MEASURE:VMIN");
				WriteIO(":MEASURE:VMIN?");
				ReadDouble(&vmin2);
				cout << "VMin 2 " << vmin2 << endl;
				iclose(oscillo);
				if(vminp!=vmin1)
				{
					file_6.open(filename6,ofstream::app);
					file_6 << vmin1 << endl;
					file_6.close();
					file_7.open(filename7,ofstream::app);
					file_7 << vmin2 << endl;
					file_7.close();
					cout << "data count number " << VMin_Count << endl;
					time ( &rawtime );
					timeinfo = localtime ( &rawtime );
					cout << "Time is: " << asctime (timeinfo) << endl;
					VMin_Count++;
				}
				Sleep(sleep);
			}
			//for(VMin_Count = 0; VMin_Count < VMin_Stats; VMin_Count++)
			//{
			//	oscillo = iopen("gpib1,7");
			//	WriteIO(":MEASURE:SOURCE CHANNEL2");
			//	WriteIO(":MEASURE:VMIN");
			//	WriteIO(":MEASURE:VMIN?");
			//	ReadDouble(&vmin2);
			//	cout << "VMin 2 " << vmin2 << endl;
			//	file_7 << vmin2 << "\n";
			//	iclose(oscillo);
			//	Sleep(sleep);
			//}
		} // matches if(xsteps...

		if ( xsteps > 1 || ysteps > 1 )
		{
			//oscillo = iopen("gpib1,7");
			WriteIO(":ACQUIRE:AVERAGE:COUNT 1500");
			WriteIO(":ACQUIRE:AVERAGE ON");

			// --- Measure the VMin for Channel 1
			WriteIO(":MEASURE:SOURCE CHANNEL1");
			WriteIO(":MEASURE:VMIN");
			WriteIO(":MEASURE:VMIN?");
			ReadDouble(&vmin1);
			cout << "VMin 1 "<< vmin1 << endl;
			iclose(oscillo);
			oscillo = iopen("gpib1,7");

			// --- Measure the VMin for Channel 2
			WriteIO(":MEASURE:SOURCE CHANNEL2");
			WriteIO(":MEASURE:VMIN");
			WriteIO(":MEASURE:VMIN?");
			ReadDouble(&vmin2);
			cout << "VMin 2 " << vmin2 << endl;

			file_4.open(filename4,ofstream::app);
			file_4 << vmin1 << endl;
			file_4.close();
			file_5.open(filename5,ofstream::app);
			file_5 << vmin2 << endl;
			file_5.close();

		} // matches if ( xsteps > 1 ...

		WriteIO(":STOP");
		t = clock() - t;

		iclose(oscillo);
		//cout << "Ch.1 integral: " << area1 << endl; //New
		//cout << "Ch.2 integral: " << area2 << endl;
		//cout << "Ch.4 integral: " << area4 << endl;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		cout << "Time is: " << asctime (timeinfo) << endl;
		printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
		cout << (float)t/CLOCKS_PER_SEC << endl;
		//cout << "Time is: " << hour << ":" << minute << ":" << second << endl;
		file_1.open(filename1,ofstream::app);
		file_1 << (float)t/CLOCKS_PER_SEC << "\n";
		file_1.close();
		//file_1 << hour << " " << minute << " " << second << "\n";
		file_2 << area1 << "\n"; //Saved as SiPM 1
		file_3 << area2 << "\n"; //New - Possibly confusing: file_1 contains time for data aquisition data, file_4 contains ave. pulse data from channel 1
		//file_4 << vmin1 << "\n";
		//file_5 << vmin2 << "\n";
		printf("Data Collected \n");
		Sleep(2000);

	}

	// Closing and rezeroing.
	file_1.close();
	file_2.close();
	file_3.close();
	file_4.close();
	file_5.close();
	file_6.close();
	file_7.close();
	printf("Returning to home positions");
	PSERIAL_Send( 1,1,64 );
	PSERIAL_Send( 2,1,64 );

	////////////////////////////////////////////////////////////////////////////
	// ENABLE IF HODOSCOPE IS ATTACHED! ----------------------------------------
	
	//Sleep(100000);
	//PSERIAL_Send( 1,36,0 );
	//PSERIAL_Send( 2,36,0 );
	
	// -------------------------------------------------------------------------
	////////////////////////////////////////////////////////////////////////////
	if (debug) std::cout<<__LINE__<<"Closing oscilliscope"<<std::endl;
	PSERIAL_Close();

	// Exiting program.
	return 0;

}

int FourCosmic(int n, int ch_1, int ch_2, int ch_3, int ch_4){
	bool debug = true;
	cout << "Starting!" << endl;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	char timestamp[100];
	char daypart[100];
	char timepart[100];

	if(tm.tm_hour>9 && tm.tm_min>9) sprintf(timepart,"%d%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour>9 && tm.tm_min<=9) sprintf(timepart,"%d0%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour<=9 && tm.tm_min>9) sprintf(timepart,"0%d%d", tm.tm_hour, tm.tm_min);
	else if(tm.tm_hour<=9 && tm.tm_min<=9) sprintf(timepart,"0%d0%d", tm.tm_hour, tm.tm_min);

	if(tm.tm_mon>8 && tm.tm_mday>9) sprintf(daypart,"%d%d%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon>8 && tm.tm_mday<=9) sprintf(daypart,"%d%d0%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon<=8 && tm.tm_mday>9) sprintf(daypart,"%d0%d%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	else if(tm.tm_mon<=8 && tm.tm_mday<=9) sprintf(daypart,"%d0%d0%d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

	sprintf(timestamp,"%s-%s",daypart,timepart);


	// Creating and opening a text file for pulse averaging method
	ofstream file_1;
	ofstream file_2;
	ofstream file_3;
	ofstream file_4;
	ofstream file_5;
	ofstream file_6;
	ofstream file_7;
	ofstream file_8;
	ofstream file_9;
	ofstream file_10;
	ofstream file_11;
	ofstream file_12;
	ofstream file_13;


	char filename1[100];
	char filename2[100];
	char filename3[100];
	char filename4[100];
	char filename5[100];
	char filename6[100];
	char filename7[100];
	char filename8[100];
	char filename9[100];
	char filename10[100];
	char filename11[100];
	char filename12[100];
	char filename13[100];

	int tile1 = ch_1;
	int tile2 = ch_2;
	int tile3 = ch_3;
	int tile4 = ch_4;

	sprintf(filename1,"%s_TIME.txt",timestamp);
	sprintf(filename2,"%s_%d_AREA_SIPM1.txt",timestamp, tile1);
	sprintf(filename3,"%s_%d_AREA_SIPM2.txt",timestamp, tile2);
	sprintf(filename4,"%s_%d_AREA_SIPM3.txt",timestamp, tile3);
	sprintf(filename5,"%s_%d_AREA_SIPM4.txt",timestamp, tile4);
	sprintf(filename6,"%s_%d_VMIN_SIPM1.txt",timestamp, tile1);
	sprintf(filename7,"%s_%d_VMIN_SIPM2.txt",timestamp, tile2);
	sprintf(filename8,"%s_%d_VMIN_SIPM3.txt",timestamp, tile3);
	sprintf(filename9,"%s_%d_VMIN_SIPM4.txt",timestamp, tile4);
	sprintf(filename10,"%s_%d_Unaveraged_VMin1.txt",timestamp, tile1);
	sprintf(filename11,"%s_%d_Unaveraged_VMin2.txt",timestamp, tile2);
	sprintf(filename12,"%s_%d_Unaveraged_VMin3.txt",timestamp, tile3);
	sprintf(filename13,"%s_%d_Unaveraged_VMin4.txt",timestamp, tile4);

	file_1.open(filename1);
	file_2.open(filename2);
	file_3.open(filename3);
	file_4.open(filename4);
	file_5.open(filename5);
	file_6.open(filename6);
	file_7.open(filename7);
	file_8.open(filename8);
	file_9.open(filename9);
	file_10.open(filename10);
	file_11.open(filename11);
	file_12.open(filename12);
	file_13.open(filename13);

	// Reports total number of available microsteps for each drive.


	// Sets origin for scan./////////////////////////////////////////////////
	if (debug) std::cout<<"Setting Origin"<<std::endl;

	double yorigincm = 0.0; // 20180720-1135
	double xorigincm = 0.0; // 20180720-1335
	MoveStepperMotor(xorigincm, yorigincm);


	oscillo = iopen("gpib1,7");
	double area1 = 0.0; //New
	double area2 = 0.0;
	double area3 = 0.0;
	double area4 = 0.0;
	double vmin1 = 10.0;
	double vmin2 = 10.0;
	double vmin3 = 10.0;
	double vmin4 = 10.0;

	time_t rawtime;
	clock_t t;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	//double hour = 0.0;
	double minute = 0.0;
	double second = 0.0;
	itimeout(oscillo, 2000000);


	//cout << "now setting up the display for DAQ" << endl;

	// FOR SOURCE TEST, CHECK EVERY TIME
	WriteIO(":CDISPLAY");
	//cout << "view channel1?" << endl;
	WriteIO(":VIEW CHANNEL1");
	WriteIO(":VIEW CHANNEL2");
	WriteIO(":VIEW CHANNEL3");
	WriteIO(":VIEW CHANNEL4");

	WriteIO(":TIMEBASE:SCALE 20E-9");
	//WriteIO(":TIMEBASE:POSITION -40E-9"); // source, cosmics
	WriteIO(":TIMEBASE:POSITION -40E-9"); // LED
	// New LED 375 nm
	WriteIO(":CHANNEL1:SCALE 50E-3");
	WriteIO(":CHANNEL2:SCALE 50E-3");
	WriteIO(":CHANNEL1:OFFSET -100E-3");
	WriteIO(":CHANNEL2:OFFSET 100E-3");
	WriteIO(":CHANNEL3:SCALE 50E-3");
	WriteIO(":CHANNEL4:SCALE 50E-3");
	WriteIO(":CHANNEL3:OFFSET -100E-3");
	WriteIO(":CHANNEL4:OFFSET -100E-3");

	//cout << "getting the clock" << endl;
	t = clock();
	//cout << "telling the scope to start " << endl;
	WriteIO(":RUN");
	WriteIO(":MEASURE:SENDVALID ON");

	// --- it's necessary to delay between unaveraged readouts so the scope doesn't choke and give duplicates
	// --- 100 (msec) is safe for source on panel, lower may also be possible
	// --- 200 is needed for off panel
	// --- 5000 is good for cosmics...

	int sleep = 5000;

	// --- see below, method to prevent data loss during crashes...
	// --- this isn't necessary in linux, but it seems to be necessary in windows...
	file_1.close();
	file_4.close();
	file_5.close();
	file_6.close();
	file_7.close();

	//cout << "gonna start doing stuff now " << endl << endl;
	WriteIO(":ACQUIRE:AVERAGE OFF");
	iclose(oscillo);
	int VMin1_Count = 0;
	int VMin_Stats = n;
	int VMin2_Count = 0;
	int VMin3_Count = 0;
	int VMin4_Count = 0;
	double vminp1 = 0;
	double vminp2 = 0;
	double vminp3 = 0;
	double vminp4 = 0;

	while((VMin1_Count < VMin_Stats)&&(VMin2_Count < VMin_Stats)&&(VMin3_Count < VMin_Stats)&&(VMin4_Count < VMin_Stats))
	{
		vminp1=vmin1;
		oscillo = iopen("gpib1,7");
		WriteIO(":MEASURE:SOURCE CHANNEL1");
		WriteIO(":MEASURE:VMIN");
		WriteIO(":MEASURE:VMIN?");
		ReadDouble(&vmin1);
		cout <<"VMin 1 "<< vmin1 << endl;
		iclose(oscillo);

		vminp2 = vmin2;
		oscillo = iopen("gpib1,7");
		WriteIO(":MEASURE:SOURCE CHANNEL2");
		WriteIO(":MEASURE:VMIN");
		WriteIO(":MEASURE:VMIN?");
		ReadDouble(&vmin2);
		cout << "VMin 2 " << vmin2 << endl;
		iclose(oscillo);

		vminp3=vmin3;
		oscillo = iopen("gpib1,7");
		WriteIO(":MEASURE:SOURCE CHANNEL3");
		WriteIO(":MEASURE:VMIN");
		WriteIO(":MEASURE:VMIN?");
		ReadDouble(&vmin3);
		cout <<"VMin 3 "<< vmin3 << endl;
		iclose(oscillo);

		vminp4 = vmin4;
		oscillo = iopen("gpib1,7");
		WriteIO(":MEASURE:SOURCE CHANNEL4");
		WriteIO(":MEASURE:VMIN");
		WriteIO(":MEASURE:VMIN?");
		ReadDouble(&vmin4);
		cout << "VMin 4 " << vmin4 << endl;
		iclose(oscillo);

		if(vminp1!=vmin1 && (VMin1_Count < VMin_Stats))
		{
			file_6.open(filename6,ofstream::app);
			file_6 << vmin1 << endl;
			file_6.close();
			cout << "data count number " << VMin1_Count << endl;
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			cout << "Time is: " << asctime (timeinfo) << endl;
			VMin1_Count++;
		}
		if(vminp2!=vmin2&& (VMin2_Count < VMin_Stats))
		{
			file_7.open(filename7,ofstream::app);
			file_7 << vmin2 << endl;
			file_7.close();
			cout << "data count number " << VMin2_Count << endl;
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			cout << "Time is: " << asctime (timeinfo) << endl;
			VMin2_Count++;
		}
		if(vminp3!=vmin3&& (VMin3_Count < VMin_Stats))
		{
			file_8.open(filename8,ofstream::app);
			file_8 << vmin3 << endl;
			file_8.close();
			cout << "data count number " << VMin3_Count << endl;
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			cout << "Time is: " << asctime (timeinfo) << endl;
			VMin3_Count++;
		}
		if(vminp4!=vmin4&& (VMin4_Count < VMin_Stats))
		{
			file_9.open(filename9,ofstream::app);
			file_9 << vmin4 << endl;
			file_9.close();
			cout << "data count number " << VMin4_Count << endl;
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			cout << "Time is: " << asctime (timeinfo) << endl;
			VMin4_Count++;
		}
		Sleep(sleep);
	}

	WriteIO(":STOP");
	t = clock() - t;

	iclose(oscillo);
	//cout << "Ch.1 integral: " << area1 << endl; //New
	//cout << "Ch.2 integral: " << area2 << endl;
	//cout << "Ch.4 integral: " << area4 << endl;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	cout << "Time is: " << asctime (timeinfo) << endl;
	printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
	cout << (float)t/CLOCKS_PER_SEC << endl;
	//cout << "Time is: " << hour << ":" << minute << ":" << second << endl;
	file_1.open(filename1,ofstream::app);
	file_1 << (float)t/CLOCKS_PER_SEC << "\n";
	file_1.close();
	//file_1 << hour << " " << minute << " " << second << "\n";

	printf("Data Collected \n");
	Sleep(2000);

	// Closing and rezeroing.
	file_1.close();
	file_2.close();
	file_3.close();
	file_4.close();
	file_5.close();
	file_6.close();
	file_7.close();
	file_8.close();
	file_9.close();
	file_10.close();
	file_11.close();
	file_12.close();
	file_13.close();

	printf("Returning to home positions");
	PSERIAL_Send( 1,1,64 );
	PSERIAL_Send( 2,1,64 );

	////////////////////////////////////////////////////////////////////////////
	// ENABLE IF HODOSCOPE IS ATTACHED! ----------------------------------------
	
	//Sleep(100000);
	//PSERIAL_Send( 1,36,0 );
	//PSERIAL_Send( 2,36,0 );
	
	// -------------------------------------------------------------------------
	////////////////////////////////////////////////////////////////////////////
	if (debug) std::cout<<__LINE__<<"Closing oscilliscope"<<std::endl;
	PSERIAL_Close();

	// Exiting program.
	return 0;
}
*/
int take_action(int list[], char *action, char *x1, char *y1, char *x2, char *y2, char *n, char *dx, char *dy, char *nx, char *ny, char *channel1, char *channel2, char *channel3, char *channel4, char *name = "OUTPUT"){
	if (!action){
		printf(" No action, please choose one: \n");
		printf("---------------------------------------------------------------------------------");
		printf("-T [action] : type of test -> 0. move stepper motor \n");
		printf("                              1. Read dark currrent \n");
		printf("                              2. Full scan \n");
		printf("                              3. Cosmics with scintillating fingers (one tile) \n");
		printf("                              4. Cosmics and junk (4 tiles) \n");
		printf("---------------------------------------------------------------------------------");
	}

	int act = atoi(action);
	double xorigincm = 0.0;
	double yorigincm = 0.0;
	double xfinalcm = 99.0;
	double yfinalcm = 49.0;
	double d_x = 0.5;
	double d_y = 0.5;
	int n_x = 1;
	int n_y = 1;
	int n_rep = 1;

	int ch_1 = 0;
	int ch_2 = 1;
	int ch_3 = 2;
	int ch_4 = 3;
	if(list[0] == 0){
		printf("Please Eneter a test type....\n");
		return 0;
	}
	if(list[1] == 1){
		xorigincm = std::stod(std::string(x1));
		printf("xorigincm: %f \n", xorigincm);
	}
	if(list[2] == 1){
		yorigincm = std::stod(std::string(y1));
		printf("yorigincm: %f \n", yorigincm);
	}
	if(list[3] == 1){
		xfinalcm = std::stod(std::string(x2));
		printf("xfinalcm: %f \n", xfinalcm);
	}
	if(list[4] == 1){
		yfinalcm = std::stod(std::string(y2));
		printf("yfinalcm: %f \n", yfinalcm);
	}
	if (list[5] == 1){
		n_rep = atoi(n);
		printf("n: %d \n", n_rep);
	}
	if (list[6] == 1){
		ch_1 = atoi(channel1);
		printf("channel 1: %d \n", ch_1);
	}
	if (list[7] == 1){
		ch_2 = atoi(channel2);
		printf("channel 2: %d \n", ch_2);
	}
	if (list[8] == 1){
		ch_3 = atoi(channel3);
		printf("channel 3: %d \n", ch_3);
	}
	if (list[9] == 1){
		ch_4 = atoi(channel4);
		printf("channel 4: %d \n", ch_4);
	}
		if (list[10] == 1){
		sscanf(nx, "%d", n_x);
		printf("Steps in x: %d \n", n_x);
	}
	if (list[11] == 1){
		sscanf(ny, "%d", n_y);
		printf("Steps in y: %d \n", n_y);
	}
	if (list[12] == 1){
		sscanf(dx, "%f", d_x);
		printf("Length of step in x: %f \n", d_x);
	}
	if (list[13] == 1){
		sscanf(dy, "%d", d_y);		
		printf("Length of step in x: %f \n", n_x);
	}
	char *filename = name;

	int cheese = 0;

	switch(act) {
		case 0:
			cheese = MoveStepperMotor(xorigincm, yorigincm);
		case 1:
			cheese = ReadDarkCurrent(n_rep, filename);
		case 2:
			cheese = DrawALine(filename, n_rep);
		case 3:
			cheese = FullSectorScan(filename, n_rep);
	}

	return cheese;
}

int main(int argc, char *argv[]){
	// Options in the main:
	/*
		0) Move Stepper Motor

		1) Read Dark current n times

		2) Full scan

		3) cosmics on trigger (1 tile)

		4) take all in ( 4 tile )

	*/
	if (argc == 1){
		printf("Please enter at least one argument... \n");
		printf("------------------------------------------------------------------------------ \n");
		printf("-T [action] : type of test -> 0. move stepper motor \n");
		printf("                              1. Read dark currrent \n");
		printf("                              2. Full scan \n");
		printf("                              3. Cosmics with scintillating fingers (one tile) \n");
		printf("                              4. Cosmics and junk (4 tiles) \n");

		printf("-x [x-position] : for test 0, 1, 2 -> pick destination/starting x-pos of source \n");
		printf("                Range : 0 cm (left) to 100 cm (right) \n");

		printf("-y [y-position] : for test 0, 1, 2 -> pick destination/starting y-pos of source \n");
		printf("                Range : 0 cm (back) to 50 cm (front) \n");

		printf("-n [repetitions] : for test 1, 2, 3, 4 -> number of measurementsd \n");
		printf("                Test 1 : Number of dark readings \n");
		printf("                Test 2 : Number of measurements at each location \n");
		printf("                Test 3 : Number of triggered events on the scintillating fingers \n");
		printf("                Test 4 : Number of triggered events for each channel \n");

		printf("-c1/c2/c3/c4 [channel numbers] : for test 3, 4 -> number of channels used \n");
		printf("                Test 3 : Only use c1 \n");
		printf("                Test 4 : Use all 4 channel options \n");
		printf("                Channel scheme:  3-5-7-9-11-13-15-17-19-21-23-25-27-29-31: \n");
		printf("                               1 \n");
		printf("                                 2-4-6-8-10-12-14-16-18-20-22-24-26-28-30: \n");
		printf("-------------------------------------------------------------------------------\n");

		printf("Examples: \n");
		printf("1) Move the stepper motor to the center of the sector\n");
		printf("           sEPD_Test.exe -T 0 -x 50 -y 25 \n");
		return 0;
	}
	char* action = new char[100];
	char* x1 = new char[100];
	char* y1 = new char[100];
	char* x2 = new char[100];
	char* y2 = new char[100];
	char* n = new char[100];
	char* dx = new char[100];
	char* dy = new char[100];
	char* nx = new char[100];
	char* ny = new char[100];
	char* filename = new char[100];
	char* channel1 = new char[100];
	char* channel2 = new char[100];
	char* channel3 = new char[100];
	char* channel4 = new char[100];
	int list[14] = {0};
	for (int i = 0; i < argc; i++){
		if (std::string(argv[i]) == "-T"){
			sprintf(action, "%s", argv[i+1]);
			list[0] = 1;
		}
		else if (std::string(argv[i]) == "-f"){
			sprintf(filename, "%s", argv[i+1]);
		}
		else if (std::string(argv[i]) == "-x" || std::string(argv[i]) == "-x1"){
			sprintf(x1, "%s", argv[i+1]);
			list[1] = 1;
		}
		else if (std::string(argv[i]) == "-y" || std::string(argv[i]) == "-y1"){
			sprintf(y1, "%s", argv[i+1]);
			list[2] = 1;
		}
		else if (std::string(argv[i]) == "-x2"){
			sprintf(x2, "%s", argv[i+1]);
			list[3] = 1;
		}
		else if (std::string(argv[i]) == "-y2"){
			sprintf(y2, "%s", argv[i+1]);
			list[4] = 1;
		}
		else if (std::string(argv[i]) == "-N"){
			sprintf(n, "%s", argv[i+1]);
			list[5] = 1;
		}
		else if (std::string(argv[i]) == "-c1"){
			sprintf(channel1, "%s", argv[i+1]);
			list[6] = 1;
		}
		else if (std::string(argv[i]) == "-c2"){
			sprintf(channel2, "%s", argv[i+1]);
			list[7] = 1;
		}
		else if (std::string(argv[i]) == "-c3"){
			sprintf(channel3, "%s", argv[i+1]);
			list[8] = 1;
		}
		else if (std::string(argv[i]) == "-c4"){
			sprintf(channel4, "%s", argv[i+1]);
			list[9] = 1;
		}
		else if (std::string(argv[i]) == "-nx"){
			sprintf(nx, "%s", argv[i+1]);
			list[10] = 1;
		}
		else if (std::string(argv[i]) == "-ny"){
			sprintf(ny, "%s", argv[i+1]);
			list[11] = 1;
		}
		else if (std::string(argv[i]) == "-dx"){
			sprintf(dx, "%s", argv[i+1]);
			list[12] = 1;
		}
		else if (std::string(argv[i]) == "-dy"){
			sprintf(dy, "%s", argv[i+1]);
			list[13] = 1;
		}
	}
	
	int cheese;

	cheese = take_action(list, action, x1, y1, x2, y2, n, dx, dy, nx, ny, channel1, channel2, channel3, channel4, filename);

	return 0;
}
