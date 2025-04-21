// Description: Task 2 - Scheduling jobs with maintenance, Naive approach

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Job {
    int id;       // Job ID for tracking
    int a;        // Processing time if before deadline
    int b;        // Processing time reduction if after deadline
    int d;        // Deadline
};

struct Maintenance {
    int start;    // Start time of maintenance
    int duration; // Duration of maintenance
};

// Function to calculate the naive scheduling
int naiveScheduling(const vector<Job> &jobs, const Maintenance &ma) {
    int currentTime = 0; // Tracks the current time on the machine
    int cMax = 0;        // Tracks the maximum completion time

    for (const auto &job : jobs) {
        // Determine the processing time for the job
        int pj = (currentTime + job.a <= job.d) ? job.a : job.a - job.b;

        // Check if the job starts or overlaps with maintenance
        if (currentTime < ma.start && currentTime + pj > ma.start) {
            currentTime = ma.start + ma.duration; // Wait until maintenance is over
        } else if (currentTime >= ma.start && currentTime < ma.start + ma.duration) {
            currentTime = ma.start + ma.duration; // Resume after maintenance
        }

        // Schedule the job
        int sj = currentTime; // Start time of the job
        int cj = sj + pj;     // Completion time of the job
        currentTime = cj;     // Update current time

        // Update C_max
        cMax = max(cMax, cj);

        // Debug output
        cout << "Job " << job.id << ": Start = " << sj << ", End = " << cj << ", P = " << pj << endl;
    }

    return cMax;
}

int main() {
    int n = 10; // Number of jobs (based on the image)
    Maintenance ma = {20, 10}; // Maintenance details

    // Define jobs based on the image data for aj and bj
    vector<Job> jobs = {
            {1, 50, 27, 55}, {2, 43, 26, 55}, {3, 39, 25, 55},
            {4, 21, 14, 55}, {5, 19, 12, 55}, {6, 18, 10, 55},
            {7, 15, 8, 55},  {8, 11, 7, 55},  {9, 10, 6, 55}, {10, 9, 5, 55}
    };

    // Sorting jobs by ID (default order in this case, optional)
    sort(jobs.begin(), jobs.end(), [](const Job &j1, const Job &j2) {
        return j1.id < j2.id;
    });

    // Calculate and output the result
    int cMax = naiveScheduling(jobs, ma);
    cout << "C_max = " << cMax << endl;

    return 0;
}




