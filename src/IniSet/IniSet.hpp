#ifndef INISET_HPP
#define INISET_HPP

#include <string>
#include <vector>
#include <map>


class IniSet {
	public:
		explicit IniSet();
		~IniSet();

        /**
         * Loads a configuration from file, and returns either it has been successfully loaded or not.
         * If it couldn't, then the configuration's object is empty.
         * @param filepath The file to read its configuration
         * @return Either it has successfully read the file and loaded the configuration
         */
        bool loadFromFile(const std::string& filepath);
        
        /**
         * Returns the configuration in a string format. Written like a .ini file.
         * @return The configuration in a string format
         */
        std::string toString() const;

        /**
         * Returns the configuration in a json format, as a string.
         * @return The configuration in a json string format
         */
        std::string toJSONString() const;
        
        friend std::ostream& operator<<(std::ostream& os, const IniSet& ini);


        std::vector<std::string> getSections() const;
        std::map<std::string, std::pair<std::string, std::string>> getEntries() const;
        std::map<std::string, std::pair<std::string, std::string>> getEntries(const std::string& section) const;

        std::vector<std::string> getKeys() const;
        std::vector<std::string> getKeys(const std::string& section) const;

        std::vector<std::string> getValues() const;
        std::vector<std::string> getValues(const std::string& section) const;

        std::string getValue(const std::string& section, const std::string& key) const;
        std::pair<std::string, std::string> getPairValue(const std::string& section, const std::string& key) const;
        int getIntValue(const std::string& section, const std::string& key, int defaultValue) const;
        float getFloatValue(const std::string& section, const std::string& key, float defaultValue) const;
        bool getBoolValue(const std::string& section, const std::string& key, bool defaultValue) const;
        std::string getType(const std::string &section, const std::string &key) const;

        std::string getValue(const std::string& key) const;
        std::pair<std::string, std::string> getPairValue(const std::string& key) const;
        int getIntValue(const std::string& key, int defaultValue) const;
        float getFloatValue(const std::string& key, float defaultValue) const;
        bool getBoolValue(const std::string& key, bool defaultValue) const;
        std::string getType(const std::string &key) const;

        bool hasSection(const std::string& section) const;
        bool hasKey(const std::string& key) const;
        bool hasKey(const std::string& section, const std::string& key) const;

    private:
        static std::string pairSeparator;

        unsigned int stringIsValidPair(const std::string& str) const;
        void assignFromRawString(const std::string& section, const std::string& str);

        // { section: { key: { type, value } } }
        std::map<std::string, std::map<std::string, std::pair<std::string, std::string>>> m_iniMap;
        // { key: { type, value } }
        std::map<std::string, std::pair<std::string, std::string>> m_rootMap;
        unsigned int m_sectionCount;
};

#endif // INISET_HPP