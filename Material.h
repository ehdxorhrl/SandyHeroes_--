#pragma once

class DescriptorManager;
struct FrameResource;

constexpr int kTextureMaskAlbedo(1 << 0);
constexpr int kTextureMaskSpecGlos(1 << 1);
constexpr int kTextureMaskMetalGlos(1 << 2);
constexpr int kTextureMaskEmission(1 << 3);
constexpr int kTextureMaskNormal(1 << 4);
constexpr int kTextureMaskCube(1 << 5);

enum class TextureType{ kAlbedoMap, kSpecGlossMap, kMetallicGlossMap, kEmissionMap, kNormalMap, kCubeMap };

struct Texture
{
    std::string name;
    int heap_index = -1;
    TextureType type;

    ComPtr<ID3D12Resource> resource = nullptr;
    ComPtr<ID3D12Resource> upload_heap = nullptr;
};

class Material
{
public:
    Material() {}
    ~Material() {}

    //getter
    std::string name() const;

    //setter
    void set_frame_resource_index(int value);
    void set_albedo_color(float r, float g, float b, float a);
    void set_albedo_color(XMFLOAT4 value);
    void set_name(const std::string& value);

    // 머터리얼을 사용하는 데 필요한 텍스처 로드
    void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list);
    
    // 머터리얼 관련 상수버퍼 업데이트 및 쉐이더에 설정한다
    void UpdateShaderVariables(ID3D12GraphicsCommandList* command_list, 
        FrameResource* curr_frame_resource, DescriptorManager* descriptor_manager);

    // SRV를 생성하고 다음 시작 인덱스를 리턴(리턴값을 받아서 다음 머터리얼의 srv 생성에 사용하면 됨)
    int CreateShaderResourceViews(ID3D12Device* device, DescriptorManager* descriptor_manager, int start_index);

    void LoadMaterialFromFile(std::ifstream& file);

    void AddTexture(Texture* texture);

    static int GetTextureCount();

private:
    static const std::string kTextureFilePath;
    static int kTextureCount;

    // 재질 정보가 저장된 frame resource의 버퍼 인덱스
    int frame_resource_index_ = -1;

    std::list<std::unique_ptr<Texture>> texture_list_;

    //상수 버퍼로 입력되는 재질 값
    XMFLOAT4 albedo_color_{ 1,1,1,1 };
    XMFLOAT3 fresnel_r0_{ 0.04, 0.04, 0.04 };
    float glossiness_{ 0.2 };
    XMFLOAT4 emission_color_{ 0,0,0,1 };
    
    std::string name_{ "None" };

};

