#include "GameMap.h"

void GameMap::initializeMap()
{
	// `cellWidth`�� `cellDepth` ũ��� 2D �迭�� �����մϴ�.
	cells.resize(cellWidth, std::vector<CELL>(cellDepth));

	std::unordered_map<std::string, DirectX::BoundingOrientedBox> data;

	
	

	std::ifstream in{ "Resource/Map/Map_Object_Info.txt" };
	if (!in) {
		std::cout << "File Error!" << std::endl;
		exit(-1);
	}
	else {
		std::string line;
		std::string objName;

		while (std::getline(in, line)) {
			// ���� ���� ������ ����
			line.erase(0, line.find_first_not_of(" \t\n\r"));

			// <Frame> �±� ó��
			if (line.find("<Frame>:") == 0) {
				objName = line.substr(9);
			}

			// <Bounds> �±� ó��
			else if (line.find("<Bounds>:") == 0) {
				// <Bounds>: ���� ���� ����
				std::istringstream iss(line.substr(9));
				float values[6];

				// �ùٸ��� 6���� �Ǽ� �� ����
				for (int i = 0; i < 6; i++) {
					if (!(iss >> values[i])) {
						std::cerr << "�Ǽ� �� ���� ���� �߻�" << std::endl;
						exit(-1);
					}
				}

				// BoundingOrientedBox�� �� ����
				DirectX::BoundingOrientedBox box;
				box.Center = DirectX::XMFLOAT3(values[0], values[1], values[2]);
				box.Extents = DirectX::XMFLOAT3(values[3], values[4], values[5]);

				// ������ ����
				data[objName] = box;
			}
		}

		//// ������ ��� (Ȯ�ο�)
		//for (const auto& pair : data) {
		//	std::cout << "Object Name: " << pair.first << std::endl;
		//	const DirectX::BoundingOrientedBox& box = pair.second;
		//	std::cout << "Center: (" << box.Center.x << ", " << box.Center.y << ", " << box.Center.z << ")" << std::endl;
		//	std::cout << "Extents: (" << box.Extents.x << ", " << box.Extents.y << ", " << box.Extents.z << ")" << std::endl;
		//}


	}


	// �� ���� �ʱ�ȭ�ϰ� ���� ��ġ �� ũ�⸦ �����մϴ�.
	for (int x = 0; x < cellWidth; ++x) {
		for (int y = 0; y < cellDepth; ++y) {
			// �� ���� ��ġ�� ũ�⸦ ����
			cells[x][y] = CELL();
			cells[x][y].cellType = GROUND;
			cells[x][y].center.x = (x + 0.5f) * mapWidth / cellWidth;
			cells[x][y].center.z = (y + 0.5f) * mapDepth / cellDepth;
			cells[x][y].width = static_cast<int>(mapWidth / cellWidth);
			cells[x][y].height = static_cast<int>(mapDepth / cellDepth);
			
		}
	}

	
}

void GameMap::printMap() const
{
	return;
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
	// printMap();
}
