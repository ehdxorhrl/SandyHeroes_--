#pragma once
#include "Component.h"

class Mesh;

enum class GunFireType { kAuto, kSemiAuto, kBoltAction, kBurst };
enum class BulletType { kNormal, kBig, kSpecial };

struct GunInfo
{
    int damage{ 0 };
    float critical_damage_rate{ 2.0 };                 // 치명타 배율
    int rpm{ 300 };                                    // 총기 RPM (rounds per minute)
    int magazine_cacity{ 12 };                         // 장탄 수
    float reload_time{ 1.75 };                          // 재장전 시간
    GunFireType fire_type{ GunFireType::kSemiAuto };   // 발사 방식
    int burst_num{ 0 };                                // 점사 총알 갯수(점사 방식의 총만 사용)
    BulletType bullet_type{ BulletType::kNormal };     // 사용하는 총알 타입
    float bullet_speed{ 350.f };                       // 탄속

};

//오브젝트에 총기기능을 추가하는 컴포넌트
//총알 발사 및 장전, 데미지정보 등 총기에 대한 기능을 가진다.
class GunComponent :
    public Component
{
public:
    GunComponent(Object* owner);
    GunComponent(const GunComponent& other);
    ~GunComponent();
    
    virtual Component* GetCopy() override;

    virtual void Update(float elapsed_time) override;

    void ReloadBullets();

    bool FireBullet(XMFLOAT3 direction, Mesh* bullet_mesh);

    void LoadGunInfo(const std::string& gun_name);

    //getter
    GunFireType fire_type() const;

    //총기 정보를 로드하는 함수
    static void LoadGunInfosFromFile(const std::string& file_name); 

private:
    static std::unordered_map<std::string, GunInfo> kGunInfos;

    std::list<Object*> fired_bullet_list_;
    int loaded_bullets_{};      //남은 총알 수
    float loading_time_{ 0.f }; //장전 중 시간
    bool is_reload_ = false;    //장전 중?
    float cooling_time_{ 0.f }; //마지막 발사로 부터 경과한 시간

    // 총기 정보
    int damage_{ 0 };                   
    float critical_damage_rate_{ 2.0 };                 // 치명타 배율
    int rpm_{ 300 };                                    // 총기 RPM (rounds per minute)
    int magazine_cacity_{ 12 };                         // 장탄 수
    float reload_time_{ 1.75 };                          // 재장전 시간
    GunFireType fire_type_{ GunFireType::kSemiAuto };   // 발사 방식
    int burst_num_{ 0 };                                // 점사 총알 갯수(점사 방식의 총만 사용)
    BulletType bullet_type_{ BulletType::kNormal };     // 사용하는 총알 타입
    float bullet_speed_{ 350.f };                       // 탄속
};

