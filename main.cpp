// Progress save 2: correct 30-day phase calculation
#include <iostream>
#include <string>

using namespace std;

const int SIMULATION_DAYS = 180;
const int DAYS_PER_PHASE = 30;

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

int main() {
    cout << "Day 30: " << getPhaseName(30) << "\n";
    cout << "Day 31: " << getPhaseName(31) << "\n";
    cout << "Day 60: " << getPhaseName(60) << "\n";
    cout << "Day 61: " << getPhaseName(61) << "\n";
    return 0;
}
