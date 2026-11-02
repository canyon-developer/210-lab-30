// Progress save 7: complete beta release
// Project: 180-Day Weight Change Simulation
// Beta release

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
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

const int DEFICIT = 0;
const int BALANCED = 1;
const int SURPLUS = 2;
const int CATEGORY_COUNT = 3;
const int SIMULATION_DAYS = 180;
const int DAYS_PER_PHASE = 30;
const double CALORIES_PER_POUND = 3500.0;

using DayLists = array<list<DailyRecord>, CATEGORY_COUNT>;
using WeightMap = map<string, DayLists>;

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
    if (cleaned.empty()) {
        return false;
    }

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

    if (!getline(input, dayText, ',') ||
        !getline(input, eatenText, ',') ||
        !getline(input, burnedText)) {
        return false;
    }

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
        cerr << "Error: could not open " << filename << ".\n";
        return false;
    }

    vector<DailyRecord> loaded;
    array<bool, SIMULATION_DAYS + 1> seen{};
    string line;
    int lineNumber = 0;

    while (getline(input, line)) {
        ++lineNumber;
        DailyRecord record{};

        if (!parseDailyRecord(line, record)) {
            cerr << "Error: malformed data on line " << lineNumber << ".\n";
            return false;
        }
        if (record.day < 1 || record.day > SIMULATION_DAYS) {
            cerr << "Error: day " << record.day
                 << " is outside the range 1-" << SIMULATION_DAYS << ".\n";
            return false;
        }
        if (record.caloriesEaten < 0 || record.caloriesBurned < 0) {
            cerr << "Error: calorie values cannot be negative on line "
                 << lineNumber << ".\n";
            return false;
        }
        if (seen[record.day]) {
            cerr << "Error: duplicate record for day " << record.day << ".\n";
            return false;
        }

        seen[record.day] = true;
        loaded.push_back(record);
    }

    for (int day = 1; day <= SIMULATION_DAYS; ++day) {
        if (!seen[day]) {
            cerr << "Error: input is missing day " << day << ".\n";
            return false;
        }
    }

    sort(loaded.begin(), loaded.end(),
         [](const DailyRecord& left, const DailyRecord& right) {
             return left.day < right.day;
         });

    records = loaded;
    return true;
}

int classifyBalance(int balance) {
    if (balance < -100) {
        return DEFICIT;
    }
    if (balance <= 100) {
        return BALANCED;
    }
    return SURPLUS;
}

double calculateWeightChange(int calorieBalance) {
    return calorieBalance / CALORIES_PER_POUND;
}

string getPhaseName(int day) {
    if (day < 1 || day > SIMULATION_DAYS) {
        return "Invalid day";
    }

    int phase = (day - 1) / DAYS_PER_PHASE + 1;
    int firstDay = (phase - 1) * DAYS_PER_PHASE + 1;
    int lastDay = phase * DAYS_PER_PHASE;

    return "Phase " + to_string(phase) + ": Days " +
           to_string(firstDay) + "-" + to_string(lastDay);
}

double simulateWeightChanges(const vector<DailyRecord>& inputRecords,
                             double startingWeight,
                             WeightMap& history,
                             array<double, 6>& phaseWeights) {
    history.clear();
    phaseWeights.fill(startingWeight);
    double runningWeight = startingWeight;

    for (DailyRecord record : inputRecords) {
        record.calorieBalance =
            record.caloriesEaten - record.caloriesBurned;
        runningWeight += calculateWeightChange(record.calorieBalance);
        record.endingWeight = runningWeight;

        string phase = getPhaseName(record.day);
        int category = classifyBalance(record.calorieBalance);
        history[phase][category].push_back(record);

        if (record.day % DAYS_PER_PHASE == 0) {
            phaseWeights[record.day / DAYS_PER_PHASE - 1] = runningWeight;
        }
    }

    return runningWeight;
}

void displayHistory(const WeightMap& history,
                    double startingWeight,
                    const array<double, 6>& phaseWeights,
                    double finalWeight) {
    const array<string, CATEGORY_COUNT> labels = {
        "Deficit", "Balanced", "Surplus"
    };
    array<int, CATEGORY_COUNT> totals{};

    cout << fixed << setprecision(2);
    cout << "Initial weight: " << startingWeight << " pounds\n";

    for (const auto& phase : history) {
        cout << "\n" << phase.first << "\n";
        for (int category = 0; category < CATEGORY_COUNT; ++category) {
            cout << "  " << labels[category] << " days:\n";

            if (phase.second[category].empty()) {
                cout << "    none\n";
                continue;
            }

            for (const DailyRecord& record : phase.second[category]) {
                cout << "    Day " << setw(3) << record.day
                     << ": eaten " << record.caloriesEaten
                     << ", burned " << record.caloriesBurned
                     << ", balance " << showpos << record.calorieBalance
                     << noshowpos << ", weight " << record.endingWeight
                     << "\n";
                ++totals[category];
            }
        }
    }

    cout << "\n30-day progress:\n";
    for (size_t phase = 0; phase < phaseWeights.size(); ++phase) {
        cout << "  Day " << (phase + 1) * DAYS_PER_PHASE
             << ": " << phaseWeights[phase] << " pounds\n";
    }

    cout << "\nFinal estimated weight: " << finalWeight << " pounds\n";
    cout << "Category totals:\n";
    cout << "  Deficit days: " << totals[DEFICIT] << "\n";
    cout << "  Balanced days: " << totals[BALANCED] << "\n";
    cout << "  Surplus days: " << totals[SURPLUS] << "\n";
}

int main(int argc, char* argv[]) {
    const string filename = argc > 1 ? argv[1] : "daily_data.csv";
    const double startingWeight = 200.0;

    vector<DailyRecord> records;
    if (!loadDailyData(filename, records)) {
        return 1;
    }

    WeightMap weightHistory;
    array<double, 6> phaseWeights{};
    double finalWeight = simulateWeightChanges(
        records, startingWeight, weightHistory, phaseWeights);

    displayHistory(weightHistory, startingWeight, phaseWeights, finalWeight);
    return 0;
}
