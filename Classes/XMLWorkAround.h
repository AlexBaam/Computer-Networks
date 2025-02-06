#pragma once
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <libxml/xpath.h>
#include <string>

#include "Helpers.h"
using namespace std;

class XMLWorkAround {
private:
    string XMLfile;
    xmlDoc* doc;
    int StationNum;
    string ID;
    string clrMSG;
    Helpers cmdHelper;

public:
    XMLWorkAround(); // Def constructor;
    XMLWorkAround(string XMLfile); // Constructor
    ~XMLWorkAround(); // Destructor

    //Fct ptr extragere informatii necesare:
    string getID(xmlNode* train);
    int getStationNum(xmlNode* station);

    // Functiile Necesare:
    bool LoadXML(); // Functie ptr a deschide fisierul
    void SaveXML(); // Functie ptr a updata fisierul
    string PrintRoutes(); // Functie afisare rute si trenuri
    string PrintDeparture(string StationName,string hour);
    string PrintArrival(string StationName,string hour);
    void AddDelay(string trainID, int stationNum, int delay); // Functie semnalare intarziere
    void AddEarly(string trainID, int stationNum, int early); // Functie semnalare ajuns devreme
};