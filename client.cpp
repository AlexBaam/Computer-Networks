// INTREGUL PROIECT ESTE REALIZAT LUAD CODUL DIN LAB7 SI MODIFICAND;
// IN CLIENT AM FOLOSIT STRING SI VECTORI DE CARACTERE PENTRU A FACE TRIMITERILE SI PRIMIRILE PTR SERVER;
// DE ASEMENEA AM ADAUGAT UN WHILE(1) PENTRU A PERMITE USER-ULUI SA FACA CERERI MULTIPLE CATRE SERVER;

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <ostream>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <vector>

#include "Classes/Helpers.h"

using namespace std;
extern int errno;

int port;
string clrMSG;
Helpers cmdHelper;

enum StateListForClient { // Nu merge login cum trebuie, incerc switch case;
  LOGGED_OUT, // Default: routes, login, register, quit;
  LOGGED_IN, // Logat: routes, delay, early, logout, quit;
  LOGIN_USERNAME,
  LOGIN_PASSWORD,
  REGISTER_USERNAME,
  REGISTER_PASSWORD,
  DELAY_ID,
  DELAY_STATION,
  DELAY_DELAY,
  EARLY_ID,
  EARLY_STATION,
  EARLY_EARLY,
  SEARCHING,
  DEPARTURE_SEARCH_STATION,
  DEPARTURE_SEARCH_HOUR,
  ARRIVAL_SEARCH_STATION,
  ARRIVAL_SEARCH_HOUR,
  QUIT // Inchidere WHILE(currentState != QUIT);
};

StateListForClient currentState = LOGGED_OUT; // Starea actuala este pe default, urmeaza introducere de comanda;

void StateChange(StateListForClient& State, StateListForClient newState) { // Functie ptr debug al schimbarii;
  State = newState;
  cout << "[Client - DEBUG] State changed to: " << State << endl;
}

int main (int argc, char *argv[]) {
  int sd;
  struct sockaddr_in server;

  if (argc != 3)
  {
    clrMSG = "[Client - DEBUG] Syntax: " + string(argv[0]) + " <IP Address> <Port>";
    cmdHelper.CheckOutput(clrMSG, true);
    return -1;
  }

  port = atoi (argv[2]);

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
    clrMSG = "[Client - ERROR] socket().";
    cmdHelper.CheckOutput(clrMSG, true);
    return errno;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);

  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
  {
    clrMSG = "[Client - ERROR] connect().";
    cmdHelper.CheckOutput(clrMSG, true);
    return errno;
  }

  cmdHelper.Open();

  while (currentState != QUIT) {
    //Infinite loop that keeps going until I call QUIT;

    switch (currentState) {
      case LOGGED_OUT: {
        string message = cmdHelper.Menu(false);

        if (message.empty()) {
          clrMSG = "[Client - ERROR] Empty command.";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        if (message == "login") {
          //Pas 1: Trimitem lungimea catre server
          int msgLen = message.size();
          if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
            break;
          }

          //Pas 2: Trimitem mesajul in sine
          if (write(sd, message.c_str(), msgLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
            break;
          }
          StateChange(currentState, LOGIN_USERNAME); // Trecem la pasul de username;
        } else if (message == "register") {
          //Pas 1: Trimitem lungimea catre server
          int msgLen = message.size();
          if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
            break;
          }

          //Pas 2: Trimitem mesajul in sine
          if (write(sd, message.c_str(), msgLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
            break;
          }
          StateChange(currentState, REGISTER_USERNAME); // Trecem la pasul de username;
        } else if (message == "routes") {
          //Pas 1: Trimitem lungimea catre server
          int msgLen = message.size();
          if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
            break;
          }

          //Pas 2: Trimitem mesajul in sine
          if (write(sd, message.c_str(), msgLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
            break;
          }

          //Pas 1: Citim lungimea raspunsului;
          int ansLen = 0;
          if (read (sd, &ansLen, sizeof (ansLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] read() length from the server.", true);
            break;
          }
          cout << "[Client - DEBUG] Received message length: " << ansLen << endl;

          //Pas 2: Citim mesajul in sine;
          vector<char> AnsBuffer(ansLen + 1, 0);
          if (read (sd, AnsBuffer.data(), ansLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] read() from the server.", true);
            break;
          }
          cout << "[Client - DEBUG] Received message: " << string(AnsBuffer.data(), ansLen) << endl;
          string answer = AnsBuffer.data();

        } else if (message == "quit") {

          //Pas 1: Trimitem lungimea mesajului:
          int msgLen = message.size();
          if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
            break;
          }

          //Pas 2: Trimitem mesajul in sine:
          if (write(sd, message.c_str(), msgLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
            break;
          }

          //Pas 1: Citim lungimea raspunsului;
          int ansLen = 0;
          if (read (sd, &ansLen, sizeof (ansLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] read() length from the server.", true);
            break;
          }
          cout << "[Client - DEBUG] Received message length: " << ansLen << endl;

          //Pas 2: Citim mesajul in sine;
          vector<char> AnsBuffer(ansLen + 1, 0);
          if (read (sd, AnsBuffer.data(), ansLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] read() from the server.", true);
            break;
          }
          cout << "[Client - DEBUG] Received message: " << string(AnsBuffer.data(), ansLen) << endl;
          string answer = AnsBuffer.data();

          StateChange(currentState, QUIT);
        }else {
          clrMSG = "[Client - ERROR] Invalid command.";
          cmdHelper.CheckOutput(clrMSG, true);
        }
        break;
      }
      case LOGGED_IN: {

        string message = cmdHelper.Menu(true);
        if (message.empty()) {
          clrMSG = "[Client - ERROR] Empty command.";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }
        if (message == "routes") {
          //Pas 1: Trimitem lungimea catre server
          int msgLen = message.size();
          if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
            break;
          }

          //Pas 2: Trimitem mesajul in sine
          if (write(sd, message.c_str(), msgLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
            break;
          }

          //Pas 1: Citim lungimea raspunsului;
          int ansLen = 0;
          if (read (sd, &ansLen, sizeof (ansLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] read() length from the server.", true);
            break;
          }
          cout << "[Client - DEBUG] Received message length: " << ansLen << endl;

          //Pas 2: Citim mesajul in sine;
          vector<char> AnsBuffer(ansLen + 1, 0);
          if (read (sd, AnsBuffer.data(), ansLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] read() from the server.", true);
            break;
          }
          cout << "[Client - DEBUG] Received message: " << string(AnsBuffer.data(), ansLen) << endl;
          string answer = AnsBuffer.data();

        } else if (message == "delay") {
          //Pas 1: Trimitem lungimea catre server
          int msgLen = message.size();
          if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
            break;
          }

          //Pas 2: Trimitem mesajul in sine
          if (write(sd, message.c_str(), msgLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
            break;
          }
          StateChange(currentState, DELAY_ID);
        } else if (message == "early") {
          //Pas 1: Trimitem lungimea catre server
          int msgLen = message.size();
          if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
            break;
          }

          //Pas 2: Trimitem mesajul in sine
          if (write(sd, message.c_str(), msgLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
            break;
          }
          StateChange(currentState, EARLY_ID);
        } else if (message == "logout") {
          //Pas 1: Trimitem lungimea catre server
          int msgLen = message.size();
          if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
            break;
          }

          //Pas 2: Trimitem mesajul in sine
          if (write(sd, message.c_str(), msgLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
            break;
          }

          //Pas 1: Citim lungimea raspunsului;
          int ansLen = 0;
          if (read (sd, &ansLen, sizeof (ansLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] read() length from the server.", true);
            break;
          }
          cout << "[Client - DEBUG] Received message length: " << ansLen << endl;

          //Pas 2: Citim mesajul in sine;
          vector<char> AnsBuffer(ansLen + 1, 0);
          if (read (sd, AnsBuffer.data(), ansLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] read() from the server.", true);
            break;
          }
          cout << "[Client - DEBUG] Received message: " << string(AnsBuffer.data(), ansLen) << endl;
          string answer = AnsBuffer.data();

          StateChange(currentState, LOGGED_OUT);
        } else if (message == "quit") {
          //Pas 1: Trimitem lungimea mesajului:
          int msgLen = message.size();
          if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
            break;
          }

          //Pas 2: Trimitem mesajul in sine:
          if (write(sd, message.c_str(), msgLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
            break;
          }

          //Pas 1: Citim lungimea raspunsului;
          int ansLen = 0;
          if (read (sd, &ansLen, sizeof (ansLen)) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] read() length from the server.", true);
            break;
          }
          cout << "[Client - DEBUG] Received message length: " << ansLen << endl;

          //Pas 2: Citim mesajul in sine;
          vector<char> AnsBuffer(ansLen + 1, 0);
          if (read (sd, AnsBuffer.data(), ansLen) <= 0) {
            cmdHelper.CheckOutput("[Client - ERROR] read() from the server.", true);
            break;
          }
          cout << "[Client - DEBUG] Received message: " << string(AnsBuffer.data(), ansLen) << endl;
          string answer = AnsBuffer.data();

          StateChange(currentState, QUIT);
        }else if (message == "search") {
            StateChange(currentState, SEARCHING); // Trecem la pasul de username;
        } else {
          clrMSG = "[Client - ERROR] Invalid command, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
        }
        break;
      }
      case LOGIN_USERNAME: {
        cout << "[Client] Enter username: ";
        string username;
        getline(cin, username);

        if (username.empty()) {
          clrMSG = "[Client - ERROR] Empty username, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = username.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, username.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        StateChange(currentState, LOGIN_PASSWORD);
        break;
      }
      case LOGIN_PASSWORD: {
        cout << "[Client] Enter password: ";
        string password;
        getline(cin, password);

        if (password.empty()) {
          clrMSG = "[Client - ERROR] Empty password, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = password.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, password.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        //Pas 1: Citim lungimea raspunsului;
        int ansLen = 0;
        if (read (sd, &ansLen, sizeof (ansLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] read() length from the server.", true);
          break;
        }
        cout << "[Client - DEBUG] Received message length: " << ansLen << endl;

        //Pas 2: Citim mesajul in sine;
        vector<char> AnsBuffer(ansLen + 1, 0);
        if (read (sd, AnsBuffer.data(), ansLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] read() from the server.", true);
          break;
        }
        cout << "[Client - DEBUG] Received message: " << string(AnsBuffer.data(), ansLen) << endl;
        string answer = AnsBuffer.data();

        if (answer == "[Server] Logged in successfully.") {
          cout << "[Client] Logged in successfully." << endl;
          StateChange(currentState, LOGGED_IN);
        } else {
          clrMSG = "[Client - ERROR] Login failed.\nPossible causes: Wrong username or password.";
          cmdHelper.CheckOutput(clrMSG, true);
          StateChange(currentState, LOGGED_OUT);
        }
        break;
      }
      case REGISTER_USERNAME: {
        cout << "[Client] Enter username: ";
        string username;
        getline(cin, username);

        if (username.empty()) {
          clrMSG = "[Client - ERROR] Empty username, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = username.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, username.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        StateChange(currentState, REGISTER_PASSWORD);
        break;
      }
      case REGISTER_PASSWORD: {
        cout << "[Client] Enter password: ";
        string password;
        getline(cin, password);

        if (password.empty()) {
          clrMSG = "[Client - ERROR] Empty password, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = password.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, password.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        //Pas 1: Citim lungimea raspunsului;
        int ansLen = 0;
        if (read (sd, &ansLen, sizeof (ansLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] read() length from the server.", true);
          break;
        }
        cout << "[Client - DEBUG] Received message length: " << ansLen << endl;

        //Pas 2: Citim mesajul in sine;
        vector<char> AnsBuffer(ansLen + 1, 0);
        if (read (sd, AnsBuffer.data(), ansLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] read() from the server.", true);
          break;
        }
        cout << "[Client - DEBUG] Received message: " << string(AnsBuffer.data(), ansLen) << endl;
        string answer = AnsBuffer.data();

        if (answer == "[Server] Registered successfully.") {
          cout << "[Client] Registered successfully." << endl;
          StateChange(currentState, LOGGED_OUT);
        } else {
          clrMSG = "[Client - ERROR] Register failed.\nPossible causes: Already existing username.";
          cmdHelper.CheckOutput(clrMSG, true);
          StateChange(currentState, LOGGED_OUT);
        }
        break;
      }
      case DELAY_ID: {
        cout << "[Client] Enter train ID: ";
        string ID;
        getline(cin, ID);

        if (ID.empty()) {
          clrMSG = "[Client - ERROR] Empty username, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = ID.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, ID.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        StateChange(currentState, DELAY_STATION);
        break;
      }
      case DELAY_STATION: {
        cout << "[Client] Enter station number: ";
       string numberStation;
        getline(cin,numberStation);

        if (numberStation.empty()) {
          clrMSG = "[Client - ERROR] Invalid station, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = numberStation.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, numberStation.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        StateChange(currentState, DELAY_DELAY);
        break;
      }
      case DELAY_DELAY: {
        cout << "[Client] Enter delay: ";
        string delay;
        getline(cin,delay);

        if (delay.empty()) {
          clrMSG = "[Client - ERROR] Invalid station, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = delay.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, delay.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        StateChange(currentState, LOGGED_IN);
        break;
      }
      case EARLY_ID: {
        cout << "[Client] Enter train ID: ";
        string ID;
        getline(cin, ID);

        if (ID.empty()) {
          clrMSG = "[Client - ERROR] Empty ID, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = ID.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, ID.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        StateChange(currentState, EARLY_STATION);
        break;
      }
      case EARLY_STATION: {
        cout << "[Client] Enter station number: ";
        string numberStation;
        getline(cin,numberStation);

        if (numberStation.empty()) {
          clrMSG = "[Client - ERROR] Invalid station, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = numberStation.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, numberStation.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        StateChange(currentState, EARLY_EARLY);
        break;
      }
      case EARLY_EARLY: {
        cout << "[Client] Enter early: ";
        string early;
        getline(cin,early);

        if (early.empty()) {
          clrMSG = "[Client - ERROR] Invalid early, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = early.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, early.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        StateChange(currentState, LOGGED_IN);
        break;
      }
      case SEARCHING: {
        cout << "[Client] Searching options:" << endl
        << "-arrival;" << endl
        <<"-departure;" << endl;
        string search;
        getline(cin, search);

        if (search.empty()) {
          clrMSG = "[Client - ERROR] No search option selected, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = search.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, search.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        if (search == "arrival") {
          StateChange(currentState, ARRIVAL_SEARCH_STATION);
        } else if (search == "departure") {
          StateChange(currentState, DEPARTURE_SEARCH_STATION);
        } else {
          clrMSG = "[Client - ERROR] Invalid searching data!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }
        break;
      }
      case ARRIVAL_SEARCH_STATION: {
        cout << "[Client] Enter station: ";
        string station;
        getline(cin, station);

        if (station.empty()) {
          clrMSG = "[Client - ERROR] Empty station, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = station.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, station.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        StateChange(currentState, ARRIVAL_SEARCH_HOUR);
        break;
      }
      case ARRIVAL_SEARCH_HOUR: {
        cout << "[Client] Enter hour: ";
        string hour;
        getline(cin, hour);

        if (hour.empty()) {
          clrMSG = "[Client - ERROR] Empty hour, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = hour.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, hour.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        int ansLen = 0;
        if (read (sd, &ansLen, sizeof (ansLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] read() length from the server.", true);
          break;
        }
        cout << "[Client - DEBUG] Received message length: " << ansLen << endl;

        //Pas 2: Citim mesajul in sine;
        vector<char> AnsBuffer(ansLen + 1, 0);
        if (read (sd, AnsBuffer.data(), ansLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] read() from the server.", true);
          break;
        }
        cout << "[Client - DEBUG] Received message: " << string(AnsBuffer.data(), ansLen) << endl;
        string answer = AnsBuffer.data();

        StateChange(currentState, LOGGED_IN);
        break;
      }
      case DEPARTURE_SEARCH_STATION: {
        cout << "[Client] Enter station: ";
        string station;
        getline(cin, station);

        if (station.empty()) {
          clrMSG = "[Client - ERROR] Empty station, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = station.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, station.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        StateChange(currentState, DEPARTURE_SEARCH_HOUR);
        break;
      }
      case DEPARTURE_SEARCH_HOUR: {
        cout << "[Client] Enter hour: ";
        string hour;
        getline(cin, hour);

        if (hour.empty()) {
          clrMSG = "[Client - ERROR] Empty hour, try again!";
          cmdHelper.CheckOutput(clrMSG, true);
          break;
        }

        //Pas 1: Trimitem lungimea catre server
        int msgLen = hour.size();
        if (write(sd, &msgLen, sizeof (msgLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() length to the server.", true);
          break;
        }

        //Pas 2: Trimitem mesajul in sine
        if (write(sd, hour.c_str(), msgLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] write() to the server.", true);
          break;
        }

        int ansLen = 0;
        if (read (sd, &ansLen, sizeof (ansLen)) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] read() length from the server.", true);
          break;
        }
        cout << "[Client - DEBUG] Received message length: " << ansLen << endl;

        //Pas 2: Citim mesajul in sine;
        vector<char> AnsBuffer(ansLen + 1, 0);
        if (read (sd, AnsBuffer.data(), ansLen) <= 0) {
          cmdHelper.CheckOutput("[Client - ERROR] read() from the server.", true);
          break;
        }
        cout << "[Client - DEBUG] Received message: " << string(AnsBuffer.data(), ansLen) << endl;
        string answer = AnsBuffer.data();

        StateChange(currentState, LOGGED_IN);
        break;
      }
      default: {
        clrMSG = "[Client - ERROR] Invalid state."; // Utilizare gresita de StateChange
        cmdHelper.CheckOutput(clrMSG, true);
        StateChange(currentState, QUIT);
        break;
      }
    }
  }
  close (sd);
  return 0;
}