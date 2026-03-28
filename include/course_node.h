#pragma once
#include <string>
using namespace std;

struct CourseNode {
    string course_id;
    string course_name;
    int    credits      = 0;
    int    unlock_count = 0;
    double difficulty   = 0.0;

    bool operator<(const CourseNode& other) const {
        if (unlock_count != other.unlock_count)
            return unlock_count < other.unlock_count;
        return difficulty > other.difficulty;
    }
};
