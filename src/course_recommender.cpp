#include "course_recommender.h"
#include <unordered_set>
#include <algorithm>

// Max-heap recommender -- ranks courses on the direct frontier of completed courses by unlock count.
std::vector<Recommendation> CourseRecommender::recommend_next_courses(
    const CourseGraph&              graph,
    const std::vector<std::string>& completed_courses,
    int                             limit) const {

    if (completed_courses.empty()) return {};

    std::unordered_set<std::string> completed_set(completed_courses.begin(), completed_courses.end());
    const auto& all_prereqs = graph.all_prereqs();

    std::unordered_set<std::string> already_covered(completed_set);
    for (const std::string& c : completed_courses) {
        const auto trans = graph.all_transitive_prereqs(c);
        already_covered.insert(trans.begin(), trans.end());
    }

    std::unordered_set<std::string> frontier;
    for (const std::string& c : completed_courses) {
        auto it = graph.edges().find(c);
        if (it == graph.edges().end()) continue;
        for (const std::string& next : it->second) frontier.insert(next);
    }

    std::vector<std::pair<std::string, const Course*>> candidates;
    for (const std::string& course_id : frontier) {
        if (already_covered.count(course_id)) continue;
        const Course* course = graph.find_course(course_id);
        if (!course) continue;
        auto it = all_prereqs.find(course_id);
        if (it != all_prereqs.end()) {
            bool all_met = true;
            for (const std::string& prereq : it->second)
                if (!completed_set.count(prereq)) { all_met = false; break; }
            if (!all_met) continue;
        }
        candidates.push_back({course_id, course});
    }

    std::sort(candidates.begin(), candidates.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });

    MaxHeap heap;
    for (const auto& [course_id, course] : candidates) {
        CourseNode node;
        node.course_id    = course_id;
        node.course_name  = course->name;
        node.credits      = course->credits;
        node.unlock_count = graph.reachable_count(course_id);
        node.difficulty   = course->difficulty;
        heap.insert(node);
    }

    const int n = (limit > 0) ? limit : heap.size();
    const std::vector<CourseNode> top = heap.top_n(n);

    std::vector<Recommendation> result;
    for (const CourseNode& node : top)
        result.push_back({node.course_id, node.course_name, node.unlock_count, node.difficulty});
    return result;
}
