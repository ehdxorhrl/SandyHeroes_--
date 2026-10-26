# PPT 콘텐츠 가이드 — 페이지별 작성 내용

> 아래 내용을 PPT 각 페이지에 맞게 편집하여 사용하세요.
> `[PIX 데이터]`로 표시된 부분은 PIX 측정 후 채워야 합니다.

---

## 페이지 3: 목차 업데이트

현재 "프로젝트 개요 및 구현 사항"만 있습니다. 아래로 교체하세요:

```
목차

• 프로젝트 개요

• CPU, GPU 동기화 문제 개선
  - 공유 자원 파악 및 FrameResource 설계
  - 원형 버퍼를 사용한 CPU, GPU 병렬 처리

• GPU 상태 변경 최소화를 위한 렌더링 최적화
  - PSO, Material, Mesh 순의 배치처리 구조
  - FPS 저하 현상 분석
  - 하드웨어 인스턴싱 설계 근거
  - 하드웨어 인스턴싱을 통한 드로우 콜 개선
  - 절두체 컬링 재적용 (Visible Indices)

• 컴포넌트 패턴 기반 게임오브젝트 프레임워크
  - 상속 기반 설계의 문제와 합성 패턴 적용
  - 순환 참조 방지를 위한 메모리 관리 설계
```

---

## 페이지 6: 수정 제안

현재 텍스트:
> 공유자원이 2개 이상인 경우(첫 프레임 제외)

수정안:
> 공유자원이 3개인 경우(kFrameCount = 3, 첫 프레임 제외)

아래 부연 문장을 추가하면 면접 대비에 좋습니다:
> "첫 프레임을 제외하는 이유는 CirculateFrameResource 함수에서 fence 값이 0(초기값)인 경우 GPU 대기를 건너뛰기 때문이다."

---

## 페이지 9 (신규): 하드웨어 인스턴싱 설계 근거

### 제목
> Sandy Heroes — 하드웨어 인스턴싱을 통한 드로우 콜 개선

### 본문

```
하드웨어 인스턴싱을 적용하기 위해서는 인스턴싱을 위한 데이터를 가진 버퍼와
Mesh가 인스턴싱 되는 횟수가 필요하다.
이를 기존 Mesh 렌더링 구조에 적용하기 위해서는 3가지 개선이 필요하다고 생각했다.

1. FrameResource의 업로드 버퍼를 ConstantBuffer에서 StructuredBuffer로 변경
   → Input Layout 방식 대신 StructuredBuffer를 선택한 이유는
     이미 FrameResource를 통해 공유자원을 관리하고 있기 때문에
     StructuredBuffer를 위한 C++ 코드의 작성이 간결하다고 생각했다.

2. 동일 메쉬를 사용하는 오브젝트끼리 모아서 업로드 버퍼 업데이트
   → 인스턴싱 렌더는 셰이더에서 [0, 인스턴스 횟수) 범위의 인덱스로
     StructuredBuffer에 접근하기 때문이다.

3. Mesh::Render 단계에서 사용할 인스턴스 횟수 파악
   → 기존 렌더 함수는 인스턴싱을 고려하지 않았기 때문에
     DrawIndexedInstanced의 인스턴싱 횟수가 1로 하드코딩 되어 있었기 때문이다.
```

### 코드 요약 (3가지 개선 핵심)

```cpp
// 개선 1: 업로드 버퍼 교체 (ConstantBuffer → StructuredBuffer)
std::unique_ptr<UploadBuffer<CBObject>>      cb_object;        // Before
std::unique_ptr<UploadBuffer<InstanceData>>  sb_instance_data; // After
int current_instance_offset = 0;  // 추가: Material 간 공유 오프셋

// 개선 2: Material 구조 변경 (평면 리스트 → Mesh별 그룹)
std::list<MeshComponent*>                   mesh_component_list_; // Before
std::map<Mesh*, std::vector<MeshComponent*>> batches_;            // After

// 개선 3: DrawIndexedInstanced 인스턴스 횟수
DrawIndexedInstanced(index_count, 1, 0, 0, 0);               // Before: 항상 1
DrawIndexedInstanced(index_count, instance_count, 0, 0, 0);  // After: 그룹 크기
```

---

## 페이지 10: 하드웨어 인스턴싱 코드 소개

### 제목
> Sandy Heroes — 하드웨어 인스턴싱을 통한 드로우 콜 개선

### 본문

```
기존 렌더링 구조에 하드웨어 인스턴싱을 적용하기 위해 크게
2가지가 필요하다고 생각하였다.

1. 인스턴싱에 필요한 데이터를 가진 StructuredBuffer
2. 인스턴스 횟수를 전달하는 렌더링 함수
```

---

### 1. StructuredBuffer — 설계 의도 및 구현

```
기존에는 Scene에서 모든 MeshComponent의 월드 행렬을 업로드 버퍼에
업데이트하고 각 MeshComponent에 오프셋을 부여하는 것으로 충분하였다.

하지만, 이제는 Material에서 같은 메쉬를 사용하는 MeshComponent끼리
모아서 차례대로 업데이트를 해야 하기 때문에, Material 간의 공용으로
사용할 오프셋이 필요하다.
```

---

**스니펫 1: FrameResource 구조 변경**

```cpp
// ═══════ Before ═══════
struct FrameResource
{
    // 오브젝트마다 1슬롯 (256바이트 정렬 ConstantBuffer)
    std::unique_ptr<UploadBuffer<CBObject>> cb_object;
    // current_instance_offset 없음 — Scene이 로컬 변수로 관리
};

// ═══════ After ═══════
struct InstanceData              // CBObject와 동일한 레이아웃
{
    XMFLOAT4X4 world_matrix;
    float time;
    float padding[3];
};

struct FrameResource
{
    // 전체 인스턴스를 연속 배열로 관리 (StructuredBuffer)
    std::unique_ptr<UploadBuffer<InstanceData>> sb_instance_data;
    int current_instance_offset = 0;  // Material 간 공유 오프셋
};
```

> 📌 **포인트**: `current_instance_offset`을 Material 로컬로 두면 각 Material이 0부터 덮어써서 데이터가 유실된다. FrameResource에 두어야 여러 Material의 업데이트가 하나의 연속 배열을 올바르게 채울 수 있다.

---

**스니펫 2: 업데이트 흐름 변경 — 인덱스 관리 주체: Scene → FrameResource**

```cpp
// ═══════ Before: Scene이 cb_object_index를 로컬 변수로 관리 ═══════
void Scene::UpdateObjectConstantBuffer(FrameResource* curr)
{
    int cb_object_index = 0;
    for (const auto& mesh : meshes_)   // 씬 순서대로 Mesh 전체 순회
        mesh->UpdateConstantBuffer(curr, cb_object_index);
        // Mesh 내부에서 cb_object_index를 int&로 증가
}

void MeshComponent::UpdateConstantBuffer(FrameResource* curr, int cb_index)
{
    constant_buffer_index_ = cb_index;
    curr->cb_object->CopyData(cb_index, object_buffer);  // cb_object에 업로드
}

// ═══════ After: Material이 batches_로 그룹화하여 연속 업데이트 ═══════
// 같은 Mesh를 사용하는 컴포넌트들이 sb_instance_data에 연속으로 기록됨
for (const auto& [mesh, components] : batches_)
    for (const auto& component : components)
        component->UpdateConstantBuffer(curr_frame_resource);

void MeshComponent::UpdateConstantBuffer(FrameResource* curr)
{
    constant_buffer_index_ = curr->current_instance_offset;          // 시작 위치 기록
    curr->sb_instance_data->CopyData(curr->current_instance_offset++, data);
}

// GameFramework: 매 프레임 시작 시 오프셋 리셋
curr_frame_resource()->current_instance_offset = 0;
```

---

**스니펫 3: Material 구조 변경 — 평면 리스트 → Mesh별 그룹**

```cpp
// ═══════ Before: 평면 리스트, 컴포넌트마다 Render 개별 호출 ═══════
// Material.h
std::list<MeshComponent*> mesh_component_list_;

// Material::Render
for (const auto& mesh_component : mesh_component_list_)
    mesh_component->Render(this, command_list, curr_frame_resource);
    // → 매 호출마다: SetCBV(cb_object[i]) + DrawIndexedInstanced(..., 1, ...)

// ═══════ After: Mesh별 그룹, 같은 Mesh 컴포넌트끼리 연속 업데이트 ═══════
// Material.h
std::map<Mesh*, std::vector<MeshComponent*>> batches_;

// Material::UpdateConstantBuffer (업데이트 단계)
for (const auto& [mesh, components] : batches_)
    for (const auto& component : components)
        component->UpdateConstantBuffer(curr_frame_resource);
// → 같은 Mesh 컴포넌트들이 sb_instance_data에 연속 슬롯 점유
//   [Mesh_A: Comp0, Comp1, Comp2] → sb_data[0], sb_data[1], sb_data[2]
```

> 📌 **포인트**: 인스턴싱 셰이더는 `SV_InstanceID`로 `[0, instance_count)` 범위를 순서대로 접근한다. 같은 Mesh의 데이터가 `sb_instance_data`에 연속되지 않으면 엉뚱한 월드 행렬이 적용된다. `batches_`가 이 연속성을 보장한다.

### 다이어그램

```
═══ Before: Scene이 인덱스 관리, 씬 순서대로 cb_object 점유 ═══

Scene::UpdateObjectConstantBuffer()  cb_object_index = 0
  Mesh_A → [Comp_A0→cb_object[0], Comp_A1→cb_object[1]]
  Mesh_B → [Comp_B0→cb_object[2]]

  Render: for mesh_component in mesh_component_list_:
            Comp_A0→SetCBV(cb_object[0])→Draw(1)  ┐
            Comp_A1→SetCBV(cb_object[1])→Draw(1)  ┤ 3 Draw Calls
            Comp_B0→SetCBV(cb_object[2])→Draw(1)  ┘

═══ After: Material이 batches_로 관리, current_instance_offset 공유 ═══

GameFramework: current_instance_offset = 0  ← 매 프레임 리셋

Material_1::batches_ {Mesh_A: [Comp_A0, Comp_A1]}:
  UpdateConstantBuffer: Comp_A0→sb_data[0], Comp_A1→sb_data[1]  offset: 0→2
Material_2::batches_ {Mesh_B: [Comp_B0]}:
  UpdateConstantBuffer: Comp_B0→sb_data[2]                       offset: 2→3

  (Render는 페이지 11에서 설명)
```

---

## 페이지 11: 하드웨어 인스턴싱 코드 소개 (개선 3)

### 제목
> Sandy Heroes — 하드웨어 인스턴싱을 통한 드로우 콜 개선

### 개선 3: Mesh::Render 단계에서 인스턴스 횟수 파악 및 전달

```
기존 Mesh::Render 함수는 인스턴싱을 고려하지 않아
DrawIndexedInstanced의 인스턴스 횟수가 1로 하드코딩되어 있었다.

Material이 batches_로 같은 Mesh의 컴포넌트를 그룹화했으므로,
그룹 크기(components.size())를 인스턴스 횟수로,
첫 번째 컴포넌트의 constant_buffer_index_를 StructuredBuffer 오프셋으로
Mesh::RenderInstancing에 전달한다.
```

---

**스니펫 1: Material::Render — 렌더링 호출 방식**

```cpp
// ═══════ Before: 평면 리스트, 오브젝트마다 개별 렌더 ═══════
void Material::Render(ID3D12GraphicsCommandList* cmd, FrameResource* curr, ...)
{
    for (const auto& mesh_component : mesh_component_list_)
        mesh_component->Render(this, cmd, curr);
        // → MeshComponent::Render가 개별 cbuffer 바인딩 후 Draw(1)
}

// ═══════ After: batches_ 순회, Mesh 단위로 RenderInstancing 1회 ═══════
void Material::Render(ID3D12GraphicsCommandList* cmd, FrameResource* curr, ...)
{
    for (const auto& [mesh, components] : batches_)
    {
        auto instance_count         = components.size();         // 그룹 크기
        auto sub_mesh_index         = components.front()->GetMaterialIndex(this);
        auto instance_buffer_offset = components.front()->constant_buffer_index(); // sb_data 시작 위치

        mesh->RenderInstancing(cmd, sub_mesh_index, curr,
            instance_count, instance_buffer_offset);
    }
}
```

---

**스니펫 2: Mesh::Render → Mesh::RenderInstancing**

```cpp
// ═══════ Before: MeshComponent::Render + Mesh::Render — 항상 Draw(1) ═══════
void MeshComponent::Render(Material* mat, ID3D12GraphicsCommandList* cmd, FrameResource* curr)
{
    auto addr = curr->cb_object->Resource()->GetGPUVirtualAddress();
    addr += d3d_util::CalculateConstantBufferSize(sizeof(CBObject)) * constant_buffer_index_;
    cmd->SetGraphicsRootConstantBufferView(kWorldMatrix, addr); // 개별 cbuffer 바인딩
    mesh_->Render(cmd, material_index);
}

void Mesh::Render(ID3D12GraphicsCommandList* cmd, int material_index)
{
    cmd->IASetVertexBuffers(0, ...);
    cmd->IASetIndexBuffer(&index_buffer_views_[material_index]);
    cmd->DrawIndexedInstanced(index_count, 1, 0, 0, 0);  // 항상 1
}

// ═══════ After: Mesh::RenderInstancing — instance_count개 한번에 ═══════
void Mesh::RenderInstancing(ID3D12GraphicsCommandList* cmd, int material_index,
    FrameResource* curr, int instance_count, int instance_buffer_offset)
{
    cmd->IASetVertexBuffers(0, ...);
    D3D12_GPU_VIRTUAL_ADDRESS base =
        curr->sb_instance_data->Resource()->GetGPUVirtualAddress();
    cmd->SetGraphicsRootShaderResourceView(kInstanceData,
        base + instance_buffer_offset * sizeof(InstanceData)); // 배치 시작 바인딩
    cmd->IASetIndexBuffer(&index_buffer_views_[material_index]);
    cmd->DrawIndexedInstanced(index_count, instance_count, 0, 0, 0); // N개 한번에
}
```

> 📌 **핵심**: `instance_buffer_offset = components.front()->constant_buffer_index()`는 이 Mesh 그룹이 `sb_instance_data`에서 차지하는 시작 인덱스다. 셰이더는 `SV_InstanceID` (0 ~ instance_count-1)로 `sb_instance_data[offset + id]`에 접근한다.

### 다이어그램

```
Material::batches_ {Mesh_A: [Comp_A0, Comp_A1, Comp_A2]}

  UpdateConstantBuffer (페이지 10):
    Comp_A0 → sb_data[0]  (constant_buffer_index_ = 0)
    Comp_A1 → sb_data[1]
    Comp_A2 → sb_data[2]

  Render (페이지 11):
    instance_count         = 3        (components.size())
    instance_buffer_offset = 0        (front()->constant_buffer_index())
    ↓
    SetSRV(sb_data + 0 * sizeof(InstanceData))
    DrawIndexedInstanced(index_count, 3, 0, 0, 0)
    ↓
    셰이더: SV_InstanceID 0→sb_data[0], 1→sb_data[1], 2→sb_data[2]
```

### 첨부 자료 (PIX 성능 결과)
- PIX Event List 캡처 스크린샷 (드로우 콜 횟수: 약 1800회 → [PIX 데이터]회)
- 인게임 fps 스크린샷 (49fps → [PIX 데이터]fps)

---

## 페이지 12: 절두체 컬링 재적용

### 제목
> Sandy Heroes - 절두체 컬링 재적용 (Visible Indices)

### 본문

```
하드웨어 인스턴싱 적용 과정에서 모든 인스턴스 데이터를 
하나의 StructuredBuffer에 넣고 SV_InstanceID로 순차 접근하는 
구조였기 때문에, 카메라 절두체 밖의 오브젝트도 그대로 
렌더링되는 문제가 발생했다.

이를 해결하기 위해 "Visible Indices" 패턴을 설계했다.

1. 모든 인스턴스 데이터는 sb_instance_data에 기록
2. 절두체 컬링/그림자 영역 판정 결과에 따라 "보이는 
   인스턴스의 인덱스"만 별도 버퍼에 기록
   - 메인 카메라용: sb_main_visible_indices
   - 그림자 맵용: sb_shadow_visible_indices
3. 셰이더에서 SV_InstanceID로 visible indices를 인덱싱하여 
   실제 인스턴스 데이터에 간접 접근
4. DrawIndexedInstanced의 인스턴스 개수를 visible count로 제한
```

### 코드 스니펫 (PPT에 넣을 코드 이미지)

**스니펫 1: FrameResource에 추가된 Visible Indices 버퍼**

```cpp
// FrameResource.h
struct FrameResource
{
    // 인스턴싱된 전체 오브젝트 데이터
    std::unique_ptr<UploadBuffer<InstanceData>> sb_instance_data;
    // 메인 카메라 절두체 내 인스턴스 인덱스
    std::unique_ptr<UploadBuffer<UINT>> sb_main_visible_indices;
    // 그림자 맵 영역 내 인스턴스 인덱스
    std::unique_ptr<UploadBuffer<UINT>> sb_shadow_visible_indices;

    int current_instance_offset = 0;
    int current_main_visible_offset = 0;
    int current_shadow_visible_offset = 0;
    // ...
};
```

**스니펫 2: Visible Index 기록 (MeshComponent::UpdateConstantBuffer)**

```cpp
// MeshComponent.cpp
void MeshComponent::UpdateConstantBuffer(FrameResource* curr)
{
    constant_buffer_index_ = curr->current_instance_offset;
    // ... InstanceData 기록 ...

    // 절두체 내에 있으면 main visible indices에 기록
    if(is_in_view_frustum_)
    {
        curr->sb_main_visible_indices->CopyData(
            curr->current_main_visible_offset++,
            constant_buffer_index_);
    }
    // 그림자 맵 영역 내에 있으면 shadow visible indices에 기록
    if(is_in_shadow_map_obb_)
    {
        curr->sb_shadow_visible_indices->CopyData(
            curr->current_shadow_visible_offset++,
            constant_buffer_index_);
    }
    // 전체 인스턴스 데이터 기록
    curr->sb_instance_data->CopyData(
        curr->current_instance_offset++, data);
}
```

**스니펫 3: HLSL에서 Visible Indices로 간접 인덱싱**

```hlsl
// GlobalValues.hlsl
StructuredBuffer<InstanceData> g_instance_data        : register(t0, space1);
StructuredBuffer<uint> g_main_visible_indices          : register(t1, space1);
StructuredBuffer<uint> g_shadow_visible_indices        : register(t2, space1);

// StandardShader.hlsl - 메인 렌더링
VertexOut MeshVS(MeshVertexIn v_in, uint instance_id : SV_InstanceID)
{
    // visible indices를 통해 실제 인스턴스 데이터에 간접 접근
    matrix world = g_instance_data[g_main_visible_indices[instance_id]].world_matrix;
    // ...
}

// Shadow.hlsl - 그림자 렌더링
VertexOut MeshVS(MeshVertexIn v_in, uint instance_id : SV_InstanceID)
{
    matrix world = g_instance_data[g_shadow_visible_indices[instance_id]].world_matrix;
    // ...
}
```

### 다이어그램 (PPT에 넣을 구조도)

```
sb_instance_data (전체 인스턴스 데이터)
┌────┬────┬────┬────┬────┬────┬────┬────┐
│ 0  │ 1  │ 2  │ 3  │ 4  │ 5  │ 6  │ 7  │
└────┴────┴────┴────┴────┴────┴────┴────┘
  ▲         ▲              ▲
  │         │              │
sb_main_visible_indices    sb_shadow_visible_indices
┌────┬────┬────┐           ┌────┬────┬────┬────┐
│ 0  │ 2  │ 5  │           │ 0  │ 2  │ 3  │ 5  │
└────┴────┴────┘           └────┴────┴────┴────┘
 (카메라 내 3개)            (그림자 영역 내 4개)

DrawIndexedInstanced(..., 3, ...)  // 메인: 3개만 렌더
DrawIndexedInstanced(..., 4, ...)  // 그림자: 4개만 렌더
```

---

## 페이지 13: 절두체 컬링 PIX 성능 비교

### 제목
> Sandy Heroes - 절두체 컬링 재적용 (PIX 활용 성능 비교)

### 본문 (템플릿 — 현재 페이지 6 복사 텍스트를 아래로 교체)

```
Visible Indices 패턴을 적용한 후 PIX를 통해 성능을 
비교하였다.

절두체 컬링 미적용 시:
• 드로우 콜당 최대 인스턴스 개수: 411개
• 드로우 콜 횟수: 144회

절두체 컬링 적용 후:
• 드로우 콜당 최대 인스턴스 개수: [PIX 데이터]개
• 드로우 콜 횟수: [PIX 데이터]회

카메라 밖 오브젝트를 렌더링 대상에서 제외함으로써 
GPU에 제출되는 인스턴스 수가 크게 감소하였다.
또한 그림자 맵 렌더링에서도 라이트 공간 AABB 기반의 
별도 컬링을 적용하여 불필요한 연산을 추가로 절감했다.

인게임 FPS: [PIX 데이터]fps → [PIX 데이터]fps
```

### 첨부 자료
- PIX Event List 캡처 스크린샷 (적용 전/후 대조)
- 인게임 fps 비교 스크린샷

---

## 페이지 14: 컴포넌트 패턴 — 문제와 해결

### 제목
> Sandy Heroes - 컴포넌트 패턴 기반 게임오브젝트 프레임워크

### 본문

```
게임을 구성하는 오브젝트들은 다양한 기능 중 일부만 필요한 
경우가 대부분이다. 상속 기반 설계에서는 불필요한 기능까지 
상속받거나, 동일한 기능 코드가 여러 클래스에 중복되는 문제가 
발생한다.

이를 해결하기 위해 오브젝트와 기능을 분리하는 컴포넌트 
패턴을 적용하였다.

• Object: 게임 월드의 기본 엔티티. Transform과 Component 
  목록을 가진다.
• Component: 재사용 가능한 기능 모듈. Object에 부착하여 
  기능을 확장한다.

기능 추가 시 새로운 Component 클래스를 만들어 부착하면 
되므로, Object 클래스의 수정 없이 다양한 게임 엔티티를 
구성할 수 있다.
```

### 코드 스니펫

**스니펫 1: Object 클래스 핵심 구조**

```cpp
// Object.h
class Object : public std::enable_shared_from_this<Object>
{
public:
    void AddChild(std::shared_ptr<Object> object);
    void AddComponent(std::shared_ptr<Component> component);

    // 타입 안전한 컴포넌트 접근 (템플릿)
    template<class T>
    static std::shared_ptr<T> GetComponent(std::shared_ptr<Object> object)
    {
        for (auto& component : object->component_list_)
        {
            if (auto res = std::dynamic_pointer_cast<T>(component))
                return res;
        }
        return nullptr;
    }

    void Update(float elapsed_time);

protected:
    std::weak_ptr<Object> parent_;            // 순환 참조 방지
    std::shared_ptr<Object> child_;           // 소유
    std::shared_ptr<Object> sibling_;         // 소유
    std::list<std::shared_ptr<Component>> component_list_; // 소유
};
```

**스니펫 2: Component 기반 클래스**

```cpp
// Component.h
class Component : public std::enable_shared_from_this<Component>
{
public:
    Component(const std::shared_ptr<Object>& owner);

    std::shared_ptr<Object> owner() const { return owner_.lock(); }
    virtual Component* GetCopy() = 0;  // 다형적 복사
    virtual void Update(float elapsed_time) {}

protected:
    std::weak_ptr<Object> owner_;      // 순환 참조 방지
};
```

**스니펫 3: 활용 예시 — 오브젝트 구성**

```cpp
// BaseScene.cpp (예시)
auto player = std::make_shared<Object>("Player");
player->AddComponent(std::make_shared<MeshComponent>(player, mesh));
player->AddComponent(std::make_shared<AnimatorComponent>(player));
player->AddComponent(std::make_shared<MovementComponent>(player));
player->AddComponent(std::make_shared<GunComponent>(player, scene));

// 타입 안전한 컴포넌트 접근
auto gun = Object::GetComponent<GunComponent>(player);
```

### 다이어그램 (PPT에 넣을 구조도)

```
┌──────────────────────────────────┐
│ Object (enable_shared_from_this) │
│                                  │
│  component_list_ (shared_ptr)    │
│  ┌─────────────┐                 │
│  │MeshComponent│───→ Mesh*, Material* (비소유 참조)
│  ├─────────────┤                 │
│  │Animator     │───→ AnimationSet, AnimationState
│  │Component    │                 │
│  ├─────────────┤                 │
│  │Movement     │                 │
│  │Component    │                 │
│  ├─────────────┤                 │
│  │GunComponent │                 │
│  └─────────────┘                 │
│                                  │
│  child_ ──(shared_ptr)──→ 자식 Object
│  sibling_ ──(shared_ptr)──→ 형제 Object
│  parent_ ──(weak_ptr)──→ 부모 Object  ← 순환 참조 방지
└──────────────────────────────────┘

모든 Component의 owner_ 는 weak_ptr<Object> ← 순환 참조 방지
```

---

## 페이지 15: 컴포넌트 패턴 — 메모리 관리 설계

### 제목
> Sandy Heroes - 순환 참조 방지를 위한 메모리 관리 설계

### 본문

```
스마트 포인터 기반의 컴포넌트 패턴에서 가장 중요한 것은 
순환 참조 방지이다. Object와 Component가 서로를 shared_ptr로 
참조하면 양쪽의 참조 카운트가 0이 되지 않아 메모리 누수가 
발생한다.

이를 방지하기 위해 다음과 같은 소유권 규칙을 설계했다.

소유 관계 (shared_ptr):
  • Object → Component (component_list_)
  • Object → 자식 Object (child_)
  • Object → 형제 Object (sibling_)
  • Scene → 최상위 Object (object_list_)

비소유 역참조 (weak_ptr):
  • Component → Object (owner_)
  • Object → 부모 Object (parent_)

삭제 흐름:
  Scene::DeleteObject() 호출 시 object_list_에서 
  shared_ptr을 제거하면, 참조 카운트가 0이 되어 Object가 
  소멸되고, 이에 따라 component_list_의 Component들도 
  자동으로 소멸된다. 
  다른 곳에서 weak_ptr로 참조하고 있던 부분은 
  lock() 실패로 자연스럽게 무효화된다.
```

### 코드 스니펫

**스니펫 1: 순환 참조 방지 — owner_ 접근**

```cpp
// Component.cpp
Component::Component(Object* owner)
{
    // raw pointer를 받아서 weak_ptr로 저장
    if (owner)
        owner_ = owner->shared_from_this();
}

std::shared_ptr<Object> Component::owner() const
{
    return owner_.lock();  // 안전한 접근
}
```

**스니펫 2: 지연 삭제 패턴 (Scene)**

```cpp
// Scene.cpp
void Scene::Update(float elapsed_time)
{
    is_updating_objects_ = true;
    for (const auto& object : object_list_)
        object->Update(elapsed_time);
    is_updating_objects_ = false;

    // 업데이트 중 추가/삭제 요청은 지연 처리
    for (const auto& object : add_object_list_)
        AddObject(object);
    add_object_list_.clear();

    for (const auto& object : delete_object_list_)
        object_list_.remove(object);
    delete_object_list_.clear();
}
```

**스니펫 3: 계층 구조에서의 소유권**

```cpp
// Object.cpp
void Object::AddChild(std::shared_ptr<Object> object)
{
    object->parent_ = shared_from_this();  // weak_ptr로 저장
    if (child_)
        child_->AddSibling(object);
    else
        child_ = object;  // shared_ptr로 소유
}
```

### 소유권 다이어그램

```
Scene (object_list_: shared_ptr)
 │
 ▼ shared_ptr
┌─────────┐ shared_ptr  ┌──────────┐ shared_ptr ┌──────────┐
│ Object A │───child───→│ Object B │───sibling──→│ Object C │
└─────────┘             └──────────┘             └──────────┘
 │                        │ weak_ptr                │
 │ shared_ptr             └──parent──→ Object A     │
 ▼                                                  │
┌──────────────┐                                    │
│MeshComponent │──owner_: weak_ptr──→ Object A      │
│GunComponent  │──owner_: weak_ptr──→ Object A      │
└──────────────┘                                    │
                                        weak_ptr    │
                              Object C ──parent──→ Object A

─── shared_ptr (소유): 참조 카운트 증가, 생명주기 관리
··· weak_ptr (관찰): 참조 카운트 미증가, lock() 실패 시 안전 처리
```

---

## 페이지 16 (선택): 컴포넌트 패턴 — 활용 예시

> 이 페이지는 선택사항입니다. 분량이 넉넉하면 추가하세요.

### 제목
> Sandy Heroes - 컴포넌트 패턴 활용 예시

### 본문

```
컴포넌트 패턴을 적용한 결과, 동일한 Object 클래스에 
다양한 Component 조합을 부착하여 서로 다른 게임 엔티티를 
구성할 수 있게 되었다.

Player Object:
  MeshComponent + AnimatorComponent + MovementComponent 
  + GunComponent + FPSControllerComponent

Monster Object:
  MeshComponent + AnimatorComponent + MovementComponent 
  + MonsterComponent + BoxColliderComponent

Chest Object (상자):
  MeshComponent + AnimatorComponent + ChestComponent

UI Object:
  UiMeshComponent + TextComponent + ProgressBarComponent

각 Component는 독립적으로 구현되어 있어 새로운 기능 추가 시 
기존 코드의 수정 없이 Component 클래스만 추가하면 된다.
```

### 구현된 Component 목록 (카탈로그)

```
렌더링 관련:
  MeshComponent, SkinnedMeshComponent, UiMeshComponent, 
  ParticleComponent

로직 관련:
  MovementComponent, GunComponent, MonsterComponent, 
  PlayerComponent, ChestComponent, ScrollComponent

입력 관련:
  InputControllerComponent, FPSControllerComponent, 
  TestControllerComponent

충돌 관련:
  ColliderComponent, BoxColliderComponent, 
  MeshColliderComponent, GroundColliderComponent, 
  WallColliderComponent

기타:
  AnimatorComponent, CameraComponent, SoundComponent, 
  RazerComponent, TextComponent, ProgressBarComponent, 
  FadeInUIComponent, CharacterComponent, SpawnerComponent
```

---

> [!IMPORTANT]
> ## PIX 측정 체크리스트
> 
> PPT를 완성하려면 아래 데이터를 PIX로 측정해야 합니다:
> 
> **페이지 10 (인스턴싱 결과):**
> - [ ] 인스턴싱 적용 후 드로우 콜 횟수
> - [ ] 인스턴싱 적용 후 인게임 fps
> 
> **페이지 12 (절두체 컬링 결과):**
> - [ ] 절두체 컬링 적용 후 드로우 콜당 최대 인스턴스 개수
> - [ ] 절두체 컬링 적용 후 드로우 콜 횟수
> - [ ] 절두체 컬링 적용 후 인게임 fps
