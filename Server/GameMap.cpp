#include "GameMap.h"
#include "Server.h"

GameMap& GameMap::GetInstance()
{
	static GameMap instance;
	return instance;
}

void GameMap::initializeMap()
{
	game_state = GAME_STATE::NOGAME;
	using namespace DirectX;
	// `cellWidth`와 `cellDepth` 크기로 2D 배열을 생성합니다.
	cells.resize(cellWidth, std::vector<CELL>(cellDepth));

	std::unordered_map<std::string, DirectX::BoundingOrientedBox> data;


	std::ifstream in{ "Resource/Map/AllMap_M_info.txt" };
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
				DirectX::BoundingOrientedBox box{};
				box.Center = DirectX::XMFLOAT3(values[0], values[1], values[2]);
				box.Extents = DirectX::XMFLOAT3(values[3], values[4], values[5]);

				// 데이터 저장
				data[objName] = box;

			}
			else if (line.find("<Rotation>:") == 0) {
				// <Rotation>: 뒤의 값을 추출
				std::istringstream iss(line.substr(11));
				char discard;
				float rotX, rotY, rotZ{};

				if (!(iss >> discard >> rotX >> discard >> rotY >> discard >> rotZ >> discard)) {					
					std::cerr << "회전 값 추출 오류 발생" << std::endl;
					exit(-1);
				}
				

				float angleX = XMConvertToRadians(rotX);
				float angleY = XMConvertToRadians(rotY);
				float angleZ = XMConvertToRadians(rotZ);
				XMVECTOR quaternionX = DirectX::XMQuaternionRotationRollPitchYaw(angleX, 0.0f, 0.0f);
				XMVECTOR quaternionY = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, angleY, 0.0f);
				XMVECTOR quaternionZ = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, angleZ);
				
				XMVECTOR quaternion = DirectX::XMQuaternionMultiply(quaternionX, DirectX::XMQuaternionMultiply(quaternionY, quaternionZ));
				
				XMFLOAT4 orientation{};
				DirectX::XMStoreFloat4(&orientation, quaternion);

				data[objName].Orientation = orientation;

				XMVECTOR rotatedExtents = XMVectorSet(data[objName].Extents.x, data[objName].Extents.y, data[objName].Extents.z, 0.f);
				XMVECTOR orientationQuat = XMVectorSet(orientation.x, orientation.y, orientation.z, orientation.w);
				
				XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(orientationQuat);
				XMMATRIX inverseRotationMatrix = XMMatrixInverse(nullptr, rotationMatrix);
				XMVECTOR originalExtents = XMVector3Transform(rotatedExtents, inverseRotationMatrix);
				
				originalExtents = XMVectorAbs(originalExtents);

				XMFLOAT3 originalExtentsFloat;
				XMStoreFloat3(&originalExtentsFloat, originalExtents);
				
				data[objName].Extents = originalExtentsFloat;
			}
				
		}


		/*for (const auto& pair : data) {
			std::cout << "Object Name: " << pair.first << std::endl;
			const DirectX::BoundingOrientedBox& box = pair.second;
			std::cout << "Center: (" << box.Center.x << ", " << box.Center.y << ", " << box.Center.z << ")" << std::endl;
			std::cout << "Orientation: (" << box.Orientation.x << ", " << box.Orientation.y << ", " << box.Orientation.z << ") - " << box.Orientation.w << std::endl;
		}*/


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

	for (int i = 0; i < MissionPos.size(); ++i)
	{
		std::string ObjName = "Mission " + std::to_string(i);

		DirectX::BoundingOrientedBox box{};
		box.Center = MissionPos[i];
		box.Extents = DirectX::XMFLOAT3(4.9f, 10.f, 4.9f);

		data[ObjName] = box;
	}

	for (int i = 0; i < KeyBox.size(); ++i)
	{
		std::string ObjName = "KeyBox " + std::to_string(i);

		DirectX::BoundingOrientedBox box{};
		box.Center = KeyBox[i];
		box.Extents = DirectX::XMFLOAT3(4.9f, 5.f, 4.9f);

		data[ObjName] = box;
	}

	for (int x = 0; x < cellWidth; ++x) {
		for (int y = 0; y < cellDepth; ++y) {
			for (auto& datas : data) {
				if (cells[x][y].InCell(datas.second)) {
					/*if (x >= 40 && x <= 55 && y >= 30 && y < 60) {
						cells[x][y].cellType = CONT;
						cells[x][y].isObstacle = true;
						std::cout << "[" << x << "," << y << "] ";
						std::cout << "Center: (" << datas.second.Center.x << ", " << datas.second.Center.y << ", " << datas.second.Center.z << ")" << std::endl;
						std::cout << "Extexts: (" << datas.second.Extents.x << ", " << datas.second.Extents.y << ", " << datas.second.Extents.z << ")" << std::endl;
						
						std::cout << "Orientation: (" << datas.second.Orientation.x << ", " << datas.second.Orientation.y << ", " << datas.second.Orientation.z << ") - " << datas.second.Orientation.w << std::endl << std::endl;
						break;
					}*/
					cells[x][y].cellType = CONT;
					cells[x][y].isObstacle = true;
					break;
				}
			}

		}
	}
	std::cout << "Map loading 완료\n";

	InitializeNPC();
	std::cout << "NPC loading 완료\n";

}

void GameMap::ChangeToMap2()
{
	using namespace DirectX;
	std::cout << "Map 초기화 중.." << std::endl;
	cells.clear();

	mapWidth = 350.f;
	mapDepth = 650.f;
	cellWidth = 35;
	cellDepth = 65;
	cool_down = false;

	BossNpc.n_state = NPC_INGAME;
	BossNpc.SetId(200);
	BossNpc.cool_time = 3000;
	BossNpc.SetPos(XMFLOAT3(170.f, 0.f, 225.f));
	CELL& cell = GetCurrentCell(BossNpc.GetPos());
	BossNpc.boundingBox.Center = BossNpc.GetPos();
	BossNpc.boundingBox.Extents = DirectX::XMFLOAT3(10.f, 100.f, 10.f);

	
	// `cellWidth`와 `cellDepth` 크기로 2D 배열을 생성합니다.
	cells.resize(cellWidth, std::vector<CELL>(cellDepth));

	std::unordered_map<std::string, DirectX::BoundingOrientedBox> data;
	std::ifstream in{ "Resource/Map/Stage2InsideObj_info.txt" };

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
				DirectX::BoundingOrientedBox box{};
				box.Center = DirectX::XMFLOAT3(values[0], values[1], values[2]);
				box.Extents = DirectX::XMFLOAT3(values[3], values[4], values[5]);

				// 데이터 저장
				data[objName] = box;

			}
			else if (line.find("<Rotation>:") == 0) {
				// <Rotation>: 뒤의 값을 추출
				std::istringstream iss(line.substr(11));
				char discard;
				float rotX, rotY, rotZ{};

				if (!(iss >> discard >> rotX >> discard >> rotY >> discard >> rotZ >> discard)) {
					std::cerr << "회전 값 추출 오류 발생" << std::endl;
					exit(-1);
				}


				float angleX = XMConvertToRadians(rotX);
				float angleY = XMConvertToRadians(rotY);
				float angleZ = XMConvertToRadians(rotZ);
				XMVECTOR quaternionX = DirectX::XMQuaternionRotationRollPitchYaw(angleX, 0.0f, 0.0f);
				XMVECTOR quaternionY = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, angleY, 0.0f);
				XMVECTOR quaternionZ = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, angleZ);

				XMVECTOR quaternion = DirectX::XMQuaternionMultiply(quaternionX, DirectX::XMQuaternionMultiply(quaternionY, quaternionZ));

				XMFLOAT4 orientation{};
				DirectX::XMStoreFloat4(&orientation, quaternion);

				data[objName].Orientation = orientation;

				XMVECTOR rotatedExtents = XMVectorSet(data[objName].Extents.x, data[objName].Extents.y, data[objName].Extents.z, 0.f);
				XMVECTOR orientationQuat = XMVectorSet(orientation.x, orientation.y, orientation.z, orientation.w);

				XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(orientationQuat);
				XMMATRIX inverseRotationMatrix = XMMatrixInverse(nullptr, rotationMatrix);
				XMVECTOR originalExtents = XMVector3Transform(rotatedExtents, inverseRotationMatrix);

				originalExtents = XMVectorAbs(originalExtents);

				XMFLOAT3 originalExtentsFloat;
				XMStoreFloat3(&originalExtentsFloat, originalExtents);

				data[objName].Extents = originalExtentsFloat;
			}

		}

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
				if (cells[x][y].InCell(datas.second)) {
					cells[x][y].cellType = CONT;
					cells[x][y].isObstacle = true;
					break;
				}
			}

		}
	}
	std::cout << "Map2 loading 완료\n";

}

void GameMap::StartGame()
{
	Server& server = Server::GetInstance();
	std::cout << "size" << cl_ids.size() << std::endl;
	for (auto& cl : cl_ids) {
		server.clients[cl].SetPos(PlayerInitPos[cl]);
		// std::cout << "player[" << cl << "]의 캐릭터 " << server.clients[cl].GetType()<< " ";
		SC_GAME_START_PACKET p;
		p.size = sizeof(p);
		p.type = SC_GAME_START;
		server.clients[cl].do_send(&p);

	}

}

void GameMap::printMap() const
{
	
	std::cout << "-----PrintMap------------------------\n";
	for (int y = cellDepth - 1; y >= 0; --y) {
		for (int x = 0; x < cellWidth; ++x) {
			const CELL& cell = cells[x][y];
			switch (cell.cellType)
			{

			case CONT:
				std::cout << "X "; break;
			case CT_NPC:
				std::cout << "N "; break;
			case GROUND:
				std::cout << ". "; break;
			default:
				break;
			}


		}
		std::cout << "\n" << std::endl;
	}

	std::cout << "-----PrintMap------------------------\n";
}

void GameMap::Update(int tick)
{
	if (!InGame) return;

	//Remain_time = End_time - std::chrono::steady_clock::now();

	//if (Remain_time.count() < 0.f) {
	//	std::cout << "게임 종료" << std::endl;
	//	EndGame();
	//	cl_ids.clear(); // 게임 리셋
	//	return;
	//}

	//if(tick == 0)
	//	std::cout << "남은시간 : " << Remain_time.count() << " 초" << std::endl;
	
	
	
	if (tick == 0 || tick == 15 || tick == 30 || tick == 45) {

	}
	else return;

	switch (game_state)
	{
	case NOGAME:
		return;
	case STAGE1:
		UpdateS1();
		break;
	case STAGE2:
		UpdateS2();
		break;
	default:
		return;
	}	
}

void GameMap::UpdateS1()
{
	if (cool_down == true) return;
	Server& server = Server::GetInstance();
	auto& players = server.clients;

	for (auto& npc : npcs) {
		npc.UpdateBB();
		bool patrol = true;
		float closed_dis = 100000.f;
		if ((npc.IsAttack == true)) continue;
		
		for (auto ids : cl_ids) {
			if (players[ids].anim == CREEP) continue;
			if (players[ids].anim == CRAWL) continue;
			// 같은 섹터
			if (npc.my_sector == getSector(players[ids].GetPos())) {
				patrol = false;

				float distance = Distance_float(npc.GetPos(), players[ids].GetPos());

				if (closed_dis > distance) {
					closed_dis = distance;
					npc.near_player = ids;
				}
			}
		}

		if (patrol == true) {
			npc.current_behavior = PATROL;
			npc.PathClear();

			if (npc.n_path.empty())
				npc.n_path.push(GetRandomPos(npc.GetPos()));

			npc.near_player = -1;
			npc.distance_near = 100000.f;
		}
		else
		{
			
			if (closed_dis < AttackRange)
			{
				npc.current_behavior = ATTACK;
				// std::cout << "쿨타임 시작" << std::endl;
				cool_lock.lock();
				cool_down = true;
				cool_lock.unlock();
			}
			else
			{
				npc.current_behavior = CHASE;
				if (npc.distance_near < closed_dis) {
					npc.PathClear();
					std::vector<DirectX::XMFLOAT3> path = BFS(npc.GetPos(), server.clients[npc.near_player].GetPos());
					for (auto& p : path) {
						npc.n_path.push(p);
					}
				}
				
			}
			npc.distance_near = closed_dis;
			
		}



		npc.DoWork();
	}
}

void GameMap::UpdateS2()
{
	BossNpc.UpdateBB();
	Server& server = Server::GetInstance();
	auto& players = server.clients;

	if (cool_down == true) return;
	if (BossNpc.IsAttack) return;

	float closed_dis = 100000.f;

	


	for (auto ids : cl_ids) {
		if (players[ids].anim == CREEP) continue;
		if (players[ids].anim == CRAWL) continue;
		float distance = Distance_float(BossNpc.GetPos(), players[ids].GetPos());

		if (closed_dis > distance) {
			closed_dis = distance;
			BossNpc.near_player = ids;
		}

	}

	if (closed_dis < (AttackRange + 2.f))
	{
		BossNpc.current_behavior = ATTACK;
		// std::cout << "쿨타임 시작" << std::endl;
		cool_lock.lock();
		cool_down = true;
		cool_lock.unlock();
	}
	else
	{
		BossNpc.current_behavior = CHASE;
		if (BossNpc.distance_near < closed_dis) {
			BossNpc.PathClear();
			std::vector<DirectX::XMFLOAT3> path = BFS(BossNpc.GetPos(), server.clients[BossNpc.near_player].GetPos());
			for (auto& p : path) {
				BossNpc.n_path.push(p);
			}
		}

	}
	BossNpc.distance_near = closed_dis;

	BossNpc.DoWork();
}



CELL& GameMap::GetCurrentCell(DirectX::XMFLOAT3 in_pos)
{
	int Cell_x = static_cast<int>((in_pos.x + 0.5f) / (mapWidth / cellWidth));
	int Cell_z = static_cast<int>((in_pos.z + 0.5f) / (mapWidth / cellDepth));

	return cells[Cell_x][Cell_z];
}

std::pair<int, int> GameMap::CoordsToIndex(const DirectX::XMFLOAT3& coords) const
{
	int Cell_x = static_cast<int>((coords.x + 0.5f) / (mapWidth / cellWidth));
	int Cell_z = static_cast<int>((coords.z + 0.5f) / (mapDepth / cellDepth));

	return std::make_pair(Cell_x, Cell_z);
}

void GameMap::InitializeNPC()
{

	for (int i = 0; i < npcs.size(); ++i) {
		npcs[i].n_state = NPC_INGAME;
		npcs[i].SetId(i + 100);
		npcs[i].SetPos(NPCInitPos[i]);
		npcs[i].cool_time = 2500;
		npcs[i].my_sector = getSector(npcs[i].GetPos());
		CELL& cell = GetCurrentCell(npcs[i].GetPos());
		npcs[i].boundingBox.Center = npcs[i].GetPos();
		npcs[i].boundingBox.Extents = DirectX::XMFLOAT3(3.f, 20.f, 3.f);
	}

	/*auto path = BFS(npcs[0].GetPos(), PlayerInitPos[0]);
	for (auto& pos : path) {
		npcs[0].n_path.push(pos);
		std::cout << pos.x << ", " << pos.z << std::endl;
	}
	std::cout << "경로 크기 : " << npcs[0].n_path.size() << std::endl;*/

}

std::vector<DirectX::XMFLOAT3> GameMap::PathFind(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& targetPos) {
	std::pair<int, int> startIndex = CoordsToIndex(startPos);
	std::pair<int, int> targetIndex = CoordsToIndex(targetPos);

	Node startNode(startIndex.first, startIndex.second, 0, 0, nullptr);
	Node targetNode(targetIndex.first, targetIndex.second, 0, 0, nullptr);

	std::vector<std::vector<bool>> visited(cellWidth, std::vector<bool>(cellDepth, false));
	std::vector<DirectX::XMFLOAT3> path;

	std::priority_queue<Node*> openList;
	openList.push(new Node(startIndex.first, startIndex.second, 0, 0, nullptr));
	visited[startIndex.first][startIndex.second] = true;

	std::vector<Node*> closedList;

	while (!openList.empty()) {
		Node* currentNode = openList.top();
		openList.pop();

		if (*currentNode == targetNode) {
			// 경로를 재구성합니다.
			while (currentNode != nullptr) {
				path.push_back(cells[currentNode->x][currentNode->z].center);
				Node* temp = currentNode;
				currentNode = currentNode->parent;
				// 이미 방문한 노드의 리스트에서 제거합니다.
				closedList.erase(std::remove(closedList.begin(), closedList.end(), temp), closedList.end());
				delete temp; // 더 이상 필요하지 않은 노드를 해제합니다.
			}
			std::reverse(path.begin(), path.end());
			break;
		}

		std::vector<Node> neighbors = GetNeighbors(*currentNode, targetNode);
		for (const Node& neighbor : neighbors) {
			int x = neighbor.x;
			int z = neighbor.z;

			if (!visited[x][z]&&!cells[x][z].isObstacle) {
				visited[x][z] = true;

				Node* newNode = nullptr;
				for (Node* visitedNode : closedList) {
					if (visitedNode->x == x && visitedNode->z == z) {
						newNode = visitedNode;
						break;
					}
				}

				if (newNode == nullptr) {
					// 새로운 노드를 생성하여 큐에 추가합니다.
					newNode = new Node(x, z, neighbor.gCost, neighbor.hCost, currentNode);
					closedList.push_back(newNode);
				}

				openList.push(newNode);
			}
		}
	}

	for (Node* node : closedList) {
		delete node;
	}

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

	std::vector<DirectX::XMFLOAT3> path{};

	// 시작 노드와 목표 노드를 생성합니다.
	Node startNode(startIndex.first, startIndex.second, 0, 0, nullptr);
	Node targetNode(targetIndex.first, targetIndex.second, 0, 0, nullptr);
	if (startNode == targetNode)
		return path;

	// 방문 여부를 나타내는 2차원 배열을 초기화합니다.
	std::vector<std::vector<bool>> visited(cellWidth, std::vector<bool>(cellDepth, false));

	// 경로를 저장할 벡터를 생성합니다.
	

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
