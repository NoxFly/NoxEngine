#include "IniSet.h"

#include <fstream>
#include <iostream>

#include "utils.h"

string IniSet::pairSeparator = "=";

IniSet::IniSet():
    iniMap{},
    rootMap{},
    sectionCount(0)
{

}

IniSet::~IniSet() {

}

bool IniSet::loadFromFile(const string& filepath) {
    ifstream file;
    std::vector<string> content = {};

    if(!endsWith(filepath, ".ini")) {
        cout << "File must have ini extension" << endl;
        return false;
    }

    file.open(filepath);

    if(file.is_open()) {
        string line;

        while(getline(file, line)) {
            if(line.length() > 0) {
                content.push_back(line);
            }
        }

        file.close();
    }

    else {
        return false;
    }

    // reinitialize maps
    iniMap = {};
    rootMap = {};

    int m = content.size();

    if(m == 0)
        return true;

    vector<pair<int, string>> sectionDetails = {};

    // detect sections
    for(int i=0; i < m; i++) {
        string line = content[i];

        if(startsWith(line, "[") && endsWith(line, "]")) {
            string sectionName = replace(line.substr(1, line.length() - 2), " ", "_");

            if(sectionName.size() > 0) {
                iniMap[sectionName] = {};
                sectionDetails.push_back(pair(i, sectionName));
            }
        }
    }

    sectionCount = sectionDetails.size();

    int n = (sectionCount == 0)? m : sectionDetails[0].first;
    int i, j, k;

    i = -1;

    while(++i < n)
        assignFromRawString("", content[i]);

    // get all keys
    for(i=0; i < sectionCount; i++) {
        pair<int, string> section = sectionDetails[i];
        j = section.first;
        k = (i < sectionCount-1)? sectionDetails[i+1].first : m;
        
        while(++j < k)
            assignFromRawString(section.second, content[j]);
    }

    return true;
}

int IniSet::stringIsValidPair(const string& str) const {
    bool foundEqual = false;
    int varSize = 0;
    int valueSize = 0;

    for(auto &c : str) {
        // value side
        if(foundEqual) {
            if(c == '=')
                return 2;
            valueSize++;
        }
        // variable side
        else {
            if(!isalpha(c) && c != '_' && !isdigit(c)) {
                if(c == '=') {
                    if(varSize == 0)
                        return 1;
                    
                    foundEqual = true;
                }
                else
                    return 1;
            }
            else
                varSize++;
        }
    }

    if(valueSize == 0)
        return 2;

    return 0;
}

void IniSet::assignFromRawString(const string& section, const string& str) {
    if(stringIsValidPair(str) == 0) {
        string key = str.substr(0, str.find(IniSet::pairSeparator));
        string value = str.substr(str.find(IniSet::pairSeparator)+1, str.size());
        string valueType = isNumber(value)? "number" : (value == "true" || value == "false")? "boolean": "string";

        if(section == "")
            rootMap[key] = pair(valueType, value);
        else
            iniMap.at(section)[key] = pair(valueType, value);
    }
}


vector<string> IniSet::getSections() const {
    vector<string> sections = {};

    if(sectionCount > 0) {
        for(auto section : iniMap) {
            sections.push_back(section.first);
        }
    }

    return sections;
}

map<string, pair<string, string>> IniSet::getEntries() const {
    return rootMap;
}

map<string, pair<string, string>> IniSet::getEntries(const string& section) const {
    if(iniMap.count(section) == 0)
        return {};
    return iniMap.at(section);
}

vector<string> IniSet::getKeys() const {
    vector<string> keys;
    for(pair<string, pair<string, string>> p : rootMap)
        keys.push_back(p.first);
    return keys;
}

vector<string> IniSet::getKeys(const string& section) const {
    if(iniMap.count(section) == 0)
        return {};
    
    vector<string> keys;
    for(pair<string, pair<string, string>> p : iniMap.at(section))
        keys.push_back(p.first);
    return keys;
}

vector<string> IniSet::getValues() const {
    vector<string> values;
    for(pair<string, pair<string, string>> p : rootMap)
        values.push_back(p.second.second);
    return values;
}

vector<string> IniSet::getValues(const string& section) const {
    if(iniMap.count(section) == 0)
        return {};

    vector<string> values;
    for(pair<string, pair<string, string>> p : iniMap.at(section))
        values.push_back(p.second.second);
    return values;
}

string IniSet::getValue(const string& section, const string& key) {
    return getPairValue(section, key).second;
}

pair<string, string> IniSet::getPairValue(const string& section, const string& key) {
    return hasKey(section, key)? iniMap[section][key] : pair("", "");
}

int IniSet::getIntValue(const string& section, const string& key, int defaultValue) {
    pair<string, string> value = getPairValue(section, key);
    return (value.second == "" || value.first != "number")? defaultValue : stoi(value.second);
}

float IniSet::getFloatValue(const string& section, const string& key, float defaultValue) {
    pair<string, string> value = getPairValue(section, key);
    return (value.second == "" || value.first != "number")? defaultValue : stof(value.second);
}

bool IniSet::getBoolValue(const string& section, const string& key, bool defaultValue) {
    pair<string, string> value = getPairValue(section, key);
    return (value.second == "" || value.first != "boolean")? defaultValue : value.second == "true";
}

string IniSet::getValue(const string& key) {
    return getPairValue(key).second;
}

string IniSet::getType(const string &section, const string &key) {
    return hasKey(section, key)? iniMap[section][key].first : "string";
}

pair<string, string> IniSet::getPairValue(const string& key) {
    return hasKey(key)? rootMap[key] : pair("", "");
}

int IniSet::getIntValue(const string& key, int defaultValue) {
    pair<string, string> value = getPairValue(key);
    return (value.second == "" || value.first != "number")? defaultValue : stoi(value.second);
}

float IniSet::getFloatValue(const string& key, float defaultValue) {
    pair<string, string> value = getPairValue(key);
    return (value.second == "" || value.first != "number")? defaultValue : stof(value.second);
}

bool IniSet::getBoolValue(const string& key, bool defaultValue) {
    pair<string, string> value = getPairValue(key);
    return (value.second == "" || value.first != "boolean")? defaultValue : value.second == "true";
}

bool IniSet::hasSection(const string& section) {
    return iniMap.count(section) > 0;
}

bool IniSet::hasKey(const string& key) {
    return rootMap.count(key) > 0;
}

bool IniSet::hasKey(const string& section, const string& key) {
    return hasSection(section) && iniMap[section].count(key) > 0;
}

string IniSet::getType(const string &key) {
    return hasKey(key)? rootMap[key].first : "string";
}


string IniSet::toString() const {
    string str = "";
    int i = 0;

    for(auto &p : rootMap)
        str += p.first + "=" + p.second.second + "\n";

    i = 0;

    for(auto &p : iniMap) {
        str += "\n[" + p.first + "]\n";

        int j = 0;
        
        for(auto &sp : p.second) {
            str += "  " + sp.first + "=" + sp.second.second;

            if(++j < p.second.size())
                str += "\n";
        }

        str += "\n";
    }

    return str;
}

string IniSet::toJSONString() const {
    string str = "{";
    int i = 0;

    if(rootMap.size() > 0 || iniMap.size() > 0)
        str += "\n";

    for(auto &p : rootMap) {
        string g = p.second.first == "string" ? "\"" : "";
        str += "  \"" + p.first + "\": " + g + p.second.second + g + ((++i < rootMap.size())? "," : "") + "\n";
    }

    i = 0;
    
    for(auto &p : iniMap) {
        str += "  \"" + p.first + "\": {\n";

        int j = 0;
        
        for(auto &sp : p.second) {
            string g = sp.second.first == "string" ? "\"" : "";
            str += "    \"" + sp.first + "\": " + g + sp.second.second + g;

            if(++j < p.second.size())
                str += ",";
            
            str += "\n";
        }

        str += "  }";

        if(++i < iniMap.size())
            str += ",";
        
        str += "\n";
    }

    str += "}";

    return str;
}

ostream& operator<<(ostream& os, const IniSet& ini) {
    os << ini.toString();
    return os;
}