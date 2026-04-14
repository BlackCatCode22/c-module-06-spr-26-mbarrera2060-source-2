#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>
using namespace std;
// Base Class
class Animal {
protected:
    string id, name, species, sex, color, origin, birthday, arrivalDate;
    int weight;
public:
    Animal(string sp, string sx, string col, int wt, string org, string arr)
        : species(sp), sex(sx), color(col), weight(wt), origin(org), arrivalDate(arr) {}
    void setID(string newID) { id = newID; }
    void setName(string newName) { name = newName; }
    void setBirthday(string bday) { birthday = bday; }
    virtual string getHabitat() = 0;
    virtual void print(ofstream &out) {
        out << id << ": " << name
            << "; birth date " << birthday
            << "; " << color << " color"
            << "; " << sex
            << "; " << weight << " pounds"
            << "; from " << origin
            << "; arrived " << arrivalDate << endl;
    }
};
// Subclasses
class Hyena : public Animal {
public:
    Hyena(string sx, string col, int wt, string org, string arr)
        : Animal("hyena", sx, col, wt, org, arr) {}
    string getHabitat() { return "Hyena Habitat"; }
};
class Lion : public Animal {
public:
    Lion(string sx, string col, int wt, string org, string arr)
        : Animal("lion", sx, col, wt, org, arr) {}
    string getHabitat() { return "Lion Habitat"; }
};
class Tiger : public Animal {
public:
    Tiger(string sx, string col, int wt, string org, string arr)
        : Animal("tiger", sx, col, wt, org, arr) {}
    string getHabitat() { return "Tiger Habitat"; }
};
class Bear : public Animal {
public:
    Bear(string sx, string col, int wt, string org, string arr)
        : Animal("bear", sx, col, wt, org, arr) {}
    string getHabitat() { return "Bear Habitat"; }
};
// Helpers
string trim(string str) {
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}
string genUniqueID(string species, map<string, int> &counter) {
    string prefix = species.substr(0, 2);
    prefix[0] = toupper(prefix[0]);
    prefix[1] = tolower(prefix[1]);
    counter[species]++;
    int num = counter[species];
    return prefix + (num < 10 ? "0" : "") + to_string(num);
}
string genBirthday(int age, string season) {
    time_t t = time(0);
    tm *now = localtime(&t);
    int year = now->tm_year + 1900 - age;
    if (season == "spring") return to_string(year) + "-03-15";
    if (season == "summer") return to_string(year) + "-06-15";
    if (season == "fall") return to_string(year) + "-09-15";
    if (season == "winter") return to_string(year) + "-12-15";
    return to_string(year) + "-01-01"; // fallback
}
string assignName(string species, map<string, vector<string>> &names) {
    if (!names[species].empty()) {
        string name = names[species].back();
        names[species].pop_back();
        return name;
    }
    return "Unknown";
}
// Main
int main() {
    ifstream inFile("arrivingAnimalsOfficial.txt");
    ifstream nameFile("animalNames.txt");
    ofstream outFile("zooPopulation.txt");
    if (!inFile || !nameFile) {
        cout << "Error opening files." << endl;
        return 1;
    }
    map<string, vector<string>> nameMap;
    map<string, int> idCounter;
    vector<Animal*> zoo;
    // Load Names
    string line;
    while (getline(nameFile, line)) {
        stringstream ss(line);
        string species, name;
        getline(ss, species, ',');
        getline(ss, name);
        species = trim(species);
        name = trim(name);
        nameMap[species].push_back(name);
    }
    // Process Animals
    while (getline(inFile, line)) {
        int age = 0, weight = 0;
        string sex, species, color, origin, season;
        // Age
        stringstream ss(line);
        ss >> age;
        // Sex & Species
        size_t pos = line.find("year old ");
        string rest = line.substr(pos + 9);
        stringstream ss2(rest);
        ss2 >> sex >> species;
        if (species.back() == ',') species.pop_back();
        // Season
        size_t sPos = line.find("born in ");
        if (sPos != string::npos) {
            season = line.substr(sPos + 8);
            season = season.substr(0, season.find(','));
        }
        // Colour
        size_t cStart = line.find(", ", sPos);
        size_t cEnd = line.find(" color");
        if (cStart != string::npos && cEnd != string::npos) {
            color = line.substr(cStart + 2, cEnd - (cStart + 2));
        }
        // Weight
        size_t wPos = line.find("color, ");
        if (wPos != string::npos) {
            string temp = line.substr(wPos + 7);
            stringstream wss(temp);
            wss >> weight;
        }
        // Origin
        size_t oPos = line.find("from ");
        if (oPos != string::npos) {
            origin = trim(line.substr(oPos + 5));
        }
        // Create Object
        Animal* animal = nullptr;
        if (species == "hyena") animal = new Hyena(sex, color, weight, origin, "2024-03-05");
        else if (species == "lion") animal = new Lion(sex, color, weight, origin, "2024-03-05");
        else if (species == "tiger") animal = new Tiger(sex, color, weight, origin, "2024-03-05");
        else if (species == "bear") animal = new Bear(sex, color, weight, origin, "2024-03-05");
        if (!animal) continue;
        // Apply Logic
        animal->setID(genUniqueID(species, idCounter));
        animal->setName(assignName(species, nameMap));
        animal->setBirthday(genBirthday(age, season));
        zoo.push_back(animal);
    }
    // Output
    map<string, vector<Animal*>> habitats;
    for (auto a : zoo) {
        habitats[a->getHabitat()].push_back(a);
    }
    for (auto &h : habitats) {
        outFile << h.first << ":" << endl;
        for (auto a : h.second) {
            a->print(outFile);
        }
        outFile << endl;
    }
    // Cleanup
    for (auto a : zoo) delete a;
    cout << "Report generated successfully." << endl;
    return 0;
}