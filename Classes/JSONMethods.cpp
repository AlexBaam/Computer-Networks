#include "JSONMethods.h"

JSONMethods::JSONMethods(string JSONpath) {
    this->JSONpath = JSONpath;
    cout << "[Class] JSONMethods initialized the JSONpath with: " << JSONpath << endl;
}

JSONMethods::~JSONMethods() {
    cout << "[Class] JSONMethods resources cleaned up!" << endl;
}

nlohmann::json JSONMethods::LoadJSON() {
    //Deschidem fisierul JSON
    ifstream Jason(JSONpath); // Deschidere ptr citire;
    if (!Jason.is_open()) {
        clrMSG = "[Class - ERROR] Failed to open " + JSONpath + " for reading!";
        cmd.CheckOutput(clrMSG, true);
        return nlohmann::json{}; // Returnam un "empty JSON object"
    }

    //Salvam toata informatia intr-un string
    string ContentJSON((istreambuf_iterator<char>(Jason)), (istreambuf_iterator<char>()));

    //Verificam informatia
    nlohmann::json jsonINFO;
    if (ContentJSON.empty()) {
        clrMSG = "[Class - ERROR] Couldn't save the JSON information in the string!";
        cmd.CheckOutput(clrMSG, true);
        return nlohmann::json{};
    }

    //Folosim try & catch
    try {
        jsonINFO = nlohmann::json::parse(ContentJSON, nullptr, false);
        if (jsonINFO.is_discarded()) {
            clrMSG = "[Class - ERROR] Couldn't parse the JSON information!";
            cmd.CheckOutput(clrMSG, true);
            return nlohmann::json{};
        }
    } catch (...){ // In cazul in care avem o eroare cu fisierul JSON, dar nu este legata de citirea acestuia;
        clrMSG = "[Class - ERROR] Unknown error! Anything else than reading JSON!";
        cmd.CheckOutput(clrMSG, true);
        return nlohmann::json{};
    }

    cout << "[Class] Successfully loaded the JSON information!" << endl;
    cout << jsonINFO.dump(0) << endl; // pretty print output
    return jsonINFO;
}

void JSONMethods::SaveJSON(nlohmann::json& userData) {
    try {
        //Acum vom deschide fisierul pentru scriere:
        ofstream Jason(JSONpath);
        if (!Jason.is_open()) {
            clrMSG = "[Class - ERROR] Couldn't open " + JSONpath + " for writing!";
            cmd.CheckOutput(clrMSG, true);
            return;
        }
        Jason << userData.dump(4);
        Jason.close();

        cout << "[Class] Successfully updated the JSON file!" << endl;
    } catch (const exception &e) {
        clrMSG = "[Class - ERROR] Couldn't update JSON " + string(e.what()) + "!";
        cmd.CheckOutput(clrMSG, true);
    }
}

// .dump(4) este o metoda  din libraria nlohmann/json;
// Aceasta metoda converteste obiectele JSON in stringuri ce pot fi citite de oameni;
// Numarul din paranteza este indent level, acesta reprezinta numarul de spatii
// default este -1, acesta va afisa un output de tipul: {"name":"Nicu","pass":"0307"}
/* indent: 0 va afisa:
{
"name":"Nicu",
"pass":"0307"
}
 */
//Maxim este 4