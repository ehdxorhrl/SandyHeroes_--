#pragma once

struct Node
{
    XMFLOAT3 position{};
    int id{};
    std::vector<Node*> neighbors;

    float gCost = FLT_MAX;  // 시작 노드로부터 거리
    float hCost = 0.0f;     // 목표 노드까지 추정 거리

    Node* parent = nullptr;

    float fCost() const { return gCost + hCost; }
    Node() {}
    Node(XMFLOAT3 pos, int node_id) : position(pos), id(node_id) {}
};

//노드를 생성한뒤 id를 통해 서로 연결하기 위한 정보 저장 구조체
struct NodeConnector
{
    Node* node = nullptr; // 이노드는
    std::vector<int> neighbors_id; // 이 id의 node와 연결되어야한다.

    NodeConnector() {}
    NodeConnector(Node* node_ptr, const std::vector<int>& ids) : node(node_ptr), neighbors_id(ids) {}
};

extern std::array<std::list<Node>, 8> kStageNodeBuffers;
extern std::vector<NodeConnector> kNodeConnectors;

namespace a_star
{
    void ConnectNodes(const std::vector<NodeConnector>& node_connectors);
    std::vector<Node*> AStar(Node* start, Node* goal);
};
