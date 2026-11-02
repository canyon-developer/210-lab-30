// Progress save 1: calculation and category functions
#include <iostream>

using namespace std;

const int DEFICIT = 0;
const int BALANCED = 1;
const int SURPLUS = 2;
const double CALORIES_PER_POUND = 3500.0;

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

int main() {
    cout << "350 calorie change: "
         << calculateWeightChange(350) << "\n";
    cout << "-100 category: "
         << classifyBalance(-100) << "\n";
    cout << "101 category: "
         << classifyBalance(101) << "\n";
    return 0;
}
