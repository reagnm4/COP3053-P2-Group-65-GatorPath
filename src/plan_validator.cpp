#include "plan_validator.h"
#include <unordered_map>
#include <unordered_set>

static constexpr int WHITE = 0, GRAY = 1, BLACK = 2;

// Validator -- checks missing prerequisites and detects cycles via DFS colouring.
ValidationResult PlanValidator::validate(
    const CourseGraph&              graph,
    const std::vector<std::string>& completed_courses) const {

    ValidationResult result;
    if (completed_courses.empty()) return result;

    std::unordered_set<std::string> completed_set(completed_courses.begin(), completed_courses.end());

    for (const std::string& course_id : completed_courses) {
        const auto needed = graph.all_transitive_prereqs(course_id);
        for (const std::string& prereq : needed)
            if (!completed_set.count(prereq)) {
                result.is_valid = false;
                result.missing_prerequisites.push_back(course_id + " requires " + prereq + " (not completed)");
            }
    }

    const auto& all_edges = graph.edges();
    std::unordered_map<std::string, std::vector<std::string>> sub_edges;
    for (const std::string& src : completed_courses) {
        auto it = all_edges.find(src);
        if (it == all_edges.end()) continue;
        for (const std::string& dst : it->second)
            if (completed_set.count(dst)) sub_edges[src].push_back(dst);
    }

    std::unordered_map<std::string, int> color;
    for (const std::string& c : completed_courses) color[c] = WHITE;

    std::vector<std::string> cycle_members;
    for (const std::string& c : completed_courses)
        if (color[c] == WHITE && dfs_cycle(c, sub_edges, color, cycle_members))
            result.is_valid = false;

    if (!cycle_members.empty()) result.cycle_members = cycle_members;
    return result;
}

bool PlanValidator::dfs_cycle(
    const std::string& node,
    const std::unordered_map<std::string, std::vector<std::string>>& edges,
    std::unordered_map<std::string, int>& color,
    std::vector<std::string>& cycle_members) const {

    color[node] = GRAY;
    auto it = edges.find(node);
    if (it != edges.end()) {
        for (const std::string& nb : it->second) {
            if (color.find(nb) == color.end()) continue;
            if (color[nb] == GRAY) { cycle_members.push_back(nb); cycle_members.push_back(node); return true; }
            if (color[nb] == WHITE && dfs_cycle(nb, edges, color, cycle_members)) return true;
        }
    }
    color[node] = BLACK;
    return false;
}
