# GatorPath — Terminal-Based Course Prerequisite Planner

**Team:** GatorPath  
**Members:** Ovidio Calvet · Pedro Villas Boas Amorim · Reagan Moore  
**Course:** COP3530 — Project 2

---

## Overview

GatorPath is an offline, terminal-based tool that models a university course catalog as a directed graph where nodes are courses and edges are prerequisite relationships. Students can explore what courses a completed course unlocks, trace the full prerequisite chain for any target course, receive personalized smart recommendations, and validate their course plan — all without needing internet access or an advisor.

---

## Features

| # | Feature | Algorithm |
|---|---------|-----------|
| 1 | View all courses unlocked by a given course | BFS (level-by-level) |
| 2 | Trace the full prerequisite chain for a target course | DFS (recursive) |
| 3 | Get top-N recommended next courses | Custom Max-Heap |
| 4 | Load a personal completed-course list | — |
| 5 | Validate a plan for missing prereqs & cycles | DFS + cycle detection |

---

## Project Structure

```
gatorpath/
├── include/
│   ├── course.h              # Course and CourseRecord structs
│   ├── course_node.h         # CourseNode struct (used by MaxHeap)
│   ├── max_heap.h            # Custom max-heap (array-backed, no STL)
│   ├── catalog_loader.h      # CSV parser
│   ├── course_graph.h        # Directed adjacency-list graph
│   ├── course_recommender.h  # Max-heap recommendation engine
│   ├── plan_validator.h      # Missing prereq + cycle detection
│   └── gator_path_app.h      # Terminal UI controller
├── src/
│   ├── main.cpp
│   ├── gator_path_app.cpp
│   ├── catalog_loader.cpp
│   ├── course_graph.cpp
│   ├── course_recommender.cpp
│   └── plan_validator.cpp
├── gatorpath_dataset.csv     # 105,000+ prerequisite tuples
└── Makefile
```

---

## Build & Run

### Requirements
- `g++` with C++17 support (GCC 7+ or Clang 5+)
- `make`

### Build

```bash
make
```

### Run

```bash
./bin/gatorpath
```

The binary expects `gatorpath_dataset.csv` in the working directory (the project root).

### Clean

```bash
make clean
```

---

## Sample Session

```
Loading course catalog from "gatorpath_dataset.csv"...
Catalog loaded: 88501 unique courses indexed.

===============================
        GATORPATH PLANNER
===============================
[1] View all courses unlocked by a course (BFS)
[2] Trace prerequisite chain for a course (DFS)
[3] Get top N recommended next courses (Max-Heap)
[4] Load my completed courses
[5] Check if my plan is valid (cycle/missing prereq detection)
[6] Exit
-------------------------------
Enter choice: 4
Enter completed course ids separated by commas
(e.g. COP3502, MAC2311, COT3100): MAC1147, COP3502C

Enter choice: 3
How many recommendations do you want? [default 5]: 5

Top 5 recommended next courses:
   1. MAC2311      | unlocks  9312 course(s) | difficulty: 4.5  [Analytic Geometry and Calculus 1]
   2. COP3530      | unlocks  7200 course(s) | difficulty: 3.8  [Data Structures and Algorithms]
   ...

Enter choice: 1
Enter a course id: COP3530
Courses unlocked by COP3530 (BFS):
  Level  1: COP4600, COP4531, CDA3101
  Level  2: COP4020, COT4400, CDA4205
  Total unlocked: 24 course(s)  [0.01 ms]

Enter choice: 2
Enter a target course id: CAP4621
Full prerequisite chain for CAP4621 (DFS):
  CAP4621 -> COP4531 -> COP3530 -> COP3502C -> MAC1147
  Deepest chain length: 5 course(s)  [0.00 ms]

Enter choice: 5
Your completed-course list is VALID.
  - All prerequisites are satisfied.
  - No prerequisite cycles detected.
```

---

## Data

- **Source:** Real UF course codes from `catalog.ufl.edu/UGRD/courses/` combined with synthetically generated prerequisite relationships to reach the 100,000-tuple scale requirement.
- **Schema:** `course_id, course_name, credits, prerequisite_id, difficulty`
- **Scale:** 105,351 rows → 88,501 unique course nodes in the graph.

---

## Algorithm Details

### BFS — Unlock Explorer (Option 1)
Implemented from scratch as a vector-with-cursor (no `std::queue`). Starting from a completed course, BFS explores the unlock graph level by level, collecting every reachable course and the depth at which it becomes available.

**Complexity:** O(V + E)

### DFS — Prerequisite Chain (Option 2)
Recursive DFS walking backwards through `prerequisite_edges`. Returns the full chain from the target course to its deepest root dependency.

**Complexity:** O(V + E)

### Max-Heap — Recommendation Engine (Option 3)
A custom array-backed max-heap (`max_heap.h`) with `heapify_up` and `heapify_down`. Priority key: **unlock_count DESC**, tie-broken by **difficulty ASC**. A direct-prerequisite index is pre-built in O(E) to check eligibility without running a DFS per candidate. Each eligible course's reachable count is computed via BFS.

**Complexity:** O(E + V log V)

### Cycle & Prerequisite Validation (Option 5)
Missing prerequisites are found by checking each completed course's DFS prerequisite chain against the completed set. Cycles are detected using the classic WHITE/GRAY/BLACK DFS colouring on the induced subgraph of completed courses.

**Complexity:** O(V + E)

---

## Distribution of Responsibility

| Member | Ownership |
|--------|-----------|
| Pedro Villas Boas Amorim | BFS traversal, DFS traversal, CSV data parsing |
| Ovidio Calvet | Graph data structure, prerequisite chain analysis, plan validation |
| Reagan Moore | Max-heap priority queue, terminal UI, recommendation engine |

---

## References

- University of Florida. *Undergraduate Course Catalog*. https://catalog.ufl.edu/UGRD/courses/
- Cormen, T. et al. *Introduction to Algorithms*, 3rd ed. MIT Press.
- Sedgewick, R. & Wayne, K. *Algorithms*, 4th ed. Addison-Wesley.
