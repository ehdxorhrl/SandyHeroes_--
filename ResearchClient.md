# SandyHeroesClient - Deep Architectural & Memory Analysis

> **Overview:** This document provides a deeper analysis of class architectures and evaluates memory management patterns against the project's refactoring objectives (shared_ptr, weak_ptr enforcement and circular reference prevention).

## File: AnimationSet.h

### Class: Object
- **Architectural Role:** The base entity in the game world. Contains a transform and a list of Components.
- **Memory Management Analysis:**
  - *Refactoring Complete:* is_dead_ member has been removed. Deletion is handled purely by Scene::DeleteObject and weak_ptr invalidation.
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: AnimationSet
- **Architectural Role:** Core data structure or utility class for animationset.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: AnimationState.h

### Class: Object
- **Architectural Role:** The base entity in the game world. Contains a transform and a list of Components.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.
  - *Refactoring Complete:* is_dead_ member has been removed. Deletion is handled purely by Scene::DeleteObject and weak_ptr invalidation.

### Class: AnimatorComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.
  - *Refactoring Complete:* Object ownership clarified. owner_ and non-owning references use weak_ptr to prevent circular references.

### Class: AnimationState
- **Architectural Role:** Defines a specific state within a Finite State Machine (e.g., animation or AI behavior).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.

---

## File: AnimationTrack.h

### Class: AnimationSet
- **Architectural Role:** Core data structure or utility class for animationset.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: Object
- **Architectural Role:** The base entity in the game world. Contains a transform and a list of Components.
- **Memory Management Analysis:**
  - *Refactoring Complete:* is_dead_ member has been removed. Deletion is handled purely by Scene::DeleteObject and weak_ptr invalidation.
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: AnimationLoopType
- **Architectural Role:** Core data structure or utility class for animationlooptype.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: AnimationTrack
- **Architectural Role:** Core data structure or utility class for animationtrack.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: AnimatorComponent.h

### Class: AnimationState
- **Architectural Role:** Defines a specific state within a Finite State Machine (e.g., animation or AI behavior).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - unique_ptr: Exclusively owns AnimationState, AnimationSet.

### Class: AnimatorComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - unique_ptr: Exclusively owns AnimationState, AnimationSet.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

---

## File: BaseScene.h

### Class: MeshColliderComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MovementComponent, ParticleComponent, Object.
  - weak_ptr: Safely references SpawnerComponent, GroundColliderComponent, BoxColliderComponent, MeshComponent, MonsterComponent, RazerComponent, Object, MovementComponent without affecting lifecycle.
  - unique_ptr: Exclusively owns ParticleSystem, TextFormat.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: SpawnerComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MovementComponent, ParticleComponent, Object.
  - weak_ptr: Safely references SpawnerComponent, GroundColliderComponent, BoxColliderComponent, MeshComponent, MonsterComponent, RazerComponent, Object, MovementComponent without affecting lifecycle.
  - unique_ptr: Exclusively owns ParticleSystem, TextFormat.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: BoxColliderComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MovementComponent, ParticleComponent, Object.
  - weak_ptr: Safely references SpawnerComponent, GroundColliderComponent, BoxColliderComponent, MeshComponent, MonsterComponent, RazerComponent, Object, MovementComponent without affecting lifecycle.
  - unique_ptr: Exclusively owns ParticleSystem, TextFormat.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: GroundColliderComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MovementComponent, ParticleComponent, Object.
  - weak_ptr: Safely references SpawnerComponent, GroundColliderComponent, BoxColliderComponent, MeshComponent, MonsterComponent, RazerComponent, Object, MovementComponent without affecting lifecycle.
  - unique_ptr: Exclusively owns ParticleSystem, TextFormat.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: WallColliderComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MovementComponent, ParticleComponent, Object.
  - weak_ptr: Safely references SpawnerComponent, GroundColliderComponent, BoxColliderComponent, MeshComponent, MonsterComponent, RazerComponent, Object, MovementComponent without affecting lifecycle.
  - unique_ptr: Exclusively owns ParticleSystem, TextFormat.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: MovementComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MovementComponent, ParticleComponent, Object.
  - weak_ptr: Safely references SpawnerComponent, GroundColliderComponent, BoxColliderComponent, MeshComponent, MonsterComponent, RazerComponent, Object, MovementComponent without affecting lifecycle.
  - unique_ptr: Exclusively owns ParticleSystem, TextFormat.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: MonsterComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MovementComponent, ParticleComponent, Object.
  - weak_ptr: Safely references SpawnerComponent, GroundColliderComponent, BoxColliderComponent, MeshComponent, MonsterComponent, RazerComponent, Object, MovementComponent without affecting lifecycle.
  - unique_ptr: Exclusively owns ParticleSystem, TextFormat.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: BaseScene
- **Inherits from:** Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MovementComponent, ParticleComponent, Object.
  - weak_ptr: Safely references SpawnerComponent, GroundColliderComponent, BoxColliderComponent, MeshComponent, MonsterComponent, RazerComponent, Object, MovementComponent without affecting lifecycle.
  - unique_ptr: Exclusively owns ParticleSystem, TextFormat.
  - *Refactoring Complete:* Scene lists (except object_list_) are managed with weak_ptr, resolving dangling pointers during object deletion.

### Struct: WallCheckObject
- **Architectural Role:** Core data structure or utility class for wallcheckobject.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MovementComponent, Object.
  - weak_ptr: Safely references MovementComponent, Object without affecting lifecycle.

---

## File: BillboardMesh.h

### Class: BillboardMesh
- **Inherits from:** Mesh
- **Architectural Role:** Contains vertex, index, and potentially bone data for rendering 3D geometry.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: BombDragonAnimationState.h

### Class: BombDragonAnimationTrack
- **Architectural Role:** Core data structure or utility class for bombdragonanimationtrack.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: BombDragonAnimationState
- **Inherits from:** AnimationState
- **Architectural Role:** Defines a specific state within a Finite State Machine (e.g., animation or AI behavior).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.

---

## File: BoxColliderComponent.h

### Class: BoxColliderComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: BreathingShader.h

### Class: BreathingShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: CameraComponent.h

### Class: MeshComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MeshComponent.

### Class: CameraComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for MeshComponent.

---

## File: CharacterComponent.h

### Class: AnimatorComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references AnimatorComponent, MovementComponent without affecting lifecycle.
  - *Refactoring Complete:* Object ownership clarified. owner_ and non-owning references use weak_ptr to prevent circular references.

### Class: MovementComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references AnimatorComponent, MovementComponent without affecting lifecycle.
  - *Refactoring Complete:* Object ownership clarified. owner_ and non-owning references use weak_ptr to prevent circular references.

### Class: CharacterComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references AnimatorComponent, MovementComponent without affecting lifecycle.
  - *Refactoring Complete:* Object ownership clarified. owner_ and non-owning references use weak_ptr to prevent circular references.

---

## File: ChestAnimationState.h

### Class: ChestAnimationTrack
- **Architectural Role:** Core data structure or utility class for chestanimationtrack.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: ChestAnimationState
- **Inherits from:** AnimationState
- **Architectural Role:** Defines a specific state within a Finite State Machine (e.g., animation or AI behavior).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.

---

## File: ChestComponent.h

### Class: ModelInfo
- **Architectural Role:** Core data structure or utility class for model information.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

### Class: Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

### Class: ChestComponent
- **Inherits from:** CharacterComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

---

## File: ColliderComponent.h

### Class: ColliderComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: ColorShader.h

### Class: ColorShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: Component.h

### Class: Object
- **Architectural Role:** The base entity in the game world. Contains a transform and a list of Components.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - *Refactoring Complete:* is_dead_ member has been removed. Deletion is handled purely by Scene::DeleteObject and weak_ptr invalidation.

### Class: Component
- **Inherits from:** std
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

---

## File: CubeMesh.h

### Class: CubeMesh
- **Inherits from:** Mesh
- **Architectural Role:** Contains vertex, index, and potentially bone data for rendering 3D geometry.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: CutSceneRecorder.h

### Class: Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: Object
- **Architectural Role:** The base entity in the game world. Contains a transform and a list of Components.
- **Memory Management Analysis:**
  - *Refactoring Complete:* is_dead_ member has been removed. Deletion is handled purely by Scene::DeleteObject and weak_ptr invalidation.
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CutSceneData
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: CutSceneRecorder
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: CutSceneTrack.h

### Class: CutSceneTrack
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.

---

## File: DebugShader.h

### Class: DebugShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: DescriptorManager.h

### Class: DescriptorManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: FMODSoundManager.h

### Class: FMODSoundManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: FPSControllerComponent.h

### Class: Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for ParticleComponent, Object.
  - weak_ptr: Safely references ParticleComponent, Object without affecting lifecycle.

### Class: FPSControllerComponent
- **Inherits from:** InputControllerComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for ParticleComponent, Object.
  - weak_ptr: Safely references ParticleComponent, Object without affecting lifecycle.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

---

## File: FadeInUIComponent.h

### Class: UiMeshComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - weak_ptr: Safely references UiMeshComponent without affecting lifecycle.

### Class: FadeInUIComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - weak_ptr: Safely references UiMeshComponent without affecting lifecycle.

---

## File: FrameResource.h

### Struct: CBShadow
- **Architectural Role:** Core data structure or utility class for cbshadow.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: FrameResource
- **Architectural Role:** Core data structure or utility class for frameresource.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: FrameResourceManager.h

### Class: FrameResourceManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - unique_ptr: Exclusively owns FrameResource.

---

## File: GameFramework.h

### Class: Timer
- **Architectural Role:** Core data structure or utility class for timer.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: FrameResourceManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: DescriptorManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: InputManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: TextFormat
- **Architectural Role:** Core data structure or utility class for textformat.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: GameFramework
- **Architectural Role:** Core data structure or utility class for gameframework.
- **Memory Management Analysis:**
  - unique_ptr: Exclusively owns DescriptorManager, Scene, Timer, FrameResourceManager, InputManager, TextRenderer, TextFormat.

---

## File: GroundColliderComponent.h

### Class: GroundColliderComponent
- **Inherits from:** MeshColliderComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: GunComponent.h

### Class: Mesh
- **Architectural Role:** Core data structure or utility class for mesh.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: GunFireType
- **Architectural Role:** Core data structure or utility class for gunfiretype.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: BulletType
- **Architectural Role:** Core data structure or utility class for bullettype.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: ElementType
- **Architectural Role:** Core data structure or utility class for elementtype.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: GunInfo
- **Architectural Role:** Core data structure or utility class for gun information.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: GunComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

---

## File: HitDragonAnimationState.h

### Class: HitDragonAnimationTrack
- **Architectural Role:** Core data structure or utility class for hitdragonanimationtrack.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: HitDragonAnimationState
- **Inherits from:** AnimationState
- **Architectural Role:** Defines a specific state within a Finite State Machine (e.g., animation or AI behavior).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.

---

## File: InputControllerComponent.h

### Class: InputControllerComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: InputManager.h

### Class: InputControllerComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: InputMessage
- **Architectural Role:** Core data structure or utility class for inputmessage.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: InputManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: Material.h

### Class: DescriptorManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: MeshComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: CameraComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: FrameResource
- **Architectural Role:** Core data structure or utility class for frameresource.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: TextureType
- **Architectural Role:** Core data structure or utility class for texturetype.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: Texture
- **Architectural Role:** Core data structure or utility class for texture.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: Material
- **Architectural Role:** Core data structure or utility class for material.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for CameraComponent.
  - weak_ptr: Safely references MeshComponent without affecting lifecycle.
  - unique_ptr: Exclusively owns Texture.

---

## File: Mesh.h

### Class: MeshComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - weak_ptr: Safely references MeshComponent without affecting lifecycle.

### Class: FrameResourceManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - weak_ptr: Safely references MeshComponent without affecting lifecycle.

### Class: FrameResource
- **Architectural Role:** Core data structure or utility class for frameresource.
- **Memory Management Analysis:**
  - weak_ptr: Safely references MeshComponent without affecting lifecycle.

### Class: DescriptorManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - weak_ptr: Safely references MeshComponent without affecting lifecycle.

### Class: Material
- **Architectural Role:** Core data structure or utility class for material.
- **Memory Management Analysis:**
  - weak_ptr: Safely references MeshComponent without affecting lifecycle.

### Class: Mesh
- **Architectural Role:** Core data structure or utility class for mesh.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: MeshColliderComponent.h

### Class: Mesh
- **Architectural Role:** Core data structure or utility class for mesh.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: MeshColliderComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: MeshComponent.h

### Class: Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: FrameResource
- **Architectural Role:** Core data structure or utility class for frameresource.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: MeshComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: ModelInfo.h

### Class: Object
- **Architectural Role:** The base entity in the game world. Contains a transform and a list of Components.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - unique_ptr: Exclusively owns Mesh, Texture, Material, AnimationSet.
  - *Refactoring Complete:* is_dead_ member has been removed. Deletion is handled purely by Scene::DeleteObject and weak_ptr invalidation.

### Class: Mesh
- **Architectural Role:** Core data structure or utility class for mesh.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - unique_ptr: Exclusively owns Mesh, Texture, Material, AnimationSet.

### Class: Material
- **Architectural Role:** Core data structure or utility class for material.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - unique_ptr: Exclusively owns Mesh, Texture, Material, AnimationSet.

### Struct: Texture
- **Architectural Role:** Core data structure or utility class for texture.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - unique_ptr: Exclusively owns Mesh, Texture, Material, AnimationSet.

### Class: ModelInfo
- **Architectural Role:** Core data structure or utility class for model information.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - unique_ptr: Exclusively owns Mesh, Texture, Material, AnimationSet.

---

## File: MonsterComponent.h

### Class: MonsterType
- **Architectural Role:** Core data structure or utility class for monstertype.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: StatusEffectType
- **Architectural Role:** Core data structure or utility class for statuseffecttype.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: StatusEffect
- **Architectural Role:** Core data structure or utility class for statuseffect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: MonsterComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

---

## File: MovementComponent.h

### Class: MovementComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - *Refactoring Complete:* Object ownership clarified. owner_ and non-owning references use weak_ptr to prevent circular references.

---

## File: Object.h

### Class: Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CollideType
- **Architectural Role:** Core data structure or utility class for collidetype.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: Object
- **Inherits from:** std
- **Architectural Role:** The base entity in the game world. Contains a transform and a list of Components.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Component, T, Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - *Refactoring Complete:* is_dead_ member has been removed. Deletion is handled purely by Scene::DeleteObject and weak_ptr invalidation.

### Struct: T
- **Architectural Role:** Core data structure or utility class for t.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Component, T, Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

### Struct: T
- **Architectural Role:** Core data structure or utility class for t.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Component, T, Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

### Struct: T
- **Architectural Role:** Core data structure or utility class for t.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Component, T, Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

### Struct: T
- **Architectural Role:** Core data structure or utility class for t.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Component, T, Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

---

## File: Packet.h

### Struct: sc_packet_player_change_animation
- **Architectural Role:** Core data structure or utility class for sc_packet_player_change_animation.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_play_cut_scene
- **Architectural Role:** Core data structure or utility class for sc_packet_play_cut_scene.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_play_reload_sound
- **Architectural Role:** Core data structure or utility class for sc_packet_play_reload_sound.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_object_set_dead
- **Architectural Role:** Core data structure or utility class for sc_packet_object_set_dead.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_shotdragon_attack
- **Architectural Role:** Core data structure or utility class for sc_packet_shotdragon_attack.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_monster_change_animation
- **Architectural Role:** Core data structure or utility class for sc_packet_monster_change_animation.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_play_mainskill
- **Architectural Role:** Core data structure or utility class for sc_packet_play_mainskill.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_take_scroll
- **Architectural Role:** Core data structure or utility class for sc_packet_take_scroll.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_scroll_info
- **Architectural Role:** Core data structure or utility class for sc_packet_scroll_ information.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_gun_change
- **Architectural Role:** Core data structure or utility class for sc_packet_gun_change.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_drop_gun
- **Architectural Role:** Core data structure or utility class for sc_packet_drop_gun.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_loaded_bullet
- **Architectural Role:** Core data structure or utility class for sc_packet_loaded_bullet.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_monster_move
- **Architectural Role:** Core data structure or utility class for sc_packet_monster_move.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_monster_damaged
- **Architectural Role:** Core data structure or utility class for sc_packet_monster_damaged.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_monster_info
- **Architectural Role:** Core data structure or utility class for sc_packet_monster_ information.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_monster_damaged_particle
- **Architectural Role:** Core data structure or utility class for sc_packet_monster_damaged_particle.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_stage_clear
- **Architectural Role:** Core data structure or utility class for sc_packet_stage_clear.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_player_damaged
- **Architectural Role:** Core data structure or utility class for sc_packet_player_damaged.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_user_info
- **Architectural Role:** Core data structure or utility class for sc_packet_user_ information.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_move
- **Architectural Role:** Core data structure or utility class for sc_packet_move.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_rotate
- **Architectural Role:** Core data structure or utility class for sc_packet_rotate.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_enter
- **Architectural Role:** Core data structure or utility class for sc_packet_enter.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_leave
- **Architectural Role:** Core data structure or utility class for sc_packet_leave.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_create_bullet
- **Architectural Role:** Core data structure or utility class for sc_packet_create_bullet.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_delete_pyramid
- **Architectural Role:** Core data structure or utility class for sc_packet_delete_pyramid.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: sc_packet_game_clear
- **Architectural Role:** Core data structure or utility class for sc_packet_game_clear.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: cs_packet_login
- **Architectural Role:** Core data structure or utility class for cs_packet_login.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: cs_packet_keyboard_input
- **Architectural Role:** Core data structure or utility class for cs_packet_keyboard_input.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: cs_packet_mouse_move
- **Architectural Role:** Core data structure or utility class for cs_packet_mouse_move.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: cs_packet_mouse_click
- **Architectural Role:** Core data structure or utility class for cs_packet_mouse_click.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: cs_packet_mouse_unclick
- **Architectural Role:** Core data structure or utility class for cs_packet_mouse_unclick.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: ParticleComponent.h

### Struct: FrameResource
- **Architectural Role:** Core data structure or utility class for frameresource.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: ParticleComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: ParticleData
- **Architectural Role:** Core data structure or utility class for particle data.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: Particle
- **Architectural Role:** Core data structure or utility class for particle.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: ParticleRenderer.h

### Struct: FrameResource
- **Architectural Role:** Core data structure or utility class for frameresource.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for CameraComponent.

### Class: DescriptorManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for CameraComponent.

### Class: ParticleComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for CameraComponent.

### Class: CameraComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for CameraComponent.

### Class: ParticleRenderer
- **Architectural Role:** Core data structure or utility class for particlerenderer.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for CameraComponent.

---

## File: ParticleShader.h

### Class: ParticleShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: ParticleSystem.h

### Class: Mesh
- **Architectural Role:** Core data structure or utility class for mesh.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

### Class: Material
- **Architectural Role:** Core data structure or utility class for material.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

### Class: Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

### Class: Object
- **Architectural Role:** The base entity in the game world. Contains a transform and a list of Components.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - *Refactoring Complete:* is_dead_ member has been removed. Deletion is handled purely by Scene::DeleteObject and weak_ptr invalidation.

### Struct: Particle
- **Architectural Role:** Core data structure or utility class for particle.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

### Class: ParticleSystem
- **Architectural Role:** Core data structure or utility class for particlesystem.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.

---

## File: PlayerAnimationState.h

### Class: PlayerAnimationTrack
- **Architectural Role:** Core data structure or utility class for playeranimationtrack.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: PlayerAnimationState
- **Inherits from:** AnimationState
- **Architectural Role:** Defines a specific state within a Finite State Machine (e.g., animation or AI behavior).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.

---

## File: PlayerComponent.h

### Class: Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: ModelInfo
- **Architectural Role:** Core data structure or utility class for model information.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: PlayerComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: ProgressBarComponent.h

### Class: ProgressBarComponent
- **Inherits from:** UiComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - *Refactoring Complete:* Object ownership clarified. owner_ and non-owning references use weak_ptr to prevent circular references.

---

## File: RazerComponent.h

### Class: RazerComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: RazerMesh.h

### Class: RazerMesh
- **Inherits from:** BillboardMesh
- **Architectural Role:** Core data structure or utility class for razermesh.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: RazerShader.h

### Class: RazerShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: RecorderScene.h

### Class: RecorderScene
- **Inherits from:** Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - unique_ptr: Exclusively owns CutSceneRecorder.
  - *Refactoring Complete:* Scene lists (except object_list_) are managed with weak_ptr, resolving dangling pointers during object deletion.

---

## File: RecordingScene.h

### Class: RecordingScene
- **Inherits from:** Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - *Refactoring Complete:* Scene lists (except object_list_) are managed with weak_ptr, resolving dangling pointers during object deletion.
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: Scene.h

### Class: WallColliderComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for InputControllerComponent, CameraComponent, Object.
  - weak_ptr: Safely references MeshComponent, WallColliderComponent, Object without affecting lifecycle.
  - unique_ptr: Exclusively owns ModelInfo, Mesh, ParticleRenderer, Texture, Shader, Material.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: FrameResourceManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for InputControllerComponent, CameraComponent, Object.
  - weak_ptr: Safely references MeshComponent, WallColliderComponent, Object without affecting lifecycle.
  - unique_ptr: Exclusively owns ModelInfo, Mesh, ParticleRenderer, Texture, Shader, Material.

### Class: DescriptorManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for InputControllerComponent, CameraComponent, Object.
  - weak_ptr: Safely references MeshComponent, WallColliderComponent, Object without affecting lifecycle.
  - unique_ptr: Exclusively owns ModelInfo, Mesh, ParticleRenderer, Texture, Shader, Material.

### Class: CameraComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for InputControllerComponent, CameraComponent, Object.
  - weak_ptr: Safely references MeshComponent, WallColliderComponent, Object without affecting lifecycle.
  - unique_ptr: Exclusively owns ModelInfo, Mesh, ParticleRenderer, Texture, Shader, Material.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: InputManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for InputControllerComponent, CameraComponent, Object.
  - weak_ptr: Safely references MeshComponent, WallColliderComponent, Object without affecting lifecycle.
  - unique_ptr: Exclusively owns ModelInfo, Mesh, ParticleRenderer, Texture, Shader, Material.

### Class: InputControllerComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for InputControllerComponent, CameraComponent, Object.
  - weak_ptr: Safely references MeshComponent, WallColliderComponent, Object without affecting lifecycle.
  - unique_ptr: Exclusively owns ModelInfo, Mesh, ParticleRenderer, Texture, Shader, Material.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: GameFramework
- **Architectural Role:** Core data structure or utility class for gameframework.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for InputControllerComponent, CameraComponent, Object.
  - weak_ptr: Safely references MeshComponent, WallColliderComponent, Object without affecting lifecycle.
  - unique_ptr: Exclusively owns ModelInfo, Mesh, ParticleRenderer, Texture, Shader, Material.

### Class: ColliderComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for InputControllerComponent, CameraComponent, Object.
  - weak_ptr: Safely references MeshComponent, WallColliderComponent, Object without affecting lifecycle.
  - unique_ptr: Exclusively owns ModelInfo, Mesh, ParticleRenderer, Texture, Shader, Material.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: MeshColliderComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for InputControllerComponent, CameraComponent, Object.
  - weak_ptr: Safely references MeshComponent, WallColliderComponent, Object without affecting lifecycle.
  - unique_ptr: Exclusively owns ModelInfo, Mesh, ParticleRenderer, Texture, Shader, Material.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

### Class: Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for InputControllerComponent, CameraComponent, Object.
  - weak_ptr: Safely references MeshComponent, WallColliderComponent, Object without affecting lifecycle.
  - unique_ptr: Exclusively owns ModelInfo, Mesh, ParticleRenderer, Texture, Shader, Material.

---

## File: ScrollComponent.h

### Class: ScrollType
- **Architectural Role:** Core data structure or utility class for scrolltype.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: ScrollComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: Sector.h

### Class: Object
- **Architectural Role:** The base entity in the game world. Contains a transform and a list of Components.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - *Refactoring Complete:* is_dead_ member has been removed. Deletion is handled purely by Scene::DeleteObject and weak_ptr invalidation.

### Class: Sector
- **Architectural Role:** Core data structure or utility class for sector.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.

---

## File: Shader.h

### Class: Material
- **Architectural Role:** Core data structure or utility class for material.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: DescriptorManager
- **Architectural Role:** A singleton or central system responsible for managing global resources or systems.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: CameraComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: FrameResource
- **Architectural Role:** Core data structure or utility class for frameresource.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: ShaderType
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: ShadowShader.h

### Class: ShadowShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: ShotDragonAnimationState.h

### Class: ShotDragonAnimationTrack
- **Architectural Role:** Core data structure or utility class for shotdragonanimationtrack.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: ShotDragonAnimationState
- **Inherits from:** AnimationState
- **Architectural Role:** Defines a specific state within a Finite State Machine (e.g., animation or AI behavior).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.

---

## File: SkinnedMesh.h

### Class: SkinnedMesh
- **Inherits from:** Mesh
- **Architectural Role:** Contains vertex, index, and potentially bone data for rendering 3D geometry.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: SkinnedMeshComponent.h

### Class: SkinnedMeshComponent
- **Inherits from:** MeshComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: SkinnedMeshShader.h

### Class: SkinnedMeshShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: SkinnedShadowShader.h

### Class: SkinnedShadowShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: SkyboxMesh.h

### Struct: Texture
- **Architectural Role:** Core data structure or utility class for texture.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: SkyboxMesh
- **Inherits from:** Mesh
- **Architectural Role:** Contains vertex, index, and potentially bone data for rendering 3D geometry.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: SkyboxShader.h

### Class: SkyboxShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: SoundComponent.h

### Class: SoundComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: SpawnerComponent.h

### Class: Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Component.

### Class: ModelInfo
- **Architectural Role:** Core data structure or utility class for model information.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Component.

### Class: SpawnerComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Component.

---

## File: StandardMeshShader.h

### Class: StandardMeshShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: StandardSkinnedMeshShader.h

### Class: StandardSkinnedMeshShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: StrongDragonAnimationState.h

### Class: StrongDragonAnimationTrack
- **Architectural Role:** Core data structure or utility class for strongdragonanimationtrack.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: StrongDragonAnimationState
- **Inherits from:** AnimationState
- **Architectural Role:** Defines a specific state within a Finite State Machine (e.g., animation or AI behavior).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.

---

## File: SuperDragonAnimationState.h

### Class: SuperDragonAnimationTrack
- **Architectural Role:** Core data structure or utility class for superdragonanimationtrack.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: SuperDragonAnimationState
- **Inherits from:** AnimationState
- **Architectural Role:** Defines a specific state within a Finite State Machine (e.g., animation or AI behavior).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.

---

## File: TestAnimationState.h

### Class: TestAnimationState
- **Inherits from:** AnimationState
- **Architectural Role:** Defines a specific state within a Finite State Machine (e.g., animation or AI behavior).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for AnimatorComponent, Object.

---

## File: TestControllerComponent.h

### Class: TestControllerComponent
- **Inherits from:** InputControllerComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: TestScene.h

### Class: TestScene
- **Inherits from:** Scene
- **Architectural Role:** Manages a specific state of the game, including objects, UI, and rendering resources within that state.
- **Memory Management Analysis:**
  - *Refactoring Complete:* Scene lists (except object_list_) are managed with weak_ptr, resolving dangling pointers during object deletion.
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: TextComponent.h

### Class: TextFormat
- **Architectural Role:** Core data structure or utility class for textformat.
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.

### Class: TextComponent
- **Inherits from:** UiComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - *Refactoring Complete:* Object ownership clarified. owner_ and non-owning references use weak_ptr to prevent circular references.

---

## File: TextFormat.h

### Class: TextFormat
- **Architectural Role:** Core data structure or utility class for textformat.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: TextRenderer.h

### Class: TextComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: TextRenderer
- **Architectural Role:** Core data structure or utility class for textrenderer.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: Timer.h

### Class: Timer
- **Architectural Role:** Core data structure or utility class for timer.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: TransparentShader.h

### Class: TransparentShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: UIMesh.h

### Class: UIMesh
- **Inherits from:** Mesh
- **Architectural Role:** Contains vertex, index, and potentially bone data for rendering 3D geometry.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: UIShader.h

### Class: UIShader
- **Inherits from:** Shader
- **Architectural Role:** Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: UiComponent.h

### Class: UiType
- **Architectural Role:** Core data structure or utility class for uitype.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: UiComponent
- **Inherits from:** Component
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - shared_ptr: Uses shared ownership for Object.
  - weak_ptr: Safely references Object without affecting lifecycle.
  - *Refactoring Complete:* owner_ and related Object references are correctly managed with weak_ptr, eliminating circular dependencies.

---

## File: UiMeshComponent.h

### Class: UiLayer
- **Architectural Role:** Core data structure or utility class for uilayer.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: UiMeshComponent
- **Inherits from:** MeshComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: UploadBuffer.h

### Class: UploadBuffer
- **Architectural Role:** Core data structure or utility class for uploadbuffer.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: WallColliderComponent.h

### Class: BoxColliderComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: WallColliderComponent
- **Inherits from:** MeshColliderComponent
- **Architectural Role:** A reusable module attached to an Object to define specific behaviors or data (ECS architecture).
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: d3dx12.h

### Struct: CD3DX12_DEFAULT
- **Architectural Role:** Core data structure or utility class for cd3dx12_default.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_RECT
- **Inherits from:** D3D12_RECT
- **Architectural Role:** Core data structure or utility class for cd3dx12_rect.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_VIEWPORT
- **Inherits from:** D3D12_VIEWPORT
- **Architectural Role:** Core data structure or utility class for cd3dx12_viewport.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_BOX
- **Inherits from:** D3D12_BOX
- **Architectural Role:** Core data structure or utility class for cd3dx12_box.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_DEPTH_STENCIL_DESC
- **Inherits from:** D3D12_DEPTH_STENCIL_DESC
- **Architectural Role:** Core data structure or utility class for cd3dx12_depth_stencil_desc.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_BLEND_DESC
- **Inherits from:** D3D12_BLEND_DESC
- **Architectural Role:** Core data structure or utility class for cd3dx12_blend_desc.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_RASTERIZER_DESC
- **Inherits from:** D3D12_RASTERIZER_DESC
- **Architectural Role:** Core data structure or utility class for cd3dx12_rasterizer_desc.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_RESOURCE_ALLOCATION_INFO
- **Inherits from:** D3D12_RESOURCE_ALLOCATION_INFO
- **Architectural Role:** Core data structure or utility class for cd3dx12_resource_allocation_ information.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_HEAP_PROPERTIES
- **Inherits from:** D3D12_HEAP_PROPERTIES
- **Architectural Role:** Core data structure or utility class for cd3dx12_heap_properties.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_HEAP_DESC
- **Inherits from:** D3D12_HEAP_DESC
- **Architectural Role:** Core data structure or utility class for cd3dx12_heap_desc.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_CLEAR_VALUE
- **Inherits from:** D3D12_CLEAR_VALUE
- **Architectural Role:** Core data structure or utility class for cd3dx12_clear_value.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_RANGE
- **Inherits from:** D3D12_RANGE
- **Architectural Role:** Core data structure or utility class for cd3dx12_range.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_SHADER_BYTECODE
- **Inherits from:** D3D12_SHADER_BYTECODE
- **Architectural Role:** Core data structure or utility class for cd3dx12_shader_bytecode.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_TILED_RESOURCE_COORDINATE
- **Inherits from:** D3D12_TILED_RESOURCE_COORDINATE
- **Architectural Role:** Core data structure or utility class for cd3dx12_tiled_resource_coordinate.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_TILE_REGION_SIZE
- **Inherits from:** D3D12_TILE_REGION_SIZE
- **Architectural Role:** Core data structure or utility class for cd3dx12_tile_region_size.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_SUBRESOURCE_TILING
- **Inherits from:** D3D12_SUBRESOURCE_TILING
- **Architectural Role:** Core data structure or utility class for cd3dx12_subresource_tiling.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_TILE_SHAPE
- **Inherits from:** D3D12_TILE_SHAPE
- **Architectural Role:** Core data structure or utility class for cd3dx12_tile_shape.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_RESOURCE_BARRIER
- **Inherits from:** D3D12_RESOURCE_BARRIER
- **Architectural Role:** Core data structure or utility class for cd3dx12_resource_barrier.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_PACKED_MIP_INFO
- **Inherits from:** D3D12_PACKED_MIP_INFO
- **Architectural Role:** Core data structure or utility class for cd3dx12_packed_mip_ information.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_SUBRESOURCE_FOOTPRINT
- **Inherits from:** D3D12_SUBRESOURCE_FOOTPRINT
- **Architectural Role:** Core data structure or utility class for cd3dx12_subresource_footprint.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_TEXTURE_COPY_LOCATION
- **Inherits from:** D3D12_TEXTURE_COPY_LOCATION
- **Architectural Role:** Core data structure or utility class for cd3dx12_texture_copy_location.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_DESCRIPTOR_RANGE
- **Inherits from:** D3D12_DESCRIPTOR_RANGE
- **Architectural Role:** Core data structure or utility class for cd3dx12_descriptor_range.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_ROOT_DESCRIPTOR_TABLE
- **Inherits from:** D3D12_ROOT_DESCRIPTOR_TABLE
- **Architectural Role:** Core data structure or utility class for cd3dx12_root_descriptor_table.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_ROOT_CONSTANTS
- **Inherits from:** D3D12_ROOT_CONSTANTS
- **Architectural Role:** Core data structure or utility class for cd3dx12_root_constants.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_ROOT_DESCRIPTOR
- **Inherits from:** D3D12_ROOT_DESCRIPTOR
- **Architectural Role:** Core data structure or utility class for cd3dx12_root_descriptor.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_ROOT_PARAMETER
- **Inherits from:** D3D12_ROOT_PARAMETER
- **Architectural Role:** Core data structure or utility class for cd3dx12_root_parameter.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_STATIC_SAMPLER_DESC
- **Inherits from:** D3D12_STATIC_SAMPLER_DESC
- **Architectural Role:** Core data structure or utility class for cd3dx12_static_sampler_desc.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_ROOT_SIGNATURE_DESC
- **Inherits from:** D3D12_ROOT_SIGNATURE_DESC
- **Architectural Role:** Core data structure or utility class for cd3dx12_root_signature_desc.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_DESCRIPTOR_RANGE1
- **Inherits from:** D3D12_DESCRIPTOR_RANGE1
- **Architectural Role:** Core data structure or utility class for cd3dx12_descriptor_range1.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_ROOT_DESCRIPTOR_TABLE1
- **Inherits from:** D3D12_ROOT_DESCRIPTOR_TABLE1
- **Architectural Role:** Core data structure or utility class for cd3dx12_root_descriptor_table1.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_ROOT_DESCRIPTOR1
- **Inherits from:** D3D12_ROOT_DESCRIPTOR1
- **Architectural Role:** Core data structure or utility class for cd3dx12_root_descriptor1.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_ROOT_PARAMETER1
- **Inherits from:** D3D12_ROOT_PARAMETER1
- **Architectural Role:** Core data structure or utility class for cd3dx12_root_parameter1.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC
- **Inherits from:** D3D12_VERSIONED_ROOT_SIGNATURE_DESC
- **Architectural Role:** Core data structure or utility class for cd3dx12_versioned_root_signature_desc.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_CPU_DESCRIPTOR_HANDLE
- **Inherits from:** D3D12_CPU_DESCRIPTOR_HANDLE
- **Architectural Role:** Core data structure or utility class for cd3dx12_cpu_descriptor_handle.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_GPU_DESCRIPTOR_HANDLE
- **Inherits from:** D3D12_GPU_DESCRIPTOR_HANDLE
- **Architectural Role:** Core data structure or utility class for cd3dx12_gpu_descriptor_handle.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Struct: CD3DX12_RESOURCE_DESC
- **Inherits from:** D3D12_RESOURCE_DESC
- **Architectural Role:** Core data structure or utility class for cd3dx12_resource_desc.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

## File: stdafx.h

### Class: RootParameterIndex
- **Architectural Role:** Core data structure or utility class for rootparameterindex.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: T
- **Architectural Role:** Core data structure or utility class for t.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

### Class: T
- **Architectural Role:** Core data structure or utility class for t.
- **Memory Management Analysis:**
  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).

---

