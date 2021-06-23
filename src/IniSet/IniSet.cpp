#include "IniSet.h"

#include <fstream>
#include <iostream>

#include "utils.h"

string IniSet::pairSeparator = "=";

IniSet::IniSet():
    m_iniMap{},
    m_rootMap{},
    m_sectionCount(0)
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
    m_iniMap = {};
    m_rootMap = {};

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
                m_iniMap[sectionName] = {};
                sectionDetails.push_back(pair(i, sectionName));
            }
        }
    }

    m_sectionCount = sectionDetails.size();

    int n = (m_sectionCount == 0)? m : sectionDetails[0].first;
    int i, j, k;

    i = -1;

    while(++i < n)
        assignFromRawString("", content[i]);

    // get all keys
    for(i=0; i < m_sectionCount; i++) {
        pair<int, string> section = sectionDetails[i];
        j = section.first;
        k = (i < m_sectionCount-1)? sectionDetails[i+1].first : m;
        
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
            m_rootMap[key] = pair(valueType, value);
        else
            m_iniMap.at(section)[key] = pair(valueType, value);
    }
}


vector<string> IniSet::getSections() const {
    vector<string> sections = {};

    if(m_sectionCount > 0) {
        for(auto section : m_iniMap) {
            sections.push_back(section.first);
        }
    }

    return sections;
}

map<string, pair<string, string>> IniSet::getEntries() const {
    return m_rootMap;
}

map<string, pair<string, string>> IniSet::getEntries(const string& section) const {
    if(m_iniMap.count(section) == 0)
        return {};
    return m_iniMap.at(section);
}

vector<string> IniSet::getKeys() const {
    vector<string> keys;
    for(pair<string, pair<string, string>> p : m_rootMap)
        keys.push_back(p.first);
    return keys;
}

vector<string> IniSet::getKeys(const string& section) const {
    if(m_iniMap.count(section) == 0)
        return {};
    
    vector<string> keys;
    for(pair<string, pair<string, string>> p : m_iniMap.at(section))
        keys.push_back(p.first);
    return keys;
}

vector<string> IniSet::getValues() const {
    vector<string> values;
    for(pair<string, pair<string, string>> p : m_rootMap)
        values.push_back(p.second.second);
    return values;
}

vector<string> IniSet::getValues(const string& section) const {
    if(m_iniMap.count(section) == 0)
        return {};

    vector<string> values;
    for(pair<string, pair<string, string>> p : m_iniMap.at(section))
        values.push_back(p.second.second);
    return values;
}

string IniSet::getValue(const string& section, const string& key) {
    return getPairValue(section, key).second;
}

pair<string, string> IniSet::getPairValue(const string& section, const string& key) {
    return hasKey(section, key)? m_iniMap[section][key] : pair("", "");
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
    return hasKey(section, key)? m_iniMap[section][key].first : "string";
}

pair<string, string> IniSet::getPairValue(const string& key) {
    return hasKey(key)? m_rootMap[key] : pair("", "");
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
    return m_iniMap.count(section) > 0;
}

bool IniSet::hasKey(const string& key) {
    return m_rootMap.count(key) > 0;
}

bool IniSet::hasKey(const string& section, const string& key) {
    return hasSection(section) && m_iniMap[section].count(key) > 0;
}

string IniSet::getType(const string &key) {
    return hasKey(key)? m_rootMap[key].first : "string";
}


string IniSet::toString() const {
    string str = "";

    for(auto &p : m_rootMap)
        str += p.first + "=" + p.second.second + "\n";

    for(auto &p : m_iniMap) {
        str += "\n[" + p.first + "]\n";

        int j = 0;
        int size = p.second.size();
        
        for(auto &sp : p.second) {
            str += "  " + sp.first + "=" + sp.second.second;

            if(++j < size)
                str += "\n";
        }

        str += "\n";
    }

    return str;
}

string IniSet::toJSONString() const {
    string str = "{";
    int i = 0;

    if(m_rootMap.size() > 0 || m_iniMap.size() > 0)
        str += "\n";

    for(auto &p : m_rootMap) {
        string g = p.second.first == "string" ? "\"" : "";
        str += "  \"" + p.first + "\": " + g + p.second.second + g + ((++i < (int)m_rootMap.size())? "," : "") + "\n";
    }

    i = 0;
    
    for(auto &p : m_iniMap) {
        str += "  \"" + p.first + "\": {\n";

        int j = 0;
        
        for(auto &sp : p.second) {
            string g = sp.second.first == "string" ? "\"" : "";
            str += "    \"" + sp.first + "\": " + g + sp.second.second + g;

            if(++j < (int)p.second.size())
                str += ",";
            
            str += "\n";
        }

        str += "  }";

        if(++i < (int)m_iniMap.size())
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