#pragma once
#include <iostream>
#include <vector>
#include <DirectXMath.h>

enum CELL_TYPE { GROUND, CONT, PLAYER };

struct CELL
{
	CELL_TYPE cellType;
    DirectX::XMFLOAT3 center; // ¼¿ Áß½É
    int width, height; // ¼¿ Å©±â
    bool isObstacle;

    CELL() : center(DirectX::XMFLOAT3(0.f, 0.f, 0.f)), width(10), height(10),
        cellType(GROUND), isObstacle(false) {}
};

class GameMap
{
public:
    GameMap(float width, float depth, int gridWidth, int gridHeight)
        : mapWidth(width), mapDepth(depth), cellWidth(gridWidth), cellDepth(gridHeight) {
        initializeMap();
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



