# Visual Studio 2022 Dark+ 코드 스니펫 스타일 가이드

HTML 슬라이드에서 VS2022 Dark+ 테마를 재현하기 위한 색상, 구조, Highlight.js 매핑 규칙

---

## 1. 창 크롬 (Window Chrome)

| 영역 | 속성 | 값 |
|---|---|---|
| 편집기 배경 | `background` | `#1E1E1E` |
| 탭 바 배경 | `background` | `#2D2D2D` |
| 활성 탭 배경 | `background` | `#1E1E1E` |
| 활성 탭 상단 테두리 | `border-top` | `2px solid #007ACC` |
| 탭 텍스트 (활성) | `color` | `#CCCCCC` |
| 창 테두리 | `border` | `1px solid #3C3C3C` |
| 탭 바 하단 구분선 | `border-bottom` | `1px solid #252526` |

### HTML 구조 예시

```html
<div class="code-window">
  <div class="code-win-header">
    <span class="win-filename">Mesh.cpp</span>
    <span class="win-label before">Before</span>  <!-- 또는 after -->
  </div>
  <pre><code class="language-cpp">/* 코드 */</code></pre>
</div>
```

### CSS

```css
.code-window {
  background: #1E1E1E;
  border-radius: 4px;
  overflow: hidden;
  border: 1px solid #3C3C3C;
  box-shadow: 0 2px 12px rgba(0,0,0,.65);
}
.code-win-header {
  background: #2D2D2D;
  display: flex;
  align-items: stretch;
  border-bottom: 1px solid #252526;
}
.win-filename {
  background: #1E1E1E;
  border-top: 2px solid #007ACC;   /* 활성 탭 표시 */
  padding: 6px 14px;
  font-family: 'Segoe UI', 'Malgun Gothic', sans-serif;
  font-size: 0.72rem;
  color: #CCCCCC;
  flex: 1;
  display: flex;
  align-items: center;
}
/* Before/After 레이블 */
.win-label.before { background: rgba(248,81,73,.12); color: #f85149; }
.win-label.after  { background: rgba(63,185,80,.12);  color: #3fb950; }
.win-label {
  font-size: 0.6rem;
  text-transform: uppercase;
  letter-spacing: .08em;
  padding: 4px 12px;
  font-weight: 700;
  display: flex;
  align-items: center;
  border-left: 1px solid #3C3C3C;
}
.code-window pre {
  margin: 0 !important;
  border-radius: 0 !important;
  background: #1E1E1E !important;
  padding: 12px 16px !important;
}
.code-window pre code {
  font-family: 'Consolas', 'D2Coding', 'Courier New', monospace !important;
  font-size: 0.78rem !important;
  line-height: 1.58 !important;
  background: transparent !important;
}
```

---

## 2. 구문 강조 색상 (Syntax Token Colors)

VS2022 Dark+ (C++ 기준) 토큰별 정확한 색상

| 토큰 | 예시 | 색상 코드 | 설명 |
|---|---|---|---|
| 기본 텍스트 | 일반 식별자, 연산자 | `#D4D4D4` | 회백색 |
| **키워드** | `void`, `int`, `auto`, `const`, `for`, `if`, `return`, `class`, `struct`, `nullptr`, `true`, `false` | `#569CD6` | 파랑 |
| **내장 타입** | `float`, `double`, `bool`, `UINT`, `DWORD` | `#569CD6` | 파랑 (키워드와 동일) |
| **숫자 리터럴** | `0`, `1`, `100`, `3.14f` | `#B5CEA8` | 연한 초록 |
| **문자열 리터럴** | `"Hello"`, `L"Wide"` | `#CE9178` | 주황-갈색 |
| **주석** | `// ...`, `/* ... */` | `#6A9955` + italic | 초록, 이탤릭 |
| **함수 이름** (정의/호출) | `Render(`, `DrawIndexedInstanced(` | `#DCDCAA` | 연한 노랑 |
| **클래스/타입 이름** | `Mesh`, `Material`, `ID3D12Resource` | `#4EC9B0` | 청록(teal) |
| **매개변수/지역 변수** | `command_list`, `instance_count` | `#9CDCFE` | 연한 하늘 |
| **전처리기 지시자** | `#include`, `#pragma`, `#define` | `#C586C0` | 보라 |
| **매크로/전처리기 인자** | `<d3d12.h>`, `_DEBUG` | `#CE9178` or `#9B9B9B` | 주황 or 회색 |
| **연산자** | `->`, `::`, `=`, `+`, `*` | `#D4D4D4` | 기본 텍스트 동일 |
| **구두점** | `{`, `}`, `(`, `)`, `;` | `#D4D4D4` | 기본 텍스트 동일 |
| **네임스페이스** | `std`, `d3d_util` | `#4EC9B0` | 청록 (타입과 동일) |
| **멤버 변수** (IntelliSense) | `positions_`, `indices_array_` | `#9CDCFE` | 연한 하늘 |
| **enum 값** | `D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST` | `#4FC1FF` | 연한 파랑 |
| **구조체 멤버** | `.BufferLocation`, `.SizeInBytes` | `#9CDCFE` | 연한 하늘 |

> **주의**: `#B5CEA8` (숫자)와 `#569CD6` (키워드)를 혼동하지 말 것.  
> `#4EC9B0` (클래스명)과 `#9CDCFE` (변수)를 혼동하지 말 것.

---

## 3. Highlight.js CSS 클래스 매핑 (전체)

Highlight.js가 자동으로 부여하는 클래스와 VS2022 색상의 매핑표

```css
/* ── 기반 ── */
.hljs                  { background: #1E1E1E !important; color: #D4D4D4 !important; }

/* ── 키워드 계열 → #569CD6 파랑 ── */
.hljs-keyword          { color: #569CD6 !important; }   /* void, int, for, return, const, auto ... */
.hljs-built_in         { color: #569CD6 !important; }   /* size_t, NULL 등 내장 */
.hljs-literal          { color: #569CD6 !important; }   /* true, false, nullptr */

/* ── 숫자 → #B5CEA8 연두 ── */
.hljs-number           { color: #B5CEA8 !important; }

/* ── 문자열 → #CE9178 주황 ── */
.hljs-string           { color: #CE9178 !important; }

/* ── 주석 → #6A9955 초록, 이탤릭 ── */
.hljs-comment          { color: #6A9955 !important; font-style: italic; }

/* ── 함수 이름 → #DCDCAA 노랑 ── */
.hljs-title.function_  { color: #DCDCAA !important; }

/* ── 클래스/타입 이름 → #4EC9B0 청록 ── */
.hljs-title.class_     { color: #4EC9B0 !important; }
.hljs-title.class_.inherited__ { color: #4EC9B0 !important; }
.hljs-type             { color: #4EC9B0 !important; }

/* ── 변수/매개변수 → #9CDCFE 하늘 ── */
.hljs-params           { color: #9CDCFE !important; }
.hljs-variable         { color: #9CDCFE !important; }
.hljs-variable.language_ { color: #569CD6 !important; }  /* this 등 언어 변수 */

/* ── 전처리기/메타 → #C586C0 보라 ── */
.hljs-meta             { color: #C586C0 !important; }    /* #include, #pragma, #define */
.hljs-meta .hljs-string { color: #CE9178 !important; }  /* #include 뒤의 경로 문자열 */
.hljs-meta .hljs-keyword { color: #C586C0 !important; } /* 전처리기 키워드 유지 */

/* ── 연산자/구두점 → #D4D4D4 기본 ── */
.hljs-operator         { color: #D4D4D4 !important; }
.hljs-punctuation      { color: #D4D4D4 !important; }

/* ── 속성/멤버 → #9CDCFE 하늘 ── */
.hljs-property         { color: #9CDCFE !important; }
.hljs-attr             { color: #9CDCFE !important; }

/* ── 기호/태그 ── */
.hljs-symbol           { color: #4FC1FF !important; }
.hljs-tag              { color: #569CD6 !important; }
```

---

## 4. 흔한 색상 오류 케이스

### `#include` 전처리기가 회색으로 나올 때

**원인**: `.hljs-meta { color: #9B9B9B }` — 이전 예제 파일의 잘못된 값  
**수정**: `#C586C0` (보라)으로 변경

```css
/* 잘못됨 */
.hljs-meta { color: #9B9B9B !important; }

/* 올바름 */
.hljs-meta { color: #C586C0 !important; }
```

### 함수 이름이 흰색/기본으로 나올 때

**원인**: Highlight.js가 함수명을 `.hljs-title.function_`이 아닌 다른 클래스로 분류  
**점검**: 브라우저 개발자 도구로 실제 적용된 클래스 확인

```css
/* 추가 시도 */
.hljs-title            { color: #DCDCAA !important; }  /* 일반 title 폴백 */
```

### 클래스명이 하늘색(변수색)으로 나올 때

**원인**: Highlight.js가 클래스명을 `.hljs-variable`로 분류  
**해결**: C++ 파일에서 `hljs.configure({ languages: ['cpp'] })`로 명시

```js
document.addEventListener('DOMContentLoaded', function() {
  hljs.configure({ languages: ['cpp'] });
  hljs.highlightAll();
});
```

### `auto` 키워드가 변수 색(하늘)으로 나올 때

**원인**: Highlight.js가 `auto`를 타입 추론 변수로 취급  
**현실**: 완벽 재현 불가. `#569CD6`와 `#9CDCFE`는 유사하므로 실용상 무시 가능

---

## 5. HTML에서 특수문자 이스케이프

`<pre><code>` 내부에 직접 코드를 작성할 때 반드시 이스케이프 필요

| 원본 | HTML 이스케이프 |
|---|---|
| `&` | `&amp;` |
| `<` | `&lt;` |
| `>` | `&gt;` |
| `"` | `&quot;` |

### 자주 발생하는 케이스 (C++)

```html
<!-- 잘못됨 -->
for (const auto& [mesh, components] : batches_)

<!-- 올바름 -->
for (const auto&amp; [mesh, components] : batches_)
```

```html
<!-- 잘못됨 -->
if (material_index < indices_array_.size())

<!-- 올바름 -->
if (material_index &lt; indices_array_.size())
```

> **주의**: `->` (포인터 멤버 접근)는 이스케이프 불필요. `<`와 `>`가 태그로 오인되는 경우만 이스케이프.

---

## 6. 폰트 설정

| 용도 | font-family | 비고 |
|---|---|---|
| 코드 (monospace) | `'Consolas', 'D2Coding', 'Courier New', monospace` | Consolas 우선, 한글 코드는 D2Coding |
| 창 제목/탭 | `'Segoe UI', 'Malgun Gothic', -apple-system, sans-serif` | VS와 동일 계열 |
| 슬라이드 UI | `-apple-system, BlinkMacSystemFont, 'Segoe UI', 'Malgun Gothic', sans-serif` | 시스템 폰트 |

**폰트 크기 권장값 (1280×720 슬라이드 기준)**

| 요소 | `font-size` | `line-height` |
|---|---|---|
| 코드 (메인) | `0.78rem` | `1.58` |
| 코드 (조밀할 때) | `0.72rem` | `1.52` |
| 창 탭 파일명 | `0.72rem` | — |
| 레이블/배지 | `0.65rem` | — |

---

## 7. 완성 CSS 템플릿 (복사 전용)

```css
/* VS2022 Dark+ — Code Window Chrome */
.code-window {
  background: #1E1E1E;
  border-radius: 4px;
  overflow: hidden;
  border: 1px solid #3C3C3C;
  box-shadow: 0 2px 12px rgba(0,0,0,.65);
}
.code-win-header {
  background: #2D2D2D;
  display: flex;
  align-items: stretch;
  border-bottom: 1px solid #252526;
}
.win-filename {
  background: #1E1E1E;
  border-top: 2px solid #007ACC;
  padding: 6px 14px;
  font-family: 'Segoe UI', 'Malgun Gothic', -apple-system, sans-serif;
  font-size: 0.72rem;
  color: #CCCCCC;
  flex: 1;
  display: flex;
  align-items: center;
}
.win-label {
  font-size: 0.6rem;
  text-transform: uppercase;
  letter-spacing: .08em;
  padding: 4px 12px;
  font-weight: 700;
  display: flex;
  align-items: center;
  border-left: 1px solid #3C3C3C;
}
.win-label.before { background: rgba(248,81,73,.12); color: #f85149; }
.win-label.after  { background: rgba(63,185,80,.12);  color: #3fb950; }
.code-window pre {
  margin: 0 !important;
  border-radius: 0 !important;
  background: #1E1E1E !important;
  padding: 12px 16px !important;
}
.code-window pre code {
  font-family: 'Consolas', 'D2Coding', 'Courier New', monospace !important;
  font-size: 0.78rem !important;
  line-height: 1.58 !important;
  background: transparent !important;
}

/* VS2022 Dark+ — Syntax Highlight (Highlight.js override) */
.hljs                      { background: #1E1E1E !important; color: #D4D4D4 !important; }
.hljs-keyword              { color: #569CD6 !important; }
.hljs-built_in             { color: #569CD6 !important; }
.hljs-literal              { color: #569CD6 !important; }
.hljs-number               { color: #B5CEA8 !important; }
.hljs-string               { color: #CE9178 !important; }
.hljs-comment              { color: #6A9955 !important; font-style: italic; }
.hljs-title                { color: #DCDCAA !important; }
.hljs-title.function_      { color: #DCDCAA !important; }
.hljs-title.class_         { color: #4EC9B0 !important; }
.hljs-title.class_.inherited__ { color: #4EC9B0 !important; }
.hljs-type                 { color: #4EC9B0 !important; }
.hljs-params               { color: #9CDCFE !important; }
.hljs-variable             { color: #9CDCFE !important; }
.hljs-variable.language_   { color: #569CD6 !important; }
.hljs-property             { color: #9CDCFE !important; }
.hljs-attr                 { color: #9CDCFE !important; }
.hljs-meta                 { color: #C586C0 !important; }         /* ← #include, #pragma */
.hljs-meta .hljs-string    { color: #CE9178 !important; }
.hljs-meta .hljs-keyword   { color: #C586C0 !important; }
.hljs-operator             { color: #D4D4D4 !important; }
.hljs-punctuation          { color: #D4D4D4 !important; }
.hljs-symbol               { color: #4FC1FF !important; }
```

---

## 8. CDN 로드 방법

```html
<head>
  <!-- vs2015 테마 (VS2022 Dark+와 가장 유사) -->
  <link rel="stylesheet"
        href="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/styles/vs2015.min.css">
  <script src="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/highlight.min.js"></script>
  <!-- 위 CSS override 규칙을 vs2015.min.css 뒤에 배치해야 적용됨 -->
  <style>
    /* VS2022 Dark+ override CSS 여기에 */
  </style>
</head>
<body>
  ...
  <script>
    document.addEventListener('DOMContentLoaded', function() {
      hljs.configure({ languages: ['cpp'] });  // C++ 전용으로 명시
      hljs.highlightAll();
    });
  </script>
</body>
```

> **순서 중요**: `vs2015.min.css` → 커스텀 `<style>` 순서여야 `!important` 없이도 오버라이드 가능.  
> `!important`를 사용하면 순서에 상관없이 적용되므로 안전.
