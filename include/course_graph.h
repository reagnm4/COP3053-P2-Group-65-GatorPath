#pragma once
#include "course.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class CourseGraph {
public:
    void build_from_records(const std::vector<CourseRecord>& records);
    bool          has_course(const std::string& course_id) const;
    const Course* find_course(const std::string& course_id) const;

    std::vector<std::vector<std::string>> bfs_unlock_levels(const std::string& start_id) const;
    // Returns the prerequisite tree as indented lines for display.
    std::vector<std::string>              dfs_prerequisite_tree(const std::string& target_id) const;
    // Returns flat list of all transitive prerequisites (for checking/filtering).
    std::unordered_set<std::string>       all_transitive_prereqs(const std::string& course_id) const;
    int                                   reachable_count(const std::string& course_id) const;

    const std::unordered_map<std::string, Course>&                        courses()      const;
    const std::unordered_map<std::string, std::vector<std::string>>&      edges()        const;
    const std::unordered_map<std::string, std::unordered_set<std::string>>& all_prereqs() const;

private:
    std::unordered_map<std::string, Course>                        courses_;
    std::unordered_map<std::string, std::vector<std::string>>      unlock_edges_;
    std::unordered_map<std::string, std::unordered_set<std::string>> prerequisite_edges_;

    void dfs_tree_visit(const std::string& course_id, int depth,
                        std::unordered_set<std::string>& visited,
                        std::vector<std::string>& lines) const;

    void collect_prereqs(const std::string& course_id,
                         std::unordered_set<std::string>& visited,
                         std::unordered_set<std::string>& result) const;
};
