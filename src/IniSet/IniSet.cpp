/**
 * @copyright (c) 2025 NoxFly
 * @date 2025-01-29
 * @license AGPL-3.0
 */

#include "IniSet.hpp"

#include <fstream>
#include <iostream>

#include "utils/string.hpp"

using namespace std;


string IniSet::pairSeparator = "=";

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
        cout << "Failed to open file" << endl;
        return false;
    }

    // reinitialize maps
    m_iniMap = {};
    m_rootMap = {};

    const size_t m = content.size();

    if(m == 0)
        return true;

    vector<pair<size_t, string>> sectionDetails = {};

    // detect sections
    for(size_t i=0; i < m; i++) {
        string line = content[i];

        if(startsWith(line, ";"))
            continue;

        if(startsWith(line, "[") && endsWith(line, "]")) {
            string sectionName = replace(line.substr(1, line.length() - 2), " ", "_");

            if(sectionName.size() > 0) {
                m_iniMap[sectionName] = {};
                sectionDetails.push_back(pair(i, sectionName));
            }
        }
    }

    m_sectionCount = sectionDetails.size();

    size_t n = (m_sectionCount == 0)
        ? m
        : sectionDetails[0].first;

    size_t j, k;
    size_t i;

    // Parse root-level keys (before first section)
    for(i = 0; i < n; i++)
        assignFromRawString("", content[i]);

    // get all keys
    for(i=0; i < m_sectionCount; i++) {
        pair<size_t, string> section = sectionDetails[i];
        j = section.first;
        const size_t idx = i + 1;
        k = (i < m_sectionCount-1)
            ? sectionDetails[idx].first 
            : m;
        
        while(++j < k)
            assignFromRawString(section.second, content[j]);
    }

    return true;
}

unsigned int IniSet::stringIsValidPair(const string& str) const {
    bool foundEqual = false;
    unsigned int varSize = 0;
    unsigned int valueSize = 0;

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
        string key = trim(str.substr(0, str.find(IniSet::pairSeparator)));
        string value = trim(str.substr(str.find(IniSet::pairSeparator)+1, str.size()));
        string valueType = "string";
        
        if(isInteger(value))
            valueType = "integer";
        else if(isFloat(value))
            valueType = "float";
        else if(value == "true" || value == "false")
            valueType = "boolean";

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

string IniSet::getValue(const string& section, const string& key) const {
    return getPairValue(section, key).second;
}

pair<string, string> IniSet::getPairValue(const string& section, const string& key) const {
    return hasKey(section, key)
        ? m_iniMap.at(section).at(key)
        : pair("", "");
}

int IniSet::getIntValue(const string& section, const string& key, int defaultValue) const {
    pair<string, string> value = getPairValue(section, key);
    return (value.second == "" || value.first != "integer")
        ? defaultValue
        : stoi(value.second);
}

float IniSet::getFloatValue(const string& section, const string& key, float defaultValue) const {
    pair<string, string> value = getPairValue(section, key);
    
    return (value.second == "" || (value.first != "integer" && value.first != "float"))
        ? defaultValue
        : stof(value.second);
}

bool IniSet::getBoolValue(const string& section, const string& key, bool defaultValue) const {
    pair<string, string> value = getPairValue(section, key);
    
    return (value.second == "" || value.first != "boolean")
        ? defaultValue
        : value.second == "true";
}

string IniSet::getValue(const string& key) const {
    return getPairValue(key).second;
}

string IniSet::getType(const string &section, const string &key) const {
    return hasKey(section, key)
        ? m_iniMap.at(section).at(key).first
        : "string";
}

pair<string, string> IniSet::getPairValue(const string& key) const {
    return hasKey(key)
        ? m_rootMap.at(key)
        : pair("", "");
}

int IniSet::getIntValue(const string& key, int defaultValue) const {
    pair<string, string> value = getPairValue(key);
    
    return (value.second == "" || value.first != "integer")
        ? defaultValue
        : stoi(value.second);
}

float IniSet::getFloatValue(const string& key, float defaultValue) const {
    pair<string, string> value = getPairValue(key);
    return (value.second == "" || (value.first != "integer" && value.first != "float"))
        ? defaultValue
        : stof(value.second);
}

bool IniSet::getBoolValue(const string& key, bool defaultValue) const {
    pair<string, string> value = getPairValue(key);
    return (value.second == "" || value.first != "boolean")
        ? defaultValue
        : value.second == "true";
}

bool IniSet::hasSection(const string& section) const {
    return m_iniMap.count(section) > 0;
}

bool IniSet::hasKey(const string& key) const {
    return m_rootMap.count(key) > 0;
}

bool IniSet::hasKey(const string& section, const string& key) const {
    return hasSection(section) && m_iniMap.at(section).count(key) > 0;
}

string IniSet::getType(const string &key) const {
    return hasKey(key)
        ? m_rootMap.at(key).first
        : "string";
}


string IniSet::toString() const {
    string str = "";

    for(auto &p : m_rootMap)
        str += p.first + "=" + p.second.second + "\n";

    for(auto &p : m_iniMap) {
        str += "\n[" + p.first + "]\n";

        size_t j = 0;
        size_t size = p.second.size();
        
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
    size_t i = 0;

    if(m_rootMap.size() > 0 || m_iniMap.size() > 0)
        str += "\n";

    for(auto &p : m_rootMap) {
        string g = p.second.first == "string"
            ? "\""
            : "";

        str += "  \"" + p.first + "\": " + g + p.second.second + g + ((++i < m_rootMap.size())? "," : "") + "\n";
    }

    i = 0;
    
    for(auto &p : m_iniMap) {
        str += "  \"" + p.first + "\": {\n";

        size_t j = 0;
        
        for(auto &sp : p.second) {
            string g = sp.second.first == "string"
                ? "\""
                : "";

            str += "    \"" + sp.first + "\": " + g + sp.second.second + g;

            if(++j < p.second.size())
                str += ",";
            
            str += "\n";
        }

        str += "  }";

        if(++i < m_iniMap.size())
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