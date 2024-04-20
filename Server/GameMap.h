#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <unordered_map>

enum CELL_TYPE { GROUND, CONT, PLAYER };

struct CELL
{
	CELL_TYPE cellType;
    DirectX::XMFLOAT3 center; // ¼¿ Áß½É
    int width, height; // ¼¿ Å©±â
    bool isObstacle;

    CELL() : center(DirectX::XMFLOAT3(0.f, 0.f, 0.f)), width(10), height(10),
        cellType(GROUND), isObstacle(false) {}

   /* bool InCell(const DirectX::XMFLOAT3& point)
    {
        
        if (abs(center.x - point.x) > (width/2)) return false;
        return (abs(center.z - point.z) <= (height/2));
    }*/

    bool InCell(const DirectX::BoundingOrientedBox& obj)
    {
        DirectX::BoundingOrientedBox bb;
        bb.Center = center;
        DirectX::XMFLOAT3 extents{ static_cast<float>(width), 1.f, static_cast<float>(height) };
        bb.Extents = extents;

        return bb.Intersects(obj);
    }
};

class GameMap
{
public:
    GameMap(float width, float depth, int gridWidth, int gridHeight)
        : mapWidth(width), mapDepth(depth), cellWidth(gridWidth), cellDepth(gridHeight) {
        // initializeMap();
    }

    ~GameMap() = default;

    void initializeMap();

    void printMap() const;

    void Update();


private:
    std::vector<std::vector<CELL>> cells;
    float mapWidth, mapDepth;
    int cellWidth, cellDepth;
};



 