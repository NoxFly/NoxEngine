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

        /**
         * Loads a configuration from file, and returns either it has been successfully loaded or not.
         * If it couldn't, then the configuration's object is empty.
         * @param filepath The file to read its configuration
         * @return Either it has successfully read the file and loaded the configuration
         */
        bool loadFromFile(const string& filepath);
        
        /**
         * Returns the configuration in a string format. Written like a .ini file.
         * @return The configuration in a string format
         */
        string toString() const;

        /**
         * Returns the configuration in a json format, as a string.
         * @return The configuration in a json string format
         */
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
        map<string, map<string, pair<string, string>>> m_iniMap;
        // { key: { type, value } }
        map<string, pair<string, string>> m_rootMap;
        int m_sectionCount;
};

#endif // INISET_H