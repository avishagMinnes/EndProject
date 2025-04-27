#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <random>
#include <chrono>
#include <cassert>

using namespace std;
using namespace std::chrono;

struct Job {
    int id;
    int a; // Processing time before deadline
    int b; // Processing time reduction if late
    int c; // Processing time reduction after maintenance (c > b)
    int d; // Deadline
};

struct Maintenance {
    int start;
    int duration;
};

const int INF = INT_MAX;

int bitmaskDPScheduling(vector<Job>& jobs, const Maintenance& ma) {
    int n = jobs.size();

    // Estimate a safe upper bound on total finish time
    int maxTime = 0;
    for (const Job& job : jobs) {
        maxTime += max(job.a, job.a - job.b);  // Late case worst time
    }
    maxTime += ma.duration + 10;  // Add buffer

    vector<vector<int>> dp(1 << n, vector<int>(maxTime + 1, INF));
    dp[0][0] = 0;

    for (int mask = 0; mask < (1 << n); ++mask) {
        for (int t = 0; t <= maxTime; ++t) {
            if (dp[mask][t] == INF) continue;

            for (int j = 0; j < n; ++j) {
                if (mask & (1 << j)) continue;

                int newMask = mask | (1 << j);
                int startTime = t;

                // Adjust for maintenance window
                if (startTime < ma.start && startTime + jobs[j].a > ma.start) {
                    startTime = ma.start + ma.duration;
                }
                if (startTime >= ma.start && startTime < ma.start + ma.duration) {
                    startTime = ma.start + ma.duration;
                }

                // Compute finish time
                int finishTime;
                if (startTime < jobs[j].d) {
                    finishTime = startTime + jobs[j].a; // Process before deadline and before maintenance
                } else if (startTime < ma.start) {
                    finishTime = startTime + jobs[j].a - jobs[j].b; // After deadline but before maintenance
                } else {
                    finishTime = startTime + jobs[j].a - jobs[j].c; // After deadline and after maintenance
                }

                // Update DP state
                dp[newMask][finishTime] = min(dp[newMask][finishTime], max(dp[mask][t], finishTime));
            }
        }
    }

    // Find optimal C_max
    int result = INF;
    for (int t = 0; t <= maxTime; ++t) {
        result = min(result, dp[(1 << n) - 1][t]);
    }

    return result;
}


int calculateCMax(const vector<Job>& jobs, const Maintenance& ma) {
    int current_time = 0;
    int C_max = 0;

    for (const auto& job : jobs) {
        if (current_time >= ma.start && current_time < ma.start + ma.duration) {
            current_time = ma.start + ma.duration;
        }
        if (current_time < ma.start && current_time + job.a > ma.start) {
            current_time = ma.start + ma.duration;
        }
        if (current_time < job.d) {
            current_time += job.a; // Before the maintenance and before the deadline
        } else if (current_time < ma.start) {
            current_time += (job.a - job.b); // After deadline but before maintenance
        } else {
            current_time += (job.a - job.c); // After deadline and after maintenance
        }
        C_max = max(C_max, current_time);
    }

    return C_max;
}

vector<Job> bruteForceScheduling(vector<Job>& jobs, const Maintenance& ma, int& bestCMax) {
    vector<Job> best_order;
    bestCMax = INT_MAX;

    sort(jobs.begin(), jobs.end(), [](const Job& j1, const Job& j2) { return j1.id < j2.id; });
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
    const int NUM_TESTS = 300;
    const int NUM_JOBS = 6;
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> a_dist(1, 5);
    uniform_int_distribution<int> b_dist(1, 3);
    uniform_int_distribution<int> c_dist(4, 5); // c > b, for this case (4 <= c <= 5)
    uniform_int_distribution<int> d_dist(5, 15);
    uniform_int_distribution<int> m_start_dist(2, 10);
    uniform_int_distribution<int> m_dur_dist(1, 4);

    for (int test = 1; test <= NUM_TESTS; ++test) {
        vector<Job> jobs;
        for (int i = 0; i < NUM_JOBS; ++i) {
            int a = a_dist(rng);
            int b = min(b_dist(rng), a); // b should not exceed a
            int c = c_dist(rng); // c is guaranteed to be larger than b
            int d = d_dist(rng);
            jobs.push_back({i + 1, a, b, c, d}); // Added `c` here
        }
        Maintenance ma = {m_start_dist(rng), m_dur_dist(rng)};

        auto jobs_copy = jobs;

        auto t1 = high_resolution_clock::now();
        int dpResult = bitmaskDPScheduling(jobs_copy, ma);
        auto t2 = high_resolution_clock::now();
        auto dpTime = duration_cast<microseconds>(t2 - t1).count();

        t1 = high_resolution_clock::now();
        int naiveResult;
        bruteForceScheduling(jobs, ma, naiveResult);
        t2 = high_resolution_clock::now();
        auto naiveTime = duration_cast<microseconds>(t2 - t1).count();

        if (dpResult != naiveResult) {
            cout << "Mismatch found in test #" << test << "!\n";
            cout << "DP result = " << dpResult << ", Naive result = " << naiveResult << "\n";
            return 1;
        }

        cout << "Test #" << test << " passed. Time: DP = " << dpTime << "ms, Naive = " << naiveTime << "ms\n";
    }

    cout << "All " << NUM_TESTS << " tests passed successfully.\n";
    return 0;
}
