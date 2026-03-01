# Sandy Heroes Server - Class Analysis

## File: `AIComponent.h`

### Class: `BTNode`
**Methods:**
- `bool Run(float elapsed_time)`

**Members:**
- (No parsed members)

### Class: `Selector`
**Inherits from:** `BTNode `

**Methods:**
- `bool Run(float elapsed_time)`

**Members:**
- `std::vector<BTNode*> children`

### Class: `Sequence`
**Inherits from:** `BTNode `

**Methods:**
- `bool Run(float elapsed_time)`

**Members:**
- `std::vector<BTNode*> children`

### Class: `ConditionNode`
**Inherits from:** `BTNode `

**Methods:**
- `ConditionNode(std::function<bool(float)> cond) : condition(std::move(cond))`
- `: condition([cond = std::move(cond)](float)`
- `bool Run(float elapsed_time)`
- `return condition(elapsed_time)`

**Members:**
- (No parsed members)

### Class: `ActionNode`
**Inherits from:** `BTNode `

**Methods:**
- `ActionNode(std::function<bool(float)> act) : action(std::move(act))`
- `: action([act = std::move(act)](float)`
- `bool Run(float elapsed_time)`
- `return action(elapsed_time)`

**Members:**
- (No parsed members)

### Class: `AIComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void SetBehaviorTree(BTNode* root)`
- `bool Move_To_Target(float dt)`
- `bool Rotate_To_Target(float elapsed_time, std::shared_ptr<Object> target)`
- `void Send_Move_Packet(float elapsed_time, float speed)`
- `std::shared_ptr<Object> GetCurrentTarget(Object* self)`
- `std::shared_ptr<Object> Set_Target(Object* self)`
- `bool InRangeXZ(Object* self, std::shared_ptr<Object> target, float r)`
- `bool InRange(Object* self, std::shared_ptr<Object> target, float r)`
- `std::unique_ptr<AIComponent> CreateMonsterAI(MonsterPtr monster)`
- `return InRangeXZ(self, target, range)`
- `return InRangeXZ(self, target, range)`

**Members:**
- `BTNode* behavior_tree_root_`
- `std::vector<Node*> path_`
- `bool direct_mode_`
- `float direct_mode_cooldown_`
- `auto* monster_component`
- `auto* monster_component`
- `float min_distance_sq`
- `std::shared_ptr<Object> nearest_player`
- `auto& sessions`
- `auto& session`
- `auto player`
- `float dist_sq`
- `auto d`
- `auto d`
- `std::unique_ptr<Node> root`
- `auto ai`
- `auto state`
- `auto target`
- `auto* ai`
- `bool is_range`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `auto monstercomp`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `auto& player_object`
- `auto playercomp`
- `auto* root`
- `auto* chase`
- `auto* attack`
- `auto* monstercomp`
- `auto* movement`
- `auto state`
- `auto& fired_thorns`
- `auto& users`
- `auto thorn`
- `auto thorn_position`
- `sc_packet_object_set_dead osd`
- `auto box`
- `auto player_box`
- `auto playercomp`
- `auto monstercomp`
- `sc_packet_object_set_dead osd`
- `auto movement`
- `XMFLOAT3 velocity`
- `XMFLOAT3 position`
- `constexpr float kGroundYOffset`
- `XMVECTOR ray_origin`
- `XMVECTOR ray_direction`
- `bool is_collide`
- `int a`
- `constexpr float MAX_DISTANCE`
- `BaseScene* base_scene`
- `int stage_num`
- `sc_packet_object_set_dead osd`
- `auto target`
- `auto* ai`
- `auto target`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `XMFLOAT3 thorn_position`
- `XMFLOAT3 direction`
- `BaseScene* base_scene`
- `auto thorn_projectile`
- `XMFLOAT3 look`
- `XMFLOAT3 rotate_axis`
- `float angle`
- `XMMATRIX rotation_matrix`
- `XMFLOAT4X4 transform_matrix`
- `MovementComponent* movement`
- `sc_packet_shotdragon_attack sa`
- `auto* root`
- `auto* seq`
- `auto* monstercomp`
- `constexpr float range`
- `constexpr float attack_cool_time`
- `auto state`
- `constexpr float animation_spf`
- `constexpr float start_attack_time`
- `constexpr float end_attack_time`
- `auto left_arm`
- `auto box`
- `auto& users`
- `auto player_box`
- `auto playercomp`
- `auto monstercomp`
- `sc_packet_player_damaged pd`
- `auto target`
- `auto target`
- `auto movement`
- `XMFLOAT3 look`
- `XMFLOAT3 direction`
- `float angle`
- `XMFLOAT3 cross`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `auto target`
- `auto* ai`
- `bool is_range`
- `auto* root`
- `auto* chase`
- `auto* attack`
- `auto* monstercomp`
- `auto* movement`
- `auto state`
- `constexpr float range`
- `constexpr float attack_cool_time`
- `constexpr float kSpawnWaitTime`
- `auto* monstercomp`
- `constexpr float animation_spf`
- `constexpr float start_attack_time`
- `constexpr float end_attack_time`
- `auto left_arm`
- `auto right_arm`
- `auto box_list`
- `auto& users`
- `auto player_box`
- `bool is_collide`
- `auto playercomp`
- `auto monstercomp`
- `sc_packet_player_damaged pd`
- `auto target`
- `auto* ai`
- `auto movement`
- `bool is_range`
- `sc_packet_monster_change_animation mca`
- `XMFLOAT3 target_position`
- `XMFLOAT3 direction`
- `XMFLOAT3 direction_xz`
- `XMFLOAT3 look`
- `float angle`
- `XMFLOAT3 cross`
- `auto& users`
- `sc_packet_monster_move mm`
- `XMFLOAT4X4 xf`
- `XMFLOAT4X4& mat`
- `auto target`
- `auto target`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `auto left_arm`
- `auto right_arm`
- `auto box_list`
- `auto& users`
- `auto player_box`
- `bool is_collide`
- `auto playercomp`
- `auto monstercomp`
- `sc_packet_player_damaged pd`
- `auto target`
- `auto* ai`
- `auto movement`
- `bool is_range`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `XMFLOAT3 target_position`
- `XMFLOAT3 direction`
- `XMFLOAT3 direction_xz`
- `XMFLOAT3 look`
- `float angle`
- `XMFLOAT3 cross`
- `auto& users`
- `sc_packet_monster_move mm`
- `XMFLOAT4X4 xf`
- `XMFLOAT4X4& mat`
- `auto target`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `auto* root`
- `auto wait_seq`
- `auto* seq_left`
- `auto* seq_right`
- `auto* monstercomp`
- `auto movement`
- `auto state`
- `constexpr float kRange`
- `constexpr float kSpeed`
- `auto* monstercomp`
- `constexpr float animation_spf`
- `constexpr float start_attack_time`
- `constexpr float end_attack_time`
- `auto head`
- `auto box`
- `auto& users`
- `auto player_box`
- `auto playercomp`
- `auto monstercomp`
- `sc_packet_player_damaged pd`
- `auto* ai`
- `auto movement`
- `XMFLOAT3 direction`
- `XMFLOAT3 look`
- `float angle`
- `XMFLOAT3 cross`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `sc_packet_monster_move mm`
- `XMFLOAT4X4 xf`
- `XMFLOAT4X4& mat`
- `auto& users`
- `auto* ai`
- `constexpr float max_revolution_time`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `constexpr float anglular_velocity`
- `float angle`
- `constexpr float dx`
- `constexpr float dz`
- `float start_angle`
- `float revolution_radius`
- `XMFLOAT3 look`
- `float rotate_angle`
- `XMFLOAT3 cross`
- `sc_packet_monster_move mm`
- `XMFLOAT4X4 xf`
- `XMFLOAT4X4& mat`
- `auto& users`
- `auto target`
- `auto* ai`
- `auto movement`
- `constexpr float kGroundY`
- `constexpr float kFlyHeight`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `XMFLOAT3 target_position`
- `XMFLOAT3 direction`
- `XMFLOAT3 direction_xz`
- `XMFLOAT3 look`
- `float angle`
- `XMFLOAT3 cross`
- `auto& users`
- `sc_packet_monster_move mm`
- `XMFLOAT4X4 xf`
- `XMFLOAT4X4& mat`
- `auto target`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `auto movement`
- `constexpr float animation_spf`
- `constexpr float start_attack_time`
- `constexpr float end_attack_time`
- `auto head`
- `auto box`
- `auto& users`
- `auto player_box`
- `auto playercomp`
- `auto monstercomp`
- `sc_packet_player_damaged pd`
- `auto target`
- `sc_packet_monster_change_animation mca`
- `auto& users`
- `auto* root`
- `auto* seq_left`
- `auto* seq_right`
- `auto* monstercomp`
- `auto movement`

## File: `AnimationSet.h`

### Class: `AnimationSet`
**Methods:**
- `float total_time()`
- `void LoadAnimationSetFromFile(std::ifstream& file, int frame_count)`
- `void AnimateBoneFrame(std::vector<XMFLOAT4X4>& animated_transforms, float animation_time, float weight)`

**Members:**
- `std::string name_`
- `float total_time_`
- `int frame_per_sec_`
- `int key_frame_count_`
- `std::vector<float> key_frame_times_`
- `std::vector<std::vector<XMFLOAT4X4>> key_frame_transforms_`

## File: `AnimationState.h`

### Class: `AnimationState`
**Methods:**
- `void Enter(int animation_track, Object* object, AnimatorComponent* animator)`
- `int Run(Object* object, bool is_end, AnimatorComponent* animator)`
- `void Exit(int animation_track, Object* object, AnimatorComponent* animator)`
- `AnimationState* GetCopy()`
- `int GetDeadAnimationTrack()`
- `void ChangeAnimationTrack(int animation_track, Object* object, AnimatorComponent* animator)`
- `void set_animation_loop_type(int value)`
- `int animation_loop_type()`
- `int animation_track()`
- `void set_animation_track(int value)`

**Members:**
- `int animation_loop_type_`
- `int repeat_count_`
- `int animation_track_`

## File: `AnimationTrack.h`

### Class: `AnimationTrack`
**Methods:**
- `bool is_end()`
- `void Start()`
- `void Start(AnimationLoopType loop_type, int repeat_count = 0)`
- `void Pause()`
- `void Stop()`
- `void PlayTrack(float elapsed_time, std::vector<XMFLOAT4X4>& animated_transforms, float weight)`

**Members:**
- `AnimationSet* animation_set_`
- `AnimationLoopType loop_type_`
- `int repeat_count_`
- `int repeat_counter_`
- `bool is_pause_`
- `bool is_end_`
- `float animation_time_`

## File: `AnimatorComponent.h`

### Class: `AnimatorComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void AttachBoneFrames()`
- `void set_animation_state(AnimationState* value)`
- `void set_speed_scale(float value)`
- `void set_is_ignore_root_bone_traslation(bool value)`
- `AnimationState* animation_state()`

**Members:**
- `std::unique_ptr<AnimationState> animation_state_`
- `float track_index_`
- `std::vector<AnimationTrack> animation_tracks_`
- `std::vector<std::string> frame_names_`
- `std::string root_bone_name_`
- `std::vector<Object*> bone_frames_`
- `Object* root_bone_frame_`
- `bool is_attached_bone_frames_`
- `bool is_root_motion_animation_`
- `bool is_ignore_root_bone_traslation_`
- `float speed_scale_`

## File: `BaseScene.h`

### Class: `BaseScene`
**Methods:**
- `void BuildMesh()`
- `void BuildModelInfo()`
- `void BuildObject()`
- `void CreateMonsterSpawner()`
- `bool ProcessInput(void* p)`
- `void Update(float elapsed_time)`
- `std::shared_ptr<Object> CreateAndRegisterPlayer(long long session_id)`
- `void AddObject(std::shared_ptr<Object> object)`
- `void DeleteObject(std::shared_ptr<Object> object)`
- `void UpdateObjectIsGround()`
- `void UpdateObjectHitWall()`
- `void UpdateObjectHitBullet(float elapsed_time)`
- `void UpdateObjectHitObject()`
- `void UpdateStageClear()`
- `void UpdateRazerHitEnemy()`
- `void PrepareGroundChecking()`
- `void CheckPlayerHitGun(std::shared_ptr<Object> object)`
- `void CheckObjectIsGround(std::shared_ptr<Object> object)`
- `void CheckPlayerHitWall(std::shared_ptr<Object> object, std::shared_ptr<MovementComponent> movement)`
- `void CheckObjectHitObject(std::shared_ptr<Object> object)`
- `void CheckPlayerHitPyramid(std::shared_ptr<Object> object)`
- `void CheckSpawnBoxHitPlayers()`
- `void CheckPlayerHitChest()`
- `void CheckRayHitEnemy(const XMFLOAT3& ray_origin, const XMFLOAT3& ray_direction, int id)`
- `void CheckObjectHitFlamethrow(std::shared_ptr<Object> object, int id, float elapsed_time)`
- `void CheckRazerHitEnemy(std::shared_ptr<RazerComponent> razer_component, std::shared_ptr<MonsterComponent> monster_component)`
- `void TickNoClipTimers(float elapsed_time)`
- `std::list<std::weak_ptr<MeshColliderComponent>> checking_maps_mesh_collider_list(int index)`
- `std::list<std::weak_ptr<WallColliderComponent>> stage_wall_collider_list(int index)`
- `int stage_clear_num()`
- `bool InRangeXZ(std::shared_ptr<Object> self, std::shared_ptr<Object> target, float r)`
- `void ActivateStageMonsterSpawner(int stage_num)`
- `void add_catch_monster_num()`
- `void add_stage_clear_num()`
- `const std::list<std::weak_ptr<MonsterComponent>>& monster_list()`
- `void set_is_activate_spawner(bool is_activate)`
- `: object(obj), movement(move)`
- `uint64_t PairKey_(int a, int b)`

**Members:**
- `bool is_activate_spawner_`
- `std::weak_ptr<Object> object`
- `std::weak_ptr<MovementComponent> movement`
- `std::list<std::weak_ptr<MonsterComponent>> monster_list_`
- `std::list<std::weak_ptr<RazerComponent>> razer_list_`
- `std::list<WallCheckObject> wall_check_object_list_`
- `std::vector<std::weak_ptr<Object>> dropped_guns_`
- `std::vector<std::weak_ptr<Object>> chests_`
- `BoundingOrientedBox stage3_clear_box_`
- `float noclip_monmon_radius_`
- `float noclip_monmon_duration_`
- `bool is_prepare_ground_checking_`
- `uint32_t x`
- `uint32_t y`

## File: `BillboardMeshComponent.h`

### Class: `BillboardMeshComponent`
**Methods:**
- `Component* GetCopy()`

**Members:**
- `Scene* scene_`

## File: `BoxColliderComponent.h`

### Class: `BoxColliderComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void set_box(const BoundingOrientedBox& box)`
- `const BoundingOrientedBox& box()`
- `BoundingOrientedBox animated_box()`

**Members:**
- (No parsed members)

## File: `CameraComponent.h`

### Class: `CameraComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void CreateProjectionMatrix(float near_plane_distance, float far_plane_distance, float aspect_ratio, float fov_angle)`
- `void UpdateCameraInfo()`
- `XMFLOAT4X4 view_matrix()`
- `XMFLOAT4X4 projection_matrix()`
- `XMFLOAT3 world_position()`

**Members:**
- `XMFLOAT4X4 view_matrix_`
- `XMFLOAT4X4 projection_matrix_`

## File: `ChestComponent.h`

### Class: `ChestComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void HendleCollision(Object* other_object, int chest_num)`
- `ScrollType TakeScroll(int chest_num)`

**Members:**
- (No parsed members)

## File: `Component.h`

### Class: `Component`
**Inherits from:** `std::enable_shared_from_this<Component>`

**Methods:**
- `void set_owner(Object* owner)`
- `std::shared_ptr<Object> owner()`
- `return new MeshComponent(*this)`
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `std::shared_ptr<Object> hierarchy_root()`

**Members:**
- `std::weak_ptr<Object> owner_`
- `std::weak_ptr<Object> hierarchy_root_`

## File: `CubeMesh.h`

### Class: `CubeMesh`
**Methods:**
- (No parsed methods)

**Members:**
- (No parsed members)

## File: `DebugMeshComponent.h`

### Class: `DebugMeshComponent`
**Methods:**
- `Component* GetCopy()`

**Members:**
- (No parsed members)

## File: `GameFramework.h`

### Class: `GameFramework`
**Methods:**
- `void Initialize()`
- `void ProcessInput()`
- `void FrameAdvance()`
- `void do_accept(SOCKET s_socket, EXP_OVER* accept_over)`
- `void worker()`
- `Scene* GetScene()`
- `GameFramework* Instance()`
- `void SetScene()`

**Members:**
- `GameFramework* kGameFramework`
- `HANDLE hIOCP_`
- `SOCKET socket_`
- `std::unique_ptr<Timer> server_timer_`
- `std::atomic<int> new_id_`
- `std::vector <std::thread> workers_`
- `std::chrono::steady_clock::time_point last_send_time_`
- `std::unique_ptr<Scene> scene_`

## File: `GroundColliderComponent.h`

### Class: `GroundColliderComponent`
**Methods:**
- `GroundColliderComponent(*this)`

**Members:**
- (No parsed members)

## File: `GunComponent.h`

### Class: `GunComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void ReloadBullets()`
- `bool FireBullet(XMFLOAT3 direction, Object* bullet_model, Scene* scene, int id)`
- `void LoadGunInfo(const std::string& gun_name)`
- `void set_gun_name(std::string& value)`
- `void set_upgrade(int value)`
- `void set_element(ElementType value)`
- `GunFireType fire_type()`
- `BulletType bullet_type()`
- `std::list<Object*> fired_bullet_list()`
- `int damage()`
- `float critical_damage_rate()`
- `int loaded_bullets()`
- `BoundingBox flamethrow_box()`
- `const std::string& gun_name()`
- `int upgrade()`
- `ElementType element()`
- `void LoadGunInfosFromFile(const std::string& file_name)`
- `XMFLOAT4 GetGunElementColor(GunComponent* gun)`

**Members:**
- `std::list<Object*> fired_bullet_list_`
- `bool is_reload_`
- `std::string gun_name_`

## File: `Material.h`

### Class: `Material`
**Methods:**
- `std::string name()`
- `int shader_type()`
- `void set_frame_resource_index(int value)`
- `void set_albedo_color(float r, float g, float b, float a)`
- `void set_albedo_color(XMFLOAT4 value)`
- `void set_name(const std::string& value)`
- `void set_shader_type(int value)`
- `void LoadMaterialFromFile(std::ifstream& file, std::vector<std::unique_ptr<Texture>>& textures)`
- `void AddTexture(Texture* texture)`
- `std::string GetTextureName(UINT index)`
- `void AddMeshComponent(MeshComponent* component)`
- `void CopyMaterialData(Material* material)`

**Members:**
- `float glossiness`
- `int frame_resource_index_`
- `std::list<Texture*> texture_list_`
- `int shader_type_`
- `std::list<MeshComponent*> mesh_component_list_`

## File: `Mesh.h`

### Class: `Mesh`
**Methods:**
- `void AddMeshComponent(MeshComponent* mesh_component)`
- `void DeleteMeshComponent(MeshComponent* mesh_component)`
- `void LoadMeshFromFile(std::ifstream& file)`
- `const std::list<MeshComponent*>& mesh_component_list()`
- `const std::vector<XMFLOAT3>& positions()`
- `const std::vector<std::vector<UINT>>& indices_array()`
- `BoundingBox bounds()`
- `std::string name()`
- `void set_name(const std::string& name)`
- `D3D_PRIMITIVE_TOPOLOGY primitive_topology()`

**Members:**
- `Mesh& operator`
- `D3D_PRIMITIVE_TOPOLOGY primitive_topology_`
- `std::vector<XMFLOAT3> positions_`
- `std::vector<XMFLOAT4> colors_`
- `std::vector<XMFLOAT2> uvs_`
- `std::vector<XMFLOAT3> normals_`
- `std::vector<XMFLOAT3> tangents_`
- `std::vector<XMFLOAT3> bi_tangents_`
- `std::vector<std::vector<UINT>> indices_array_`
- `std::string name_`
- `std::list<MeshComponent*> mesh_component_list_`

## File: `MeshColliderComponent.h`

### Class: `MeshColliderComponent`
**Methods:**
- `Component* GetCopy()`
- `bool CollisionCheckByRay(FXMVECTOR ray_origin, FXMVECTOR ray_direction, float& out_distance)`
- `bool CollisionCheckByObb(BoundingOrientedBox obb)`
- `BoundingOrientedBox GetWorldOBB()`
- `Mesh* mesh()`
- `void set_mesh(Mesh* mesh)`

**Members:**
- (No parsed members)

## File: `MeshComponent.h`

### Class: `MeshComponent`
**Methods:**
- `Component* GetCopy()`
- `Mesh* GetMesh()`
- `void set_mesh(Mesh* mesh)`
- `void AddMaterial(Material* material)`
- `bool ChangeMaterial(int index, Material* material)`
- `bool IsVisible()`
- `void set_is_visible(bool value)`

**Members:**
- `MeshComponent& operator`
- `Mesh* mesh_`
- `std::vector<Material*> materials_`
- `bool is_visible_`
- `bool is_in_view_frustum_`

## File: `ModelInfo.h`

### Class: `ModelInfo`
**Methods:**
- `std::string model_name()`
- `void LoadAnimationInfoFromFile(std::ifstream& file)`
- `std::shared_ptr<Object> GetInstance()`
- `std::shared_ptr<Object> hierarchy_root()`
- `void set_hierarchy_root(std::shared_ptr<Object> root)`
- `void set_model_name(const std::string& value)`

**Members:**
- `std::string model_name_`
- `std::shared_ptr<Object> hierarchy_root_`
- `std::vector<std::unique_ptr<AnimationSet>> animation_sets_`
- `std::vector<std::string> frame_names_`
- `std::string root_bone_name_`

## File: `MonsterComponent.h`

### Class: `MonsterComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void UpdateTargetPath()`
- `void InitAfterOwnerSet()`
- `void HitDamage(float damage)`
- `void set_max_shield(float value)`
- `void set_shield(float value)`
- `void set_max_hp(float value)`
- `void set_hp(float value)`
- `void set_attack_force(float value)`
- `void set_target(std::shared_ptr<Object> target)`
- `void set_is_pushed(bool is_pushed)`
- `void set_push_timer(float value)`
- `void set_scene(Scene* value)`
- `void RebuildBehaviorTree_()`
- `float shield()`
- `float hp()`
- `float max_hp()`
- `float max_shield()`
- `float attack_force()`
- `std::shared_ptr<Object> target()`
- `Scene* scene()`
- `bool IsDead()`

**Members:**
- `std::weak_ptr<Object> target_`
- `AIComponent* ai_`
- `std::vector<Node*> path_`

## File: `MovementComponent.h`

### Class: `MovementComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void EnableGravity()`
- `void DisableGarvity()`
- `void EnableFriction()`
- `void DisableFriction()`
- `void Move(float x, float y, float z, float speed)`
- `void Move(XMFLOAT3 direction, float speed)`
- `void MoveXZ(float x, float z, float speed)`
- `void Jump(float speed, float max_height = 999.f)`
- `void Stop()`
- `void BackupPosition()`
- `void set_gravity_acceleration(float value)`
- `void set_max_speed_xz(float value)`
- `void set_max_speed(float value)`
- `void set_velocity(const XMFLOAT3& velocity)`
- `XMFLOAT3 velocity()`
- `float max_speed_xz()`

**Members:**
- (No parsed members)

## File: `Object.h`

### Class: `Object`
**Inherits from:** `std::enable_shared_from_this<Object>`

**Methods:**
- `UINT id()`
- `void set_id(const long long id)`
- `XMFLOAT4X4 transform_matrix()`
- `XMFLOAT3 position_vector()`
- `XMFLOAT3 look_vector()`
- `XMFLOAT3 right_vector()`
- `XMFLOAT3 up_vector()`
- `XMFLOAT4X4 world_matrix()`
- `XMFLOAT3 world_position_vector()`
- `XMFLOAT3 world_look_vector()`
- `XMFLOAT3 world_right_vector()`
- `XMFLOAT3 world_up_vector()`
- `XMFLOAT3 old_position()`
- `XMFLOAT3 velocity()`
- `std::string tag()`
- `Object* child()`
- `Object* sibling()`
- `bool is_ground()`
- `bool is_player()`
- `bool is_movable()`
- `CollideType collide_type()`
- `void ApplyGravity(float elapsed_time)`
- `void Destroy()`
- `XMFLOAT3 local_scale()`
- `XMFLOAT3 local_rotation()`
- `XMFLOAT3 local_position()`
- `MonsterType monster_type()`
- `int animation_state()`
- `void set_transform_matrix(const XMFLOAT4X4& value)`
- `void set_position_vector(const XMFLOAT3& value)`
- `void set_position_vector(float x, float y, float z)`
- `void set_local_scale(const XMFLOAT3& value)`
- `void set_local_rotation(const XMFLOAT3& value)`
- `void set_local_position(const XMFLOAT3& value)`
- `void set_look_vector(const XMFLOAT3& value)`
- `void set_right_vector(const XMFLOAT3& value)`
- `void set_up_vector(const XMFLOAT3& value)`
- `void set_old_position(const XMFLOAT3& value)`
- `void set_is_player()`
- `void set_name(const std::string& value)`
- `void set_velocity(const XMFLOAT3& value)`
- `void set_is_ground(bool on_ground)`
- `void set_tag(const std::string& value)`
- `void set_monster_type(MonsterType type)`
- `void set_animation_state(int animation_state)`
- `void AddChild(std::shared_ptr<Object> object)`
- `void AddSibling(std::shared_ptr<Object> object)`
- `void AddComponent(std::shared_ptr<Component> component)`
- `std::shared_ptr<Object> FindFrame(const std::string& name)`
- `void PrintFrameNamesRecursive(int depth)`
- `std::shared_ptr<Object> GetHierarchyRoot()`
- `std::string name()`
- `void UpdateWorldMatrix(const XMFLOAT4X4* const parent_transform)`
- `void Update(float elapsed_time)`
- `void Rotate(float pitch, float yaw, float roll)`
- `void Scale(float value)`
- `void OnDestroy(std::function<void(Object*)> func)`
- `void ChangeChild(std::shared_ptr<Object> src, const std::string& dst_name, bool is_delete)`
- `std::shared_ptr<Object> DeepCopy(const std::shared_ptr<Object>& value, const std::shared_ptr<Object>& parent = nullptr)`
- `void set_collide_type(bool ground_check, bool wall_check)`
- `void set_collide_type(const CollideType& collide_type)`
- `Object* GetCopy()`

**Members:**
- `std::list<std::shared_ptr<T>> r_value`
- `std::shared_ptr<T> component`
- `std::list<std::shared_ptr<T>> component_list`
- `std::list<std::shared_ptr<T>> r_value`
- `auto sibling_components`
- `auto child_components`
- `XMFLOAT4X4 transform_matrix_`
- `std::weak_ptr<Object> parent_`
- `std::shared_ptr<Object> child_`
- `std::shared_ptr<Object> sibling_`
- `std::list<std::shared_ptr<Component>> component_list_`
- `std::string name_`
- `std::string tag_`
- `MonsterType type_`
- `float gravity_`
- `bool is_player_`
- `bool is_ground_`
- `bool is_movable_`
- `CollideType collide_type_`
- `bool is_in_view_sector_`
- `XMFLOAT4X4 world_matrix_`
- `UINT kObjectNextId`
- `UINT id_`

## File: `PlayerAnimationState.h`

### Class: `PlayerAnimationState`
**Methods:**
- `void Enter(int animation_track, Object* object, AnimatorComponent* animator)`
- `int Run(Object* object, bool is_end, AnimatorComponent* animator)`
- `void Exit(int animation_track, Object* object, AnimatorComponent* animator)`
- `AnimationState* GetCopy()`

**Members:**
- `int kGunTypeOffset`

## File: `PlayerComponent.h`

### Class: `PlayerComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void Heal(float amount)`
- `void HealHp(float heal_amount)`
- `void HealShield(float heal_amount)`
- `void HitDamage(float damage)`
- `void AddScroll(ScrollType type)`
- `bool HasScroll(ScrollType type)`
- `bool ActivateMainSkill()`
- `float max_hp()`
- `float hp()`
- `float max_shield()`
- `float shield()`
- `const std::unordered_set<ScrollType>& acquired_scrolls()`
- `float main_skill_gage()`
- `float main_skill_max_gage()`
- `float dash_gage()`
- `float dash_max_gage()`
- `void set_scene(Scene* scene)`
- `void set_dash_gage(float gage)`

**Members:**
- `Scene* scene_`
- `float max_hp_`
- `float hp_`
- `float max_shield_`
- `float shield_`
- `std::unordered_set<ScrollType> acquired_scrolls_`
- `ModelInfo* razer_model_info_`
- `float main_skill_gage_`
- `float main_skill_max_gage_`
- `float main_skill_activation_time_`
- `float main_skill_max_activation_time_`
- `float main_skill_razer_shot_time_`
- `float main_skill_razer_cool_time_`
- `float main_skill_range_`
- `float dash_gage_`
- `float dash_max_gage_`
- `float damage_cool_time_`
- `float last_damage_time_`
- `bool is_damaged`

## File: `RazerComponent.h`

### Class: `RazerComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void InitRazer(XMFLOAT3 start, XMFLOAT3 end)`
- `float life_time()`
- `XMFLOAT3 start_position()`
- `XMFLOAT3 end_position()`
- `float damage()`
- `float is_collision_active()`
- `bool is_collided()`
- `void set_is_collided(bool value)`

**Members:**
- `float life_time_`
- `float max_life_time_`
- `float damage_`
- `bool is_collision_active_`
- `bool is_collided_`

## File: `RazerMesh.h`

### Class: `RazerMesh`
**Methods:**
- (No parsed methods)

**Members:**
- (No parsed members)

## File: `Scene.h`

### Class: `Scene`
**Methods:**
- `void Initialize(GameFramework* game_framework)`
- `void BuildObject()`
- `void BuildMesh()`
- `void BuildScene(const std::string& scene_name)`
- `bool CheckObjectByObjectCollisions()`
- `bool ProcessInput(void* p)`
- `XMVECTOR GetPickingPointAtWorld(float sx, float sy, Object* picked_object)`
- `void AddObject(std::shared_ptr<Object> object)`
- `void Update(float elapsed_time)`
- `void DeleteObject(std::shared_ptr<Object> object)`
- `void UpdateObjectWorldMatrix()`
- `std::shared_ptr<Object> FindObject(const std::string& object_name)`
- `ModelInfo* FindModelInfo(const std::string& name)`
- `Mesh* FindMesh(const std::string& mesh_name, const std::vector<std::unique_ptr<Mesh>>& meshes)`
- `Texture* FindTexture(const std::string& texture_name, const std::vector<std::unique_ptr<Texture>>& textures)`
- `Material* FindMaterial(const std::string& material_name, const std::vector<std::unique_ptr<Material>>& materials)`
- `std::shared_ptr<Object> CreatePlayerObject(long long session_id)`
- `const std::vector<std::unique_ptr<Mesh>>& meshes()`

**Members:**
- `std::list<std::shared_ptr<Object>> object_list_`
- `std::vector<std::unique_ptr<Mesh>> meshes_`
- `std::vector<std::unique_ptr<ModelInfo>> model_infos_`
- `std::vector<std::unique_ptr<Material>> materials_`
- `std::vector<std::unique_ptr<Texture>> textures_`
- `std::vector<Sector> sectors_`
- `bool is_prepare_ground_checking_`
- `std::list<std::weak_ptr<Object>> ground_check_object_list_`

## File: `ScrollComponent.h`

### Class: `ScrollComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`

**Members:**
- `float moved_distance_`
- `bool is_active_`
- `ScrollType type_`

## File: `Sector.h`

### Class: `Sector`
**Methods:**
- `bool InsertObject(std::shared_ptr<Object> object)`
- `void DeleteOutOfBoundsObjects()`
- `void DeleteObject(Object* object)`
- `void set_bounds(const BoundingBox& bounds)`
- `BoundingBox bounds()`
- `std::list<std::weak_ptr<Object>>& object_list()`

**Members:**
- `auto locked`
- `std::string name_`
- `BoundingBox bounds_`
- `std::list<std::weak_ptr<Object>> object_list_`

## File: `SessionManager.h`

### Class: `SessionManager`
**Methods:**
- `SessionManager& getInstance()`
- `void add(int clientId, std::shared_ptr<Session> sess)`
- `std::shared_ptr<Session> get(int clientId)`
- `void remove(int clientId)`
- `Session* GetSessionByPlayerObject(std::shared_ptr<Object> object)`

**Members:**
- `SessionManager inst`
- `SessionManager& operator`
- `std::mutex mtx_`

## File: `SkinnedMesh.h`

### Class: `SkinnedMesh`
**Methods:**
- `void LoadSkinnedMeshFromFile(std::ifstream& file)`

**Members:**
- `int bones_per_vertex_`
- `std::vector<std::string> bone_names_`
- `std::vector<XMFLOAT4X4> bone_offsets_`
- `ComPtr<ID3D12Resource> d3d_bone_offset_buffer_`
- `ComPtr<ID3D12Resource> d3d_bone_offset_upload_buffer_`
- `std::vector<XMINT4> bone_indices_`
- `ComPtr<ID3D12Resource> d3d_bone_index_buffer_`
- `ComPtr<ID3D12Resource> d3d_bone_index_upload_buffer_`
- `std::vector<XMFLOAT4> bone_weights_`
- `ComPtr<ID3D12Resource> d3d_bone_weight_buffer_`
- `ComPtr<ID3D12Resource> d3d_bone_weight_upload_buffer_`

## File: `SkinnedMeshComponent.h`

### Class: `SkinnedMeshComponent`
**Methods:**
- `Component* GetCopy()`
- `void AttachBoneFrames(const std::vector<std::string>& bone_names)`

**Members:**
- `SkinnedMeshComponent& operator`
- `std::vector<Object*> bone_frames_`
- `bool is_attached_bone_frames_`

## File: `SpawnerComponent.h`

### Class: `SpawnerComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void SetSpawnerInfo(int spawn_count, float spawn_time, float spawn_cool_time)`
- `void SetMonsterType(MonsterType type)`
- `void ActivateSpawn()`
- `void DeactivateSpawn()`
- `void AddComponent(std::unique_ptr<Component> component)`
- `void AddComponent(Component* component)`
- `void ForceSpawn()`

**Members:**
- `std::list<std::unique_ptr<Component>> component_list_`
- `MonsterType monster_type_`

## File: `Timer.h`

### Class: `Timer`
**Methods:**
- `float TotalTime()`
- `float PlayTime()`
- `float ElapsedTime()`
- `int Fps()`
- `void Reset()`
- `void Start()`
- `void Stop()`
- `void Tick()`

**Members:**
- `double seconds_per_count_`
- `double elapsed_time_`
- `__int64 base_time_`
- `__int64 paused_time_`
- `__int64 stop_time_`
- `__int64 prev_time_`
- `__int64 curr_time_`
- `bool is_stop_`

## File: `User.h`

### Class: `EXP_OVER`
**Methods:**
- (No parsed methods)

**Members:**
- `WSAOVERLAPPED over_`
- `IO_OP io_op_`
- `SOCKET accept_socket_`

### Class: `Session`
**Methods:**
- `void update(float elapsed_time)`
- `void do_recv()`
- `void do_send(void* buff)`
- `void send_player_info_packet()`
- `void send_player_position()`
- `void process_packet(unsigned char* p, float elapsed_time)`
- `void set_player_object(Object* obj)`
- `void set_camera_object(Object* obj)`
- `long long get_id()`
- `std::shared_ptr<Object> get_player_object()`
- `std::shared_ptr<Object> get_camera_object()`
- `bool IsKeyDown(int key_code)`
- `void SetKeyDown(int key_code, bool is_down)`
- `bool is_firekey_down()`

**Members:**
- `SOCKET c_socket_`
- `long long id_`
- `std::string name_`
- `std::shared_ptr<Object> player_object_`
- `std::shared_ptr<Object> camera_object_`
- `bool is_firekey_down_`
- `bool is_jumpkey_pressed_`
- `float jump_speed_`
- `float y_axis_velocity_`
- `float gravity_`
- `bool is_dash_pressed_`
- `float dash_cool_delta_time_`
- `float dash_cool_time_`
- `unsigned char remained_`

## File: `WallColliderComponent.h`

### Class: `WallColliderComponent`
**Methods:**
- `WallColliderComponent(*this)`
- `bool CollisionCheckByRay(FXMVECTOR ray_origin, FXMVECTOR ray_direction, float& out_distance)`

**Members:**
- `BoxColliderComponent* box_collider_`

