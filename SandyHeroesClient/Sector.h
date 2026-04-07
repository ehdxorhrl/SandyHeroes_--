#pragma once
#include <memory>

class MeshComponent;

class Sector
{
public:
	Sector() = default;
	Sector(const std::string& name, const BoundingBox& bounds);
	~Sector() = default;

	//오브젝트가 이 Sector에 속하는지 검사하고 속하면 오브젝트를 추가
	//반환 값: true면 오브젝트가 추가됨
	bool InsertMeshComponent(const std::shared_ptr<MeshComponent>& mesh_component);

	void DeleteOutOfBounds();

	bool CheckObjectInSectorMeshComponentList(const std::shared_ptr<MeshComponent>& mesh_component)
	{
		return std::find_if(mesh_component_list_.begin(), mesh_component_list_.end(), 
			[mesh_component](const std::weak_ptr<MeshComponent>& wp) {
			auto locked = wp.lock();
			return locked && (locked == mesh_component);
		}) != mesh_component_list_.end();
	}

	void DeleteMeshComponent(const std::shared_ptr<MeshComponent>& mesh_component);
	//setter
	void set_bounds(const BoundingBox& bounds);

	//getter
	BoundingBox bounds() const { return bounds_; }
	std::list<std::weak_ptr<MeshComponent>>& mesh_component_list() { return mesh_component_list_; }

private:
	std::string name_;
	BoundingBox bounds_;
	std::list<std::weak_ptr<MeshComponent>> mesh_component_list_;
};

