using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEditor.Rendering;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SceneExtract : MonoBehaviour
{
	public GameObject[] exceptionObjects = null;

	private SkinnedMeshRenderer[] m_pSkinnedMeshRenderers = null;

	private List<string> m_pTextureNamesListForCounting = new List<string>();
	private List<string> m_pTextureNamesListForWriting = new List<string>();

	private List<string> m_pModelInfoNames = new();

	private BinaryWriter binaryWriter = null;
	private int m_nFrames = 0;

	bool FindTextureByName(List<string> pTextureNamesList, Texture texture)
	{
		if (texture)
		{
			string strTextureName = string.Copy(texture.name).Replace(" ", "_");
			for (int i = 0; i < pTextureNamesList.Count; i++)
			{
				if (pTextureNamesList.Contains(strTextureName)) return (true);
			}
			pTextureNamesList.Add(strTextureName);
			return (false);
		}
		else
		{
			return (true);
		}
	}

	void WriteObjectName(Object obj)
	{
		binaryWriter.Write((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
	}

	void WriteObjectName(int i, Object obj)
	{
		binaryWriter.Write(i);
		binaryWriter.Write((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
	}

	void WriteObjectName(string strHeader, Object obj)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
	}

	void WriteObjectName(string strHeader, int i, Object obj)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(i);
		binaryWriter.Write((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
	}

	void WriteObjectName(string strHeader, int i, int j, Object obj)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(i);
		binaryWriter.Write(j);
		binaryWriter.Write((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
	}

	void WriteObjectName(string strHeader, int i, Object obj, float f, int j, int k)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(i);
		binaryWriter.Write((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
		binaryWriter.Write(f);
		binaryWriter.Write(j);
		binaryWriter.Write(k);
	}

	void WriteString(string strToWrite)
	{
		binaryWriter.Write(strToWrite);
	}

	void WriteString(string strHeader, string strToWrite)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(strToWrite);
	}

	void WriteString(string strToWrite, int i)
	{
		binaryWriter.Write(strToWrite);
		binaryWriter.Write(i);
	}

	void WriteString(string strToWrite, int i, float f)
	{
		binaryWriter.Write(strToWrite);
		binaryWriter.Write(i);
		binaryWriter.Write(f);
	}

	void WriteTextureName(string strHeader, Texture texture)
	{
		binaryWriter.Write(strHeader);
		if (texture)
		{

			//if (FindTextureByName(m_pTextureNamesListForWriting, texture))
			//{
			//    binaryWriter.Write("@" + string.Copy(texture.name).Replace(" ", "_"));
			//}
			//else
			//{
			//    binaryWriter.Write(string.Copy(texture.name).Replace(" ", "_"));
			//}
			binaryWriter.Write(string.Copy(texture.name).Replace(" ", "_"));

		}
		else
		{
			binaryWriter.Write("null");
		}
	}

	void WriteInteger(int i)
	{
		binaryWriter.Write(i);
	}

	void WriteInteger(string strHeader, int i)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(i);
	}

	void WriteFloat(string strHeader, float f)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(f);
	}

	void WriteVector(Vector2 v)
	{
		binaryWriter.Write(v.x);
		binaryWriter.Write(v.y);
	}

	void WriteVector(BinaryWriter bWriter, Vector2 v)
	{
		bWriter.Write(v.x);
		bWriter.Write(v.y);
	}
	void WriteVector(string strHeader, Vector2 v)
	{
		binaryWriter.Write(strHeader);
		WriteVector(v);
	}

	void WriteVector(Vector3 v)
	{
		binaryWriter.Write(v.x);
		binaryWriter.Write(v.y);
		binaryWriter.Write(v.z);
	}

	void WriteVector(BinaryWriter bWriter, Vector3 v)
	{
		bWriter.Write(v.x);
		bWriter.Write(v.y);
		bWriter.Write(v.z);
	}
	void WriteVector(string strHeader, Vector3 v)
	{
		binaryWriter.Write(strHeader);
		WriteVector(v);
	}

	void WriteVector(Vector4 v)
	{
		binaryWriter.Write(v.x);
		binaryWriter.Write(v.y);
		binaryWriter.Write(v.z);
		binaryWriter.Write(v.w);
	}

	void WriteVector(string strHeader, Vector4 v)
	{
		binaryWriter.Write(strHeader);
		WriteVector(v);
	}

	void WriteVector(Quaternion q)
	{
		binaryWriter.Write(q.x);
		binaryWriter.Write(q.y);
		binaryWriter.Write(q.z);
		binaryWriter.Write(q.w);
	}

	void WriteVector(string strHeader, Quaternion q)
	{
		binaryWriter.Write(strHeader);
		WriteVector(q);
	}

	void WriteColor(Color c)
	{
		binaryWriter.Write(c.r);
		binaryWriter.Write(c.g);
		binaryWriter.Write(c.b);
		binaryWriter.Write(c.a);
	}

	void WriteColor(string strHeader, Color c)
	{
		binaryWriter.Write(strHeader);
		WriteColor(c);
	}

	void WriteTextureCoord(Vector2 uv)
	{
		binaryWriter.Write(uv.x);
		binaryWriter.Write(1.0f - uv.y);
	}

	void WriteVectors(string strHeader, Vector2[] vectors)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(vectors.Length);
		if (vectors.Length > 0) foreach (Vector2 v in vectors) WriteVector(v);
	}

	void WriteVectors(string strHeader, Vector3[] vectors)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(vectors.Length);
		if (vectors.Length > 0) foreach (Vector3 v in vectors) WriteVector(v);
	}

	void WriteVectors(string strHeader, Vector4[] vectors)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(vectors.Length);
		if (vectors.Length > 0) foreach (Vector4 v in vectors) WriteVector(v);
	}

	void WriteColors(string strHeader, Color[] colors)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(colors.Length);
		if (colors.Length > 0) foreach (Color c in colors) WriteColor(c);
	}

	void WriteTextureCoords(string strHeader, Vector2[] uvs)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(uvs.Length);
		if (uvs.Length > 0) foreach (Vector2 uv in uvs) WriteTextureCoord(uv);
	}

	void WriteIntegers(int[] pIntegers)
	{
		binaryWriter.Write(pIntegers.Length);
		foreach (int i in pIntegers) binaryWriter.Write(i);
	}

	void WriteIntegers(string strHeader, int[] pIntegers)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(pIntegers.Length);
		if (pIntegers.Length > 0) foreach (int i in pIntegers) binaryWriter.Write(i);
	}

	void WriteIntegers(string strHeader, int n, int[] pIntegers)
	{
		binaryWriter.Write(strHeader);
		binaryWriter.Write(n);
		binaryWriter.Write(pIntegers.Length);
		if (pIntegers.Length > 0) foreach (int i in pIntegers) binaryWriter.Write(i);
	}

	void WriteBoundingBox(string strHeader, Bounds bounds)
	{
		binaryWriter.Write(strHeader);
		WriteVector(bounds.center);
		WriteVector(bounds.extents);
	}
	void WriteBoundingBox(string strHeader, Vector3 center, Vector3 size)
	{
		Vector3 extents = new Vector3(size.x / 2, size.y / 2, size.z / 2);
		binaryWriter.Write(strHeader);
		WriteVector(center);
		WriteVector(extents);
	}
	void WriteBoundingBox(BinaryWriter bWriter, string strHeader, Vector3 center, Vector3 size)
	{
		Vector3 extents = new Vector3(size.x / 2, size.y / 2, size.z / 2);
		bWriter.Write(strHeader);
		WriteVector(bWriter, center);
		WriteVector(bWriter, extents);
	}

	void WriteMatrix(Matrix4x4 matrix)
	{
		binaryWriter.Write(matrix.m00);
		binaryWriter.Write(matrix.m10);
		binaryWriter.Write(matrix.m20);
		binaryWriter.Write(matrix.m30);
		binaryWriter.Write(matrix.m01);
		binaryWriter.Write(matrix.m11);
		binaryWriter.Write(matrix.m21);
		binaryWriter.Write(matrix.m31);
		binaryWriter.Write(matrix.m02);
		binaryWriter.Write(matrix.m12);
		binaryWriter.Write(matrix.m22);
		binaryWriter.Write(matrix.m32);
		binaryWriter.Write(matrix.m03);
		binaryWriter.Write(matrix.m13);
		binaryWriter.Write(matrix.m23);
		binaryWriter.Write(matrix.m33);
	}
	void WriteMatrix(BinaryWriter bWriter, Matrix4x4 matrix)
	{
		bWriter.Write(matrix.m00);
		bWriter.Write(matrix.m10);
		bWriter.Write(matrix.m20);
		bWriter.Write(matrix.m30);
		bWriter.Write(matrix.m01);
		bWriter.Write(matrix.m11);
		bWriter.Write(matrix.m21);
		bWriter.Write(matrix.m31);
		bWriter.Write(matrix.m02);
		bWriter.Write(matrix.m12);
		bWriter.Write(matrix.m22);
		bWriter.Write(matrix.m32);
		bWriter.Write(matrix.m03);
		bWriter.Write(matrix.m13);
		bWriter.Write(matrix.m23);
		bWriter.Write(matrix.m33);
	}

	void WriteMatrix(Vector3 position, Quaternion rotation, Vector3 scale)
	{
		Matrix4x4 matrix = Matrix4x4.identity;
		matrix.SetTRS(position, rotation, scale);
		WriteMatrix(matrix);
	}

	void WriteTransform(string strHeader, Transform current)
	{
		binaryWriter.Write(strHeader);
		WriteVector(current.localPosition);
		WriteVector(current.localEulerAngles);
		WriteVector(current.localScale);
		WriteVector(current.localRotation);
	}

	void WriteLocalMatrix(string strHeader, Transform current)
	{
		binaryWriter.Write(strHeader);
		Matrix4x4 matrix = Matrix4x4.identity;
		matrix.SetTRS(current.localPosition, current.localRotation, current.localScale);
		WriteMatrix(matrix);
	}
	void WriteLocalMatrix(BinaryWriter bWriter, string strHeader, Transform current)
	{
		bWriter.Write(strHeader);
		Matrix4x4 matrix = Matrix4x4.identity;
		matrix.SetTRS(current.localPosition, current.localRotation, current.localScale);
		WriteMatrix(bWriter, matrix);
	}

	void WriteWorldMatrix(string strHeader, Transform current)
	{
		binaryWriter.Write(strHeader);
		Matrix4x4 matrix = Matrix4x4.identity;
		matrix.SetTRS(current.position, current.rotation, current.lossyScale);
		WriteMatrix(matrix);
	}

	void WriteBoneTransforms(string strHeader, Transform[] bones)
	{
		WriteString(strHeader, bones.Length);
		if (bones.Length > 0)
		{
			foreach (Transform bone in bones)
			{
				WriteMatrix(bone.localPosition, bone.localRotation, bone.localScale);
			}
		}
	}

	void WriteBoneNames(string strHeader, Transform[] bones)
	{
		WriteString(strHeader, bones.Length);
		if (bones.Length > 0)
		{
			foreach (Transform bone in bones) WriteObjectName(bone.gameObject);
		}
	}

	void WriteMatrixes(string strHeader, Matrix4x4[] matrixes)
	{
		WriteString(strHeader, matrixes.Length);
		if (matrixes.Length > 0)
		{
			foreach (Matrix4x4 matrix in matrixes) WriteMatrix(matrix);
		}
	}

	void WriteBoneIndex(BoneWeight boneWeight)
	{
		binaryWriter.Write(boneWeight.boneIndex0);
		binaryWriter.Write(boneWeight.boneIndex1);
		binaryWriter.Write(boneWeight.boneIndex2);
		binaryWriter.Write(boneWeight.boneIndex3);
	}

	void WriteBoneWeight(BoneWeight boneWeight)
	{
		binaryWriter.Write(boneWeight.weight0);
		binaryWriter.Write(boneWeight.weight1);
		binaryWriter.Write(boneWeight.weight2);
		binaryWriter.Write(boneWeight.weight3);
	}

	void WriteBoneIndices(string strHeader, BoneWeight[] boneWeights)
	{
		WriteString(strHeader, boneWeights.Length);
		if (boneWeights.Length > 0)
		{
			foreach (BoneWeight boneWeight in boneWeights) WriteBoneIndex(boneWeight);
		}
	}

	void WriteBoneWeights(string strHeader, BoneWeight[] boneWeights)
	{
		WriteString(strHeader, boneWeights.Length);
		if (boneWeights.Length > 0)
		{
			foreach (BoneWeight boneWeight in boneWeights) WriteBoneWeight(boneWeight);
		}
	}

	int GetTexturesCount(Material[] materials)
	{
		int nTextures = 0;
		for (int i = 0; i < materials.Length; i++)
		{
			if (materials[i].HasProperty("_MainTex"))
			{
				if (!FindTextureByName(m_pTextureNamesListForCounting, materials[i].GetTexture("_MainTex"))) nTextures++;
			}
			if (materials[i].HasProperty("_SpecGlossMap"))
			{
				if (!FindTextureByName(m_pTextureNamesListForCounting, materials[i].GetTexture("_SpecGlossMap"))) nTextures++;
			}
			if (materials[i].HasProperty("_MetallicGlossMap"))
			{
				if (!FindTextureByName(m_pTextureNamesListForCounting, materials[i].GetTexture("_MetallicGlossMap"))) nTextures++;
			}
			if (materials[i].HasProperty("_BumpMap"))
			{
				if (!FindTextureByName(m_pTextureNamesListForCounting, materials[i].GetTexture("_BumpMap"))) nTextures++;
			}
			if (materials[i].HasProperty("_EmissionMap"))
			{
				if (!FindTextureByName(m_pTextureNamesListForCounting, materials[i].GetTexture("_EmissionMap"))) nTextures++;
			}
			if (materials[i].HasProperty("_DetailAlbedoMap"))
			{
				if (!FindTextureByName(m_pTextureNamesListForCounting, materials[i].GetTexture("_DetailAlbedoMap"))) nTextures++;
			}
			if (materials[i].HasProperty("_DetailNormalMap"))
			{
				if (!FindTextureByName(m_pTextureNamesListForCounting, materials[i].GetTexture("_DetailNormalMap"))) nTextures++;
			}
		}
		return (nTextures);
	}

	int GetTexturesCount(Transform current)
	{
		int nTextures = 0;
		MeshRenderer meshRenderer = current.gameObject.GetComponent<MeshRenderer>();
		if (meshRenderer) nTextures = GetTexturesCount(meshRenderer.materials);

		for (int k = 0; k < current.childCount; k++) nTextures += GetTexturesCount(current.GetChild(k));

		return (nTextures);
	}

	void WriteMeshInfo(Mesh mesh)
	{
		WriteObjectName("<Mesh>:", mesh.vertexCount, mesh);

		WriteBoundingBox("<Bounds>:", mesh.bounds);

		WriteVectors("<Positions>:", mesh.vertices);
		// albedo 텍스처의 색상 사용
		//WriteColors("<Colors>:", mesh.colors);
		WriteTextureCoords("<TextureCoords>:", mesh.uv);
		//WriteTextureCoords("<TextureCoords1>:", mesh.uv2);
		WriteVectors("<Normals>:", mesh.normals);

		if ((mesh.normals.Length > 0) && (mesh.tangents.Length > 0))
		{
			Vector3[] tangents = new Vector3[mesh.tangents.Length];
			//Vector3[] biTangents = new Vector3[mesh.tangents.Length];
			for (int i = 0; i < mesh.tangents.Length; i++)
			{
				tangents[i] = new Vector3(mesh.tangents[i].x, mesh.tangents[i].y, mesh.tangents[i].z);
				//biTangents[i] = Vector3.Normalize(Vector3.Cross(mesh.normals[i], tangents[i])) * mesh.tangents[i].w;
			}

			WriteVectors("<Tangents>:", tangents);
			//WriteVectors("<BiTangents>:", biTangents);
		}

		// 인덱스
		WriteInteger("<SubSets>:", mesh.subMeshCount);
		if (mesh.subMeshCount > 0)
		{
			for (int i = 0; i < mesh.subMeshCount; i++)
			{
				int[] subindicies = mesh.GetTriangles(i);
				WriteIntegers("<SubSet>:", subindicies);
			}
		}

		WriteString("</Mesh>");
	}

	void WriteMaterials(Material[] materials)
	{
		WriteInteger("<Materials>:", materials.Length);
		for (int i = 0; i < materials.Length; i++)
		{
			WriteObjectName("<Material>:", i, materials[i]);

			if (materials[i].HasProperty("_Color"))
			{
				Color albedo = materials[i].GetColor("_Color");
				WriteColor("<AlbedoColor>:", albedo);
			}
			if (materials[i].HasProperty("_EmissionColor"))
			{
				Color emission = materials[i].GetColor("_EmissionColor");
				WriteColor("<EmissiveColor>:", emission);
			}
			if (materials[i].HasProperty("_SpecColor"))
			{
				Color specular = materials[i].GetColor("_SpecColor");
				WriteColor("<SpecularColor>:", specular);
			}
			if (materials[i].HasProperty("_Glossiness"))
			{
				WriteFloat("<Glossiness>:", materials[i].GetFloat("_Glossiness"));
			}
			if (materials[i].HasProperty("_Smoothness"))
			{
				WriteFloat("<Smoothness>:", materials[i].GetFloat("_Smoothness"));
			}
			if (materials[i].HasProperty("_Metallic"))
			{
				WriteFloat("<Metallic>:", materials[i].GetFloat("_Metallic"));
			}
			//if (materials[i].HasProperty("_SpecularHighlights"))
			//{
			//    WriteFloat("<SpecularHighlight>:", materials[i].GetFloat("_SpecularHighlights"));
			//}
			//if (materials[i].HasProperty("_GlossyReflections"))
			//{
			//    WriteFloat("<GlossyReflection>:", materials[i].GetFloat("_GlossyReflections"));
			//}

			if (materials[i].HasProperty("_MainTex"))
			{
				Texture mainAlbedoMap = materials[i].GetTexture("_MainTex");
				WriteTextureName("<AlbedoMap>:", mainAlbedoMap);
			}
			if (materials[i].HasProperty("_SpecGlossMap"))
			{
				Texture specularcMap = materials[i].GetTexture("_SpecGlossMap");
				WriteTextureName("<SpecGlossMap>:", specularcMap);
			}
			if (materials[i].HasProperty("_MetallicGlossMap"))
			{
				Texture metallicMap = materials[i].GetTexture("_MetallicGlossMap");
				WriteTextureName("<MetallicGlossMap>:", metallicMap);
			}
			if (materials[i].HasProperty("_BumpMap"))
			{
				Texture bumpMap = materials[i].GetTexture("_BumpMap");
				WriteTextureName("<NormalMap>:", bumpMap);
			}
			if (materials[i].HasProperty("_EmissionMap"))
			{
				Texture emissionMap = materials[i].GetTexture("_EmissionMap");
				WriteTextureName("<EmissionMap>:", emissionMap);
			}
			//TODO: 향후 디테일 uv 추출시 디테일 텍스처 정보도 추출
			//if (materials[i].HasProperty("_DetailAlbedoMap"))
			//{
			//    Texture detailAlbedoMap = materials[i].GetTexture("_DetailAlbedoMap");
			//    WriteTextureName("<DetailAlbedoMap>:", detailAlbedoMap);
			//}
			//if (materials[i].HasProperty("_DetailNormalMap"))
			//{
			//    Texture detailNormalMap = materials[i].GetTexture("_DetailNormalMap");
			//    WriteTextureName("<DetailNormalMap>:", detailNormalMap);
			//}
			WriteString("</Material>");
		}
		WriteString("</Materials>");
	}

	void WriteSkinnedMeshInfo(SkinnedMeshRenderer skinMeshRenderer)
	{
		// bones 정보가 없으면 일반메쉬와 다를바 없다...
		if (skinMeshRenderer.bones.Length <= 0)
		{
			WriteMeshInfo(skinMeshRenderer.sharedMesh);
			return;
		}

		WriteObjectName("<SkinningInfo>:", skinMeshRenderer);

		// 정점 1개가 최대 몇개의 bone에 영향을 받는가
		int nBonesPerVertex = (int)skinMeshRenderer.quality; //SkinQuality.Auto:0, SkinQuality.Bone1:1, SkinQuality.Bone2:2, SkinQuality.Bone4:4
		if (nBonesPerVertex == 0) nBonesPerVertex = 4;
		WriteInteger("<BonesPerVertex>:", nBonesPerVertex);

		//WriteBoundingBox("<Bounds>:", skinMeshRenderer.localBounds);

		// bone의 이름들
		WriteBoneNames("<BoneNames>:", skinMeshRenderer.bones);
		// bone offset 행렬
		WriteMatrixes("<BoneOffsets>:", skinMeshRenderer.sharedMesh.bindposes);
		// 정점의 영향을 주는 bone의 인덱스 4개
		WriteBoneIndices("<BoneIndices>:", skinMeshRenderer.sharedMesh.boneWeights);
		// 정점의 영향을 주는 bone의 weight 4개
		WriteBoneWeights("<BoneWeights>:", skinMeshRenderer.sharedMesh.boneWeights);

		WriteString("</SkinningInfo>");

		WriteMeshInfo(skinMeshRenderer.sharedMesh);
	}

	void WriteFrameInfo(Transform current)
	{
		int nTextures = GetTexturesCount(current);
		WriteObjectName("<Frame>:", current.gameObject);
		++m_nFrames;

		//WriteTransform("<Transform>:", current);
		WriteLocalMatrix("<TransformMatrix>:", current);

		MeshFilter meshFilter = current.gameObject.GetComponent<MeshFilter>();
		MeshRenderer meshRenderer = current.gameObject.GetComponent<MeshRenderer>();

		BoxCollider boxCollider = current.gameObject.GetComponent<BoxCollider>();

		GroundCollider groundCollider = current.gameObject.GetComponent<GroundCollider>();
		WallCollider wallCollider = current.gameObject.GetComponent<WallCollider>();

		MapNode mapNode = current.gameObject.GetComponent<MapNode>();

		if (boxCollider)
		{
			WriteBoundingBox("<BoxCollider>:", boxCollider.center, boxCollider.size);
		}
		if (mapNode)
		{
			if (mapNode.neighbors.Length > 0)
			{
				//노드의 위치
				WriteVector("<MapNode>:", current.position);
				//노드의 아이디
				WriteInteger(mapNode.GetNodeId());
				//이웃의 개수
				WriteInteger(mapNode.neighbors.Length);
				foreach (var neighbor in mapNode.neighbors)
				{
					MapNode neighborMapNode = neighbor.gameObject.GetComponent<MapNode>();
					if (neighborMapNode) WriteInteger(neighborMapNode.GetNodeId());
					else print("Not Find neighborMapNode");
				}
				return;
			}
		}
		if (meshFilter && meshRenderer)
		{
			WriteMeshInfo(meshFilter.sharedMesh);

			Material[] materials = meshRenderer.materials;
			if (materials.Length > 0) WriteMaterials(materials);

			if (groundCollider)
			{
				WriteString("<GroundCollider>:");
			}
			if (wallCollider)
			{
				WriteString("<WallCollider>:");
			}

		}
		else
		{
			SkinnedMeshRenderer skinMeshRenderer = current.gameObject.GetComponent<SkinnedMeshRenderer>();
			if (skinMeshRenderer)
			{
				WriteSkinnedMeshInfo(skinMeshRenderer);

				Material[] materials = skinMeshRenderer.materials;
				if (materials.Length > 0) WriteMaterials(materials);

			}
		}

	}

	void WriteFrameHierarchyInfo(Transform child)
	{
		WriteFrameInfo(child);

		WriteInteger("<Children>:", child.childCount);

		if (child.childCount > 0)
		{
			for (int k = 0; k < child.childCount; k++)
			{
				WriteFrameHierarchyInfo(child.GetChild(k));
			}
		}

		WriteString("</Frame>");
	}

	void WriteAnimationPositionHierarchy(Transform current)
	{
		WriteVector(current.localPosition);

		if (current.childCount > 0)
		{
			for (int k = 0; k < current.childCount; k++) WriteAnimationPositionHierarchy(current.GetChild(k));
		}
	}

	void WriteAnimationRotationHierarchy(Transform current)
	{
		WriteVector(current.localRotation);

		if (current.childCount > 0)
		{
			for (int k = 0; k < current.childCount; k++) WriteAnimationRotationHierarchy(current.GetChild(k));
		}
	}

	void WriteAnimationScaleHierarchy(Transform current)
	{
		WriteVector(current.localScale);

		if (current.childCount > 0)
		{
			for (int k = 0; k < current.childCount; k++) WriteAnimationScaleHierarchy(current.GetChild(k));
		}
	}

	void WriteAnimationMatrixHierarchy(Transform current)
	{
		WriteMatrix(current.localPosition, current.localRotation, current.localScale);

		if (current.childCount > 0)
		{
			for (int k = 0; k < current.childCount; k++) WriteAnimationMatrixHierarchy(current.GetChild(k));
		}
	}

	void WriteAnimationTransforms(string strHeader, int nKeyFrame, float fKeyFrameTime)
	{
		WriteString(strHeader, nKeyFrame, fKeyFrameTime);

		//root 프레임을 제외한 하위 노드들에 대한 애니메이션 정보만 저장한다.(root 프레임은 실제 게임에서 사용하기 때문에 덮어씌우면 안됨)
		if (transform.childCount > 0)
		{
			for (int k = 0; k < transform.childCount; k++) WriteAnimationMatrixHierarchy(transform.GetChild(k));
		}

		//WriteAnimationScaleHierarchy(transform);
		//WriteAnimationRotationHierarchy(transform);
		//WriteAnimationPositionHierarchy(transform);
	}

	void WriteFrameNameHierarchy(Transform current)
	{
		WriteObjectName(current.gameObject);

		if (current.childCount > 0)
		{
			for (int k = 0; k < current.childCount; k++) WriteFrameNameHierarchy(current.GetChild(k));
		}
	}

	void WriteFrameNames(string strHeader)
	{
		//root 프레임을 제외한 하위 노드들에 대한 애니메이션 정보만 저장한다.(root 프레임은 실제 게임에서 사용하기 때문에 덮어씌우면 안됨)
		WriteString(strHeader, m_nFrames - 1);

		if (transform.childCount > 0)
		{
			for (int k = 0; k < transform.childCount; k++) WriteFrameNameHierarchy(transform.GetChild(k));
		}

	}

	void WriteSkinnedMeshAnimationTransforms(string strHeader, int nKeyFrame, float fKeyFrameTime)
	{
		WriteString(strHeader, nKeyFrame, fKeyFrameTime);
		for (int i = 0; i < m_pSkinnedMeshRenderers.Length; i++)
		{
			WriteInteger(i);
			for (int j = 0; j < m_pSkinnedMeshRenderers[i].bones.Length; j++)
			{
				WriteMatrix(m_pSkinnedMeshRenderers[i].bones[j].localPosition, m_pSkinnedMeshRenderers[i].bones[j].localRotation, m_pSkinnedMeshRenderers[i].bones[j].localScale);
			}
		}
	}

	void WriteSkinnedMeshAnimationFrameNames(string strHeader)
	{
		WriteInteger(strHeader, m_pSkinnedMeshRenderers.Length);
		for (int i = 0; i < m_pSkinnedMeshRenderers.Length; i++)
		{
			WriteInteger(i);
			WriteObjectName(m_pSkinnedMeshRenderers[i]);
			WriteInteger(m_pSkinnedMeshRenderers[i].bones.Length);
			for (int j = 0; j < m_pSkinnedMeshRenderers[i].bones.Length; j++)
			{
				WriteObjectName(m_pSkinnedMeshRenderers[i].bones[j].gameObject);
			}
		}
	}
	void Start()
	{
		

		Scene scene = transform.gameObject.scene;
		GameObject[] rootGameObjects = scene.GetRootGameObjects();

		BinaryWriter sceneWriter = new BinaryWriter(File.Open("./Map/" +
					string.Copy(scene.name).Replace(" ", "_") + "/" + "Scene.bin", FileMode.Create));

		sceneWriter.Write(rootGameObjects.Length - exceptionObjects.Length);

		foreach (GameObject rootObject in rootGameObjects) 
		{
			if (exceptionObjects.Contains(rootObject))
				continue;

			string strToRemove = " (";
			int index = rootObject.name.IndexOf(strToRemove);
			string strNewName = (index > 0) ? string.Copy(rootObject.name).Remove(index) : string.Copy(rootObject.name);
			string objectModelName = strNewName.Replace(' ', '_'); // 오브젝트가 사용한 모델의 이름

			if(m_pModelInfoNames.Contains(objectModelName)) // 이미 이 모델에 대한 정보를 추출했음
			{
				sceneWriter.Write("@" + objectModelName);								// objectModelName 이 모델을

				Sector sector = rootObject.GetComponent<Sector>();
				if(sector)
				{
					WriteBoundingBox(sceneWriter, "<SectorBounds>:", sector.sectorCollider.center, sector.sectorCollider.size);
				}

				WriteLocalMatrix(sceneWriter, "<Transform>", rootObject.transform);		// gameOject.transform을 사용하여 씬에 배치하라
			}
			else
			{
				sceneWriter.Write(objectModelName);
				binaryWriter = new BinaryWriter(File.Open("./Map/" + 
					string.Copy(scene.name).Replace(" ", "_") + "/" + string.Copy(rootObject.name).Replace(" ", "_") + ".bin", FileMode.Create));
				
				m_pSkinnedMeshRenderers = rootObject.GetComponentsInChildren<SkinnedMeshRenderer>();
				for (int i = 0; i < m_pSkinnedMeshRenderers.Length; i++)
				{
					m_pSkinnedMeshRenderers[i].forceMatrixRecalculationPerRender = true;
				}
				Sector sector = rootObject.GetComponent<Sector>();
				if (sector)
				{
					WriteBoundingBox(sceneWriter, "<SectorBounds>:", sector.sectorCollider.center, sector.sectorCollider.size);
				}

				WriteLocalMatrix(sceneWriter, "<Transform>", rootObject.transform);
				
				Vector3 position = rootObject.transform.position;
				Quaternion rotation = rootObject.transform.rotation;
				Vector3 scale = rootObject.transform.localScale;

				rootObject.transform.position = Vector3.zero;
				rootObject.transform.rotation = Quaternion.identity;
				rootObject.transform.localScale = Vector3.one;

				WriteString("<Hierarchy>:");
				WriteFrameHierarchyInfo(rootObject.transform);
				WriteString("</Hierarchy>");

				rootObject.transform.position = position;
				rootObject.transform.rotation = rotation;
				rootObject.transform.localScale = scale;


				binaryWriter.Flush();
				binaryWriter.Close();

				m_pModelInfoNames.Add(objectModelName);
			}
		}
		sceneWriter.Flush();
		sceneWriter.Close();

		print("Model Binary Write Completed");
	}
}

