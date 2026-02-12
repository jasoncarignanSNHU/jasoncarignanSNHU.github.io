//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Jason Carignan
// Version     : 2.2 (Enhanced for CS 499 Milestone Three - Algorithms & Data Structures)
// Date        : 1/27/2026
// Description : Course Planner Application
//               Enhancements Added:
//               1) Eliminates repeated sorting by maintaining ordered course IDs in a std::set
//               2) Adds stronger CSV validation and duplicate ID protection
//               3) Builds a prerequisite graph (adjacency list)
//               4) Expands full prerequisite chains using DFS + memoization
//               5) Detects prerequisite cycles using a recursion stack (DFS cycle detection)
//               6) Adds lightweight performance timing output for load and repeated schedule printing
//               7) Adds legacy print path to demonstrate original methodology for comparison
//               8) Adds performance demo that compares Enhanced vs Legacy without screeen clutter
//               9) Improves benchmarking accuracy by:
//                  - using steady_clock for elapsed time measurement
//                  - timing in microseconds
//                  - using a sink so the compiler can't optimize loops away
//============================================================================

#include <algorithm>      // sort, transform
#include <cctype>         // toupper, isspace
#include <chrono>         // steady_clock, duration
#include <exception>      // exception
#include <fstream>        // ifstream
#include <iomanip>        // setw, left, setprecision
#include <iostream>       // cout, cin
#include <limits>         // numeric_limits
#include <sstream>        // stringstream
#include <string>         // string
#include <unordered_map>  // unordered_map
#include <unordered_set>  // unordered_set
#include <set>            // set (keeps sorted order)
#include <vector>         // vector

using namespace std;

//============================================================================
// Performance sink: prevents the compiler from optimizing away loops in the performance demo.
//============================================================================
static volatile size_t g_sink = 0;

//============================================================================
// Course domain model
//============================================================================
struct Course {
    string courseId;                 // Example: CSCI100
    string courseTitle;              // Example: Introduction to Programming
    vector<string> prerequisites;    // Example: ["CSCI050", "MATH101"]

    Course() = default;
};

//============================================================================
// Helper functions
//============================================================================

/**
 * Trim leading/trailing whitespace.
 */
static string Trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }
    return s.substr(start, end - start);
}

/**
 * Convert a string to uppercase for consistent internal keys.
 */
static string ToUpper(string s) {
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return s;
}

/**
 * Split a CSV line on commas.
 */
static vector<string> SplitCSV(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(Trim(token));
    }
    return tokens;
}

//============================================================================
// CoursePlanner (formerly HashTable)
//============================================================================
class CoursePlanner {
private:
    // Primary storage: fast lookup by ID (near O(1))
    unordered_map<string, Course> courses;

    // Ordered index: always sorted, eliminates repeated sorting in PrintAllCourses()
    set<string> orderedIds;

    // Graph representation: adjacency list mapping course to direct prerequisites
    unordered_map<string, vector<string>> prereqGraph;

    // Memoization cache: course to expanded prerequisite chain
    unordered_map<string, vector<string>> prereqMemo;

    // Internal helper: rebuild graph after loading data
    void BuildGraph();

    // Internal helper: DFS prerequisite expansion with cycle detection
    vector<string> ExpandPrereqsDFS(const string& courseId,
        unordered_set<string>& visiting);

    // Benchmark helpers:
    // These return a checksum so the compiler can't optimize the loop away.
    // Both methods see courseTitle.size() so they do comparable work.
    uint64_t IterateEnhanced_NoOutput() const;
    uint64_t IterateLegacy_NoOutput() const;

public:
    // Load data with validation and safe behaviors
    bool LoadData(const string& filename);

    // Print all courses
    void PrintAllCourses() const;

	// Legacy print, using original approach (sort every time)
    void PrintAllCourses_Legacy() const;

    // Timing helper: Enhanced approach (iterate already-sorted IDs)
    long long TimeEnhancedIterateOnlyUs(int iterations) const;

    // Timing helper: Legacy approach (rebuild + sort IDs each time) 
    long long TimeLegacySortOnlyUs(int iterations) const;

    // Print course details + direct prerequisites + full prerequisite chain
    void PrintCourse(const string& courseId);

    // Utility: check if loaded
    bool IsLoaded() const { return !courses.empty(); }
};

//============================================================================
// LoadData: parsing + data integrity checks
//============================================================================
bool CoursePlanner::LoadData(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file '" << filename
            << "'. Please check the filename/path and try again." << endl;
        return false;
    }

    // Clear previous state so re-loading behaves predictably
    courses.clear();
    orderedIds.clear();
    prereqGraph.clear();
    prereqMemo.clear();

    string line;
    int lineNumber = 0;
    int loadedCount = 0;
    int skippedCount = 0;

    while (getline(file, line)) {
        ++lineNumber;

        line = Trim(line);
        if (line.empty()) {
            // Skip blank lines
            continue;
        }

        vector<string> fields = SplitCSV(line);

        // Minimum fields: courseId, courseTitle
        if (fields.size() < 2) {
            cerr << "Warning: Skipping line " << lineNumber
                << " (missing course ID or title)." << endl;
            ++skippedCount;
            continue;
        }

        Course c;
        c.courseId = ToUpper(fields[0]);
        c.courseTitle = fields[1];

        // Validate required fields
        if (c.courseId.empty() || c.courseTitle.empty()) {
            cerr << "Warning: Skipping line " << lineNumber
                << " (empty course ID or title)." << endl;
            ++skippedCount;
            continue;
        }

        // Parse prerequisites, trimming and normalizing
        for (size_t i = 2; i < fields.size(); ++i) {
            string prereq = ToUpper(fields[i]);
            if (!prereq.empty()) {
                c.prerequisites.push_back(prereq);
            }
        }

        // Detect duplicate IDs (original code would silently overwrite)
        if (courses.find(c.courseId) != courses.end()) {
            cerr << "Warning: Duplicate course ID '" << c.courseId
                << "' on line " << lineNumber
                << ". Keeping the first occurrence, skipping this one." << endl;
            ++skippedCount;
            continue;
        }

        courses[c.courseId] = c;
        orderedIds.insert(c.courseId);
        ++loadedCount;
    }

    file.close();

    if (loadedCount == 0) {
        cerr << "Warning: No valid courses were loaded. Please verify the input file." << endl;
        return false;
    }

    // After loading, build the adjacency list for prerequisite algorithms
    BuildGraph();

    // Warning about prerequisite references that do not exist
    int missingPrereqRefs = 0;
    for (const auto& pair : courses) {
        const Course& c = pair.second;
        for (const string& prereq : c.prerequisites) {
            if (courses.find(prereq) == courses.end()) {
                ++missingPrereqRefs;
            }
        }
    }
    if (missingPrereqRefs > 0) {
        cout << "Note: " << missingPrereqRefs
            << " prerequisite reference(s) do not match a loaded course ID." << endl;
    }

    cout << loadedCount << " course(s) loaded successfully."
        << " (" << skippedCount << " line(s) skipped)" << endl;

    return true;
}

//============================================================================
// BuildGraph: adjacency list course to direct prerequisites
//============================================================================
void CoursePlanner::BuildGraph() {
    prereqGraph.clear();

    // Ensure every course has a key in the graph (even if no prereqs)
    for (const auto& pair : courses) {
        prereqGraph[pair.first] = {};
    }

    for (const auto& pair : courses) {
        const string& courseId = pair.first;
        const Course& course = pair.second;

        for (const string& prereq : course.prerequisites) {
            prereqGraph[courseId].push_back(prereq);
        }
    }
}

//============================================================================
// ExpandPrereqsDFS: DFS + memoization + cycle detection
//============================================================================
vector<string> CoursePlanner::ExpandPrereqsDFS(const string& courseId,
    unordered_set<string>& visiting) {

    // If already computed, return cached result
    if (prereqMemo.find(courseId) != prereqMemo.end()) {
        return prereqMemo[courseId];
    }

    // Cycle detection: courseId already in active recursion path
    if (visiting.find(courseId) != visiting.end()) {
        throw runtime_error("Cycle detected involving course: " + courseId);
    }

    visiting.insert(courseId);

    vector<string> result;

    // If courseId not in graph treat it as if having no prereqs.
    auto it = prereqGraph.find(courseId);
    if (it != prereqGraph.end()) {
        const vector<string>& directPrereqs = it->second;

        for (const string& prereq : directPrereqs) {
            // Include the direct prerequisite
            result.push_back(prereq);

            // Recurse to include prereq's prerequisites
            vector<string> sub = ExpandPrereqsDFS(prereq, visiting);
            result.insert(result.end(), sub.begin(), sub.end());
        }
    }

    visiting.erase(courseId);

    // Deduplicate while preserving a stable order (first occurrence wins).
    unordered_set<string> seen;
    vector<string> uniqueResult;
    for (const string& id : result) {
        if (seen.insert(id).second) {
            uniqueResult.push_back(id);
        }
    }

    prereqMemo[courseId] = uniqueResult;
    return prereqMemo[courseId];
}

//============================================================================
// PrintAllCourses: uses orderedIds (std::set) so no repeated sorting
//============================================================================
void CoursePlanner::PrintAllCourses() const {
    if (courses.empty()) {
        cout << "No courses available. Load data first." << endl;
        return;
    }

    cout << "Here is a sample schedule:" << endl;

    // Column widths for clean output
    const int courseIdWidth = 15;
    const int courseTitleWidth = 50;

    cout << left << setw(courseIdWidth) << "Course Number"
        << setw(courseTitleWidth) << "Course Name" << endl;
    cout << string(courseIdWidth + courseTitleWidth, '-') << endl;

    for (const auto& id : orderedIds) {
        auto it = courses.find(id);
        if (it != courses.end()) {
            cout << left << setw(courseIdWidth) << it->second.courseId
                << setw(courseTitleWidth) << it->second.courseTitle << endl;
        }
    }
}

//============================================================================
// PrintAllCourses_Legacy: original methodology (sort every time)
//============================================================================
void CoursePlanner::PrintAllCourses_Legacy() const {
    if (courses.empty()) {
        cout << "No courses available. Load data first." << endl;
        return;
    }

    cout << "Here is a sample schedule (Legacy - sort every time):" << endl;

    // Build a list of IDs from the hash table each time
    vector<string> courseIds;
    courseIds.reserve(courses.size());
    for (const auto& pair : courses) {
        courseIds.push_back(pair.first);
    }

    // Sort IDs every time (this is the old behavior)
    sort(courseIds.begin(), courseIds.end());

    // Column widths for clean output (matches enhanced output)
    const int courseIdWidth = 15;
    const int courseTitleWidth = 50;

    cout << left << setw(courseIdWidth) << "Course Number"
        << setw(courseTitleWidth) << "Course Name" << endl;
    cout << string(courseIdWidth + courseTitleWidth, '-') << endl;

    for (const auto& id : courseIds) {
        auto it = courses.find(id);
        if (it != courses.end()) {
            cout << left << setw(courseIdWidth) << it->second.courseId
                << setw(courseTitleWidth) << it->second.courseTitle << endl;
        }
    }
}

//============================================================================
// IterateEnhanced_NoOutput: iterate the already-sorted IDs, touch course data
//============================================================================
uint64_t CoursePlanner::IterateEnhanced_NoOutput() const {
    uint64_t checksum = 0;

    for (const auto& id : orderedIds) {
        auto it = courses.find(id);
        if (it != courses.end()) {
            // Touch course data (not just the ID) so both paths do comparable work
            checksum += it->second.courseTitle.size();
        }
    }

    return checksum;
}

//============================================================================
// IterateLegacy_NoOutput: rebuild + sort IDs, touch course data
//============================================================================
uint64_t CoursePlanner::IterateLegacy_NoOutput() const {
    vector<string> ids;
    ids.reserve(courses.size());

    for (const auto& pair : courses) {
        ids.push_back(pair.first);
    }

    sort(ids.begin(), ids.end());

    uint64_t checksum = 0;
    for (const auto& id : ids) {
        auto it = courses.find(id);
        if (it != courses.end()) {
            checksum += it->second.courseTitle.size();
        }
    }

    return checksum;
}

//============================================================================
// TimeEnhancedIterateOnlyUs: measures new enhanced approach.
//============================================================================
long long CoursePlanner::TimeEnhancedIterateOnlyUs(int iterations) const {
    using Clock = chrono::steady_clock;

    auto start = Clock::now();

    uint64_t local = 0;
    for (int i = 0; i < iterations; ++i) {
        local += IterateEnhanced_NoOutput();
    }

    auto end = Clock::now();

    // Sink the value so the compiler can't remove the work
    g_sink += static_cast<size_t>(local);

    return chrono::duration_cast<chrono::microseconds>(end - start).count();
}

//============================================================================
// TimeLegacySortOnlyUs: measures legacy approach.
//============================================================================
long long CoursePlanner::TimeLegacySortOnlyUs(int iterations) const {
    using Clock = chrono::steady_clock;

    auto start = Clock::now();

    uint64_t local = 0;
    for (int i = 0; i < iterations; ++i) {
        local += IterateLegacy_NoOutput();
    }

    auto end = Clock::now();

    // Sink the value so the compiler can't remove the work
    g_sink += static_cast<size_t>(local);

    return chrono::duration_cast<chrono::microseconds>(end - start).count();
}

//============================================================================
// PrintCourse: shows course, direct prereqs, and full prerequisite chain
//============================================================================
void CoursePlanner::PrintCourse(const string& courseId) {
    if (courses.empty()) {
        cout << "No courses available. Load data first." << endl;
        return;
    }

    string id = ToUpper(Trim(courseId));

    auto it = courses.find(id);
    if (it == courses.end()) {
        cout << "Course not found: " << id << endl;
        return;
    }

    const Course& course = it->second;

    // Print core course info
    cout << course.courseId << ", " << course.courseTitle << endl;

    // Print direct prerequisites
    if (!course.prerequisites.empty()) {
        cout << "Direct Prerequisites: ";
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            cout << course.prerequisites[i];
            if (i + 1 < course.prerequisites.size()) {
                cout << ", ";
            }
        }
        cout << endl;
    }
    else {
        cout << "Direct Prerequisites: None" << endl;
    }

    // Print full prerequisite chain
    prereqMemo.clear();

    try {
        unordered_set<string> visiting;
        vector<string> chain = ExpandPrereqsDFS(course.courseId, visiting);

        if (!chain.empty()) {
            cout << "Full Prerequisite Chain: ";
            for (size_t i = 0; i < chain.size(); ++i) {
                cout << chain[i];
                if (i + 1 < chain.size()) {
                    cout << ", ";
                }
            }
            cout << endl;
        }
        else {
            cout << "Full Prerequisite Chain: None" << endl;
        }
    }
    catch (const exception& e) {
        cout << "Full Prerequisite Chain: ERROR (" << e.what() << ")" << endl;
    }
}

//============================================================================
// Main Menu Helpers
//============================================================================
static void PrintMenu() {
    cout << "\n1. Load Data Structure." << endl;
    cout << "2. Print Course List (Enhanced)" << endl;
    cout << "3. Print Course." << endl;
    cout << "4. Run Performance Demo (Enhanced vs Legacy WITHOUT printing + load time)" << endl;
    cout << "5. Print Course List (Legacy - sort every time)" << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

/**
 * Read an integer choice from stdin, handling invalid input.
 */
static bool ReadInt(int& outChoice) {
    if (!(cin >> outChoice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

/**
 * Performance demo:
 * Measures load time, then compares enhanced vs legacy approaches.
 */
static void RunPerformanceDemo(CoursePlanner& planner, const string& filename) {
    using Clock = chrono::steady_clock;

    // Measure load time
    auto loadStart = Clock::now();
    bool loaded = planner.LoadData(filename);
    auto loadEnd = Clock::now();

    if (!loaded) {
        cout << "Performance Demo aborted, data did not load." << endl;
        return;
    }

    auto loadUs = chrono::duration_cast<chrono::microseconds>(loadEnd - loadStart).count();

    // Warm-up (reduces first-run timing artifacts)
    g_sink += static_cast<size_t>(planner.TimeEnhancedIterateOnlyUs(1));
    g_sink += static_cast<size_t>(planner.TimeLegacySortOnlyUs(1));

    // Increase iterations
	// This is used to display the time difference more clearly.
    const int iterations = 5000;

    long long enhancedUs = planner.TimeEnhancedIterateOnlyUs(iterations);
    long long legacyUs = planner.TimeLegacySortOnlyUs(iterations);

    cout << "================ PERFORMANCE RESULTS ================" << endl;
    cout << "File Loaded: " << filename << endl;
    cout << "Load Time: " << loadUs << " us" << endl;
    cout << "Iterations per method: " << iterations << endl;
    cout << "Enhanced (iterate set + touch data): " << enhancedUs << " us" << endl;
    cout << "Legacy   (rebuild+sort + touch data): " << legacyUs << " us" << endl;

    if (enhancedUs > 0) {
        double ratio = static_cast<double>(legacyUs) / static_cast<double>(enhancedUs);
        cout << "Legacy / Enhanced ratio: " << fixed << setprecision(2) << ratio << "x" << endl;
    }

    cout << "====================================================" << endl;
}

//============================================================================
// Main Function
//============================================================================
int main() {
    CoursePlanner planner;
    int choice = 0;

    // Filename is defaulted to updated file, but still allows the user to enter a different one.
    string filename = "ProjectTwo_Enhanced.csv";

    cout << "Welcome to the course planner." << endl;
    cout << "Default CSV file: " << filename << endl;

    while (choice != 9) {
        PrintMenu();

        if (!ReadInt(choice)) {
            cout << "Invalid input. Please enter a number from the menu." << endl;
            continue;
        }

        switch (choice) {
        case 1: {
            cout << "Enter CSV filename (press Enter is not supported here, type it in), or retype the default: ";
            cin >> filename;

            cout << "\n";
            planner.LoadData(filename);
            break;
        }
        case 2:
            cout << "\n";
            planner.PrintAllCourses();
            break;

        case 3: {
            cout << "What course do you want to know about? ";
            string courseId;
            cin >> courseId;
            cout << "\n";
            planner.PrintCourse(courseId);
            break;
        }

        case 4:
            cout << "\n";
            RunPerformanceDemo(planner, filename);
            break;

        case 5:
            cout << "\n";
            planner.PrintAllCourses_Legacy();
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option. Please try again." << endl;
            break;
        }
    }

    return 0;
}
