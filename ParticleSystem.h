#pragma once

class Mesh;
class Material;
class Scene;
class Object;

struct Particle
{
	Object* particle_object{ nullptr };
	float life_time{ 2.f };
	Particle(Object* object, float life_time) : particle_object(object), life_time(life_time) {}
};

class ParticleSystem
{
public:
	ParticleSystem() {}
	ParticleSystem(Mesh* paricle_mesh, Material* particle_material);
	~ParticleSystem() {}

	Object* CreateParticle(XMFLOAT3 position, XMFLOAT3 direction, float speed);
	void SpawnParticle(Scene* scene, XMFLOAT3 position, int particle_count, float particle_time);

	void Update(float elapsed_time);

private:
	Mesh* particle_mesh_{ nullptr };
	Material* particle_material_{ nullptr };
	std::list<Particle> particle_list_;

};

