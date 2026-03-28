#pragma once
#include <string>

struct Course {
    std::string id;
    std::string name;
    int         credits    = 0;
    double      difficulty = 0.0;
};

struct CourseRecord {
    Course      course;
    std::string prerequisite_id;
};
