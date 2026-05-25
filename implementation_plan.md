# 엔진 프로그래머 포트폴리오 완성 계획 (v2)

> Q1~Q5 답변 반영 + 최신 코드 분석 완료

---

## 현재 포트폴리오 구조 (12페이지 + 추가 예정)

| 페이지 | 제목 | 상태 |
|--------|------|------|
| 1 | 표지 (시프트업 엔진 프로그래머 포트폴리오) | ✅ 완성 |
| 2 | 이력서 | ✅ 완성 |
| 3 | 목차 | ⚠️ 항목 추가 필요 |
| 4 | 프로젝트 개요 | ⚠️ 인게임 스크린샷 추가 필요 |
| 5 | CPU, GPU 동기화 문제 개선 (1/2) | ✅ 완성 |
| 6 | CPU, GPU 동기화 문제 개선 (2/2) | ✅ 완성 (수정 제안 있음) |
| 7 | 배치처리 구조 설명 | ✅ 완성 (Scene→Shader→Material 코드 스니펫 확인됨) |
| 8 | FPS 저하 현상 분석 | ⚠️ PIX/fps 스크린샷 확인 필요 |
| 9 | 하드웨어 인스턴싱 (1/2) 코드 | ✅ 코드 소개 완성 |
| 10 | 하드웨어 인스턴싱 (2/2) 결과 | 🔴 **PIX 데이터/fps 스크린샷 미완성** |
| 11 | 절두체 컬링 재적용 (1/2) | 🔴 **설명 미완성** |
| 12 | 절두체 컬링 재적용 (2/2) PIX | 🔴 **데이터/텍스트 미완성** |
| 13~15 | 컴포넌트 패턴 (신규) | 🔴 **미작성** |

---

## 1. CPU, GPU 병렬성을 고려한 리소스 설계 (페이지 5-6) ✅

> [!TIP]
> **수정 제안 (페이지 6)**: "공유자원이 2개 이상인 경우"를 "공유자원이 3개인 경우 (kFrameCount = 3)"로 구체화하면 좋습니다. 또한 "첫 프레임 제외" 조건은 `CirculateFrameResource()`에서 `fence != 0`일 때만 대기하는 로직 때문인데, 이를 한 줄 부연하면 면접 질문에도 대비됩니다.

---

## 2. GPU 상태 변경 최소화 + 인스턴싱 + 절두체 컬링 (페이지 7-12)

### 페이지 7: 배치처리 구조 ✅
- Scene→Shader→Material 코드 스니펫 3장 확인됨 (Q4 답변)
- 별도 수정 불필요

### 페이지 8: FPS 저하 현상 분석 ⚠️
- [ ] PIX 드로우 콜 캡처 스크린샷이 첨부되어 있는지 확인 (PDF 이미지 2장 확인됨)
- [ ] 인게임 49fps 스크린샷이 첨부되어 있는지 확인

### 페이지 10: 인스턴싱 성능 결과 🔴
- [ ] 해당 커밋으로 체크아웃 후 PIX 캡처
  - 드로우 콜 횟수 (1800회 → ?회)
  - 인게임 fps (49fps → ?fps)
- [ ] "스샷 설명" 플레이스홀더 텍스트를 실제 설명으로 교체

### 페이지 11: 절두체 컬링 재적용 설명 🔴

> [!IMPORTANT]
> 최신 코드 분석 결과, 절두체 컬링은 **Visible Indices** 패턴으로 구현되어 있습니다. 아래에 PPT 서술 가이드를 제안합니다.

#### PPT 서술 가이드 (제안)

**문제 상황:**
하드웨어 인스턴싱 적용 시, 모든 인스턴스 데이터를 하나의 `StructuredBuffer(sb_instance_data)`에 넣어 `DrawIndexedInstanced()`로 렌더링하는데, `SV_InstanceID`가 0부터 순차적으로 인덱싱되기 때문에 **카메라 절두체 밖의 오브젝트도 그대로 렌더링**되는 문제가 있었다. 또한 그림자 매핑을 위해 별도의 컬링 기준이 필요했다.

**해결 방법 (Visible Indices 패턴):**
1. 모든 인스턴스 데이터는 `sb_instance_data`에 기록 (인덱스 = `constant_buffer_index_`)
2. 절두체 컬링 결과에 따라 **보이는 인스턴스의 인덱스만** 별도 버퍼에 기록:
   - 메인 카메라용: `sb_main_visible_indices`
   - 그림자 맵용: `sb_shadow_visible_indices`
3. HLSL 셰이더에서 `SV_InstanceID`로 visible indices 버퍼를 인덱싱하여 실제 인스턴스 데이터에 접근:
   ```hlsl
   // 메인 렌더: g_instance_data[g_main_visible_indices[instance_id]]
   // 그림자 렌더: g_instance_data[g_shadow_visible_indices[instance_id]]
   ```
4. `DrawIndexedInstanced()`의 인스턴스 개수를 **visible count**로 제한

#### 핵심 코드 스니펫 후보

**1. FrameResource에 추가된 Visible Indices 버퍼** ([FrameResource.h](file:///d:/SandyHeroesClient/SandyHeroes_--/SandyHeroesClient/FrameResource.h))
```cpp
// 인스턴싱된 전체 오브젝트 데이터
std::unique_ptr<UploadBuffer<InstanceData>> sb_instance_data;
// 메인 카메라 절두체 내 오브젝트의 인덱스 목록
std::unique_ptr<UploadBuffer<UINT>> sb_main_visible_indices; 
// 그림자 맵 영역 내 오브젝트의 인덱스 목록
std::unique_ptr<UploadBuffer<UINT>> sb_shadow_visible_indices;
```

**2. 절두체 컬링 로직** ([Scene.cpp](file:///d:/SandyHeroesClient/SandyHeroes_--/SandyHeroesClient/Scene.cpp) `RunViewFrustumCulling()`)
```cpp
void Scene::RunViewFrustumCulling()
{
    const auto& world_frustum = main_camera_->world_frustum();
    for (auto& sector : sectors_)
    {
        if (sector.bounds().Intersects(world_frustum))
        {
            for (const auto& mesh_component : sector.mesh_component_list())
            {
                // OBB로 변환 후 절두체와 교차 테스트
                BoundingOrientedBox obb;
                BoundingOrientedBox::CreateFromBoundingBox(obb, aabb);
                obb.Transform(obb, XMLoadFloat4x4(&owner->world_matrix()));
                locked_mesh_component->set_is_in_view_frustum(
                    world_frustum.Intersects(obb));
            }
        }
    }
}
```

**3. Visible Index 기록** ([MeshComponent.cpp](file:///d:/SandyHeroesClient/SandyHeroes_--/SandyHeroesClient/MeshComponent.cpp) `UpdateConstantBuffer()`)
```cpp
if(is_in_view_frustum_)
{
    current_frame_resource->sb_main_visible_indices->CopyData(
        current_frame_resource->current_main_visible_offset++,
        constant_buffer_index_);
}
if(is_in_shadow_map_obb_)
{
    current_frame_resource->sb_shadow_visible_indices->CopyData(
        current_frame_resource->current_shadow_visible_offset++,
        constant_buffer_index_);
}
```

**4. HLSL에서 Visible Indices로 간접 인덱싱** ([GlobalValues.hlsl](file:///d:/SandyHeroesClient/SandyHeroes_--/SandyHeroesClient/GlobalValues.hlsl) + [StandardShader.hlsl](file:///d:/SandyHeroesClient/SandyHeroes_--/SandyHeroesClient/StandardShader.hlsl))
```hlsl
// GlobalValues.hlsl
StructuredBuffer<InstanceData> g_instance_data : register(t0, space1);
StructuredBuffer<uint> g_main_visible_indices : register(t1, space1);
StructuredBuffer<uint> g_shadow_visible_indices : register(t2, space1);

// StandardShader.hlsl (메인 렌더)
matrix world = g_instance_data[g_main_visible_indices[instance_id]].world_matrix;

// Shadow.hlsl (그림자 렌더)
matrix world = g_instance_data[g_shadow_visible_indices[instance_id]].world_matrix;
```

**5. Material::Render에서 visible count로 인스턴싱** ([Material.cpp](file:///d:/SandyHeroesClient/SandyHeroes_--/SandyHeroesClient/Material.cpp))
```cpp
if (bShadow)
    mesh->RenderInstancing(command_list, sub_mesh_index, 
        curr_frame_resource, instance_count.shadow_visible_count);
else
    mesh->RenderInstancing(command_list, sub_mesh_index, 
        curr_frame_resource, instance_count.main_visible_count);
```

### 페이지 12: 절두체 컬링 PIX 성능 비교 🔴

- [ ] 해당 커밋으로 체크아웃 후 PIX 캡처
  - 드로우 콜 횟수: "144 → ?" 수치 기입
  - 인스턴스 개수: "411 → ?" 수치 기입
- [ ] **텍스트 교체 필요**: 현재 페이지 6(프레임리소스)의 텍스트가 복사되어 있음 → 절두체 컬링 결과 설명으로 교체

---

## 3. 컴포넌트 패턴 기반 게임오브젝트 프레임워크 (신규 페이지) 🔴

> Q1: 추가할 예정이지만 소개 방향을 못 잡으셨다고 하셨으므로, 3가지 방향을 제안드립니다.

### 소개 방향 제안

#### 방향 A: 문제-해결 스토리텔링 (추천 ⭐)

PPT 2~3페이지 구성:

**페이지 A-1: 문제 → 해결 (아키텍처)**
- **문제**: 기존 상속 기반 설계의 한계 (다이아몬드 상속, 불필요한 기능 상속, 코드 중복)
- **해결**: Object + Component 합성 패턴 적용
- **다이어그램**: Object-Component 관계도 (Object가 `component_list_`로 Component들을 소유)
- **코드**: `Object::AddComponent()`, `Object::GetComponent<T>()` 템플릿

```
┌─────────────────────────────────────────┐
│ Object (enable_shared_from_this)         │
│ ├─ component_list_: list<shared_ptr>     │
│ ├─ child_: shared_ptr<Object>            │
│ ├─ sibling_: shared_ptr<Object>          │
│ └─ parent_: weak_ptr<Object>  ← 순환참조방지 │
└─────────────────────────────────────────┘
         │ owns (shared_ptr)
         ▼
┌──────────────────────────────────────┐
│ Component (enable_shared_from_this)   │
│ └─ owner_: weak_ptr<Object> ← 순환참조방지│
└──────────────────────────────────────┘
         △ 상속
    ┌────┴────┬──────────┬──────────────┐
MeshComponent  AnimatorComponent  GunComponent  ...
```

**페이지 A-2: 메모리 관리 설계**
- `shared_ptr` / `weak_ptr` 사용 전략과 그 이유
- Object→Component: `shared_ptr` (소유)
- Component→Object (owner_): `weak_ptr` (순환 참조 방지)
- Object 계층: child/sibling은 `shared_ptr`, parent는 `weak_ptr`
- 삭제 시 `Scene::DeleteObject` → `weak_ptr` 자동 무효화

**페이지 A-3 (선택): 실제 활용 예시**
- 같은 Object에 다양한 Component 조합으로 몬스터/플레이어/총기 등 생성
- `GetComponentInChildren<T>()` 계층 탐색 활용 사례

#### 방향 B: 설계 의도 + 코드 깊이

- Component 패턴을 선택한 이유 (ECS vs 순수 상속 vs 합성)
- `enable_shared_from_this`를 사용해야 했던 이유와 주의점
- `dynamic_pointer_cast`를 사용한 타입 안전 컴포넌트 접근

#### 방향 C: 기능 카탈로그

- 구현한 컴포넌트 전체 목록과 각각의 역할을 간결하게 카탈로그 형식으로 소개
- 다만 깊이가 부족해 보일 수 있어서 비추천

### 핵심 코드 위치 (공통)

| 역할 | 파일 | 핵심 코드 |
|------|------|-----------|
| Object 클래스 | [Object.h](file:///d:/SandyHeroesClient/SandyHeroes_--/SandyHeroesClient/Object.h) | `enable_shared_from_this`, `GetComponent<T>()`, `AddChild()`, `component_list_`, `child_`/`sibling_`(`shared_ptr`) vs `parent_`(`weak_ptr`) |
| Component 기반 | [Component.h](file:///d:/SandyHeroesClient/SandyHeroes_--/SandyHeroesClient/Component.h) | `enable_shared_from_this`, `weak_ptr<Object> owner_`, 순수 가상 `GetCopy()` |
| Scene의 Object 관리 | [Scene.h](file:///d:/SandyHeroesClient/SandyHeroes_--/SandyHeroesClient/Scene.h) | `AddObject()`, `DeleteObject()`, `object_list_<shared_ptr>`, 지연 삭제 패턴 |

---

## 전체 작업 체크리스트

### Phase 1: 즉시 작업 가능 (코드/스크린샷 불필요)

- [ ] 페이지 3: 목차 업데이트 (배치처리→인스턴싱→절두체 컬링→컴포넌트 패턴)
- [ ] 페이지 4: 인게임 스크린샷 추가
- [ ] 페이지 6: "공유자원이 2개 이상" → "공유자원이 3개 (kFrameCount=3)" 수정 검토
- [ ] 페이지 11: 절두체 컬링 Visible Indices 패턴 설명 작성 (위 가이드 참조)
- [ ] 페이지 12: 텍스트 교체 (프레임리소스 설명 → 절두체 컬링 결과 설명)
- [ ] 페이지 13~15: 컴포넌트 패턴 섹션 작성 (방향 결정 후)

### Phase 2: 커밋 체크아웃 + PIX 측정 필요

- [ ] 인스턴싱 적용 직후 커밋 → PIX 캡처 → 페이지 10 데이터 채움
- [ ] 절두체 컬링 적용 전 커밋 → PIX 캡처 → 페이지 12 "전" 데이터
- [ ] 절두체 컬링 적용 후(최신) → PIX 캡처 → 페이지 12 "후" 데이터
- [ ] 페이지 8의 PIX/fps 스크린샷 유효성 재확인

### Phase 3: 최종 검토

- [ ] 전체 페이지 흐름 검토 (스토리라인 일관성)
- [ ] 코드 스니펫이 읽기 쉽게 편집되었는지 확인
- [ ] 오탈자 및 수치 정확성 검증

---

## Open Questions

> [!IMPORTANT]
> **Q6**: 컴포넌트 패턴 소개는 **방향 A (문제-해결 스토리텔링)**, **방향 B (설계 의도 + 코드 깊이)**, **방향 C (기능 카탈로그)** 중 어떤 방향이 좋으신가요? A를 추천드립니다.

> [!NOTE]
> **Q7**: 페이지 11-12의 절두체 컬링 설명을 위에서 제안한 "Visible Indices 패턴" 가이드대로 작성하는 것에 동의하시나요?

> [!NOTE]
> **Q8**: PIX 측정을 위해 체크아웃할 커밋 위치를 알고 계시나요? (인스턴싱 적용 직후, 절두체 컬링 적용 전/후)
