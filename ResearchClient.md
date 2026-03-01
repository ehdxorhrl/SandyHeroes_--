# Sandy Heroes Client - Class Analysis

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
- `int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)`
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
- `void set_max_change_time(float value)`
- `AnimationState* animation_state()`

**Members:**
- `std::unique_ptr<AnimationState> animation_state_`
- `float track_index_`
- `float before_track_index_`
- `std::vector<AnimationTrack> animation_tracks_`
- `std::vector<std::string> frame_names_`
- `std::string root_bone_name_`
- `std::vector<XMFLOAT4X4> animated_tramsforms_`
- `float change_time_`
- `float max_change_time_`
- `std::vector<Object*> bone_frames_`
- `Object* root_bone_frame_`
- `bool is_attached_bone_frames_`
- `bool is_root_motion_animation_`
- `bool is_ignore_root_bone_traslation_`
- `float speed_scale_`

## File: `BaseScene.h`

### Class: `BaseScene`
**Methods:**
- `void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)`
- `void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildTextFormat(IDWriteFactory* dwrite_factory)`
- `void BuildModelInfo(ID3D12Device* device)`
- `void CreatePlayerUI()`
- `void CreateMonsterSpawner()`
- `void ActivateStageMonsterSpawner(int stage_num)`
- `void ShowClearRogo()`
- `bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)`
- `const std::list<std::weak_ptr<MeshComponent>>& GetShadowMeshList(int index)`
- `void Update(float elapsed_time)`
- `void AddObject(std::shared_ptr<Object> object)`
- `void DeleteObject(std::shared_ptr<Object> object)`
- `void DeleteKeyObject(int idx)`
- `void PrepareGroundChecking()`
- `void SpawnMonsterDamagedParticle(const XMFLOAT3& position, const XMFLOAT4& color)`
- `void set_stage_clear_num(int value)`
- `std::list<std::weak_ptr<MonsterComponent>> monster_list()`
- `int stage_clear_num()`
- `std::shared_ptr<ParticleComponent> dragon_particle()`
- `void add_stage_clear_num()`
- `void add_catch_monster_num()`
- `void add_remote_player(int id, const std::string& name, const XMFLOAT4X4& value)`
- `void add_drop_gun(int id, uint8_t gun_type, uint8_t upgrade_level, uint8_t element_type, const XMFLOAT4X4& matrix)`
- `void change_gun(uint32_t gun_id, const std::string& gun_name, uint8_t upgrade_level, uint8_t element_type, uint32_t player_id)`
- `void OpenScrollChest(uint8_t scroll_type, uint8_t chest_num)`
- `void TakeScroll(uint8_t chest_num)`
- `void PlayCutScene(uint8_t track_num)`
- `: object(obj), movement(move)`

**Members:**
- `std::shared_ptr<ParticleComponent> dragon_particle_`
- `bool is_activate_spawner_`
- `std::weak_ptr<Object> object`
- `std::weak_ptr<MovementComponent> movement`
- `std::list<std::weak_ptr<RazerComponent>> razer_list_`
- `std::list<std::weak_ptr<MonsterComponent>> monster_list_`
- `std::vector<std::weak_ptr<Object>> monster_hit_particles_`
- `std::vector<std::weak_ptr<Object>> dropped_guns_`
- `std::vector<std::weak_ptr<Object>> chests_`
- `std::vector<std::weak_ptr<Object>> sounds_`

## File: `BillboardMesh.h`

### Class: `BillboardMesh`
**Methods:**
- `void UpdateConstantBuffer(FrameResource* curr_frame_resource, int& start_index)`
- `void UpdateConstantBufferForShadow(FrameResource* curr_frame_resource, int& start_index)`

**Members:**
- (No parsed members)

## File: `BombDragonAnimationState.h`

### Class: `BombDragonAnimationState`
**Methods:**
- `void Enter(int animation_track, Object* object, AnimatorComponent* animator)`
- `int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)`
- `void Exit(int animation_track, Object* object, AnimatorComponent* animator)`
- `AnimationState* GetCopy()`
- `int GetDeadAnimationTrack()`

**Members:**
- (No parsed members)

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

## File: `BreathingShader.h`

### Class: `BreathingShader`
**Inherits from:** `Shader`

**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`
- `D3D12_BLEND_DESC CreateBlendState()`

**Members:**
- (No parsed members)

## File: `CameraComponent.h`

### Class: `CameraComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void CreateProjectionMatrix(float near_plane_distance, float far_plane_distance, float aspect_ratio, float fov_angle)`
- `bool CollisionCheckByMeshComponent(MeshComponent* mesh_component)`
- `void GetPickingRay(int screen_x, int screen_y, XMFLOAT3& ray_origin, XMFLOAT3& ray_direction)`
- `void UpdateCameraInfo()`
- `XMFLOAT4X4 view_matrix()`
- `XMFLOAT4X4 projection_matrix()`
- `XMFLOAT3 world_position()`
- `XMFLOAT3 up_vector()`
- `BoundingFrustum view_frustum()`

**Members:**
- `XMFLOAT4X4 view_matrix_`
- `XMFLOAT4X4 projection_matrix_`

## File: `CharacterComponent.h`

### Class: `CharacterComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void HendleCollision(Object* other_object)`

**Members:**
- (No parsed members)

## File: `ChestAnimationState.h`

### Class: `ChestAnimationState`
**Methods:**
- `void Enter(int animation_track, Object* object, AnimatorComponent* animator)`
- `int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)`
- `void Exit(int animation_track, Object* object, AnimatorComponent* animator)`
- `AnimationState* GetCopy()`
- `int GetDeadAnimationTrack()`

**Members:**
- (No parsed members)

## File: `ChestComponent.h`

### Class: `ChestComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void HendleCollision(Object* other_object)`
- `void OpenChest(uint8_t scroll_type, ModelInfo* scroll_model)`
- `ScrollType TakeScroll()`

**Members:**
- (No parsed members)

## File: `ColliderComponent.h`

### Class: `ColliderComponent`
**Inherits from:** `Component`

**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `bool CheckOBBMeshCollision(const MeshComponent* other_mesh, const XMFLOAT4X4& player_world_matrix, const XMFLOAT4X4& other_world_matrix)`
- `bool CheckRayGroundCollision(const MeshComponent* other_mesh, const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDir, float maxDistance, const XMFLOAT4X4& worldMatrix)`
- `bool RayIntersectsTriangle(XMVECTOR rayOrigin, XMVECTOR rayDir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float& t)`
- `bool Intersects(ColliderComponent* collider_object)`
- `XMFLOAT3 extent()`

**Members:**
- `BoundingOrientedBox* bounding_box_`

## File: `ColorShader.h`

### Class: `ColorShader`
**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`

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
- `Object* hierarchy_root()`

**Members:**
- `std::weak_ptr<Object> owner_`
- `std::weak_ptr<Object> hierarchy_root_`

## File: `CubeMesh.h`

### Class: `CubeMesh`
**Methods:**
- (No parsed methods)

**Members:**
- (No parsed members)

## File: `CutSceneRecorder.h`

### Class: `CutSceneRecorder`
**Methods:**
- `void Start()`
- `void Pause()`
- `void Stop()`
- `void CaptureCameraTransform(float elapsed_time)`
- `void WriteCutScene(const std::string& cut_scene_name)`

**Members:**
- `std::list<CutSceneData> cut_scene_data_list_`

## File: `CutSceneTrack.h`

### Class: `CutSceneTrack`
**Methods:**
- `void LoadCutSceneFromFile(const std::string& cut_scene_name)`
- `void Play(Scene* scene)`
- `void Pause()`
- `void Stop()`
- `void Run(float elapsed_time)`
- `void set_camera(std::shared_ptr<Object> value)`
- `std::shared_ptr<Object> camera()`

**Members:**
- `std::shared_ptr<Object> camera_`
- `std::shared_ptr<Object> scene_camera_`

## File: `DebugShader.h`

### Class: `DebugShader`
**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_RASTERIZER_DESC CreateRasterizerState()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`

**Members:**
- (No parsed members)

## File: `DescriptorManager.h`

### Class: `DescriptorManager`
**Methods:**
- `UINT cbv_bone_transform_offset()`
- `UINT cbv_pass_offset()`
- `UINT srv_offset()`
- `UINT texture_count()`
- `void ResetDescriptorHeap(ID3D12Device* device, int texture_count)`
- `ID3D12DescriptorHeap* GetDescriptorHeap()`
- `D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(int index)`
- `D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(int index)`

**Members:**
- `DescriptorManager& operator`
- `UINT kCbvSrvUavDescriptorSize`
- `DescriptorManager* kDescriptorManager`
- `ComPtr<ID3D12DescriptorHeap> d3d_cbv_srv_uav_heap_`
- `UINT cbv_bone_transform_offset_`
- `UINT cbv_pass_offset_`
- `UINT srv_offset_`
- `UINT texture_count_`

## File: `FadeInUIComponent.h`

### Class: `FadeInUIComponent`
**Inherits from:** `Component`

**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void Reset()`

**Members:**
- (No parsed members)

## File: `FMODSoundManager.h`

### Class: `FMODSoundManager`
**Methods:**
- `FMODSoundManager& Instance()`
- `void Initialize()`
- `void Release()`
- `void LoadSound(const std::string& name, const std::string& path, bool loop = false)`
- `void PlaySound(const std::string& name, bool loop = false, float volume = 1.0f)`
- `void StopSound(const std::string& name)`
- `FMOD::System* system()`

**Members:**
- `FMOD::System* system_`

## File: `FPSControllerComponent.h`

### Class: `FPSControllerComponent`
**Methods:**
- `Component* GetCopy()`
- `bool ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)`
- `void Update(float elapsed_time)`
- `void Stop()`
- `void set_camera_object(std::shared_ptr<Object> value)`
- `void set_scene(Scene* value)`
- `void set_particle(std::shared_ptr<ParticleComponent> value)`
- `bool is_firekey_down()`
- `std::shared_ptr<Object> camera_object()`

**Members:**
- `Scene* scene_`
- `std::weak_ptr<Object> camera_object_`
- `std::weak_ptr<ParticleComponent> particle_`
- `bool is_firekey_down_`
- `bool is_jumpkey_pressed_`
- `float jump_speed_`
- `float y_axis_velocity_`
- `float gravity_`
- `bool is_dash_pressed_`
- `float dash_cool_delta_time_`
- `float dash_cool_time_`

## File: `FrameResourceManager.h`

### Class: `FrameResourceManager`
**Methods:**
- `FrameResource* curr_frame_resource()`
- `int curr_frame_resource_index()`
- `int pass_count()`
- `int object_count()`
- `int skinned_mesh_object_count()`
- `FrameResource* GetResource(int index)`
- `void CirculateFrameResource(ID3D12Fence* fence)`

**Members:**
- `FrameResourceManager& operator`
- `int kFrameCount`
- `FrameResourceManager* kFrameResourceManager`
- `std::vector<std::unique_ptr<FrameResource>> frame_resources_`
- `FrameResource* curr_frame_resource_`
- `int curr_frame_resource_index_`
- `int pass_count_`
- `int object_count_`
- `int skinned_mesh_object_count_`
- `int material_count_`

## File: `GameFramework.h`

### Class: `GameFramework`
**Methods:**
- `void Initialize(HINSTANCE hinstance, HWND hwnd)`
- `void InitDirect3D()`
- `void CreateCommandObject()`
- `void CreateSwapChain()`
- `void CreateRtvAndDsvDescriptorHeaps()`
- `void BuildRootSignature()`
- `void OnResize()`
- `void ChangeWindowMode()`
- `void ProcessInput()`
- `void ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)`
- `void FrameAdvance()`
- `GameFramework* Instance()`
- `void FlushCommandQueue()`
- `D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()`
- `D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()`
- `LRESULT CALLBACK ProcessWindowMessage(HWND h_wnd, UINT message_id, WPARAM w_param, LPARAM l_param)`
- `void AddDebugText(const std::wstring& text)`
- `FrameResourceManager* frame_resource_manager()`
- `DescriptorManager* descriptor_manager()`
- `HWND main_wnd()`
- `XMFLOAT2 client_size()`
- `SOCKET socket()`
- `Scene* scene()`
- `TextRenderer* text_renderer()`
- `void ConnectServer(const char* ip, uint16_t port)`
- `void send_login_packet()`
- `void send_mouse_click_packet()`
- `void send_mouse_unclick_packet()`
- `void send_mouse_move_packet(int x1, int x2)`
- `void send_keyboard_input_packet(WPARAM w_param, bool is_press)`
- `void ProcessPacket(char* p)`
- `void RecvThreadProc()`
- `void do_send(void* p)`

**Members:**
- `GameFramework* kGameFramework`
- `UINT kInitFenceValue`
- `UINT kSwapChainBufferCount`
- `UINT kDepthStencilBufferCount`
- `constexpr UINT SHADOW_MAP_SIZE`
- `HINSTANCE app_instance_`
- `HWND main_wnd_`
- `ComPtr<IDXGIFactory4> dxgi_factory_`
- `ComPtr<ID3D12Device> d3d_device_`
- `ComPtr<ID3D12Fence> d3d_fence_`
- `UINT64 current_fence_value_`
- `ComPtr<ID3D12CommandQueue> d3d_command_queue_`
- `ComPtr<ID3D12CommandAllocator> d3d_command_allocator_`
- `ComPtr<ID3D12GraphicsCommandList> d3d_command_list_`
- `ComPtr<IDXGISwapChain3> dxgi_swap_chain_`
- `ComPtr<ID3D12DescriptorHeap> d3d_rtv_heap_`
- `ComPtr<ID3D12DescriptorHeap> d3d_dsv_heap_`
- `ComPtr<ID3D12Resource> d3d_depth_stencil_buffer_`
- `ComPtr<ID3D12Resource> d3d_shadow_depth_buffer_`
- `ComPtr<ID3D12RootSignature> d3d_root_signature_`
- `ComPtr<ID3D11On12Device> d3d11on12_device_`
- `ComPtr<ID3D11DeviceContext> d3d11_device_context_`
- `ComPtr<ID2D1Factory3> d2d_factory_`
- `ComPtr<ID2D1Device2> d2d_device_`
- `ComPtr<ID2D1DeviceContext2> d2d_device_context_`
- `ComPtr<IDWriteFactory> dwrite_factory_`
- `UINT rtv_descriptor_size_`
- `UINT dsv_descriptor_size_`
- `UINT cbv_srv_uav_descriptor_size_`
- `UINT msaa_quality_`
- `bool msaa_state_`
- `DXGI_FORMAT back_buffer_format_`
- `UINT current_back_buffer_`
- `DXGI_FORMAT depth_stencil_buffer_format_`
- `int client_width_`
- `int client_height_`
- `int client_refresh_rate_`
- `BOOL client_full_screen_state_`
- `D3D12_VIEWPORT client_viewport_`
- `D3D12_RECT scissor_rect_`
- `D3D12_VIEWPORT shadow_viewport_`
- `D3D12_RECT shadow_scissor_rect_`
- `std::unique_ptr<Timer> client_timer_`
- `std::unique_ptr<Scene> scene_`
- `std::unique_ptr<FrameResourceManager> frame_resource_manager_`
- `std::unique_ptr<DescriptorManager> descriptor_manager_`
- `std::unique_ptr<InputManager> input_manager_`
- `ComPtr<ID2D1SolidColorBrush> d2d_text_brush_`
- `std::unique_ptr<TextFormat> debug_text_format_`
- `std::unique_ptr<TextRenderer> text_renderer_`
- `SOCKET socket_`
- `long long id_`
- `EXP_OVER recv_over_`
- `std::thread recv_thread_`
- `bool recv_running_`
- `std::chrono::steady_clock::time_point last_mouse_packet_time_`
- `bool is_initialized_`

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
- `bool FireBullet(XMFLOAT3 direction, Object* bullet_mesh, Scene* scene)`
- `void LoadGunInfo(const std::string& gun_name)`
- `void set_gun_name(std::string& value)`
- `void set_upgrade(int value)`
- `void set_element(ElementType value)`
- `void set_loaded_bullets(int value)`
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

**Members:**
- `std::list<Object*> fired_bullet_list_`
- `bool is_reload_`
- `std::string gun_name_`

## File: `HitDragonAnimationState.h`

### Class: `HitDragonAnimationState`
**Methods:**
- `void Enter(int animation_track, Object* object, AnimatorComponent* animator)`
- `int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)`
- `void Exit(int animation_track, Object* object, AnimatorComponent* animator)`
- `AnimationState* GetCopy()`
- `int GetDeadAnimationTrack()`

**Members:**
- (No parsed members)

## File: `InputControllerComponent.h`

### Class: `InputControllerComponent`
**Methods:**
- `Component* GetCopy()`
- `bool ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)`
- `void set_client_wnd(HWND value)`

**Members:**
- (No parsed members)

## File: `InputManager.h`

### Class: `InputManager`
**Methods:**
- `void EnQueueInputMessage(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)`
- `InputMessage DeQueueInputMessage(float play_time)`
- `bool IsEmpty()`
- `bool IsFull()`

**Members:**
- `InputManager* kInputManager`
- `constexpr UINT kMaxInputMessage`
- `constexpr float kExpirationDateInputMessage`
- `int head_`
- `int tail_`

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
- `int CreateShaderResourceViews(ID3D12Device* device, DescriptorManager* descriptor_manager, int start_index)`
- `void LoadMaterialFromFile(std::ifstream& file, std::vector<std::unique_ptr<Texture>>& textures)`
- `void AddTexture(Texture* texture)`
- `std::string GetTextureName(UINT index)`
- `void AddMeshComponent(std::weak_ptr<MeshComponent> component)`
- `bool DeleteMeshComponent(std::weak_ptr<MeshComponent> component)`
- `void CopyMaterialData(Material* material)`

**Members:**
- `float glossiness`
- `int frame_resource_index_`
- `std::list<Texture*> texture_list_`
- `int shader_type_`
- `std::list<std::weak_ptr<MeshComponent>> mesh_component_list_`

## File: `Mesh.h`

### Class: `Mesh`
**Methods:**
- `void AddMeshComponent(MeshComponent* mesh_component)`
- `void DeleteMeshComponent(MeshComponent* mesh_component)`
- `void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void ReleaseUploadBuffer()`
- `void UpdateConstantBuffer(FrameResource* curr_frame_resource, int& start_index)`
- `void UpdateConstantBufferForShadow(FrameResource* curr_frame_resource, int& start_index)`
- `void Render(ID3D12GraphicsCommandList* command_list, int material_index, FrameResource* curr_frame_resource)`
- `void LoadMeshFromFile(std::ifstream& file)`
- `void ClearColors()`
- `void ClearUvs()`
- `void ClearNormals()`
- `void ClearTangents()`
- `std::string name()`
- `const std::list<MeshComponent*>& mesh_component_list()`
- `BoundingBox bounds()`
- `const std::vector<XMFLOAT3>& positions()`
- `const std::vector<std::vector<UINT>>& indices_array()`
- `D3D12_PRIMITIVE_TOPOLOGY primitive_topology()`
- `int instance_count()`
- `void set_name(const std::string& name)`

**Members:**
- `Mesh& operator`
- `D3D12_PRIMITIVE_TOPOLOGY primitive_topology_`
- `std::vector<XMFLOAT3> positions_`
- `ComPtr<ID3D12Resource> d3d_position_buffer_`
- `ComPtr<ID3D12Resource> d3d_position_upload_buffer_`
- `std::vector<XMFLOAT4> colors_`
- `ComPtr<ID3D12Resource> d3d_color_buffer_`
- `ComPtr<ID3D12Resource> d3d_color_upload_buffer_`
- `std::vector<XMFLOAT2> uvs_`
- `ComPtr<ID3D12Resource> d3d_uv_buffer_`
- `ComPtr<ID3D12Resource> d3d_uv_upload_buffer_`
- `std::vector<XMFLOAT3> normals_`
- `ComPtr<ID3D12Resource> d3d_normal_buffer_`
- `ComPtr<ID3D12Resource> d3d_normal_upload_buffer_`
- `std::vector<XMFLOAT3> tangents_`
- `ComPtr<ID3D12Resource> d3d_tangent_buffer_`
- `ComPtr<ID3D12Resource> d3d_tangent_upload_buffer_`
- `std::vector<XMFLOAT3> bi_tangents_`
- `ComPtr<ID3D12Resource> d3d_bi_tangent_buffer_`
- `ComPtr<ID3D12Resource> d3d_bi_tangent_upload_buffer_`
- `std::vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffer_views_`
- `std::vector<std::vector<UINT>> indices_array_`
- `std::vector<ComPtr<ID3D12Resource>> d3d_index_buffers_`
- `std::vector<ComPtr<ID3D12Resource>> d3d_index_upload_buffers_`
- `std::vector<D3D12_INDEX_BUFFER_VIEW> index_buffer_views_`
- `std::string name_`
- `std::list<MeshComponent*> mesh_component_list_`
- `int instance_count_`
- `int instance_buffer_offset_`

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
- `void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index)`
- `void UpdateConstantBufferForShadow(FrameResource* current_frame_resource, int cb_index)`
- `void Render(Material* material, ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource)`
- `void AddMaterial(Material* material)`
- `bool ChangeMaterial(int index, Material* material)`
- `bool IsVisible()`
- `void set_is_visible(bool value)`
- `void set_is_in_view_frustum(bool value)`
- `bool is_in_view_frustum()`
- `Mesh* GetMesh()`
- `Material* GetMaterial(int index = 0)`
- `void set_mesh(Mesh* mesh)`
- `UINT constant_buffer_index()`

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
- `void HitDamage(float damage)`
- `void set_shield(float value)`
- `void set_hp(float value)`
- `void set_attack_force(float value)`
- `void set_target(std::shared_ptr<Object> target)`
- `void set_is_pushed(bool is_pushed)`
- `void set_push_timer(float value)`
- `float shield()`
- `float hp()`
- `float max_hp()`
- `float max_shield()`
- `float attack_force()`
- `std::shared_ptr<Object> target()`
- `void set_scene(Scene* value)`
- `bool IsDead()`

**Members:**
- `std::weak_ptr<Object> target_`

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
- `void set_gravity_acceleration(float value)`
- `void set_max_speed_xz(float value)`
- `void set_max_speed(float value)`
- `void set_velocity(XMFLOAT3 value)`
- `XMFLOAT3 velocity()`
- `float max_speed_xz()`

**Members:**
- (No parsed members)

## File: `Object.h`

### Class: `Object`
**Inherits from:** `std::enable_shared_from_this<Object>`

**Methods:**
- `XMFLOAT4X4 transform_matrix()`
- `XMFLOAT3 position_vector()`
- `XMFLOAT3 look_vector()`
- `XMFLOAT3 right_vector()`
- `XMFLOAT3 up_vector()`
- `bool is_player()`
- `XMFLOAT4X4 world_matrix()`
- `XMFLOAT3 world_position_vector()`
- `XMFLOAT3 world_look_vector()`
- `XMFLOAT3 world_right_vector()`
- `XMFLOAT3 world_up_vector()`
- `UINT id()`
- `std::string name()`
- `std::string tag()`
- `Object* child()`
- `Object* sibling()`
- `Object* parent()`
- `bool is_ground()`
- `CollideType collide_type()`
- `XMFLOAT3 local_scale()`
- `XMFLOAT3 local_rotation()`
- `XMFLOAT3 local_position()`
- `bool is_movable()`
- `bool is_in_view_sector()`
- `float life_time()`
- `void set_transform_matrix(const XMFLOAT4X4& value)`
- `void set_position_vector(const XMFLOAT3& value)`
- `void set_position_vector(float x, float y, float z)`
- `void set_look_vector(const XMFLOAT3& value)`
- `void set_right_vector(const XMFLOAT3& value)`
- `void set_up_vector(const XMFLOAT3& value)`
- `void set_name(const std::string& value)`
- `void set_id(const long long id)`
- `void set_tag(const std::string& value)`
- `void set_is_ground(bool on_ground)`
- `void set_collide_type(bool ground_check, bool wall_check)`
- `void set_collide_type(const CollideType& collide_type)`
- `void set_local_scale(const XMFLOAT3& value)`
- `void set_local_rotation(const XMFLOAT3& value)`
- `void set_local_position(const XMFLOAT3& value)`
- `void set_is_player()`
- `void set_is_movable(bool value)`
- `void set_is_in_view_sector(bool value)`
- `void ResetSRTFromTransformMatrix()`
- `void AddChild(std::shared_ptr<Object> object)`
- `void AddSibling(std::shared_ptr<Object> object)`
- `void AddComponent(std::shared_ptr<Component> component)`
- `std::shared_ptr<Object> FindFrame(const std::string& name)`
- `std::shared_ptr<Object> GetHierarchyRoot()`
- `void DeleteChild(const std::string& name)`
- `void ChangeChild(std::shared_ptr<Object> src, const std::string& dst_name, bool is_delete = true)`
- `void UpdateWorldMatrix(const XMFLOAT4X4* const parent_transform)`
- `void Update(float elapsed_time)`
- `void Rotate(float pitch, float yaw, float roll)`
- `void Scale(float value)`
- `void EnableFuncInHeirachy(std::function<void(Object*, void*)> func, void* value)`
- `void OnDestroy(std::function<void(Object*)> func)`
- `void Destroy()`
- `std::shared_ptr<Object> DeepCopy(const std::shared_ptr<Object>& value, const std::shared_ptr<Object>& parent = nullptr)`

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
- `bool is_player_`
- `float life_time_`
- `bool is_ground_`
- `CollideType collide_type_`
- `bool is_movable_`
- `bool is_in_view_sector_`
- `XMFLOAT4X4 world_matrix_`
- `UINT kObjectNextId`
- `UINT id_`

## File: `ParticleComponent.h`

### Class: `ParticleComponent`
**Methods:**
- `bool IsActive()`
- `Component* GetCopy()`
- `void Initialize(Object* owner, ID3D12Device* device, UINT particle_count, eShape shape, Material* material)`
- `void Update(float elapsed_time)`
- `void Render(ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource)`
- `bool IsAlive(const int index)`
- `Particle particle(const int index)`
- `UINT capacity()`
- `Material* material()`
- `XMFLOAT4 color()`
- `void set_scene(Scene* value)`
- `void set_hit_position(XMFLOAT3 value)`
- `void set_loop(bool value)`
- `void set_direction_pivot_object(Object* value)`
- `void set_color(const XMFLOAT4& color)`
- `void Play(int particle_count)`

**Members:**
- `XMVECTOR velocity_`
- `float speed_`
- `float life_time_`
- `float max_life_time_`
- `XMFLOAT4 color_`
- `XMFLOAT3 position_`
- `XMFLOAT2 size_`
- `ParticleRenderer* kParticleRenderer`
- `Scene* scene_`
- `Object* direction_pivot_object_`
- `ID3D12Device* device_`
- `UINT capacity_`
- `UINT alive_count_`
- `std::vector<Particle> particles_`
- `std::vector<ParticleData> particle_data_`
- `D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_`
- `std::unique_ptr<UploadBuffer<Particle>> particle_buffer_`
- `Material* material_`
- `eShape shape_`
- `XMFLOAT3 hit_position_`
- `XMFLOAT4 color_`

## File: `ParticleRenderer.h`

### Class: `ParticleRenderer`
**Methods:**
- (No parsed methods)

**Members:**
- `std::list<ParticleComponent*> particle_components_`

## File: `ParticleShader.h`

### Class: `ParticleShader`
**Inherits from:** `Shader`

**Methods:**
- `void CreateShader(ID3D12Device* device, ID3D12RootSignature* root_signature)`
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob** shader_blob)`
- `D3D12_BLEND_DESC CreateBlendState()`
- `D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState()`

**Members:**
- (No parsed members)

## File: `ParticleSystem.h`

### Class: `ParticleSystem`
**Methods:**
- `Object* CreateParticle(XMFLOAT3 position, XMFLOAT3 direction, float speed)`
- `void SpawnParticle(Scene* scene, XMFLOAT3 position, int particle_count, float particle_time)`
- `void Update(float elapsed_time)`

**Members:**
- `std::list<Particle> particle_list_`

## File: `PlayerAnimationState.h`

### Class: `PlayerAnimationState`
**Methods:**
- `void Enter(int animation_track, Object* object, AnimatorComponent* animator)`
- `int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)`
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
- `void ActivateMainSkill()`
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
- `void set_main_skill_gage(float gage)`
- `void set_hp(float hp)`
- `void set_shield(float shield)`

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

## File: `ProgressBarComponent.h`

### Class: `ProgressBarComponent`
**Methods:**
- `ProgressBarComponent(*this)`
- `void Update(float elapsed_time)`
- `void set_max_value(float value)`
- `void set_current_value(float value)`

**Members:**
- (No parsed members)

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

## File: `RazerShader.h`

### Class: `RazerShader`
**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_RASTERIZER_DESC CreateRasterizerState()`
- `D3D12_BLEND_DESC CreateBlendState()`
- `D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`

**Members:**
- (No parsed members)

## File: `RecorderScene.h`

### Class: `RecorderScene`
**Methods:**
- `void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)`
- `void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)`
- `void Update(float elapsed_time)`

**Members:**
- (No parsed members)

## File: `RecordingScene.h`

### Class: `RecordingScene`
**Methods:**
- `void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)`
- `void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void Render(ID3D12GraphicsCommandList* command_list)`
- `bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)`
- `void Update(float elapsed_time)`

**Members:**
- (No parsed members)

## File: `Scene.h`

### Class: `Scene`
**Methods:**
- `void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)`
- `void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildFrameResources(ID3D12Device* device)`
- `void BuildDescriptorHeap(ID3D12Device* device)`
- `void BuildConstantBufferViews(ID3D12Device* device)`
- `void BuildShaderResourceViews(ID3D12Device* device)`
- `void BuildTextFormat(IDWriteFactory* dwrite_factory)`
- `void BuildScene()`
- `void InitializeSectorObjectlist()`
- `bool CheckObjectByObjectCollisions()`
- `void AddObject(std::shared_ptr<Object> object)`
- `void DeleteObject(std::shared_ptr<Object> object)`
- `void ReleaseMeshUploadBuffer()`
- `void UpdateRenderPassConstantBuffer(ID3D12GraphicsCommandList* command_list)`
- `void UpdateRenderPassShadowBuffer(ID3D12GraphicsCommandList* command_list)`
- `const std::list<std::weak_ptr<MeshComponent>>& GetShadowMeshList(int index = 0)`
- `void UpdateObjectConstantBuffer(FrameResource* curr_frame_resource)`
- `void Render(ID3D12GraphicsCommandList* command_list)`
- `void ShadowRender(ID3D12GraphicsCommandList* command_list)`
- `void RenderText(ID2D1DeviceContext2* d2d_device_context)`
- `void ParticleRender(ID3D12GraphicsCommandList* command_list)`
- `bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)`
- `void Update(float elapsed_time)`
- `void UpdateObjectWorldMatrix()`
- `void UpdateSector()`
- `void RunViewFrustumCulling()`
- `std::shared_ptr<Object> FindObject(const std::string& object_name)`
- `std::shared_ptr<Object> FindObject(const long long id)`
- `ModelInfo* FindModelInfo(const std::string& name)`
- `Mesh* FindMesh(const std::string& mesh_name, const std::vector<std::unique_ptr<Mesh>>& meshes)`
- `Material* FindMaterial(const std::string& material_name, const std::vector<std::unique_ptr<Material>>& materials)`
- `Texture* FindTexture(const std::string& texture_name, const std::vector<std::unique_ptr<Texture>>& textures)`
- `const std::vector<std::unique_ptr<Mesh>>& meshes()`
- `std::shared_ptr<CameraComponent> main_camera()`
- `XMFLOAT2 screen_size()`
- `bool is_play_cutscene()`
- `Object* player()`
- `std::list<std::weak_ptr<WallColliderComponent>> stage_wall_collider_list(int index)`
- `void set_main_camera(std::shared_ptr<CameraComponent> value)`
- `void set_is_play_cutscene(bool value)`

**Members:**
- `std::list<std::shared_ptr<Object>> object_list_`
- `std::vector<std::unique_ptr<Mesh>> meshes_`
- `std::vector<std::unique_ptr<ModelInfo>> model_infos_`
- `std::vector<std::unique_ptr<Material>> materials_`
- `std::vector<std::unique_ptr<Texture>> textures_`
- `std::vector<Sector> sectors_`
- `bool is_render_debug_mesh_`
- `bool is_prepare_ground_checking_`
- `std::weak_ptr<Object> player_`

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

## File: `Shader.h`

### Class: `Shader`
**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_RASTERIZER_DESC CreateRasterizerState()`
- `D3D12_BLEND_DESC CreateBlendState()`
- `D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`
- `void CreateShader(ID3D12Device* device, ID3D12RootSignature* root_signature)`
- `ID3D12PipelineState* GetPipelineState()`
- `ShaderType shader_type()`
- `void ReserveMaterials(UINT capacity)`
- `void AddMaterial(Material* material)`
- `void set_is_render(bool value)`

**Members:**
- `ComPtr<ID3D12PipelineState> d3d_pipeline_state_`
- `ShaderType shader_type_`
- `bool is_frustum_culling_`
- `bool is_render_`

## File: `ShadowShader.h`

### Class: `ShadowShader`
**Inherits from:** `Shader`

**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`
- `D3D12_RASTERIZER_DESC CreateRasterizerState()`
- `void CreateShader(ID3D12Device* device, ID3D12RootSignature* root_signature)`

**Members:**
- (No parsed members)

## File: `ShotDragonAnimationState.h`

### Class: `ShotDragonAnimationState`
**Methods:**
- `void Enter(int animation_track, Object* object, AnimatorComponent* animator)`
- `int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)`
- `void Exit(int animation_track, Object* object, AnimatorComponent* animator)`
- `AnimationState* GetCopy()`
- `int GetDeadAnimationTrack()`

**Members:**
- (No parsed members)

## File: `SkinnedMesh.h`

### Class: `SkinnedMesh`
**Methods:**
- `void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void ReleaseUploadBuffer()`
- `void UpdateConstantBuffer(FrameResource* curr_frame_resource, int& cb_index)`
- `void UpdateConstantBufferForShadow(FrameResource* curr_frame_resource, int& cb_index)`
- `void Render(ID3D12GraphicsCommandList* command_list, int material_index, FrameResource* curr_frame_resource)`
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
- `void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index)`
- `void UpdateConstantBufferForShadow(FrameResource* current_frame_resource, int cb_index)`
- `void Render(Material* material, ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource)`
- `void AttachBoneFrames(const std::vector<std::string>& bone_names)`

**Members:**
- `SkinnedMeshComponent& operator`
- `std::vector<Object*> bone_frames_`
- `bool is_attached_bone_frames_`

## File: `SkinnedMeshShader.h`

### Class: `SkinnedMeshShader`
**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_RASTERIZER_DESC CreateRasterizerState()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`

**Members:**
- (No parsed members)

## File: `SkinnedShadowShader.h`

### Class: `SkinnedShadowShader`
**Inherits from:** `Shader`

**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`
- `D3D12_RASTERIZER_DESC CreateRasterizerState()`

**Members:**
- (No parsed members)

## File: `SkyboxMesh.h`

### Class: `SkyboxMesh`
**Methods:**
- `Material* CreateSkyboxMaterial(const std::string& texture_name, Texture* skybox_texture)`

**Members:**
- (No parsed members)

## File: `SkyboxShader.h`

### Class: `SkyboxShader`
**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`
- `D3D12_RASTERIZER_DESC CreateRasterizerState()`
- `D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState()`

**Members:**
- (No parsed members)

## File: `SoundComponent.h`

### Class: `SoundComponent`
**Inherits from:** `Component`

**Methods:**
- `Component* GetCopy()`
- `void Load(const std::string& name, const std::string& path, bool loop = false)`
- `void Play(const std::string& name, float volume = 1.0f)`
- `void Stop(const std::string& name)`

**Members:**
- (No parsed members)

## File: `SpawnerComponent.h`

### Class: `SpawnerComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void SetSpawnerInfo(int spawn_count, float spawn_time, float spawn_cool_time)`
- `void ActivateSpawn()`
- `void AddComponent(std::unique_ptr<Component> component)`
- `void AddComponent(Component* component)`
- `void ForceSpawn()`

**Members:**
- `std::list<std::unique_ptr<Component>> component_list_`

## File: `StandardMeshShader.h`

### Class: `StandardMeshShader`
**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`
- `D3D12_RASTERIZER_DESC CreateRasterizerState()`

**Members:**
- (No parsed members)

## File: `StandardSkinnedMeshShader.h`

### Class: `StandardSkinnedMeshShader`
**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_BLEND_DESC CreateBlendState()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`

**Members:**
- (No parsed members)

## File: `StrongDragonAnimationState.h`

### Class: `StrongDragonAnimationState`
**Methods:**
- `void Enter(int animation_track, Object* object, AnimatorComponent* animator)`
- `int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)`
- `void Exit(int animation_track, Object* object, AnimatorComponent* animator)`
- `AnimationState* GetCopy()`
- `int GetDeadAnimationTrack()`

**Members:**
- (No parsed members)

## File: `SuperDragonAnimationState.h`

### Class: `SuperDragonAnimationState`
**Methods:**
- `void Enter(int animation_track, Object* object, AnimatorComponent* animator)`
- `int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)`
- `void Exit(int animation_track, Object* object, AnimatorComponent* animator)`
- `AnimationState* GetCopy()`
- `int GetDeadAnimationTrack()`

**Members:**
- `float attack_time_`
- `bool is_attack_`

## File: `TestAnimationState.h`

### Class: `TestAnimationState`
**Methods:**
- `void Enter(int animation_track, Object* object, AnimatorComponent* animator)`
- `int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator)`
- `void Exit(int animation_track, Object* object, AnimatorComponent* animator)`
- `AnimationState* GetCopy()`

**Members:**
- (No parsed members)

## File: `TestControllerComponent.h`

### Class: `TestControllerComponent`
**Methods:**
- `Component* GetCopy()`
- `bool ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)`
- `void Update(float elapsed_time)`

**Members:**
- (No parsed members)

## File: `TestScene.h`

### Class: `TestScene`
**Methods:**
- `void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)`
- `void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)`
- `void BuildConstantBufferViews(ID3D12Device* device)`
- `void BuildScene()`
- `bool CheckObjectByObjectCollisions()`
- `bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)`

**Members:**
- `Object* player_`

## File: `TextComponent.h`

### Class: `TextComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void Render(ID2D1DeviceContext2* d2d_device_context, ID2D1SolidColorBrush* brush)`
- `void set_text(const std::wstring& text)`
- `void set_get_text_func(const std::function<std::wstring(Object*)>& func)`
- `void set_is_static(bool value)`
- `void set_color(const D2D1_COLOR_F& color)`

**Members:**
- `std::wstring& text`
- `std::wstring text_`
- `D2D1_COLOR_F color_`
- `D2D1::Matrix3x2F transform_`
- `TextFormat* text_format_`

## File: `TextFormat.h`

### Class: `TextFormat`
**Methods:**
- `IDWriteTextFormat* dwrite_text_format()`
- `std::wstring font_name()`
- `float font_size()`
- `DWRITE_FONT_WEIGHT font_weight()`
- `DWRITE_FONT_STYLE font_style()`
- `DWRITE_FONT_STRETCH font_stretch()`
- `std::wstring locale_name()`

**Members:**
- `std::wstring& locale_name`
- `ComPtr<IDWriteTextFormat> dwrite_text_format_`
- `std::wstring font_name_`
- `float font_size_`
- `DWRITE_TEXT_ALIGNMENT text_alignment_`
- `DWRITE_PARAGRAPH_ALIGNMENT paragraph_alignment_`
- `DWRITE_FONT_WEIGHT font_weight_`
- `DWRITE_FONT_STYLE font_style_`
- `DWRITE_FONT_STRETCH font_stretch_`
- `std::wstring locale_name_`

## File: `TextRenderer.h`

### Class: `TextRenderer`
**Methods:**
- `void Render(ID2D1DeviceContext2* d2d_device_context, ID2D1SolidColorBrush* brush)`

**Members:**
- `std::list<TextComponent*> text_list_`
- `bool is_render_`

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

## File: `TransparentShader.h`

### Class: `TransparentShader`
**Inherits from:** `Shader`

**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`
- `D3D12_BLEND_DESC CreateBlendState()`

**Members:**
- (No parsed members)

## File: `UiComponent.h`

### Class: `UiComponent`
**Methods:**
- `Component* GetCopy()`
- `void Update(float elapsed_time)`
- `void set_type(UiType type)`
- `void set_view(Object* view)`

**Members:**
- (No parsed members)

## File: `UIMesh.h`

### Class: `UIMesh`
**Methods:**
- `XMFLOAT3 ScreenToNDC(float x, float y, float width, float height)`
- `XMFLOAT2 screen_position()`
- `XMFLOAT2 ui_size()`
- `void UpdateConstantBuffer(FrameResource* curr_frame_resource, int& cb_index)`
- `void UpdateConstantBufferForShadow(FrameResource* curr_frame_resource, int& cb_index)`
- `void Render(ID3D12GraphicsCommandList* command_list, int material_index, FrameResource* curr_frame_resource)`

**Members:**
- (No parsed members)

## File: `UiMeshComponent.h`

### Class: `UiMeshComponent`
**Methods:**
- `Component* GetCopy()`
- `void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index)`
- `void Render(Material* material, ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource)`
- `std::string name()`
- `void set_name(const std::string& value)`
- `void set_is_static(bool value)`
- `void set_ui_ratio(XMFLOAT2 value)`
- `void set_ui_layer(UiLayer value)`
- `void set_texture_offset(XMFLOAT2 value)`
- `void set_gage_value(XMFLOAT2 value)`
- `void set_position_offset(XMFLOAT2 value)`
- `void set_alpha(float alpha)`

**Members:**
- `UiMeshComponent& operator`
- `float alpha_`

## File: `UIShader.h`

### Class: `UIShader`
**Methods:**
- `D3D12_INPUT_LAYOUT_DESC CreateInputLayout()`
- `D3D12_BLEND_DESC CreateBlendState()`
- `D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState()`
- `D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob)`
- `D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob)`

**Members:**
- (No parsed members)

## File: `UploadBuffer.h`

### Class: `UploadBuffer`
**Methods:**
- (No parsed methods)

**Members:**
- `UploadBuffer& operator`
- `ComPtr<ID3D12Resource> d3d_buffer_`
- `BYTE* mapped_data_`
- `UINT element_byte_size_`
- `bool is_constant_buffer_`

## File: `WallColliderComponent.h`

### Class: `WallColliderComponent`
**Methods:**
- `WallColliderComponent(*this)`
- `bool CollisionCheckByRay(FXMVECTOR ray_origin, FXMVECTOR ray_direction, float& out_distance)`

**Members:**
- `BoxColliderComponent* box_collider_`

