#include "GameMap.h"

void GameMap::initializeMap()
{
    // `cellWidth`�� `cellDepth` ũ��� 2D �迭�� �����մϴ�.
    cells.resize(cellWidth, std::vector<CELL>(cellDepth));

    // �� ���� �ʱ�ȭ�ϰ� ���� ��ġ �� ũ�⸦ �����մϴ�.
    for (int x = 0; x < cellWidth; ++x) {
        for (int y = 0; y < cellDepth; ++y) {
            // �� ���� ��ġ�� ũ�⸦ ����
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
    std::cout << "���� �� ��:\n";
    for (int x = 0; x < cellWidth; ++x) {
        for (int y = 0; y < cellDepth; ++y) {
            const CELL& cell = cells[x][y];
            std::cout << "CELL �߽���ǥ(" << cell.center.x << ", " << cell.center.z << ") - "
                << "����: " << cell.width << ", ����: " << cell.height << "\n";
        }
    }
}

void GameMap::Update()
{
    std::cout << "��ü �� ���� : " << cellWidth * cellDepth << "��ֹ� ����: " << std::endl;
    printMap();
}
