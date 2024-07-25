#include "Utilities.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>

std::string getBaseName(const std::string& path) {
    std::string filename = path.substr(path.find_last_of("/\\") + 1);
    size_t lastDotPosition = filename.find_last_of(".");
    if (lastDotPosition != std::string::npos) {
        return filename.substr(0, lastDotPosition);
    }
    return filename;
}

std::string wrapText(const std::string& text, unsigned int lineLength) {
    std::string result;
    std::string line;
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        if (line.length() + word.length() > lineLength + 20) {
            result += line + "\n";
            line = word + " ";
        } else {
            line += word + " ";
        }
    }
    result += line;
    return result;
}

void filterMusicFiles(const std::vector<std::string>& musicFiles, const std::string& query, std::vector<std::string>& filtered) {
    filtered.clear();
    std::string lowercaseQuery = query;
    std::transform(lowercaseQuery.begin(), lowercaseQuery.end(), lowercaseQuery.begin(),
        [](unsigned char c){ return std::tolower(c); });

    for (const auto& file : musicFiles) {
        std::string lowercaseFile = getBaseName(file);
        std::transform(lowercaseFile.begin(), lowercaseFile.end(), lowercaseFile.begin(),
            [](unsigned char c){ return std::tolower(c); });

        if (lowercaseFile.find(lowercaseQuery) != std::string::npos) {
            filtered.push_back(file);
        }
    }
}