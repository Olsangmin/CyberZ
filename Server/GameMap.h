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
#include <random>

enum CELL_TYPE { GROUND, CONT, PLAYER, CT_NPC };

struct CELL
{
	CELL_TYPE cellType;
    DirectX::XMFLOAT3 center; // 셀 중심
    int width, height; // 셀 크기
    bool isObstacle;

    CELL() : center(DirectX::XMFLOAT3(0.f, 0.f, 0.f)), width(10), height(10),
        cellType(GROUND), isObstacle(false) {}

    bool InCell(const DirectX::BoundingOrientedBox& obj) const
    {
        DirectX::BoundingOrientedBox bb{};
        bb.Center = center;
        DirectX::XMFLOAT3 extents{ static_cast<float>(width/2), 3.0f, static_cast<float>(height/2) };
        
        bb.Extents = extents;

        return bb.Intersects(obj);
    }
    bool operator==(const CELL& rhs) const
    {
        // return (x == other.x && z == other.z);
        if (1.f < abs(center.x - rhs.center.x)) return false;
        return abs(center.x - rhs.center.x) <= 1.f;

    }
};


enum GAME_STATE {NOGAME, LOADING, STAGE1, STAGE2};

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

    void ChangeToMap2();

    
    void StartGame();
    void EndGame() { InGame = false; }
    void PlayGame() { 
        SetStage(GAME_STATE::STAGE1);
        InGame = true; 
        Start_time = std::chrono::steady_clock::now();
        End_time = Start_time + std::chrono::seconds(10);
    }
    void SetStage(GAME_STATE state) { game_state = state; }
    GAME_STATE GetStage() { return game_state; }
    bool is_InGame() const { return InGame; }

    void printMap() const;

    void Update(int tick);

    void UpdateS1();
    void UpdateS2();

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

    DirectX::XMFLOAT3 GetRandomPos(DirectX::XMFLOAT3 pos)
    {
        DirectX::XMFLOAT3 next_pos;

        int curr_sertor = getSector(pos);
        
        std::pair<int, int> index = CoordsToIndex(pos);
        while (true) {
            int x = index.first, y = index.second;
            std::uniform_int_distribution uid{ 0, 3 };
            switch (uid(m_m64RandomEngine))
            {
            case 0: x++; break;
            case 1: x--; break;
            case 2: y++; break;
            case 3: y--; break;

            default:
                break;
            }
            
            if (x >= 100 || y >= 100) continue;

            if (false == cells[x][y].isObstacle) {
                next_pos = cells[x][y].center;
                if (curr_sertor != getSector(next_pos)) continue;
                break;
            }
        }

        return next_pos;
    }

    std::vector<Node> GetNeighbors(const Node& node) const;

    std::vector<Node> GetNeighbors(const Node& node, const Node& targetNode) const {
        std::vector<Node> neighbors;

        int Cell_x = node.x;
        int Cell_z = node.z;

        for (int dz = -1; dz <= 1; ++dz) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dz == 0 && dx == 0) {
                    continue;
                }

                int newX = Cell_x + dx;
                int newZ = Cell_z + dz;

                if (newX >= 0 && newX < cellWidth && newZ >= 0 && newZ < cellDepth &&
                    !cells[newX][newZ].isObstacle) {
                    float g = node.gCost + 1.0f; // 시작 노드로부터의 거리는 항상 1
                    float h = CalculateHeuristic(newX, newZ, targetNode);
                    neighbors.emplace_back(newX, newZ, g, h, nullptr);
                }
            }
        }

        return neighbors;
    }

    float CalculateHeuristic(int x, int z, const Node& targetNode) const {
        // 맨해튼 거리를 사용하거나 유클리드 거리를 사용할 수 있음
        return std::abs(x - targetNode.x) + std::abs(z - targetNode.z); // 맨해튼 거리
        // return std::sqrt((x - targetNode.x) * (x - targetNode.x) + (z - targetNode.z) * (z - targetNode.z)); // 유클리드 거리
    }

    float Distance_float(DirectX::XMFLOAT3 n_pos, DirectX::XMFLOAT3 p_pos)
    {
        // return std::abs(n_pos.x - p_pos.x) + std::abs(n_pos.z - p_pos.z);
        return std::sqrt((n_pos.x - p_pos.x) * (n_pos.x - p_pos.x) + (n_pos.z - p_pos.z) * (n_pos.z - p_pos.z));
    }

    std::array<NPC, NUM_NPC> npcs;
    NPC BossNpc;


    std::array<S2_COM_STATE, 5> coms = { TURNOFF };

    std::vector<int> cl_ids;

    int getSector(const DirectX::XMFLOAT3& pos)
    {
        int num{};
        int x = static_cast<int>((pos.x + 0.5f) / (mapWidth / 3));
        int z = static_cast<int>((pos.z + 0.5f) / (mapDepth / 3));
        
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

    bool cool_down{ false };
    std::mutex cool_lock;
    std::vector<std::vector<CELL>> cells;
private:
    float mapWidth, mapDepth;
    int cellWidth, cellDepth;
    bool InGame;

    std::random_device m_RandomDevice;
    std::mt19937_64 m_m64RandomEngine{ m_RandomDevice() };


    std::chrono::steady_clock::time_point Start_time;
        
    std::chrono::steady_clock::time_point End_time;

    std::chrono::duration<double> Remain_time;
    GAME_STATE game_state;
};





 