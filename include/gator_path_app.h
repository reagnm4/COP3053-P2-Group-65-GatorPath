#pragma once
#include "catalog_loader.h"
#include "course_graph.h"
#include "course_recommender.h"
#include "plan_validator.h"
#include <string>
#include <vector>
#include <unordered_set>

class GatorPathApp {
public:
    void run(const std::string& exe_path = "");

private:
    CatalogLoader     loader_;
    CourseGraph       graph_;
    CourseRecommender recommender_;
    PlanValidator     validator_;

    std::string              loaded_catalog_path_;
    std::vector<std::string> completed_courses_;

    static std::string resolve_csv_path(const std::string& exe_path);
    void show_menu() const;
    void seed_sample_data();
    void handle_bfs()                     const;
    void handle_dfs()                     const;
    void handle_recommendations()         const;
    void handle_load_completed_courses();
    void handle_validation()              const;

    static std::vector<std::string> split_codes(const std::string& line);
};
