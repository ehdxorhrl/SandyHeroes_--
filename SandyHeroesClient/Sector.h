#pragma once

class Object;

class Sector
{
public:
	Sector() = default;
	Sector(const std::string& name, const BoundingBox& bounds);
	~Sector() = default;

	//오브젝트가 이 Sector에 속하는지 검사하고 속하면 오브젝트를 추가
	//반환 값: true면 오브젝트가 추가됨
	bool InsertObject(Object* object);

	void DeleteOutOfBoundsObjects();

	bool CheckObjectInSectorObjectList(Object* object)
	{
		return std::find(object_list_.begin(), object_list_.end(), object) != object_list_.end();
	}

	void DeleteObject(Object* object);
	void DeleteDeadObject();

	//setter
	void set_bounds(const BoundingBox& bounds);

	//getter
	BoundingBox bounds() const { return bounds_; }
	std::list<Object*>& object_list() { return object_list_; }

private:
	std::string name_;
	BoundingBox bounds_;
	std::list<Object*> object_list_;
};

