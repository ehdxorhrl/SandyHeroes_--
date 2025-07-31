#pragma once

struct Node
{
    XMFLOAT3 position{};
    int id{};
    std::vector<Node*> neighbors;

    float gCost = FLT_MAX;  // ���� ���κ��� �Ÿ�
    float hCost = 0.0f;     // ��ǥ ������ ���� �Ÿ�

    Node* parent = nullptr;

    float fCost() const { return gCost + hCost; }
    Node() {}
    Node(XMFLOAT3 pos, int node_id) : position(pos), id(node_id) {}
};

//��带 �����ѵ� id�� ���� ���� �����ϱ� ���� ���� ���� ����ü
struct NodeConnector
{
    Node* node = nullptr; // �̳���
    std::vector<int> neighbors_id; // �� id�� node�� ����Ǿ���Ѵ�.

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
