cmake_minimum_required(VERSION 3.16)
project(gatorpath)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(gatorpath
    src/main.cpp
    src/gator_path_app.cpp
    src/catalog_loader.cpp
    src/course_graph.cpp
    src/course_recommender.cpp
    src/plan_validator.cpp
)

# Tell CLion to run the executable from the project root so it finds the CSV.
set_target_properties(gatorpath PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
