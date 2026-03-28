#pragma once
#include "course_graph.h"
#include "max_heap.h"
#include <string>
#include <vector>

struct Recommendation {
    std::string course_id;
    std::string course_name;
    int         unlock_count = 0;
    double      difficulty   = 0.0;
};

class CourseRecommender {
public:
    std::vector<Recommendation> recommend_next_courses(
        const CourseGraph&              graph,
        const std::vector<std::string>& completed_courses,
        int                             limit) const;
};
