#include "XMLWorkAround.h"
#include <iostream>
#include <ostream>

XMLWorkAround::XMLWorkAround() {
}

XMLWorkAround::XMLWorkAround(string XMLfile) {
    this->XMLfile = XMLfile;
    this->doc = nullptr;
    clrMSG = "[Class] XMLWorkAround initialized the XMLfile with: " + XMLfile;
    cmdHelper.CheckOutput(clrMSG, false);
}

XMLWorkAround::~XMLWorkAround() {
    if (doc != nullptr) {
        xmlFreeDoc(doc); // Fct ce goleste fisierul XML din memorie;
        clrMSG = "[Class] XML document freed from the memory.";
        cmdHelper.CheckOutput(clrMSG, false);
    }
    xmlCleanupParser(); // Stergem resursele utilizate de LibXML2;
    clrMSG = "[Class] XML resources cleaned up.";
    cmdHelper.CheckOutput(clrMSG, false);
}

string XMLWorkAround::getID(xmlNode *train) {
    xmlChar* id = xmlGetProp(train, BAD_CAST "id");
    string trainID;
    if (id != nullptr) {
        trainID = (const char*) id;
        clrMSG = "[Class - DEBUG] ID receieved: " + trainID;
       cmdHelper.PurpleText(clrMSG);
        free(id);
    } else {
        clrMSG = "[Class - ERROR] ID cannot be retrieved!";
        cmdHelper.CheckOutput(clrMSG, true);
    }
    return trainID;
}

int XMLWorkAround::getStationNum(xmlNode *station) {
    xmlChar* num = xmlGetProp(station, BAD_CAST "num");
    int stationNum = -1;
    if (num != nullptr) {
        stationNum = atoi((const char*) num);
        clrMSG = "[Class - DEBUG] Station number receieved: " + stationNum;
        cmdHelper.PurpleText(clrMSG);
        xmlFree(num);
    } else {
        clrMSG = "[Class - ERROR] Station number cannot be retrieved!";
        cmdHelper.CheckOutput(clrMSG, true);
    }

    return stationNum;
}

bool XMLWorkAround::LoadXML() {
    // Primul pas e sa ne asiguram ca doc este liber;
    if (doc != nullptr) {
        xmlFreeDoc(doc);
        doc = nullptr;
    }
    // Pasul 2 este sa dam load fisierului XML in doc;
    doc = xmlReadFile(XMLfile.c_str(), NULL, 0);
    if (doc == nullptr) {
        clrMSG = "[Class - ERROR] XML could not parse the file: " + XMLfile;
        cmdHelper.CheckOutput(clrMSG, true);
        return false;
    }

    clrMSG = "[Class] XML document loaded successfully!";
    cmdHelper.CheckOutput(clrMSG, false);
    cout << "[Class] XML document loaded successfully." << endl;
    return true;
}

void XMLWorkAround::SaveXML() {
    if (doc == nullptr) {
        clrMSG = "[Class - ERROR] No XML file to be saved!";
        cmdHelper.CheckOutput(clrMSG, true);
        return;
    }

    // Updatare a documentului:
    int bytes = xmlSaveFileEnc(XMLfile.c_str(), doc, "UTF-8");
    if (bytes < 0) {
        clrMSG =  "[Class - ERROR] XML could not be saved.";
        cmdHelper.CheckOutput(clrMSG, true);
        return;
    }

    clrMSG =  "[Class] XML document saved successfully!";
    cmdHelper.CheckOutput(clrMSG, false);
}

string XMLWorkAround::PrintRoutes() {
    if (doc == nullptr) {
        clrMSG = "[Class - ERROR] No XML file to be printed!";
        cmdHelper.CheckOutput(clrMSG, true);
        return clrMSG;
    }

    xmlNode* root = xmlDocGetRootElement(doc);
    // Basically noi cand salvam XML in doc, acesta este interpretat ca un arbore;
    if (root == nullptr || xmlStrcmp(root->name, BAD_CAST "trains") != 0) {
        clrMSG = "[Class - ERROR]  XML root element <trains> not found.";
        cmdHelper.CheckOutput(clrMSG, true);
        return clrMSG;
    }

    string ROUTES;
    for (xmlNode* train = root->children; train != nullptr; train = train->next) {
        if (train->type == XML_ELEMENT_NODE && xmlStrcmp(train->name, BAD_CAST "train") == 0) {
            xmlChar* trainID = xmlGetProp(train, BAD_CAST "id");
            ROUTES += "Train ID: " + string((const char*)trainID) + "\n";

            //Acum ne trebuie nodurile "from" si "to" pentru a afisa ruta
            xmlNode* trainFrom = nullptr;
            xmlNode* trainTo = nullptr;
            xmlNode* stations = nullptr;
            bool foundFrom = false;
            bool foundTo = false;
            bool foundStations = false;

            for (xmlNode* child = train->children; child != nullptr; child = child->next) {
                if (child->type == XML_ELEMENT_NODE) {
                    if (xmlStrcmp(child->name, BAD_CAST "from") == 0) {
                        trainFrom = child;
                        foundFrom = true;
                    }
                    if (xmlStrcmp(child->name, BAD_CAST "to") == 0) {
                        trainTo = child;
                        foundTo = true;
                    }
                    if (xmlStrcmp(child->name, BAD_CAST "stations") == 0) {
                        stations = child;
                        foundStations = true;
                    }
                }
            }

            ROUTES += "Route: " +
                string((const char*)xmlNodeGetContent(trainFrom)) + " " +
                "-> " +
                string((const char*)xmlNodeGetContent(trainTo)) + "\n";

            // Adaug si statiile la printare;
           if (stations) {
                ROUTES += "Stations: \n";
                for (xmlNode* station = stations->children; station != nullptr; station = station->next) {
                    if (station->type == XML_ELEMENT_NODE && xmlStrcmp(station->name, BAD_CAST "station") == 0) {
                        // Luam detaliile legate de statii:
                        xmlNode* StationName = nullptr;
                        xmlNode* StationArrival = nullptr;
                        xmlNode* StationDeparture = nullptr;
                        xmlNode* Delay = nullptr;
                        xmlNode* Early = nullptr;
                        bool FoundName = false, FoundArrival = false, FoundDeparture = false, FoundDelay = false, FoundEarly = false;

                        for (xmlNode* info = station->children; info != nullptr; info = info->next) {
                            if (info->type == XML_ELEMENT_NODE) {
                                if (xmlStrcmp(info->name, BAD_CAST "name") == 0) {
                                    StationName = info;
                                    FoundName = true;
                                }
                                if (xmlStrcmp(info->name, BAD_CAST "arrival") == 0) {
                                    StationArrival = info;
                                    FoundArrival = true;
                                }
                                if (xmlStrcmp(info->name, BAD_CAST "departure") == 0) {
                                    StationDeparture = info;
                                    FoundDeparture = true;
                                }
                                if (xmlStrcmp(info->name, BAD_CAST "delay") == 0) {
                                    Delay = info;
                                    FoundDelay = true;
                                }
                                if (xmlStrcmp(info->name, BAD_CAST "early") == 0) {
                                    Early = info;
                                    FoundEarly = true;
                                }
                            }
                        }
                        if (FoundName) {
                            ROUTES +="-" + string((const char*)xmlNodeGetContent(StationName)) +" ";
                        } else {
                            ROUTES += "-Unknown Station Name ";
                        }
                        if (FoundArrival) {
                            ROUTES += "(Arrival: " + string((const char*)xmlNodeGetContent(StationArrival)) +",";
                        } else {
                            ROUTES += "(Starting Station,";
                        }
                        if (FoundDeparture) {
                            ROUTES += " Departure: " + string((const char*)xmlNodeGetContent(StationDeparture)) +",";
                        } else {
                            ROUTES += " Last Station,";
                        }
                        if (FoundDelay) {
                            ROUTES += " Delay: " + string((const char*)xmlNodeGetContent(Delay));
                        }
                        if (FoundEarly) {
                            ROUTES += ", Early: " + string((const char*)xmlNodeGetContent(Early)) +")\n";
                        } else {
                            ROUTES += ")\n";
                        }
                    }

                }
            }

            if (foundFrom == false) {
                clrMSG = "[Class - ERROR] XML element <from> not found.";
                cmdHelper.CheckOutput(clrMSG, true);
            }
            if (foundTo == false) {
                clrMSG = "[Class] Error: XML element <to> not found." ;
                cmdHelper.CheckOutput(clrMSG, true);
            }
            if (foundStations == false) {
                clrMSG = "[Class] Error: XML element <stations> not found." ;
                cmdHelper.CheckOutput(clrMSG, true);
            }
        }
    }
    return ROUTES;
}

string XMLWorkAround::PrintDeparture(string StationName, string hour) {
    if (doc == nullptr) {
        clrMSG = "[Class - ERROR] No XML document to be printed.";
        cmdHelper.CheckOutput(clrMSG, true);
        return clrMSG;
    }

    cout << "Station: " << StationName << " Departure: " << hour << "\n";
    if ((hour.length() != 5) || hour[2] != ':') {
        clrMSG = "[Class - ERROR] Invalid hour format!";
        cmdHelper.CheckOutput(clrMSG, true);
        return clrMSG;
    }

    string ROUTES;
    int SentDepHour = stoi(hour.substr(0, 2));
    cout << "[Class - DEBUG] SentDepHour: " << SentDepHour << "\n";
    int SentDepMinute = stoi(hour.substr(3, 2));
    cout << "[Class - DEBUG] SentDepMinute: " << SentDepMinute << "\n";
    int SentTotalNrMinutes = SentDepMinute + SentDepHour * 60;
    cout << "[Class - DEBUG] Departure time in minute: " << SentTotalNrMinutes << endl;
    int LatestHour = SentTotalNrMinutes + 60;
    cout << "[Class - DEBUG] LatestHour: " << LatestHour << "\n";

    xmlNode* root = xmlDocGetRootElement(doc);
    if ((root == nullptr) || xmlStrcmp(root->name, BAD_CAST "trains") != 0) {
        clrMSG = "[Class - ERROR] XML root element <trains> not found.\n";
        cmdHelper.CheckOutput(clrMSG, true);
        return clrMSG;
    }

    cout << "[Class - DEBUG] Trains based on station and departure: " << endl;
    for (xmlNode* train = root->children; train != nullptr; train = train->next) {
        if ((train->type == XML_ELEMENT_NODE) &&  xmlStrcmp(train->name, BAD_CAST "train") == 0 ) {
            string trainDIDI;
            xmlChar* id = xmlGetProp(train, BAD_CAST "id");
            if (id != nullptr) {
                trainDIDI = (const char*)id;
                xmlFree(id);
            }
            for (xmlNode* stations = train->children; stations != nullptr; stations = stations->next) {
                if ((train->type == XML_ELEMENT_NODE) &&  xmlStrcmp(stations->name, BAD_CAST "stations") == 0) {
                    for (xmlNode* station = stations->children; station != nullptr; station = station->next) {
                        if ((station->type == XML_ELEMENT_NODE) &&  xmlStrcmp(station->name, BAD_CAST "station") == 0 ) {
                            string StationIamAt;
                            string StationTime;
                            string StationDelay;
                            for (xmlNode* info = station->children; info != nullptr; info = info->next) {
                                if ((info->type == XML_ELEMENT_NODE) &&  xmlStrcmp(info->name, BAD_CAST "name") == 0) {
                                    StationIamAt = string((const char*)xmlNodeGetContent(info));
                                    cout << "[Class - DEBUG] StationIamAt: " << StationIamAt << "\n";
                                }
                                if ((info->type == XML_ELEMENT_NODE) &&  xmlStrcmp(info->name, BAD_CAST "departure") == 0) {
                                   StationTime = string((const char*)xmlNodeGetContent(info));
                                    cout << "[Class - DEBUG] StationTime: " << StationTime << "\n";
                                }
                                if ((info->type == XML_ELEMENT_NODE) &&  xmlStrcmp(info->name, BAD_CAST "delay") == 0) {
                                    StationDelay = string((const char*)xmlNodeGetContent(info));
                                    cout << "[Class - DEBUG] StationDelay: " << StationDelay << "\n";
                                }
                            }
                            if (!StationIamAt.empty() && StationIamAt == StationName ) {
                                if (!StationTime.empty()) {
                                    int depHour = stoi(StationTime.substr(0, 2));
                                    cout << "[Class - DEBUG] Departure hour of train: " << depHour << "\n";
                                    int depMinute = stoi(StationTime.substr(3, 2));
                                    cout << "[Class - DEBUG] Departure minutes of train: " << depMinute << "\n";
                                    int totalNrMinutes = depHour * 60 + depMinute;
                                    cout << "[Class - DEBUG] Departure of this train: " << totalNrMinutes << "\n";
                                    int depDelay = stoi(StationDelay);
                                    cout << "[Class - DEBUG] Delay of this train: " << depDelay << "\n";
                                    int MinAndDelay = totalNrMinutes + depDelay;
                                    cout << "[Class - DEBUG] Delay of this train in mins: " << MinAndDelay << "\n";

                                    if ((MinAndDelay >= SentTotalNrMinutes) && (MinAndDelay <= LatestHour)) {
                                        ROUTES += "Train ID: " +  trainDIDI + " Station: " + StationName + " Departure: " + StationTime + "\n";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (ROUTES.empty()) {
            ROUTES = "No train found on this route.\n";
        }
    }
    return ROUTES;
}

string XMLWorkAround::PrintArrival(string StationName, string hour) {
       if (doc == nullptr) {
        return "[Class - ERROR] No XML document to be printed.\n";
    }

    cout << "Station: " << StationName << " Arrival: " << hour << "\n";
    if ((hour.length() != 5) || hour[2] != ':') {
        return "[Class - ERROR] Invalid hour format!\n";
    }

    string ROUTES;
    int SentArvHour = stoi(hour.substr(0, 2));
    cout << "[Class - DEBUG] SentArvHour: " << SentArvHour << "\n";
    int SentArvMinute = stoi(hour.substr(3, 2));
    cout << "[Class - DEBUG] SentArvMinute: " << SentArvMinute << "\n";
    int SentTotalNrMinutes = SentArvMinute + SentArvHour * 60;
    int EarliestHour = SentTotalNrMinutes + 60;
    cout << "[Class - DEBUG] Arrival time in minute: " << SentTotalNrMinutes << endl;
    cout << "[Class - DEBUG] EarliestHour: " << EarliestHour << "\n";

    xmlNode* root = xmlDocGetRootElement(doc);
    if ((root == nullptr) || xmlStrcmp(root->name, BAD_CAST "trains") != 0) {
        return "[Class - DEBUG] Error: XML root element <trains> not found.\n";
    }

    cout << "[Class - DEBUG] Trains based on station and departure: " << endl;
    for (xmlNode* train = root->children; train != nullptr; train = train->next) {
        if ((train->type == XML_ELEMENT_NODE) &&  xmlStrcmp(train->name, BAD_CAST "train") == 0 ) {
            string trainDIDI;
            xmlChar* id = xmlGetProp(train, BAD_CAST "id");
            if (id != nullptr) {
                trainDIDI = (const char*)id;
                xmlFree(id);
            }
            for (xmlNode* stations = train->children; stations != nullptr; stations = stations->next) {
                if ((train->type == XML_ELEMENT_NODE) &&  xmlStrcmp(stations->name, BAD_CAST "stations") == 0) {
                    for (xmlNode* station = stations->children; station != nullptr; station = station->next) {
                        if ((station->type == XML_ELEMENT_NODE) &&  xmlStrcmp(station->name, BAD_CAST "station") == 0 ) {
                            string StationIamAt;
                            string StationTime;
                            string StationEarly;
                            for (xmlNode* info = station->children; info != nullptr; info = info->next) {
                                if ((info->type == XML_ELEMENT_NODE) &&  xmlStrcmp(info->name, BAD_CAST "name") == 0) {
                                    StationIamAt = string((const char*)xmlNodeGetContent(info));
                                    cout << "[Class - DEBUG] StationIamAt: " << StationIamAt << "\n";
                                }
                                if ((info->type == XML_ELEMENT_NODE) &&  xmlStrcmp(info->name, BAD_CAST "departure") == 0) {
                                   StationTime = string((const char*)xmlNodeGetContent(info));
                                    cout << "[Class - DEBUG] StationTime: " << StationTime << "\n";
                                }
                                if ((info->type == XML_ELEMENT_NODE) &&  xmlStrcmp(info->name, BAD_CAST "early") == 0) {
                                    StationEarly = string((const char*)xmlNodeGetContent(info));
                                    cout << "[Class - DEBUG] StationEarly: " << StationEarly << "\n";
                                }
                            }
                            if (!StationIamAt.empty() && StationIamAt == StationName ) {
                                if (!StationTime.empty()) {
                                    int arvHour = stoi(StationTime.substr(0, 2));
                                    cout << "[Class - DEBUG] Arrival hour of train: " << arvHour << "\n";
                                    int arvMinute = stoi(StationTime.substr(3, 2));
                                    cout << "[Class - DEBUG] Arrival minutes of train: " << arvMinute << "\n";
                                    int totalNrMinutes = arvHour * 60 + arvMinute;
                                    cout << "[Class - DEBUG] Arrival of this train: " << totalNrMinutes << "\n";
                                    int arvEarly = stoi(StationEarly);
                                    cout << "[Class - DEBUG] Early of this train: " << arvEarly << "\n";
                                    int MinAndEarly = totalNrMinutes - arvEarly;
                                    cout << "[Class - DEBUG] Early of this train in mins: " << MinAndEarly << "\n";

                                    if ((MinAndEarly >= SentTotalNrMinutes) && (MinAndEarly <= EarliestHour)) {
                                        ROUTES += "Train ID: " +  trainDIDI + " Station: " + StationName + " Departure: " + StationTime + "\n";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (ROUTES.empty()) {
            ROUTES = "No train found on this route.\n";
        }
    }
    return ROUTES;
}

void XMLWorkAround::AddDelay(string trainID, int stationNum, int delay) {
    if (doc == nullptr) {
        cerr << "[Class - ERROR] XML document not loaded!" << endl;
        return;
    }

    xmlNode* root = xmlDocGetRootElement(doc);
    if ((root == nullptr) || xmlStrcmp(root->name, BAD_CAST "trains") != 0 ) {
        cerr << "[Class - ERROR] Invalid root or <trains> not found!" << endl;
        return;
    }

    cout << "[Class - DEBUG] Adding delay to train ID: " << trainID << ", station num: " << stationNum << ", delay: " << delay << endl;
    for (xmlNode* train = root->children; train != nullptr; train = train->next) {
        if (train->type == XML_ELEMENT_NODE && xmlStrcmp(train->name, BAD_CAST "train") == 0) {
            string ID = getID(train);
            if (ID == trainID) {
                for (xmlNode* stations = train->children; stations != nullptr; stations = stations->next) {
                    if (stations->type == XML_ELEMENT_NODE && xmlStrcmp(stations->name, BAD_CAST "stations") == 0) {
                        for (xmlNode* station = stations->children; station != nullptr; station = station->next) {
                            if (station->type == XML_ELEMENT_NODE && xmlStrcmp(station->name, BAD_CAST "station") == 0) {
                                int currentStation = getStationNum(station);
                                if (currentStation >= stationNum) {
                                    for (xmlNode* detail = station->children; detail != nullptr; detail = detail->next) {
                                        if (detail->type == XML_ELEMENT_NODE && xmlStrcmp(detail->name, BAD_CAST "delay") == 0) {
                                            int currentDelay = atoi((const char*)xmlNodeGetContent(detail));
                                            xmlNodeSetContent(detail, BAD_CAST to_string(currentDelay + delay).c_str());
                                            cout << "[Class - DEBUG] Added delay to train ID: " << ID << ", station num: " << currentStation << ", delay: " << delay << endl;
                                        }
                                    }
                                } else {
                                    cout << "[Class - DEBUG] Error: Number does not match!" << endl;
                                }
                            }
                        }
                    }
                }
            } else {
                cout << "[Class - DEBUG] Error: ID does not match!" << endl;
            }
        }
    }
    SaveXML();
}

void XMLWorkAround::AddEarly(string trainID, int stationNum, int early) {
     if (doc == nullptr) {
        cerr << "[Class - ERROR] XML document not loaded!" << endl;
        return;
    }

    xmlNode* root = xmlDocGetRootElement(doc);
    if ((root == nullptr) || xmlStrcmp(root->name, BAD_CAST "trains") != 0 ) {
        cerr << "[Class - ERROR] Invalid root or <trains> not found!" << endl;
        return;
    }

    cout << "[Class - DEBUG] Adding early to train ID: " << trainID << ", station num: " << stationNum << ", early: " << early << endl;
    for (xmlNode* train = root->children; train != nullptr; train = train->next) {
        if (train->type == XML_ELEMENT_NODE && xmlStrcmp(train->name, BAD_CAST "train") == 0) {
            string ID = getID(train);
            if (ID == trainID) {
                for (xmlNode* stations = train->children; stations != nullptr; stations = stations->next) {
                    if (stations->type == XML_ELEMENT_NODE && xmlStrcmp(stations->name, BAD_CAST "stations") == 0) {
                        for (xmlNode* station = stations->children; station != nullptr; station = station->next) {
                            if (station->type == XML_ELEMENT_NODE && xmlStrcmp(station->name, BAD_CAST "station") == 0) {
                                int currentStation = getStationNum(station);
                                if (currentStation >= stationNum) {
                                    for (xmlNode* detail = station->children; detail != nullptr; detail = detail->next) {
                                        if (detail->type == XML_ELEMENT_NODE && xmlStrcmp(detail->name, BAD_CAST "early") == 0) {
                                            int currentEarly = atoi((const char*)xmlNodeGetContent(detail));
                                            xmlNodeSetContent(detail, BAD_CAST to_string(currentEarly + early).c_str());
                                            cout << "[Class - DEBUG] Added delay to train ID: " << ID << ", station num: " << currentStation << ", delay: " << early << endl;
                                        }
                                    }
                                } else {
                                    cout << "[Class - DEBUG] Error: Number does not match!" << endl;
                                }
                            }
                        }
                    }
                }
            } else {
                cout << "[Class - DEBUG] Error: ID does not match!" << endl;
            }
        }
    }
    SaveXML();
}