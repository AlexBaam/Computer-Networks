/* Stiu ca este ciudat sa scriu in romana si engleza;
Dar tutorialele si tot contentul folosit pentru a invata este in engleza, asa ca imi este mai comod sa
adaug explicatii in limba engleza, si orice alt comentariu in limba romana; */


// INTREGUL PROIECT ESTE REALIZAT LUAD CODUL DIN LAB7 SI MODIFICAND;
// IN SERVER AM MODIFICAT SUBPROGRAMUL RESPONSE SA FOLOSEASCA STRINGS SI VECTORI DE CARACTERE PENTRU RASPUNSURI;
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "Classes/RegLog.h"
#include "Classes/XMLWorkAround.h"
#include "Classes/Helpers.h"
#include "Classes/JSONMethods.h"
using namespace std;
#define PORT 2507

extern int errno;

enum Commands {
	ROUTES,
	ARRIVAL,
	DEPARTURE,
	SIG_DELAY,
	SIG_EARLY,
	LOGIN,
	REGISTER,
	LOGOUT,
	QUIT,
	UNKNOWN
};

unordered_map<string, Commands> commands_LoggedOut = {
{"routes", ROUTES},
	{"login", LOGIN},
	{"register", REGISTER},
	{"quit", QUIT},
	{"unknown", UNKNOWN}
};

unordered_map<string, Commands> commands_LoggedIn = {
	{"routes", ROUTES},
    {"arrival", ARRIVAL},
    {"departure", DEPARTURE},
	{"delay", SIG_DELAY},
	{"early", SIG_EARLY},
	{"logout", LOGOUT},
	{"quit", QUIT},
	{"unknown", UNKNOWN}
};

typedef struct thData{
	int idThread;
	int cl;
}thData;

Helpers cmdHelper;

static void *treat(void *);
void response(void *);

void sendToClient(const thData& tdL, string &msgForClient){

	//Pas 1: Trimitem lungimea mesajului:
	int ansLen =  msgForClient.size();
	if (write(tdL.cl, &ansLen, sizeof (ansLen)) <= 0) {
		cmdHelper.CheckOutput("[Server - ERROR] write() length to the Client.", true);
		return;
	}

	//Pas 2: Trimitem mesajul in sine:
	if (write(tdL.cl, msgForClient.c_str(), ansLen) <= 0) {
		cmdHelper.CheckOutput("[Server - ERROR] write() to the Client.", true);
		return;
	};

	cout << "[Thread] " << tdL.idThread << " Response sent: " << msgForClient << endl;
}
string receiveFromClient(const thData& tdL) {

	int msgLen = 0;
	cout << "[Server - DEBUG] Received message length: " << msgLen << endl;
	if (read (tdL.cl, &msgLen,sizeof(msgLen)) <= 0){
		cout<<"[Thread] " << tdL.idThread << endl;
		cmdHelper.CheckOutput("[Server - ERROR] read() length from the Client.", true);
		return "";

	}

	vector<char> MsgBuffer(msgLen + 1, 0);
	cout << "[Server - DEBUG] Received message: " << string(MsgBuffer.data(), msgLen) << endl;
	if (read(tdL.cl, MsgBuffer.data(), msgLen) <= 0)
	{
		cout<<"[Thread] " << tdL.idThread << " failed read()." << endl;
		cmdHelper.CheckOutput("[Server - ERROR] read() message from the Client.", true);
		return "";
	}

	string message = (string) MsgBuffer.data();
	cout << "[Thread] " << tdL.idThread << " Received message: " << message << endl;
	return message;

}

int main () {
  struct sockaddr_in server;
  struct sockaddr_in from;	
  int nr;
  int sd;
  int pid;
  pthread_t th[100];
	int i=0;

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[Server - ERROR] socket().\n");
      return errno;
    }

  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));

    server.sin_family = AF_INET;	

    server.sin_addr.s_addr = htonl (INADDR_ANY);

    server.sin_port = htons (PORT);

  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[Server - ERROR] bind().\n");
      return errno;
    }

  if (listen (sd, 2) == -1)
    {
      perror ("[Server - ERROR] listen().\n");
      return errno;
    }
  while (1){
      int client;
      thData * td;
  	  socklen_t length = sizeof (from);

  	  cout << "[Server] Waiting for client on port: " << PORT << endl;
      fflush (stdout);

      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
	{
	  perror ("[Server - ERROR] accept().\n");
	  continue;
	}

	td=(struct thData*)malloc(sizeof(struct thData));	
	td->idThread=i++;
	td->cl=client;

	pthread_create(&th[i], NULL, &treat, td);	      
				
	}
};

static void *treat(void * arg)
{		
		struct thData tdL; 
		tdL= *((struct thData*)arg);
		cout<<"[Thread] " << tdL.idThread << " Waiting for message..." << endl;
		fflush (stdout);		 
		pthread_detach(pthread_self());		
		response((struct thData*)arg);
		close ((intptr_t)arg);
		return(NULL);	
  		
};

void response(void *arg){
	struct thData tdL; 
	tdL= *((struct thData*)arg);

	RegLog user;
	string clrMSG;

	while (1) { // Added a loop to the code from the course so I can handle multiple requests;

		string msgFromClient = receiveFromClient(tdL);
		if (msgFromClient.empty()) {
			clrMSG = "[Server - ERROR] Empty message from the Client!";
			cmdHelper.CheckOutput(clrMSG, true);
			break;
		}

		string msgForClient; // A response for the client from the server
		if (user.logCheck() == false) {
			Commands cmd = commands_LoggedOut.count(msgFromClient) ? commands_LoggedOut[msgFromClient] : UNKNOWN;
			switch (cmd) {
				case ROUTES: {
					XMLWorkAround x("/home/alex/RC/TrainApp/DataBases/Trains.XML");

					if (x.LoadXML() != true) {
						msgForClient = "[Server] Error: Failed to load the XML file";
						break;
					}
					msgForClient = x.PrintRoutes();
					sendToClient(tdL, msgForClient);
					break;
				}
				case LOGIN: {
					//Mesaj de la client:
					string NameFromClient = receiveFromClient(tdL);
					if (NameFromClient.empty()) {
						clrMSG = "[Server - ERROR] No name received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received name: " << NameFromClient << endl;

					//Mesaj de la client:
					string PassFromClient = receiveFromClient(tdL);
					if (PassFromClient.empty()) {
						clrMSG = "[Server - ERROR] No name received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received password: " << PassFromClient << endl;

					if (user.login(NameFromClient, PassFromClient) == true) {
						msgForClient = "[Server] Logged in successfully.";
						cout << "[Thread] Sending to client: " << msgForClient << endl;
						sendToClient(tdL, msgForClient);
					}
					else {
						msgForClient = "[Server - ERROR] Failed to login to the application.";
						cout << "[Thread] Sending to client: " << msgForClient << endl;
						sendToClient(tdL, msgForClient);
					};
					break;
				}
				case REGISTER: {

					//Mesaj de la client:
					string NameFromClient = receiveFromClient(tdL);
					if (NameFromClient.empty()) {
						clrMSG = "[Server - ERROR] No name received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received name: " << NameFromClient << endl;

					//Mesaj de la client:
					string PassFromClient = receiveFromClient(tdL);
					if (PassFromClient.empty()) {
						clrMSG = "[Server - ERROR] No name received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received password: " << PassFromClient << endl;

					if (user.regUser(NameFromClient, PassFromClient) == true) {
						msgForClient = "[Server] Registered successfully.";
						cout << "[Thread] Sending to client: " << msgForClient << endl;
						sendToClient(tdL, msgForClient);
					}
					else {
						msgForClient = "[Server - ERROR] Failed to register.";
						cout << "[Thread] Sending to client: " << msgForClient << endl;
						sendToClient(tdL, msgForClient);
					};
					break;
				}
				case QUIT: {
					msgForClient = "[Server] You are trying to quit the application.";
					int ansLen =  msgForClient.size();
					write(tdL.cl, &ansLen, sizeof (ansLen)); // Send response length;
					write(tdL.cl, msgForClient.c_str(), ansLen); // Send response message;
					close(tdL.cl);
					return;
				}
				default: {
					msgForClient = "[Server - ERROR] Command Unknown!";
					sendToClient(tdL, msgForClient);
				}
			}
		}else {
			Commands cmd = commands_LoggedIn.count(msgFromClient) ? commands_LoggedIn[msgFromClient] : UNKNOWN;
			switch (cmd) {
				case ROUTES: {
					XMLWorkAround x("/home/alex/RC/TrainApp/DataBases/Trains.XML");

					if (x.LoadXML() != true) {
						msgForClient = "[Server] Error: Failed to load the XML file";
						break;
					}
					msgForClient = x.PrintRoutes();
					sendToClient(tdL, msgForClient);
					break;
				}
				case ARRIVAL: {
					XMLWorkAround x("/home/alex/RC/TrainApp/DataBases/Trains.XML");

					if (x.LoadXML() != true) {
						msgForClient = "[Server] Error: Failed to load the XML file";
						break;
					}

					//Mesaj de la client:
					string StationNameFromClient = receiveFromClient(tdL);
					if (StationNameFromClient.empty()) {
						clrMSG = "[Server - ERROR] No station name received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received station name: " << StationNameFromClient << endl;

					//Mesaj de la client:
					string HourFromClient = receiveFromClient(tdL);
					if (HourFromClient.empty()) {
						clrMSG = "[Server - ERROR] No hour received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received hour: " << HourFromClient << endl;

					msgForClient = x.PrintArrival(StationNameFromClient, HourFromClient);
					sendToClient(tdL, msgForClient);
					break;
				}
				case DEPARTURE: {
					XMLWorkAround x("/home/alex/RC/TrainApp/DataBases/Trains.XML");

					if (x.LoadXML() != true) {
						msgForClient = "[Server] Error: Failed to load the XML file";
						break;
					}

					//Mesaj de la client:
					string StationNameFromClient = receiveFromClient(tdL);
					if (StationNameFromClient.empty()) {
						clrMSG = "[Server - ERROR] No station name received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received station name: " << StationNameFromClient << endl;

					//Mesaj de la client:
					string HourFromClient = receiveFromClient(tdL);
					if (HourFromClient.empty()) {
						clrMSG = "[Server - ERROR] No hour received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received hour: " << HourFromClient << endl;

					msgForClient = x.PrintDeparture(StationNameFromClient, HourFromClient);
					sendToClient(tdL, msgForClient);
					break;
				}
				case SIG_DELAY: {
					XMLWorkAround x("/home/alex/RC/TrainApp/DataBases/Trains.XML");

					if (x.LoadXML() != true) {
						msgForClient = "[Server] Error: Failed to load the XML file";
						break;
					}

					//Mesaj de la client:
					string idFromClient = receiveFromClient(tdL);
					if (idFromClient.empty()) {
						clrMSG = "[Server - ERROR] No id received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received ID: " << idFromClient << endl;

					//Mesaj de la client:
					string stationNumSTR = receiveFromClient(tdL);
					cout << "[Server - DEBUG] Received raw station num string: " << stationNumSTR << endl;
					int stationNum = atoi(stationNumSTR.c_str());
					cout << "[Server - DEBUG] Received integer station num: " << stationNum << endl;
					if (stationNum < 0) {
						clrMSG = "[Server - ERROR] Invalid station number received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received station num: " << stationNum << endl;

					//Mesaj de la client:
					string delaySTR = receiveFromClient(tdL);
					cout << "[Server - DEBUG] Received raw delay string: " << delaySTR << endl;
					int delay = atoi(delaySTR.c_str());
					cout << "[Server - DEBUG] Received integer delay: " << delay << endl;
					/*if (delay < 0) {
						clrMSG = "[Server] Error: Invalid delay received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}*/
					cout << "[Thread] " << tdL.idThread << " Received delay:  " << stationNum << endl;

					cout << "[Server - DEBUG] Adding delay to train ID: " << idFromClient << ", station num: " << stationNum << ", delay: " << delay << endl;
					x.AddDelay(idFromClient, stationNum, delay);
					break;
				}
				case SIG_EARLY: {
					XMLWorkAround x("/home/alex/RC/TrainApp/DataBases/Trains.XML");

					if (x.LoadXML() != true) {
						msgForClient = "[Server] Error: Failed to load the XML file";
						break;
					}

					//Mesaj de la client:
					string idFromClient = receiveFromClient(tdL);
					if (idFromClient.empty()) {
						clrMSG = "[Server - ERROR] No id received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received ID: " << idFromClient << endl;

					//Mesaj de la client:
					string stationNumSTR = receiveFromClient(tdL);
					cout << "[Server - DEBUG] Received raw station num string: " << stationNumSTR << endl;
					int stationNum = atoi(stationNumSTR.c_str());
					cout << "[Server - DEBUG] Received integer station num: " << stationNum << endl;
					if (stationNum < 0) {
						clrMSG = "[Server - ERROR] Invalid station number received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}
					cout << "[Thread] " << tdL.idThread << " Received station num: " << stationNum << endl;

					//Mesaj de la client:
					string earlySTR = receiveFromClient(tdL);
					cout << "[Server - DEBUG] Received raw early string: " << earlySTR << endl;
					int early = atoi(earlySTR.c_str());
					cout << "[Server - DEBUG] Received integer early: " << early << endl;
					/*if (delay < 0) {
						clrMSG = "[Server] Error: Invalid delay received from the Client!";
						cmdHelper.CheckOutput(clrMSG, true);
						break;
					}*/
					cout << "[Thread] " << tdL.idThread << " Received early arrival:  " << stationNum << endl;

					cout << "[Server - DEBUG] Adding early arrival to train ID: " << idFromClient << ", station num: " << stationNum << ", delay: " << early << endl;
					x.AddEarly(idFromClient, stationNum, early);
					break;
				}
				case LOGOUT: {
					msgForClient = "[Server] You are trying to log out the application.";
					sendToClient(tdL, msgForClient);
					user.logout();
					break;
				}
				case QUIT: {
					msgForClient = "[Server] You are trying to quit the application.";
					int ansLen =  msgForClient.size();
					write(tdL.cl, &ansLen, sizeof (ansLen)); // Send response length;
					write(tdL.cl, msgForClient.c_str(), ansLen); // Send response message;
					close(tdL.cl);
					return;
				}
				default: {
					msgForClient = "[Server - ERROR] Command Unknown!";
					sendToClient(tdL, msgForClient);
				}
			}
		}
	}
		close(tdL.cl); // Closing the client socket after the loop;
}