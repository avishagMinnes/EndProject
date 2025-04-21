#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

struct Job {
    int id;
    int a; // Processing time before deadline
    int b; // Processing time reduction if late
    int d; // Deadline
};

struct Maintenance {
    int start;
    int duration;
};

int calculateCMax(const vector<Job>& jobs, const Maintenance& ma) {
    int current_time = 0;
    int C_max = 0;

    cout << "Evaluating job sequence:\n";
    for (const auto& job : jobs) {
        cout << "Job ID: " << job.id << " starts at " << current_time;

        // Delay if job starts during maintenance
        if (current_time >= ma.start && current_time < ma.start + ma.duration) {
            cout << " (delayed by maintenance)";
            current_time = ma.start + ma.duration;
        }

        // Delay if job starts before maintenance but would end during maintenance
        if (current_time < ma.start && current_time + job.a > ma.start) {
            cout << " (delayed to avoid maintenance)";
            current_time = ma.start + ma.duration;
        }

        // Penalty check: based only on start time (your requirement)
        if (current_time < job.d) {
            current_time += job.a;
        } else {
            current_time += job.a - job.b;
        }

        cout << " ends at " << current_time << "\n";
        C_max = max(C_max, current_time);
    }
    cout << "Final C_max: " << C_max << "\n";
    return C_max;
}

// Brute-force function to find the best job order
vector<Job> bruteForceScheduling(vector<Job>& jobs, const Maintenance& ma, int& bestCMax) {
    vector<Job> best_order;
    bestCMax = INT_MAX;

    do {
        int C_max = calculateCMax(jobs, ma);
        if (C_max < bestCMax) {
            bestCMax = C_max;
            best_order = jobs;
        }
    } while (next_permutation(jobs.begin(), jobs.end(), [](const Job& j1, const Job& j2) { return j1.id < j2.id; }));

    return best_order;
}

int main() {
    vector<Job> jobs = {
            {1, 2, 1, 5},
            {2, 1, 1, 4},
            {3, 3, 2, 6},
            {4, 2, 1, 5}
    };
    Maintenance ma = {3, 4};

    int bestCMax;
    vector<Job> best_order = bruteForceScheduling(jobs, ma, bestCMax);

    cout << "Best job order to minimize C_max:\n";
    for (const auto& job : best_order) {
        cout << "Job ID: " << job.id << "\n";
    }
    cout << "Minimum C_max: " << bestCMax << endl;

    return 0;
}






