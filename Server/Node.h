#pragma once

struct Node
{
    int x, z;
    int gCost, hCost;
    Node* parent;

    Node(int _x, int _z, int _gCost, int _hCost, Node* _parent)
        : x(_x), z(_z), gCost(_gCost), hCost(_hCost), parent(_parent) {}

    bool operator==(const Node& other) const {
        return (x == other.x && z == other.z);
    }

    bool operator<(const Node& other) const {
        return (gCost + hCost) > (other.gCost + other.hCost);
    }
    
};

//int CalculateManhattanDistance(const Node& nodeA, const Node& nodeB) {
//    return abs(nodeA.x - nodeB.x) + abs(nodeA.z - nodeB.z);
//}

struct CompareNodes {
    bool operator()(const Node* a, const Node* b) const {
        return (a->gCost + a->hCost) > (b->gCost + b->hCost);
    }
};
