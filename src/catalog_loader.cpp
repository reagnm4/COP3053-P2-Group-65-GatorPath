#include "catalog_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// CSV loader -- parses course catalog into CourseRecord list, handles quoted fields.
std::vector<CourseRecord> CatalogLoader::load_from_csv(const std::string& file_path) const {
    std::ifstream input(file_path);
    std::vector<CourseRecord> records;
    if (!input.is_open()) {
        std::cerr << "[CatalogLoader] Cannot open: " << file_path << '\n';
        return records;
    }
    std::string row;
    bool is_header = true;
    while (std::getline(input, row)) {
        if (row.empty()) continue;
        if (is_header) { is_header = false; continue; }
        const std::vector<std::string> columns = split_csv_row(row);
        if (columns.size() < 5) continue;
        CourseRecord record;
        record.course.id         = strip_quotes(trim(columns[0]));
        record.course.name       = strip_quotes(trim(columns[1]));
        record.course.credits    = columns[2].empty() ? 0 : std::stoi(trim(columns[2]));
        record.prerequisite_id   = strip_quotes(trim(columns[3]));
        record.course.difficulty = columns[4].empty() ? 0.0 : std::stod(trim(columns[4]));
        if (record.prerequisite_id == "NONE" || record.prerequisite_id == "none")
            record.prerequisite_id.clear();
        records.push_back(record);
    }
    return records;
}

std::vector<std::string> CatalogLoader::split_csv_row(const std::string& row) const {
    std::vector<std::string> columns;
    std::string cell;
    bool in_quotes = false;
    for (std::size_t i = 0; i < row.size(); ++i) {
        char c = row[i];
        if (c == '"') {
            if (in_quotes && i + 1 < row.size() && row[i + 1] == '"') { cell += '"'; ++i; }
            else in_quotes = !in_quotes;
        } else if (c == ',' && !in_quotes) { columns.push_back(cell); cell.clear(); }
        else cell += c;
    }
    columns.push_back(cell);
    return columns;
}

std::string CatalogLoader::trim(const std::string& s) {
    const auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return {};
    return s.substr(start, s.find_last_not_of(" \t\r\n") - start + 1);
}

std::string CatalogLoader::strip_quotes(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') return s.substr(1, s.size() - 2);
    return s;
}
