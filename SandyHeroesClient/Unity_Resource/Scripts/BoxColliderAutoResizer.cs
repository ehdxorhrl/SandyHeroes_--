using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BoxColliderAutoResizer : MonoBehaviour
{
	[ContextMenu("Box Collider 자동 설정")]
	void ResizeBoxColliderToFitChildren()
	{
		BoxCollider collider = GetComponent<BoxCollider>();
		if (collider == null)
		{
			collider = gameObject.AddComponent<BoxCollider>();
		}

		Bounds bounds = new Bounds(transform.position, Vector3.zero);
		bool hasBounds = false;

		Renderer[] renderers = GetComponentsInChildren<Renderer>();

		foreach (Renderer rend in renderers)
		{
			if (rend == null || rend.gameObject == this.gameObject) continue;

			if (!hasBounds)
			{
				bounds = rend.bounds;
				hasBounds = true;
			}
			else
			{
				bounds.Encapsulate(rend.bounds);
			}
		}

		if (hasBounds)
		{
			Vector3 localCenter = transform.InverseTransformPoint(bounds.center);
			Vector3 localSize = transform.InverseTransformVector(bounds.size);

			collider.center = localCenter;
			collider.size = localSize;
		}
		else
		{
			Debug.LogWarning("자식 렌더러가 없거나 바운드를 계산할 수 없습니다.");
		}
	}
}