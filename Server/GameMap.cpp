#include "GameMap.h"
#include "Server.h"

GameMap& GameMap::GetInstance()
{
	static GameMap instance;
	return instance;
}

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


		//for (const auto& pair : data) {
		//	std::cout << "Object Name: " << pair.first << std::endl;
		//	const DirectX::BoundingOrientedBox& box = pair.second;
		//	std::cout << "Center: (" << box.Center.x << ", " << box.Center.y << ", " << box.Center.z << ")" << std::endl;
		//	// std::cout << "Extents: (" << box.Extents.x << ", " << box.Extents.y << ", " << box.Extents.z << ")" << std::endl;
		//}


	}


	// 각 셀을 초기화하고 셀의 위치 및 크기를 설정합니다.
	for (int x = 0; x < cellWidth; ++x) {
		for (int y = 0; y < cellDepth; ++y) {
			// 각 셀의 위치와 크기를 설정
			cells[x][y] = CELL();
			cells[x][y].cellType = GROUND;
			cells[x][y].isObstacle = false;
			cells[x][y].center.x = (x + 0.5f) * mapWidth / cellWidth;
			cells[x][y].center.z = (y + 0.5f) * mapDepth / cellDepth;
			cells[x][y].width = static_cast<int>(mapWidth / cellWidth);
			cells[x][y].height = static_cast<int>(mapDepth / cellDepth);

		}
	}


	for (int x = 0; x < cellWidth; ++x) {
		for (int y = 0; y < cellDepth; ++y) {
			for (auto& datas : data) {
				/*if (cells[x][y].InCell(datas.second.Center)) {
					cells[x][y].isObstacle = true;
				}*/
				if (cells[x][y].InCell(datas.second)) {
					cells[x][y].cellType = CONT;
					cells[x][y].isObstacle = true;
				}
			}

		}
	}
	std::cout << "Map loading 완료\n";

	InitializeNPC();
	std::cout << "NPC loading 완료\n";

}

void GameMap::StartGame()
{
	Server& server = Server::GetInstance();
	
	for (int i = 0; i < cl_ids.size(); ++i) {
		server.clients[i].SetPos(server.random_pos[i]);
		std::cout << "player[" << i << "]의 캐릭터 " << server.clients[i].GetType() << std::endl;
		/*for (auto& npc : npcs) {
			SC_ADD_PLAYER_PACKET p;
			p.size = sizeof(p);
			p.type = SC_ADD_PLAYER;
			p.id = npc.GetId();
			p.position = npc.GetPos();
			p.rotation = npc.GetRotation();
			p.c_type = Robot;
			server.clients[cl_ids[i]].do_send(&p);
		}*/
	}

	for (auto& cl : cl_ids) {
		std::cout << cl << " ";
		SC_GAME_START_PACKET p;
		p.size = sizeof(p);
		p.type = SC_GAME_START;
		server.clients[cl].do_send(&p);
		
	}
	std::cout << std::endl;

	/*for (auto& cl : cl_ids) {
		server.clients[cl].send_add_player_packet(cl, server.clients[cl].GetPos(), server.clients[cl].GetRotation(), server.clients[cl].GetType());
	}*/
	
	


	return;
	//cl_ids.push_back(0); // 임시
	//cl_ids.push_back(1);// 임시
	//cl_ids.push_back(2);// 임시
	
	// std::vector<DirectX::XMFLOAT3> p = BFS(npcs[0].GetPos(), DirectX::XMFLOAT3(50.f, 0.f, 50.f));
	/*for (auto& path : p) {
		// npcs[0].n_path.push(path);
	}*/

	InGame = true;
}

void GameMap::printMap() const
{
	std::cout << "-----PrintMap------------------------\n";
	for (int y = cellDepth - 1; y >= 0; --y) {
		for (int x = 0; x < cellWidth; ++x) {
			const CELL& cell = cells[x][y];
			/*if (cell.isObstacle) {
				switch (cell.cellType)
				{

				case CONT:
					std::cout << "X"; break;
				case CT_NPC:
					std::cout << "N"; break;

				default:
					break;
				}
				continue;
			}*/
			switch (cell.cellType)
			{

			case CONT:
				std::cout << "X"; break;
			case CT_NPC:
				std::cout << "N"; break;
			case GROUND:
				std::cout << "."; break;
			default:
				break;
			}


		}
		std::cout << "\n" << std::endl;
	}

	std::cout << "-----PrintMap------------------------\n";
}

void GameMap::Update()
{

	if (!InGame) return;
	Server& server = Server::GetInstance();

	for (auto& npc : npcs) {
		std::set<std::pair<int, float>> s;
		for (auto& ids : cl_ids) {
			float distance_x = abs(server.clients[ids].GetPos().x - npc.GetPos().x);
			float distance_z = abs(server.clients[ids].GetPos().z - npc.GetPos().z);

			if (distance_x < 300.f && distance_z < 300.f) {
				s.insert({ ids, distance_x * distance_z });
			}
		}
		if (s.size() == 0) continue;
		std::cout << npc.GetId() << ", " << s.begin()->first << std::endl;

		npc.near_player = s.begin()->first;
		
		std::vector<DirectX::XMFLOAT3> path = BFS(npc.GetPos(), server.clients[npc.near_player].GetPos());
		for (auto& p : path) {
			npc.n_path.push(p);
		}


		npc.Update();
	}
	EndGame();
}


/*SC_TEST_PACKET p;

for (int i = 0; i < cl_ids.size(); ++i) {
	server.clients[cl_ids[i]].do_send(&p);
}*/

CELL& GameMap::GetCurrentCell(DirectX::XMFLOAT3 in_pos)
{
	int Cell_x = static_cast<int>((in_pos.x + 0.5f) / (mapWidth / cellWidth));
	int Cell_z = static_cast<int>((in_pos.z + 0.5f) / (mapWidth / cellDepth));

	return cells[Cell_x][Cell_z];
}

std::pair<int, int> GameMap::CoordsToIndex(const DirectX::XMFLOAT3& coords) const
{
	int Cell_x = static_cast<int>((coords.x + 0.5f) / (mapWidth / cellWidth));
	int Cell_z = static_cast<int>((coords.z + 0.5f) / (mapWidth / cellDepth));

	return std::make_pair(Cell_x, Cell_z);
}

void GameMap::InitializeNPC()
{

	std::vector<DirectX::XMFLOAT3> init_pos{ DirectX::XMFLOAT3(500.f, 0.f, 600.f),
	DirectX::XMFLOAT3(900.f, 0.f, 970.f) ,
	DirectX::XMFLOAT3(500.f, 0.f, 967.f) };


	for (int i = 0; i < npcs.size(); ++i) {
		npcs[i].n_state = NPC_INGAME;
		npcs[i].SetId(i + 100);
		npcs[i].SetPos(init_pos[i]);
		CELL& cell = GetCurrentCell(npcs[i].GetPos());

		cell.cellType = CT_NPC;
		std::cout << "NPC[" << npcs[i].GetId() << "] goto " << std::endl;
	}

	/*TIMER_EVENT start;
	start.wakeup_time = std::chrono::system_clock::now() + std::chrono::seconds(3);
	timer_queue.push(start);*/
}

std::vector<DirectX::XMFLOAT3> GameMap::PathFind(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& targetPos) {
	std::vector<DirectX::XMFLOAT3> path;

	// 시작 위치와 목표 위치를 노드의 인덱스로 변환
	std::pair<int, int> startIndex = CoordsToIndex(startPos);
	std::pair<int, int> targetIndex = CoordsToIndex(targetPos);

	// 시작 노드와 목표 노드를 생성합니다.
	Node startNode(startIndex.first, startIndex.second, 0, 0, nullptr);
	Node targetNode(targetIndex.first, targetIndex.second, 0, 0, nullptr);

	std::vector<std::vector<bool>> visited(cellWidth, std::vector<bool>(cellDepth, false));

	// 우선순위 큐를 생성합니다.
	std::priority_queue<Node*, std::vector<Node*>, CompareNodes> openSet;

	// 시작 노드를 우선순위 큐에 추가합니다.
	openSet.push(new Node(startIndex.first, startIndex.second, 0, 0, nullptr));

	visited[startIndex.first][startIndex.second] = true;
	// 열린 목록이 비어있을 때까지 반복합니다.
	while (!openSet.empty()) {
		// 열린 목록에서 현재 노드를 선택합니다.
		Node* currentNode = openSet.top();
		openSet.pop();

		// 현재 노드가 목표 노드인지 확인합니다.
		if (*currentNode == targetNode) {
			// 경로를 재구성합니다.

			Node* current = currentNode;
			while (current != nullptr) {
				path.push_back(cells[current->x][current->z].center);
				current = current->parent;
			}
			std::reverse(path.begin(), path.end());

			// 메모리 해제
			return path;
		}


		// 현재 노드의 이웃 노드를 가져옵니다.
		std::vector<Node> neighbors = GetNeighbors(*currentNode);
		for (Node& neighbor : neighbors) {
			// 이웃 노드가 장애물이거나 이미 방문한 노드인지 확인합니다.
			Node* cnode = &neighbor;

			if (!visited[neighbor.x][neighbor.z] && !cells[neighbor.x][neighbor.z].isObstacle) {
				// 새로운 예상 비용을 계산합니다.
				int newGCost = currentNode->gCost + 1; // 이동 비용은 1로 가정합니다.
				// int newHCost = CalculateManhattanDistance(neighbor, targetNode);
				// 이웃 노드가 이미 열린 목록에 있는지 확인합니다.


				// 새로운 경로가 더 나은 경우 또는 이웃 노드가 아직 열린 목록에 없는 경우에만 업데이트합니다.

			}
		}
		delete currentNode;
	}

	// 경로를 찾지 못한 경우, 빈 벡터 반환


	// 메모리 해제
	return path;
}



std::vector<Node> GameMap::GetNeighbors(const Node& node) const {
	std::vector<Node> neighbors;

	// 현재 노드의 인덱스 값
	int Cell_x = node.x;
	int Cell_z = node.z;

	// 현재 노드의 주변 8개의 셀을 확인하여 이웃 노드를 탐색
	for (int dz = -1; dz <= 1; ++dz) {
		for (int dx = -1; dx <= 1; ++dx) {
			// 현재 셀은 이웃이 아님 (자기 자신)
			if (dz == 0 && dx == 0) {
				continue;
			}

			int newX = Cell_x + dx;
			int newZ = Cell_z + dz;

			// 새로운 좌표가 유효한지 확인하고, 장애물이 아닌지 확인
			if ((abs(dx) + abs(dz) == 1) && newX >= 0 && newX < cellWidth && newZ >= 0 && newZ < cellDepth &&
				!cells[newX][newZ].isObstacle) {
				// 이웃 노드를 생성하여 이웃 목록에 추가
				neighbors.emplace_back(newX, newZ, 0, 0, nullptr); // 부모 노드를 nullptr로 설정
			}
		}
	}

	return neighbors;
}

std::vector<DirectX::XMFLOAT3> GameMap::BFS(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& targetPos) {
	// 시작 위치와 목표 위치를 노드의 인덱스로 변환
	std::pair<int, int> startIndex = CoordsToIndex(startPos);
	std::pair<int, int> targetIndex = CoordsToIndex(targetPos);

	// 시작 노드와 목표 노드를 생성합니다.
	Node startNode(startIndex.first, startIndex.second, 0, 0, nullptr);
	Node targetNode(targetIndex.first, targetIndex.second, 0, 0, nullptr);

	// 방문 여부를 나타내는 2차원 배열을 초기화합니다.
	std::vector<std::vector<bool>> visited(cellWidth, std::vector<bool>(cellDepth, false));

	// 경로를 저장할 벡터를 생성합니다.
	std::vector<DirectX::XMFLOAT3> path;

	// 너비 우선 탐색을 위한 큐를 생성하고 시작 노드를 추가합니다.
	std::queue<Node*> queue;
	queue.push(new Node(startIndex.first, startIndex.second, 0, 0, nullptr));
	visited[startIndex.first][startIndex.second] = true;

	// 이미 방문한 노드의 리스트를 유지합니다.
	std::vector<Node*> visitedNodes;
	// 목표 노드에 도달할 때까지 반복합니다.
	while (!queue.empty()) {
		// 큐에서 현재 노드를 가져옵니다.
		Node* currentNode = queue.front();
		queue.pop();

		// 현재 노드가 목표 노드인지 확인합니다.
		if (*currentNode == targetNode) {
			// 경로를 재구성합니다.
			while (currentNode != nullptr) {
				path.push_back(cells[currentNode->x][currentNode->z].center);
				Node* temp = currentNode;
				currentNode = currentNode->parent;
				// 이미 방문한 노드의 리스트에서 제거합니다.
				visitedNodes.erase(std::remove(visitedNodes.begin(), visitedNodes.end(), temp), visitedNodes.end());
				delete temp; // 더 이상 필요하지 않은 노드를 해제합니다.
			}
			std::reverse(path.begin(), path.end());
			break;
		}

		// 현재 노드의 이웃 노드를 가져와서 큐에 추가합니다.
		std::vector<Node> neighbors = GetNeighbors(*currentNode);
		for (const Node& neighbor : neighbors) {
			int x = neighbor.x;
			int z = neighbor.z;
			// 이웃 노드가 방문되지 않았고, 장애물이 아닌 경우에만 처리합니다.
			if (!visited[x][z] && !cells[x][z].isObstacle) {
				visited[x][z] = true;
				// 이미 방문한 노드인지 확인합니다.
				Node* newNode = nullptr;
				for (Node* visitedNode : visitedNodes) {
					if (visitedNode->x == x && visitedNode->z == z) {
						newNode = visitedNode;
						break;
					}
				}
				if (newNode == nullptr) {
					// 새로운 노드를 생성하여 큐에 추가합니다.
					newNode = new Node(x, z, 0, 0, currentNode);
					visitedNodes.push_back(newNode);
				}
				queue.push(newNode);
			}
		}
	}

	// 사용된 모든 노드를 해제합니다.
	for (Node* node : visitedNodes) {
		delete node;
	}

	return path;
}
