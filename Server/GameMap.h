#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <unordered_map>
#include <atomic>
#include "NPC.h"
#include "Node.h"

#include <set>
#include <queue>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <functional>
#include <ranges>

enum CELL_TYPE { GROUND, CONT, PLAYER, CT_NPC };

struct CELL
{
	CELL_TYPE cellType;
    DirectX::XMFLOAT3 center; // 셀 중심
    int width, height; // 셀 크기
    bool isObstacle;

    CELL() : center(DirectX::XMFLOAT3(0.f, 0.f, 0.f)), width(10), height(10),
        cellType(GROUND), isObstacle(false) {}

    bool InCell(const DirectX::BoundingOrientedBox& obj)
    {
        DirectX::BoundingOrientedBox bb;
        bb.Center = center;
        DirectX::XMFLOAT3 extents{ static_cast<float>(width/2), 1.f, static_cast<float>(height/2) };
        bb.Extents = extents;

        return bb.Intersects(obj);
    }
};


class GameMap
{
public:
    GameMap() = default;
    GameMap(float width, float depth, int gridWidth, int gridHeight)
        : mapWidth(width), mapDepth(depth), cellWidth(gridWidth), cellDepth(gridHeight), InGame(false){
        // initializeMap();
    }

    ~GameMap() = default;

    static GameMap& GetInstance();

    void initializeMap();

    void StartGame();
    void EndGame() { InGame = false; }
    bool is_InGame() const { return InGame; }

    void printMap() const;

    void Update();

    CELL& GetCurrentCell(DirectX::XMFLOAT3 in_pos);

    std::pair<int, int> CoordsToIndex(const DirectX::XMFLOAT3& coords) const;

    void InitializeNPC();

    std::vector<DirectX::XMFLOAT3> PathFind(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& targetPos);
    std::vector<DirectX::XMFLOAT3> BFS(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& targetPos);


    int GetDistance(const Node& nodeA, const Node& nodeB) const {
        int dx = nodeA.x - nodeB.x;
        int dz = nodeA.z - nodeB.z;
        return static_cast<int>(std::sqrt(dx * dx + dz * dz));
    }
    bool IsInOpenSet(Node* node, const std::priority_queue<Node*, std::vector<Node*>, CompareNodes>& openSet) {
        std::priority_queue<Node*, std::vector<Node*>, CompareNodes> tempOpenSet = openSet; // 복사본 생성
        while (!tempOpenSet.empty()) {
            Node* currentNode = tempOpenSet.top();
            tempOpenSet.pop();
            if (*currentNode == *node) {
                return true;
            }
        }
        return false;
    }

    std::vector<Node> GetNeighbors(const Node& node) const;

    std::array<NPC, NUM_NPC> npcs;

    std::vector<int> cl_ids;

    int getSector(const DirectX::XMFLOAT3& pos)
    {
        int num{};
        int x = static_cast<int>((pos.x + 0.5f) / (mapWidth / 3));
        int z = static_cast<int>((pos.z + 0.5f) / (mapWidth / 3));
        
        switch (z)
        {
        case 0:
            return x;
        case 1:
            return x + 3;
        case 2:
            return x + 6;

        default:
            std::cout << "?!?!" << std::endl;
            break;
        }
    }


private:
    std::vector<std::vector<CELL>> cells;
    float mapWidth, mapDepth;
    int cellWidth, cellDepth;
    bool InGame;

};





 