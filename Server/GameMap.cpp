#include "GameMap.h"
#include "Server.h"

GameMap& GameMap::GetInstance()
{
	static GameMap instance;
	return instance;
}

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


		//for (const auto& pair : data) {
		//	std::cout << "Object Name: " << pair.first << std::endl;
		//	const DirectX::BoundingOrientedBox& box = pair.second;
		//	std::cout << "Center: (" << box.Center.x << ", " << box.Center.y << ", " << box.Center.z << ")" << std::endl;
		//	// std::cout << "Extents: (" << box.Extents.x << ", " << box.Extents.y << ", " << box.Extents.z << ")" << std::endl;
		//}


	}


	// �� ���� �ʱ�ȭ�ϰ� ���� ��ġ �� ũ�⸦ �����մϴ�.
	for (int x = 0; x < cellWidth; ++x) {
		for (int y = 0; y < cellDepth; ++y) {
			// �� ���� ��ġ�� ũ�⸦ ����
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
	std::cout << "Map loading �Ϸ�\n";

	InitializeNPC();
	std::cout << "NPC loading �Ϸ�\n";

}

void GameMap::StartGame()
{
	Server& server = Server::GetInstance();
	
	for (int i = 0; i < cl_ids.size(); ++i) {
		server.clients[i].SetPos(server.random_pos[i]);
		std::cout << "player[" << i << "]�� ĳ���� " << server.clients[i].GetType() << std::endl;
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
	//cl_ids.push_back(0); // �ӽ�
	//cl_ids.push_back(1);// �ӽ�
	//cl_ids.push_back(2);// �ӽ�
	
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

	// ���� ��ġ�� ��ǥ ��ġ�� ����� �ε����� ��ȯ
	std::pair<int, int> startIndex = CoordsToIndex(startPos);
	std::pair<int, int> targetIndex = CoordsToIndex(targetPos);

	// ���� ���� ��ǥ ��带 �����մϴ�.
	Node startNode(startIndex.first, startIndex.second, 0, 0, nullptr);
	Node targetNode(targetIndex.first, targetIndex.second, 0, 0, nullptr);

	std::vector<std::vector<bool>> visited(cellWidth, std::vector<bool>(cellDepth, false));

	// �켱���� ť�� �����մϴ�.
	std::priority_queue<Node*, std::vector<Node*>, CompareNodes> openSet;

	// ���� ��带 �켱���� ť�� �߰��մϴ�.
	openSet.push(new Node(startIndex.first, startIndex.second, 0, 0, nullptr));

	visited[startIndex.first][startIndex.second] = true;
	// ���� ����� ������� ������ �ݺ��մϴ�.
	while (!openSet.empty()) {
		// ���� ��Ͽ��� ���� ��带 �����մϴ�.
		Node* currentNode = openSet.top();
		openSet.pop();

		// ���� ��尡 ��ǥ ������� Ȯ���մϴ�.
		if (*currentNode == targetNode) {
			// ��θ� �籸���մϴ�.

			Node* current = currentNode;
			while (current != nullptr) {
				path.push_back(cells[current->x][current->z].center);
				current = current->parent;
			}
			std::reverse(path.begin(), path.end());

			// �޸� ����
			return path;
		}


		// ���� ����� �̿� ��带 �����ɴϴ�.
		std::vector<Node> neighbors = GetNeighbors(*currentNode);
		for (Node& neighbor : neighbors) {
			// �̿� ��尡 ��ֹ��̰ų� �̹� �湮�� ������� Ȯ���մϴ�.
			Node* cnode = &neighbor;

			if (!visited[neighbor.x][neighbor.z] && !cells[neighbor.x][neighbor.z].isObstacle) {
				// ���ο� ���� ����� ����մϴ�.
				int newGCost = currentNode->gCost + 1; // �̵� ����� 1�� �����մϴ�.
				// int newHCost = CalculateManhattanDistance(neighbor, targetNode);
				// �̿� ��尡 �̹� ���� ��Ͽ� �ִ��� Ȯ���մϴ�.


				// ���ο� ��ΰ� �� ���� ��� �Ǵ� �̿� ��尡 ���� ���� ��Ͽ� ���� ��쿡�� ������Ʈ�մϴ�.

			}
		}
		delete currentNode;
	}

	// ��θ� ã�� ���� ���, �� ���� ��ȯ


	// �޸� ����
	return path;
}



std::vector<Node> GameMap::GetNeighbors(const Node& node) const {
	std::vector<Node> neighbors;

	// ���� ����� �ε��� ��
	int Cell_x = node.x;
	int Cell_z = node.z;

	// ���� ����� �ֺ� 8���� ���� Ȯ���Ͽ� �̿� ��带 Ž��
	for (int dz = -1; dz <= 1; ++dz) {
		for (int dx = -1; dx <= 1; ++dx) {
			// ���� ���� �̿��� �ƴ� (�ڱ� �ڽ�)
			if (dz == 0 && dx == 0) {
				continue;
			}

			int newX = Cell_x + dx;
			int newZ = Cell_z + dz;

			// ���ο� ��ǥ�� ��ȿ���� Ȯ���ϰ�, ��ֹ��� �ƴ��� Ȯ��
			if ((abs(dx) + abs(dz) == 1) && newX >= 0 && newX < cellWidth && newZ >= 0 && newZ < cellDepth &&
				!cells[newX][newZ].isObstacle) {
				// �̿� ��带 �����Ͽ� �̿� ��Ͽ� �߰�
				neighbors.emplace_back(newX, newZ, 0, 0, nullptr); // �θ� ��带 nullptr�� ����
			}
		}
	}

	return neighbors;
}

std::vector<DirectX::XMFLOAT3> GameMap::BFS(const DirectX::XMFLOAT3& startPos, const DirectX::XMFLOAT3& targetPos) {
	// ���� ��ġ�� ��ǥ ��ġ�� ����� �ε����� ��ȯ
	std::pair<int, int> startIndex = CoordsToIndex(startPos);
	std::pair<int, int> targetIndex = CoordsToIndex(targetPos);

	// ���� ���� ��ǥ ��带 �����մϴ�.
	Node startNode(startIndex.first, startIndex.second, 0, 0, nullptr);
	Node targetNode(targetIndex.first, targetIndex.second, 0, 0, nullptr);

	// �湮 ���θ� ��Ÿ���� 2���� �迭�� �ʱ�ȭ�մϴ�.
	std::vector<std::vector<bool>> visited(cellWidth, std::vector<bool>(cellDepth, false));

	// ��θ� ������ ���͸� �����մϴ�.
	std::vector<DirectX::XMFLOAT3> path;

	// �ʺ� �켱 Ž���� ���� ť�� �����ϰ� ���� ��带 �߰��մϴ�.
	std::queue<Node*> queue;
	queue.push(new Node(startIndex.first, startIndex.second, 0, 0, nullptr));
	visited[startIndex.first][startIndex.second] = true;

	// �̹� �湮�� ����� ����Ʈ�� �����մϴ�.
	std::vector<Node*> visitedNodes;
	// ��ǥ ��忡 ������ ������ �ݺ��մϴ�.
	while (!queue.empty()) {
		// ť���� ���� ��带 �����ɴϴ�.
		Node* currentNode = queue.front();
		queue.pop();

		// ���� ��尡 ��ǥ ������� Ȯ���մϴ�.
		if (*currentNode == targetNode) {
			// ��θ� �籸���մϴ�.
			while (currentNode != nullptr) {
				path.push_back(cells[currentNode->x][currentNode->z].center);
				Node* temp = currentNode;
				currentNode = currentNode->parent;
				// �̹� �湮�� ����� ����Ʈ���� �����մϴ�.
				visitedNodes.erase(std::remove(visitedNodes.begin(), visitedNodes.end(), temp), visitedNodes.end());
				delete temp; // �� �̻� �ʿ����� ���� ��带 �����մϴ�.
			}
			std::reverse(path.begin(), path.end());
			break;
		}

		// ���� ����� �̿� ��带 �����ͼ� ť�� �߰��մϴ�.
		std::vector<Node> neighbors = GetNeighbors(*currentNode);
		for (const Node& neighbor : neighbors) {
			int x = neighbor.x;
			int z = neighbor.z;
			// �̿� ��尡 �湮���� �ʾҰ�, ��ֹ��� �ƴ� ��쿡�� ó���մϴ�.
			if (!visited[x][z] && !cells[x][z].isObstacle) {
				visited[x][z] = true;
				// �̹� �湮�� ������� Ȯ���մϴ�.
				Node* newNode = nullptr;
				for (Node* visitedNode : visitedNodes) {
					if (visitedNode->x == x && visitedNode->z == z) {
						newNode = visitedNode;
						break;
					}
				}
				if (newNode == nullptr) {
					// ���ο� ��带 �����Ͽ� ť�� �߰��մϴ�.
					newNode = new Node(x, z, 0, 0, currentNode);
					visitedNodes.push_back(newNode);
				}
				queue.push(newNode);
			}
		}
	}

	// ���� ��� ��带 �����մϴ�.
	for (Node* node : visitedNodes) {
		delete node;
	}

	return path;
}
