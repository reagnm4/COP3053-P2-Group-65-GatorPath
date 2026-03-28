# GatorPath — Terminal-Based Course Prerequisite Planner

**Team:** GatorPath  
**Members:**  
- Ovidio Calvet  
- Pedro Villas Boas Amorim  
- Reagan Moore  

**Course:** COP3530 — Project 2  

---

## 📌 Overview

GatorPath is a terminal-based application that models a university course catalog as a directed graph.  

- Nodes = Courses  
- Edges = Prerequisite relationships  

The system allows students to:
- Explore which courses they unlock
- Trace prerequisite chains
- Get smart recommendations
- Validate course plans  

All functionality is implemented **offline without external APIs**.

---

## 🚀 Features

| Feature | Description | Algorithm |
|--------|------------|----------|
| Unlock Courses | Shows all courses unlocked by a given course | BFS |
| Prerequisite Chain | Displays full prerequisite chain | DFS |
| Course Recommendations | Suggests top-N next courses | Custom Max Heap |
| Load Completed Courses | Stores user progress | — |
| Plan Validation | Detects missing prereqs + cycles | DFS + Cycle Detection |

---

## 🧠 Algorithms & Data Structures

- **Graph Representation:** Adjacency List  
- **BFS:** Used for level-by-level unlock exploration  
- **DFS:** Used for prerequisite chain tracing  
- **Max Heap:** Custom implementation (array-based, no STL priority queue)  
- **Cycle Detection:** DFS with coloring (WHITE/GRAY/BLACK)  

---

## 📂 Project Structure
gatorpath/
├── include/
│ ├── course.h
│ ├── course_node.h
│ ├── max_heap.h
│ ├── catalog_loader.h
│ ├── course_graph.h
│ ├── course_recommender.h
│ ├── plan_validator.h
│ └── gator_path_app.h
├── src/
│ ├── main.cpp
│ ├── gator_path_app.cpp
│ ├── catalog_loader.cpp
│ ├── course_graph.cpp
│ ├── course_recommender.cpp
│ └── plan_validator.cpp
├── gatorpath_dataset.csv
└── Makefile


---

## ⚙️ How to Run

### Requirements
- C++17 compiler (`g++` recommended)
- `make`

### Steps

1. Clone the repository
```bash
git clone <https://github.com/reagnm4/COP3053-P2-Group-65-GatorPath>
cd gatorpath
make
./bin/gatorpath
Data
Source: UF Course Catalog 
Size:
105,000+ prerequisite relationships
88,000+ unique courses
Format:
course_id, course_name, credits, prerequisite_id, difficulty
Complexity Analysis
Feature	Complexity
BFS Unlock	O(V + E)
DFS Chain	O(V + E)
Recommendations	O(E + V log V)
Validation	O(V + E)
Contributions
Member	Work Done
Pedro Villas Boas Amorim	BFS, DFS, CSV parsing
Ovidio Calvet	Graph structure, validation
Reagan Moore	Max heap, UI, recommendations
Demo Video

<         YOUTUBE LINK       >

[https://youtube.com/your-video](https://www.youtube.com/watch?v=D_KMIuIqsMg)
 Repository

https://github.com/reagnm4/COP3053-P2-Group-65-GatorPath

References
UF Course Catalog: https://catalog.ufl.edu/UGRD/courses/
Introduction to Algorithms — Cormen et al.
Algorithms — Sedgewick & Wayne

