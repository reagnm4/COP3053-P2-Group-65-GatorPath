#pragma once
#include "course.h"
#include <string>
#include <vector>

class CatalogLoader {
public:
    std::vector<CourseRecord> load_from_csv(const std::string& file_path) const;

private:
    std::vector<std::string> split_csv_row(const std::string& row) const;
    static std::string trim(const std::string& s);
    static std::string strip_quotes(const std::string& s);
};
