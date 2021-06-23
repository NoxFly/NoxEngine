#include "utils.h"

#include <algorithm>

bool endsWith(string_view str, string_view suffix) {
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

bool startsWith(string_view str, string_view prefix) {
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

bool isNumber(const string& str) {
    for(char const &c : str) {
        if(isdigit(c) == 0)
            return false;
    }

    return true;
}

string replace(const string& str, const string& find, const string& replace) {
    string result;
    size_t find_len = find.size();
    size_t pos,from = 0;

    while(string::npos != (pos = str.find(find, from))) {
        result.append(str, from, pos-from);
        result.append(replace);
        from = pos + find_len;
    }

    result.append(str, from, string::npos);
    
    return result;
}

int inInterval(int min, int val, int max) {
    return std::max(min, std::min(val, max));
}

vector<string> split(string sequence, string separator) {
    size_t pos = 0;
    vector<string> splitted = {};

    while((pos = sequence.find(separator)) != string::npos) {
        splitted.push_back(sequence.substr(0, pos));
        sequence.erase(0, pos + separator.length());
    }

    if(sequence.size() > 0)
        splitted.push_back(sequence);

    return splitted;
}

Color getColorFromString(string str) {
    Color color = { 0, 0, 0 };

    if(str != "") {
        std::vector<std::string> vColor = split(str, ",");

        if(vColor.size() == 3 || vColor.size() == 4) {
            int arr[] = { 0, 0, 0, 255 };

            for(int i=0; i < (int)vColor.size(); i++) {
                string c = vColor[i];

                if(isNumber(c))
                    arr[i] = stoi(c);
                else
                    return color;
            }

            color = { arr[0], arr[1], arr[2], arr[3] };
        }
    }

    return color;
}