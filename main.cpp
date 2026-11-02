// Progress save 3: phase calculation and safe map insertion
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <string>

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

int classifyBalance(int balance) {
    if (balance < -100) return DEFICIT;
    if (balance <= 100) return BALANCED;
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

bool addRecord(WeightMap& history, DailyRecord record) {
    string phase = getPhaseName(record.day);
    if (phase == "Invalid day") {
        return false;
    }

    record.calorieBalance =
        record.caloriesEaten - record.caloriesBurned;
    record.endingWeight += calculateWeightChange(record.calorieBalance);
    history[phase][classifyBalance(record.calorieBalance)].push_back(record);
    return true;
}

int main() {
    WeightMap history;
    addRecord(history, {30, 2200, 2200, 0, 200.0});
    addRecord(history, {31, 2500, 2200, 0, 200.0});

    if (!addRecord(history, {181, 2200, 2200, 0, 200.0})) {
        cout << "Rejected invalid day 181.\n";
    }

    for (const auto& phase : history) {
        cout << phase.first << "\n";
    }
    return 0;
}
