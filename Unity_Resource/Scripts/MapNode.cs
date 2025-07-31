using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapNode : MonoBehaviour
{
    public Transform[] neighbors;
    static private uint id = 0;
    private int node_id = -1;

    //�� ������ ��ŸƮ �Լ������ϴµ� �� ������Ʈ�� ����������� �ݵ�� Start�� ȣ��Ǵ��� ������
    //�Ʒ� �ڵ带 Start�� �ƴ϶� �Ϲ� ��� �Լ��� ����
	public int GetNodeId()
    {
        if(node_id < 0) node_id = (int)id++; 
        return node_id;
    }



    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
