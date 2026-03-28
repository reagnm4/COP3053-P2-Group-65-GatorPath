#include "gator_path_app.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;
static const std::string CSV_NAME = "gatorpath_dataset.csv";

std::string GatorPathApp::resolve_csv_path(const std::string& exe_path) {
    if (std::ifstream(CSV_NAME).good()) return CSV_NAME;
    fs::path dir = exe_path.empty() ? fs::current_path() : fs::path(exe_path).parent_path();
    for (int i = 0; i < 6; ++i) {
        fs::path candidate = dir / CSV_NAME;
        if (std::ifstream(candidate.string()).good()) return candidate.string();
        if (dir.has_parent_path() && dir != dir.parent_path()) dir = dir.parent_path();
        else break;
    }
    return CSV_NAME;
}

void GatorPathApp::run(const std::string& exe_path) {
    loaded_catalog_path_ = resolve_csv_path(exe_path);
    std::cout << "\nLoading course catalog from \"" << loaded_catalog_path_ << "\"...\n";
    seed_sample_data();
    std::cout << "Catalog loaded: " << graph_.courses().size() << " unique courses indexed.\n\n";

    bool is_running = true;
    while (is_running) {
        show_menu();
        int choice = 0;
        std::cin >> choice;
        std::cin.ignore(10000, '\n');
        std::cout << '\n';
        switch (choice) {
            case 1: handle_load_completed_courses(); break;
            case 2: handle_bfs();                    break;
            case 3: handle_dfs();                    break;
            case 4: handle_recommendations();        break;
            case 5: handle_validation();             break;
            case 6: is_running = false;              break;
            default: std::cout << "Invalid option. Please enter 1-6.\n"; break;
        }
        std::cout << '\n';
    }
    std::cout << "Goodbye, Gator!\n";
}

void GatorPathApp::show_menu() const {
    std::cout << "===============================\n";
    std::cout << "        GATORPATH PLANNER      \n";
    std::cout << "===============================\n";
    std::cout << "[1] Load my completed courses\n";
    std::cout << "[2] View all courses unlocked by a course (BFS)\n";
    std::cout << "[3] Trace prerequisite tree for a course (DFS)\n";
    std::cout << "[4] Get top N recommended next courses (Max-Heap)\n";
    std::cout << "[5] Check what I still need for a target course\n";
    std::cout << "[6] Exit\n";
    std::cout << "-------------------------------\n";
    std::cout << "Catalog: " << loaded_catalog_path_ << '\n';
    std::cout << "Completed courses loaded: " << completed_courses_.size() << '\n';
    std::cout << "Enter choice: ";
}

void GatorPathApp::seed_sample_data() {
    const std::vector<CourseRecord> records = loader_.load_from_csv(loaded_catalog_path_);
    graph_.build_from_records(records);
}

void GatorPathApp::handle_load_completed_courses() {
    std::cout << "Enter completed course ids separated by commas: ";
    std::string line;
    std::getline(std::cin, line);
    completed_courses_ = split_codes(line);
    for (const std::string& id : completed_courses_)
        if (!graph_.has_course(id))
            std::cout << "  Warning: \"" << id << "\" not found in catalog.\n";
    std::cout << "Loaded " << completed_courses_.size() << " completed course(s).\n";
}

void GatorPathApp::handle_bfs() const {
    std::string course_id;
    std::cout << "Enter a course id: ";
    std::getline(std::cin, course_id);
    const auto s = course_id.find_first_not_of(" \t");
    if (s != std::string::npos) course_id = course_id.substr(s);
    const auto e = course_id.find_last_not_of(" \t");
    if (e != std::string::npos) course_id = course_id.substr(0, e + 1);

    if (!graph_.has_course(course_id)) {
        std::cout << "Course \"" << course_id << "\" not found in catalog.\n";
        return;
    }
    auto t0 = std::chrono::high_resolution_clock::now();
    const auto levels = graph_.bfs_unlock_levels(course_id);
    double ms = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - t0).count();

    if (levels.empty()) { std::cout << "No courses unlocked by " << course_id << ".\n"; return; }

    int total = 0;
    std::cout << "\nCourses unlocked by " << course_id << " (BFS):\n";
    for (std::size_t i = 0; i < levels.size(); ++i) {
        std::cout << "  Level " << std::setw(2) << (i + 1) << ": ";
        for (std::size_t j = 0; j < levels[i].size(); ++j) {
            if (j > 0) std::cout << ", ";
            std::cout << levels[i][j];
        }
        std::cout << '\n';
        total += static_cast<int>(levels[i].size());
    }
    std::cout << "  Total unlocked: " << total << " course(s)  ["
              << std::fixed << std::setprecision(2) << ms << " ms]\n";
}

void GatorPathApp::handle_dfs() const {
    std::string course_id;
    std::cout << "Enter a target course id: ";
    std::getline(std::cin, course_id);
    const auto s = course_id.find_first_not_of(" \t");
    if (s != std::string::npos) course_id = course_id.substr(s);
    const auto e = course_id.find_last_not_of(" \t");
    if (e != std::string::npos) course_id = course_id.substr(0, e + 1);

    if (!graph_.has_course(course_id)) {
        std::cout << "Course \"" << course_id << "\" not found in catalog.\n";
        return;
    }
    auto t0 = std::chrono::high_resolution_clock::now();
    const std::vector<std::string> lines = graph_.dfs_prerequisite_tree(course_id);
    double ms = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - t0).count();

    std::cout << "\nPrerequisite tree for " << course_id << " (DFS):\n";
    for (const std::string& line : lines)
        std::cout << "  " << line << '\n';
    std::cout << "  [" << std::fixed << std::setprecision(2) << ms << " ms]\n";
}

void GatorPathApp::handle_recommendations() const {
    int limit = 5;
    std::cout << "How many recommendations do you want? ";
    std::string line;
    std::getline(std::cin, line);
    if (!line.empty()) { try { limit = std::stoi(line); } catch (...) { limit = 5; } }
    if (limit <= 0) limit = 5;

    auto t0 = std::chrono::high_resolution_clock::now();
    const auto recs = recommender_.recommend_next_courses(graph_, completed_courses_, limit);
    double ms = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - t0).count();

    if (recs.empty()) {
        std::cout << "No recommendations available. Load completed courses first [option 1].\n";
        return;
    }
    std::cout << "\nTop " << recs.size() << " recommended next courses (Max-Heap):\n";
    for (std::size_t i = 0; i < recs.size(); ++i) {
        const Recommendation& r = recs[i];
        std::cout << "  " << (i + 1) << ". " << r.course_id
                  << " unlocks " << r.unlock_count << " course(s)"
                  << " | difficulty: " << std::fixed << std::setprecision(1) << r.difficulty << '\n';
    }
    std::cout << "  [" << std::fixed << std::setprecision(2) << ms << " ms]\n";
}

void GatorPathApp::handle_validation() const {
    std::string course_id;
    std::cout << "Enter the course you want to take: ";
    std::getline(std::cin, course_id);
    const auto s = course_id.find_first_not_of(" \t");
    if (s != std::string::npos) course_id = course_id.substr(s);
    const auto e = course_id.find_last_not_of(" \t");
    if (e != std::string::npos) course_id = course_id.substr(0, e + 1);

    if (!graph_.has_course(course_id)) {
        std::cout << "Course \"" << course_id << "\" not found in catalog.\n";
        return;
    }
    auto t0 = std::chrono::high_resolution_clock::now();

    const std::unordered_set<std::string> all_needed = graph_.all_transitive_prereqs(course_id);
    std::unordered_set<std::string> completed_set(completed_courses_.begin(), completed_courses_.end());

    std::vector<std::string> still_needed;
    for (const std::string& c : all_needed)
        if (!completed_set.count(c))
            still_needed.push_back(c);
    std::sort(still_needed.begin(), still_needed.end());

    double ms = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - t0).count();

    if (still_needed.empty()) {
        std::cout << "You have everything you need to take " << course_id << "!\n";
    } else {
        std::cout << "\nTo take " << course_id << " you still need (" << still_needed.size() << " course(s)):\n";
        for (const std::string& c : still_needed)
            std::cout << "  - " << c << '\n';
    }
    std::cout << "  [" << std::fixed << std::setprecision(2) << ms << " ms]\n";
}

std::vector<std::string> GatorPathApp::split_codes(const std::string& line) {
    std::vector<std::string> codes;
    std::stringstream stream(line);
    std::string code;
    while (std::getline(stream, code, ',')) {
        const auto s = code.find_first_not_of(" \t\r\n");
        if (s == std::string::npos) continue;
        const auto e = code.find_last_not_of(" \t\r\n");
        codes.push_back(code.substr(s, e - s + 1));
    }
    return codes;
}
