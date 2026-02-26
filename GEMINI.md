\# 코드 리팩토링 프로젝트



\## 리팩토링 목표

* 코드 컨벤션 2번의 맞게 Object, Component 클래스를 'shared\_ptr', 'weak\_ptr'로 관리
* Scene의 object\_list\_만 shared\_ptr로 변경하고 나머지 오브젝트 및 컴포넌트 관련 리스트를 'weak\_ptr'로 변경하여 오브젝트 삭제시 나머지 리스트는 리스트 순회시 포인터의 유효성 체크 후 안정적으로 제거되게 수정
* 최종적으로 Object의 'is\_dead\_' 멤버 및 Scene의 오브젝트 지연삭제 기능을 제거하고 대신 'shared\_ptr', 'weak\_ptr'이 댕글링 포인터를 방지



\## 리팩토링 원칙

* 기능 변경하지 않기
* 단계적으로 코드를 수정하며 대규모 재작성 피하기
* ModelInfo 클래스의 파일 로드 로직을 절대 수정하지 말 것
* 렌더링 및 Directx12 관련 로직을 절대 수정하지 말 것
* 리팩토링 작업을 할 때 마다 '## 리팩토링 상황' 아래에 기록한다.

## 리팩토링 상황

* [2026-02-27] Object 클래스 리팩토링 완료 (shared_ptr, weak_ptr 적용 및 is_dead_ 멤버 제거)
* [2026-02-27] Scene 클래스 리팩토링 완료 (object_list_ shared_ptr 적용, 지연 삭제 로직 제거 및 기타 참조 리스트 weak_ptr 적용)
