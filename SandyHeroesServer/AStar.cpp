#include "stdafx.h"
#include "AStar.h"

std::array<std::list<Node>, 8> kStageNodeBuffers;
std::vector<NodeConnector> kNodeConnectors;

void a_star::ConnectNodes(const std::vector<NodeConnector>& node_connectors)
{
    //��� ��� Ŀ���͸� ��ȸ�Ͽ� �� ����� �̿��� �����Ѵ�.
    for (auto& node_connector : node_connectors)
    {
        auto& neighbors = node_connector.node->neighbors;
        const auto& neighbors_id = node_connector.neighbors_id;
        for (const int& id : neighbors_id)
        {
            auto it = std::find_if(node_connectors.begin(), node_connectors.end(), [&id](const NodeConnector& node_connector)
                {
                    return id == node_connector.node->id;
                });
            if (it != node_connectors.end())
            {
                neighbors.push_back(it->node);
            }
            else
            {
                std::wstring str = L"Do not find node's neighbor!! \nid: " + std::to_wstring(id) + L"\n";
                OutputDebugString(str.c_str());
            }
        }
    }
    OutputDebugString(L"Complate Node Connect! \n");

}

std::vector<Node*> a_star::AStar(Node* start, Node* goal)
{
    for(auto& node_list : kStageNodeBuffers)
    {
        for (auto& node : node_list)
        {
            node.gCost = FLT_MAX; // �ʱ�ȭ
            node.hCost = 0.0f; // �ʱ�ȭ
            node.parent = nullptr; // �ʱ�ȭ
        }
	}

    auto heuristic = [](Node* a, Node* b) {
        XMVECTOR pa = XMLoadFloat3(&a->position);
        XMVECTOR pb = XMLoadFloat3(&b->position);
        XMVECTOR d = XMVectorSubtract(pa, pb);
        return XMVectorGetX(XMVector3Length(d)); // ��Ŭ���� �Ÿ�
        };

    std::priority_queue<Node*, std::vector<Node*>,
        decltype([](Node* a, Node* b) { return a->fCost() > b->fCost(); })> openSet;

    start->gCost = 0.0f;
    start->hCost = heuristic(start, goal);
    openSet.push(start);

    std::unordered_set<Node*> closedSet;

    while (!openSet.empty())
    {
        Node* current = openSet.top();
        openSet.pop();

        if (current == goal)
        {
            // ��� ����
            std::vector<Node*> path;
			while (current) 
            {
                path.push_back(current);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closedSet.insert(current);

        for (Node* neighbor : current->neighbors)
        {
            if (closedSet.count(neighbor))
                continue;

            float tentativeG = current->gCost + heuristic(current, neighbor);

            if (tentativeG < neighbor->gCost)
            {
                neighbor->parent = current;
                neighbor->gCost = tentativeG;
                neighbor->hCost = heuristic(neighbor, goal);
                openSet.push(neighbor);
            }
        }
    }

    return {}; // ��� ����
}