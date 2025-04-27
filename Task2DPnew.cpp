#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

struct Job {
    int id;
    int a; // Processing time before deadline
    int b; // Processing time reduction if late
    int c; // Processing time reduction after maintenance
    int d; // Deadline
};

struct Maintenance {
    int start;
    int duration;
};

const int INF = INT_MAX;

// Function to compute the minimum C_max using Bitmask DP
int bitmaskDPScheduling(vector<Job>& jobs, const Maintenance& ma) {
    int n = jobs.size();
    int maxTime = 0;
    for (const Job &job : jobs) maxTime += job.a; // Upper bound on time
    maxTime = max(maxTime, ma.start + ma.duration + 10);

    // DP table: dp[mask][t] = min C_max achievable with subset 'mask' ending at time 't'
    vector<vector<int>> dp(1 << n, vector<int>(maxTime + 1, INF));
    dp[0][0] = 0; // Base case: no jobs scheduled, time is 0

    // Iterate over all subsets of jobs
    for (int mask = 0; mask < (1 << n); ++mask) {
        for (int t = 0; t <= maxTime; ++t) {
            if (dp[mask][t] == INF) continue; // Skip invalid states

            // Try scheduling the next available job
            for (int j = 0; j < n; ++j) {
                if (mask & (1 << j)) continue; // Job already scheduled

                int newMask = mask | (1 << j);
                int startTime = t;

                // If the job starts before maintenance but ends after, delay it
                if (startTime < ma.start && startTime + jobs[j].a > ma.start) {
                    startTime = ma.start + ma.duration;
                }
                // If the job starts during maintenance, delay it
                if (startTime >= ma.start && startTime < ma.start + ma.duration) {
                    startTime = ma.start + ma.duration;
                }

                int finishTime;
                if (startTime < jobs[j].d) {
                    finishTime = startTime + jobs[j].a; // Process before deadline and before maintenance
                } else if (startTime < ma.start) {
                    finishTime = startTime + jobs[j].a - jobs[j].b; // After deadline but before maintenance
                } else {
                    finishTime = startTime + jobs[j].a - jobs[j].c; // After deadline and after maintenance
                }

                // Update DP table
                dp[newMask][finishTime] = min(dp[newMask][finishTime], max(dp[mask][t], finishTime));
            }
        }
    }

    // Find the minimum C_max among all full sets of jobs
    int result = INF;
    for (int t = 0; t <= maxTime; ++t) {
        result = min(result, dp[(1 << n) - 1][t]);
    }

    return result;
}

int main() {
    vector<Job> jobs = {
            {1, 2, 1, 1, 5}, // (a=2, b=1, c=1, deadline=5)
            {2, 1, 1, 1, 4}, // (a=1, b=1, c=1, deadline=4)
            {3, 3, 2, 1, 6}, // (a=3, b=2, c=1, deadline=6)
            {4, 2, 1, 1, 5}  // (a=2, b=1, c=1, deadline=5)
    };
    Maintenance ma = {3, 4}; // Maintenance start at 3 and lasts for 4 units

    int cMax = bitmaskDPScheduling(jobs, ma);
    cout << "Optimal C_max using Bitmask DP = " << cMax << endl;

    return 0;
}
