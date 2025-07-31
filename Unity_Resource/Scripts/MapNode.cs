using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapNode : MonoBehaviour
{
    public Transform[] neighbors;
    static private uint id = 0;
    private int node_id = -1;

    //씬 추출을 스타트 함수에서하는데 이 컴포넌트가 씬추출시점에 반드시 Start가 호출되는지 모름으로
    //아래 코드를 Start가 아니라 일반 멤버 함수로 구현
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
