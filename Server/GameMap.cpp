#include "GameMap.h"

void GameMap::initializeMap()
{
	// `cellWidth`와 `cellDepth` 크기로 2D 배열을 생성합니다.
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
			// 줄의 시작 공백을 제거
			line.erase(0, line.find_first_not_of(" \t\n\r"));

			// <Frame> 태그 처리
			if (line.find("<Frame>:") == 0) {
				objName = line.substr(9);
			}

			// <Bounds> 태그 처리
			else if (line.find("<Bounds>:") == 0) {
				// <Bounds>: 뒤의 값을 추출
				std::istringstream iss(line.substr(9));
				float values[6];

				// 올바르게 6개의 실수 값 추출
				for (int i = 0; i < 6; i++) {
					if (!(iss >> values[i])) {
						std::cerr << "실수 값 추출 오류 발생" << std::endl;
						exit(-1);
					}
				}

				// BoundingOrientedBox에 값 설정
				DirectX::BoundingOrientedBox box;
				box.Center = DirectX::XMFLOAT3(values[0], values[1], values[2]);
				box.Extents = DirectX::XMFLOAT3(values[3], values[4], values[5]);

				// 데이터 저장
				data[objName] = box;
			}
		}

		//// 데이터 출력 (확인용)
		//for (const auto& pair : data) {
		//	std::cout << "Object Name: " << pair.first << std::endl;
		//	const DirectX::BoundingOrientedBox& box = pair.second;
		//	std::cout << "Center: (" << box.Center.x << ", " << box.Center.y << ", " << box.Center.z << ")" << std::endl;
		//	std::cout << "Extents: (" << box.Extents.x << ", " << box.Extents.y << ", " << box.Extents.z << ")" << std::endl;
		//}


	}


	// 각 셀을 초기화하고 셀의 위치 및 크기를 설정합니다.
	for (int x = 0; x < cellWidth; ++x) {
		for (int y = 0; y < cellDepth; ++y) {
			// 각 셀의 위치와 크기를 설정
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
	// printMap();
}
