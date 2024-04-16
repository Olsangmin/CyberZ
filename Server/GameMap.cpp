#include "GameMap.h"

void GameMap::initializeMap()
{
    // `cellWidth`와 `cellDepth` 크기로 2D 배열을 생성합니다.
    cells.resize(cellWidth, std::vector<CELL>(cellDepth));

    // 각 셀을 초기화하고 셀의 위치 및 크기를 설정합니다.
    for (int x = 0; x < cellWidth; ++x) {
        for (int y = 0; y < cellDepth; ++y) {
            // 각 셀의 위치와 크기를 설정
            cells[x][y] = CELL();
            cells[x][y].center.x = (x + 0.5f) * mapWidth / cellWidth;
            cells[x][y].center.z = (y + 0.5f) * mapDepth / cellDepth;
            cells[x][y].width = static_cast<int>(mapWidth / cellWidth);
            cells[x][y].height = static_cast<int>(mapDepth / cellDepth);
        }
    }
}

void GameMap::printMap() const
{
    std::cout << "맵의 각 셀:\n";
    for (int x = 0; x < cellWidth; ++x) {
        for (int y = 0; y < cellDepth; ++y) {
            const CELL& cell = cells[x][y];
            std::cout << "CELL 중심좌표(" << cell.center.x << ", " << cell.center.z << ") - "
                << "가로: " << cell.width << ", 세로: " << cell.height << "\n";
        }
    }
}

void GameMap::Update()
{
    std::cout << "전체 셀 개수 : " << cellWidth * cellDepth << "장애물 갯수: " << std::endl;
    printMap();
}
