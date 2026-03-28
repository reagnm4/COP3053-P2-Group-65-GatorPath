#pragma once
#include "course_graph.h"
#include <string>
#include <vector>

struct ValidationResult {
    bool                     is_valid = true;
    std::vector<std::string> missing_prerequisites;
    std::vector<std::string> cycle_members;
};

class PlanValidator {
public:
    ValidationResult validate(const CourseGraph&              graph,
                              const std::vector<std::string>& completed_courses) const;

private:
    bool dfs_cycle(const std::string& node,
                   const std::unordered_map<std::string, std::vector<std::string>>& edges,
                   std::unordered_map<std::string, int>& color,
                   std::vector<std::string>& cycle_members) const;
};
