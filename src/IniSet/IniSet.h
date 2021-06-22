#ifndef INISET_H
#define INISET_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class IniSet {
	public:
		IniSet();
		~IniSet();

        bool loadFromFile(const string& filepath);
        string toString() const;
        string toJSONString() const;
        friend ostream& operator<<(ostream& os, const IniSet& ini);

        vector<string> getSections() const;

        map<string, pair<string, string>> getEntries() const;
        map<string, pair<string, string>> getEntries(const string& section) const;

        vector<string> getKeys() const;
        vector<string> getKeys(const string& section) const;

        vector<string> getValues() const;
        vector<string> getValues(const string& section) const;

        string getValue(const string& section, const string& key);
        pair<string, string> getPairValue(const string& section, const string& key);
        int getIntValue(const string& section, const string& key, int defaultValue);
        float getFloatValue(const string& section, const string& key, float defaultValue);
        bool getBoolValue(const string& section, const string& key, bool defaultValue);
        string getType(const string &section, const string &key);

        string getValue(const string& key);
        pair<string, string> getPairValue(const string& key);
        int getIntValue(const string& key, int defaultValue);
        float getFloatValue(const string& key, float defaultValue);
        bool getBoolValue(const string& key, bool defaultValue);
        string getType(const string &key);

        bool hasSection(const string& section);
        bool hasKey(const string& key);
        bool hasKey(const string& section, const string& key);

    private:
        static string pairSeparator;

        int stringIsValidPair(const string& str) const;
        void assignFromRawString(const string& section, const string& str);

        // { section: { key: { type, value } } }
        map<string, map<string, pair<string, string>>> iniMap;
        // { key: { type, value } }
        map<string, pair<string, string>> rootMap;
        int sectionCount;
};

#endif // INISET_H