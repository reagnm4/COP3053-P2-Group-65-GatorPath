#pragma once
#include "course_node.h"
#include <vector>
using namespace std;

class MaxHeap {
    vector<CourseNode> data;
    int parent(int i) { return (i - 1) / 2; }
    int left(int i)   { return 2 * i + 1; }
    int right(int i)  { return 2 * i + 2; }

    void heapify_up(int i) {
        while (i > 0 && data[parent(i)] < data[i]) {
            swap(data[parent(i)], data[i]);
            i = parent(i);
        }
    }

    void heapify_down(int i) {
        int size = data.size();
        int largest = i;
        if (left(i)  < size && data[largest] < data[left(i)])  largest = left(i);
        if (right(i) < size && data[largest] < data[right(i)]) largest = right(i);
        if (largest != i) {
            swap(data[i], data[largest]);
            heapify_down(largest);
        }
    }

public:
    void insert(CourseNode course) {
        data.push_back(course);
        heapify_up(data.size() - 1);
    }

    CourseNode extract_max() {
        CourseNode top = data[0];
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) heapify_down(0);
        return top;
    }

    CourseNode& peek() { return data[0]; }
    bool empty()       { return data.empty(); }
    int  size()        { return data.size(); }

    vector<CourseNode> top_n(int n) {
        MaxHeap copy = *this;
        vector<CourseNode> result;
        while (!copy.empty() && n-- > 0)
            result.push_back(copy.extract_max());
        return result;
    }
};
