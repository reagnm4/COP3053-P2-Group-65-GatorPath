#include "course_graph.h"
#include <algorithm>
#include <utility>

void CourseGraph::build_from_records(const std::vector<CourseRecord>& records) {
    courses_.clear();
    unlock_edges_.clear();
    prerequisite_edges_.clear();

    for (const CourseRecord& rec : records) {
        courses_[rec.course.id] = rec.course;
        if (!rec.prerequisite_id.empty()) {
            if (courses_.find(rec.prerequisite_id) == courses_.end()) {
                Course stub;
                stub.id = rec.prerequisite_id;
                courses_[rec.prerequisite_id] = stub;
            }
            auto& unlocks = unlock_edges_[rec.prerequisite_id];
            if (std::find(unlocks.begin(), unlocks.end(), rec.course.id) == unlocks.end())
                unlocks.push_back(rec.course.id);
            prerequisite_edges_[rec.course.id].insert(rec.prerequisite_id);
        }
    }
}

bool CourseGraph::has_course(const std::string& course_id) const {
    return courses_.find(course_id) != courses_.end();
}

const Course* CourseGraph::find_course(const std::string& course_id) const {
    const auto it = courses_.find(course_id);
    return (it == courses_.end()) ? nullptr : &it->second;
}

const std::unordered_map<std::string, Course>& CourseGraph::courses() const { return courses_; }
const std::unordered_map<std::string, std::vector<std::string>>& CourseGraph::edges() const { return unlock_edges_; }
const std::unordered_map<std::string, std::unordered_set<std::string>>& CourseGraph::all_prereqs() const { return prerequisite_edges_; }

// BFS implementation -- returns courses unlocked level by level from start_id.
std::vector<std::vector<std::string>> CourseGraph::bfs_unlock_levels(const std::string& start_id) const {
    std::vector<std::vector<std::string>> levels;
    if (!has_course(start_id)) return levels;

    std::vector<std::pair<std::string, int>> pending;
    std::unordered_map<std::string, bool>    visited;
    pending.push_back({start_id, 0});
    visited[start_id] = true;
    std::size_t cursor = 0;

    while (cursor < pending.size()) {
        const auto [curr, depth] = pending[cursor++];
        const auto edge_it = unlock_edges_.find(curr);
        if (edge_it == unlock_edges_.end()) continue;
        for (const std::string& nb : edge_it->second) {
            if (visited[nb]) continue;
            visited[nb] = true;
            const int level = depth + 1;
            if (static_cast<int>(levels.size()) < level) levels.push_back({});
            levels[level - 1].push_back(nb);
            pending.push_back({nb, level});
        }
    }
    return levels;
}

// DFS implementation -- returns prerequisite tree as indented lines.
std::vector<std::string> CourseGraph::dfs_prerequisite_tree(const std::string& target_id) const {
    std::vector<std::string>        lines;
    std::unordered_set<std::string> expanded;
    if (!has_course(target_id)) return lines;
    dfs_tree_visit(target_id, 0, expanded, lines);
    return lines;
}

void CourseGraph::dfs_tree_visit(const std::string& course_id, int depth,
                                  std::unordered_set<std::string>& expanded,
                                  std::vector<std::string>& lines) const {
    std::string indent(depth * 2, ' ');

    if (expanded.count(course_id)) {
        lines.push_back(indent + course_id + " (see above)");
        return;
    }

    const auto it = prerequisite_edges_.find(course_id);
    const bool has_prereqs = (it != prerequisite_edges_.end() && !it->second.empty());

    if (has_prereqs) {
        bool all_seen = true;
        for (const std::string& p : it->second)
            if (!expanded.count(p)) { all_seen = false; break; }
        if (all_seen) {
            lines.push_back(indent + course_id + " (see above)");
            return;
        }
    }

    expanded.insert(course_id);
    lines.push_back(indent + course_id);
    if (!has_prereqs) return;

    std::vector<std::string> sorted_prereqs(it->second.begin(), it->second.end());
    std::sort(sorted_prereqs.begin(), sorted_prereqs.end());
    for (const std::string& prereq : sorted_prereqs)
        dfs_tree_visit(prereq, depth + 1, expanded, lines);
}

// Returns full set of transitive prerequisites for a course.
std::unordered_set<std::string> CourseGraph::all_transitive_prereqs(const std::string& course_id) const {
    std::unordered_set<std::string> visited, result;
    collect_prereqs(course_id, visited, result);
    result.erase(course_id);
    return result;
}

void CourseGraph::collect_prereqs(const std::string& course_id,
                                   std::unordered_set<std::string>& visited,
                                   std::unordered_set<std::string>& result) const {
    if (visited.count(course_id)) return;
    visited.insert(course_id);
    const auto it = prerequisite_edges_.find(course_id);
    if (it == prerequisite_edges_.end()) return;
    for (const std::string& prereq : it->second) {
        result.insert(prereq);
        collect_prereqs(prereq, visited, result);
    }
}

// BFS reachability count -- total courses unlockable from start_id.
int CourseGraph::reachable_count(const std::string& start_id) const {
    if (!has_course(start_id)) return 0;
    std::vector<std::string>              pending;
    std::unordered_map<std::string, bool> visited;
    pending.push_back(start_id);
    visited[start_id] = true;
    std::size_t cursor = 0;
    int count = 0;
    while (cursor < pending.size()) {
        const std::string curr = pending[cursor++];
        const auto edge_it = unlock_edges_.find(curr);
        if (edge_it == unlock_edges_.end()) continue;
        for (const std::string& nb : edge_it->second) {
            if (visited[nb]) continue;
            visited[nb] = true;
            ++count;
            pending.push_back(nb);
        }
    }
    return count;
}
