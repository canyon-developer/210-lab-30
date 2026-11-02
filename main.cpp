// Progress save 4: strict CSV parsing and duplicate-day checks
#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct DailyRecord {
    int day;
    int caloriesEaten;
    int caloriesBurned;
    int calorieBalance;
    double endingWeight;
};

const int SIMULATION_DAYS = 180;

string trim(const string& value) {
    size_t first = 0;
    while (first < value.size() &&
           isspace(static_cast<unsigned char>(value[first]))) {
        ++first;
    }
    size_t last = value.size();
    while (last > first &&
           isspace(static_cast<unsigned char>(value[last - 1]))) {
        --last;
    }
    return value.substr(first, last - first);
}

bool parseInteger(const string& text, int& value) {
    string cleaned = trim(text);
    if (cleaned.empty()) return false;

    try {
        size_t used = 0;
        long long parsed = stoll(cleaned, &used);
        if (used != cleaned.size() ||
            parsed < numeric_limits<int>::min() ||
            parsed > numeric_limits<int>::max()) {
            return false;
        }
        value = static_cast<int>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

bool parseDailyRecord(const string& line, DailyRecord& record) {
    if (count(line.begin(), line.end(), ',') != 2) {
        return false;
    }

    stringstream input(line);
    string dayText;
    string eatenText;
    string burnedText;
    getline(input, dayText, ',');
    getline(input, eatenText, ',');
    getline(input, burnedText);

    if (!parseInteger(dayText, record.day) ||
        !parseInteger(eatenText, record.caloriesEaten) ||
        !parseInteger(burnedText, record.caloriesBurned)) {
        return false;
    }

    record.calorieBalance = 0;
    record.endingWeight = 0.0;
    return true;
}

bool loadDailyData(const string& filename, vector<DailyRecord>& records) {
    ifstream input(filename);
    if (!input) {
        cerr << "Could not open input file.\n";
        return false;
    }

    array<bool, SIMULATION_DAYS + 1> seen{};
    string line;
    int lineNumber = 0;

    while (getline(input, line)) {
        ++lineNumber;
        DailyRecord record{};
        if (!parseDailyRecord(line, record)) {
            cerr << "Malformed line " << lineNumber << ".\n";
            return false;
        }
        if (record.day < 1 || record.day > SIMULATION_DAYS ||
            seen[record.day]) {
            cerr << "Invalid or duplicate day on line "
                 << lineNumber << ".\n";
            return false;
        }
        seen[record.day] = true;
        records.push_back(record);
    }
    return true;
}

int main(int argc, char* argv[]) {
    string filename = argc > 1 ? argv[1] : "daily_data.csv";
    vector<DailyRecord> records;

    if (!loadDailyData(filename, records)) {
        return 1;
    }

    cout << "Loaded " << records.size() << " unique records.\n";
    return 0;
}
