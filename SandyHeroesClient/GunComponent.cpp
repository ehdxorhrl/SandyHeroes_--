#include "stdafx.h"
#include "GunComponent.h"
#include "Object.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MovementComponent.h"
#include "BoxColliderComponent.h"
#include "DebugMeshComponent.h"
#include "MeshColliderComponent.h"
#include "Scene.h"
#include "ParticleComponent.h"
#include "FMODSoundManager.h"
#include "SoundComponent.h"

std::unordered_map<std::string, GunInfo> GunComponent::kGunInfos{};
const std::array<XMFLOAT4, kElementCount> GunComponent::kElementColors{
    XMFLOAT4{ 1.0f, 0.0f, 0.0f, 0.5f },
    XMFLOAT4{ 0.9f, 0.9f, 0.1f, 0.5f },
    XMFLOAT4{ 0.1f, 0.9f, 0.1f, 0.5f }
};

GunComponent::GunComponent(Object* owner) : Component(owner)
{
}

GunComponent::GunComponent(const GunComponent& other) : Component(other.owner_), 
    gun_name_(other.gun_name_), 
    damage_(other.damage_), 
    critical_damage_rate_(other.critical_damage_rate_),
    rpm_(other.rpm_), 
    magazine_cacity_(other.magazine_cacity_), 
    reload_time_(other.reload_time_),
    fire_type_(other.fire_type_), 
    burst_num_(other.burst_num_), 
    bullet_type_(other.bullet_type_),
    bullet_speed_(other.bullet_speed_), 
    loaded_bullets_(other.loaded_bullets_),
    loading_time_(other.loading_time_), 
    cooling_time_(other.cooling_time_),
    is_reload_(other.is_reload_),
	flamethrow_box_(other.flamethrow_box_)
{
}

Component* GunComponent::GetCopy()
{
    return new GunComponent(*this);
}

void GunComponent::Update(float elapsed_time)
{
    loading_time_ -= elapsed_time;
    cooling_time_ += elapsed_time;

    if (is_reload_ && loading_time_ <= 0.f)
    {
        loaded_bullets_ = magazine_cacity_;
        is_reload_ = false;
    }

    for (const auto& bullet : fired_bullet_list_)
    {
        auto bullet_position = bullet->world_position_vector();
        if (xmath_util_float3::Length(bullet_position - owner_->world_position_vector()) > 100.f)
        {
            bullet->set_is_dead(true);
        }
    }
}


void GunComponent::ReloadBullets()
{
    if (!is_reload_)
    {
        loading_time_ = reload_time_;
        //loading_time_ = reload_time_;
        is_reload_ = true;
    }
}

bool GunComponent::FireBullet(XMFLOAT3 direction, Object* bullet_model, Scene* scene)
{
    const float rps = rpm_ / 60.f;
    if (rps * cooling_time_ >= 1.f)
    {
        cooling_time_ = 0.f;

        auto root = owner_->GetHierarchyRoot();
        auto particle = Object::GetComponentInChildren<ParticleComponent>(root);
        if (particle)
        {
            particle->Play(25);
        }

        if (gun_name_ == "flamethrower")
        {
            return true;
        }

        if (!(bullet_type_ == BulletType::kSpecial))
        {
            FMODSoundManager::Instance().PlaySound("gun_fire", false, 0.3f);
        }

        Object* bullet = bullet_model;
        bullet->set_is_movable(true);
        XMFLOAT3 bullet_look = xmath_util_float3::Normalize(bullet->look_vector());
        XMFLOAT3 rotate_axis = xmath_util_float3::CrossProduct(bullet_look, direction);
        float angle = xmath_util_float3::AngleBetween(bullet_look, direction);

        XMMATRIX rotation_matrix = XMMatrixRotationAxis(XMLoadFloat3(&rotate_axis), angle);
        XMFLOAT4X4 transform_matrix = bullet->transform_matrix();
        XMStoreFloat4x4(&transform_matrix, rotation_matrix * XMLoadFloat4x4(&transform_matrix));
        bullet->set_transform_matrix(transform_matrix);

        MovementComponent* movement = new MovementComponent(bullet);
        bullet->AddComponent(movement);
        bullet->set_position_vector(owner_->world_position_vector());
        movement->DisableFriction();
        movement->set_gravity_acceleration(1.f);
        movement->set_max_speed_xz(bullet_speed_);
        movement->Move(direction, bullet_speed_);
        bullet->Scale(3.f);
        scene->AddObject(bullet);
        std::function<void(Object*)> on_destroy_func = [this](Object* bullet) {
            fired_bullet_list_.remove(bullet);
        };
        bullet->OnDestroy(on_destroy_func);
        fired_bullet_list_.push_back(bullet);

    }

    return false;
}

void GunComponent::LoadGunInfo(const std::string& gun_name)
{
    GunInfo gun_info{ kGunInfos[gun_name] };

	gun_name_ = gun_name;
    damage_ = gun_info.damage;
    critical_damage_rate_ = gun_info.critical_damage_rate;
    rpm_ = gun_info.rpm;
    magazine_cacity_ = gun_info.magazine_cacity;
    reload_time_ = gun_info.reload_time;
    fire_type_ = gun_info.fire_type;
    burst_num_ = gun_info.burst_num;
    bullet_type_ = gun_info.bullet_type;
    bullet_speed_ = gun_info.bullet_speed;

    loaded_bullets_ = magazine_cacity_;
}

void GunComponent::set_gun_name(std::string& value)
{
    gun_name_ = value;
}

void GunComponent::set_upgrade(int value)
{
    upgrade_ = value;
}

void GunComponent::set_element(ElementType value)
{
    element_ = value;
}

void GunComponent::set_loaded_bullets(int value)
{
    loaded_bullets_ = value;
}

GunFireType GunComponent::fire_type() const
{
    return fire_type_;
}

BulletType GunComponent::bullet_type() const
{
    return bullet_type_;
}

std::list<Object*> GunComponent::fired_bullet_list() const
{
    return fired_bullet_list_;
}

int GunComponent::damage() const
{
    return damage_;
}

float GunComponent::critical_damage_rate() const
{
    return critical_damage_rate_;
}

BoundingBox GunComponent::flamethrow_box() const
{
    return flamethrow_box_;
}

int GunComponent::upgrade() const
{
    return upgrade_;
}

ElementType GunComponent::element() const
{
    return element_;
}

int GunComponent::loaded_bullets() const
{
    return loaded_bullets_;
}

void GunComponent::LoadGunInfosFromFile(const std::string& file_name)
{
    std::ifstream file(file_name);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] != '@') continue;

        std::istringstream ss(line);
        std::string at_sign, name, bullet_type_str, fire_type_str;
        GunInfo info;

        ss >> at_sign >> name >> bullet_type_str
            >> info.damage >> info.critical_damage_rate >> info.rpm
            >> info.magazine_cacity >> info.reload_time >> fire_type_str >> info.burst_num >> info.bullet_speed;
 
        if (bullet_type_str == "normal")
        {
            info.bullet_type = BulletType::kNormal;
        }
        if (bullet_type_str == "big")
        {
            info.bullet_type = BulletType::kBig;
        }
        if (bullet_type_str == "special")
        {
            info.bullet_type = BulletType::kSpecial;
        }
        if (fire_type_str == "auto")
        {
            info.fire_type = GunFireType::kAuto;
        }
        if (fire_type_str == "semiauto")
        {
            info.fire_type = GunFireType::kSemiAuto;
        }
        if (fire_type_str == "boltaction")
        {
            info.fire_type = GunFireType::kBoltAction;
        }
        if (fire_type_str == "burst")
        {
            info.fire_type = GunFireType::kBurst;
        }
        kGunInfos[name] = info;
    }
}

