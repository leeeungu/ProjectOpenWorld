# 🌏 ProjectOpenWorld

> Unreal Engine 5 / C++ 로 제작한 **1인 개발 오픈월드 생존·제작 게임**.
> 쿼드트리 기반 무한 월드 스트리밍, 데이터 드리븐 아이템 시스템, 동료 몬스터(Pal) 커맨드 시스템, GPT API 연동까지 직접 설계·구현했습니다.

<p>
  <img src="https://img.shields.io/badge/Unreal%20Engine-5.4-0E1128?style=flat-square&logo=unrealengine&logoColor=white">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat-square&logo=cplusplus&logoColor=white">
  <img src="https://img.shields.io/badge/Type-Solo%20Project-2DD4BF?style=flat-square">
  <img src="https://img.shields.io/badge/Genre-Open%20World%20Survival-555?style=flat-square">
</p>

---

## 📌 Introduction

정부지원 게임 개발 국비과정(SBS) 수료 후, **"화려한 기능보다 시스템 안정성이 게임의 재미를 지속시킨다"** 는 개발 철학을 검증하기 위해 시작한 개인 프로젝트입니다.

빌딩·크래프팅 같은 단일 기능 데모에 머무르지 않고, **확장 가능한 데이터 드리븐 아키텍처** 위에서 무한 월드, 자원 채집, 동료 몬스터 작업 분배, 전투까지 하나의 흐름으로 동작하도록 설계하는 것을 목표로 했습니다.

- **엔진 / 언어**: Unreal Engine 5.4, C++ (게임 로직 전반을 C++ 중심으로 구현)
- **개발 형태**: 1인 개발 (기획 · 시스템 설계 · 구현)
- **장르**: 오픈월드 생존 · 제작 · 동료 육성

---

## ✨ Core Systems

### 🗺️ 무한 오픈월드 생성 & 스트리밍
플레이어 위치를 기준으로 월드를 섹션 단위로 동적 생성·해제하여, 메모리 한계 없이 무한히 확장되는 월드를 구현했습니다.
- `UQuadtreeWorldSubsystem` — 쿼드트리로 플레이어 주변 섹션만 활성화 (`SectionRadius` 기반)
- `AWorldGenerator` + `Generate*Component` — 지형 / 식생 / 풀 / 몬스터 / 스포너 / 정적 오브젝트를 컴포넌트 단위로 분리 생성
- 섹션 기반 분할로 **월드 갱신 처리 시간을 약 1/10 수준으로 단축**

### 🎒 데이터 드리븐 아이템 시스템 (Fragment 패턴)
아이템을 상속 트리가 아닌 **조합 가능한 Fragment 집합**으로 정의하여, 새 아이템 추가 시 데이터만 수정하면 되도록 설계했습니다.
- `Item/Object/Fragment` — `ItemDataSlotFragment`, `ItemVisibleDataFragment`, `StatusFragment`, `PlayerAnimationDataFragment` 등
- `UItemDataSubsystem` + DataAsset / DataTable 기반의 단일 데이터 소스
- **Composition over Inheritance** 원칙 적용으로 아이템 종류 확장이 자유로움

### 🐾 동료 몬스터(Pal) 커맨드 시스템
Palworld류의 동료 몬스터가 채집·운반·건설·전투 등의 작업을 자율 수행하도록, **Command 패턴** 으로 행동을 모듈화했습니다.
- `PalCommandExecutorBase` ← `Attack` / `Mining` / `Transport` / `Architecture` / `MoveActor` / `DetectPlayer` 등 9종 Executor
- `PalWorkComponent` 계열 + Behavior Tree Task 로 작업 상태 관리
- `PalGroupBanner`(베이스캠프)를 중심으로 작업을 탐색·분배하는 구조

### 🏗️ 빌딩 & 자원 시스템
- `UBuildingDataSubsystem` / `UBuildingWidgetSubsystem` — 건축물 데이터와 배치 UI 관리
- `Resource` / `Item` 모듈 — 자원 채집 → 인벤토리 → 제작으로 이어지는 파이프라인

### 🎒 인벤토리 & 상호작용
- `UInventoryComponent` + `InventorySlotInterface` — 인터페이스 기반 슬롯 추상화
- `Interaction/Component` — 월드 오브젝트와의 상호작용 처리

### ⚔️ 플레이어 전투 & 스탯
- `UStatComponent` / `StatComponent_Level` — 체력·레벨 등 스탯 관리
- `PlayerAttackComponent` + `PlayerDetectCollision` — 공격 판정 및 데미지 파이프라인
- `PlayerStatusWidget` / `PlayerInfoWidget` — HP 등 상태 UI

### 🤖 GPT API 연동
- `UGPTInstanceSubsystem` (VaRest 기반) + `GPTResponseInterface` — 텍스트 / 이미지 요청 처리
- 콘텐츠 데이터 생성 자동화에 활용

### 💾 세이브 / 로드
- `UPalSaveSubsystem` 등 서브시스템 기반 게임 상태 직렬화

---

## 🧱 Architecture & Design Principles

| 원칙 | 적용 사례 |
|------|-----------|
| **Composition over Inheritance** | 아이템 Fragment 시스템, 플레이어 기능별 Component 분리 |
| **Command Pattern** | Pal 행동 Executor 모듈화 |
| **Dependency Inversion (Interface)** | `InventorySlotInterface`, `GPTResponseInterface` 등 인터페이스 기반 의존성 분리 |
| **Subsystem 중심 설계** | 월드 / 아이템 / 빌딩 / 세이브를 각 Subsystem 으로 책임 분리 |
| **Data-Driven** | DataAsset · DataTable 로 콘텐츠와 로직 분리 |

---

## 📂 Project Structure

```text
📦 ProjectOpenWorld
 ┣ 📂 Source/ProjectOpenWorld
 ┃ ┣ 📂 GameBase        # 핵심 프레임워크 (QuadTree, Subsystem, Component, GameMode)
 ┃ ┣ 📂 Player          # 플레이어 캐릭터 · 컴포넌트 · 컨트롤러 · UI
 ┃ ┣ 📂 Pal             # 동료 몬스터 (CommandExecutor, Work, Factory, AI)
 ┃ ┣ 📂 Item            # 데이터 드리븐 아이템 (Fragment, System, DataAsset)
 ┃ ┣ 📂 Inventory       # 인벤토리 컴포넌트 · 인터페이스 · 위젯
 ┃ ┣ 📂 Building        # 건축 시스템 (Subsystem, Actor, Widget)
 ┃ ┣ 📂 Landscape       # 월드 생성 (WorldGenerator, Generate* Component)
 ┃ ┣ 📂 Interaction     # 월드 상호작용
 ┃ ┣ 📂 Resource        # 자원 채집
 ┃ ┗ 📂 SubSystem/GPT   # GPT API 연동 (VaRest)
 ┣ 📂 Content           # 게임 에셋
 ┣ 📂 Config            # 설정 파일
 ┣ 📂 Plugins           # NiagaraTextToolkit 외
 ┗ 📜 ProjectOpenWorld.uproject
```

---

## 🛠️ Tech Stack & Plugins

- **Engine**: Unreal Engine 5.4
- **Language**: C++ (게임 로직), Blueprint (프로토타이핑)
- **Modules**: UMG, AIModule, Foliage, DeveloperSettings, PCG
- **Plugins**: VaRest (GPT API 통신), Chaos Cloth, Hair Strands, NiagaraTextToolkit

---

## 🚀 Build & Run

```text
1. Unreal Engine 5.4 설치
2. ProjectOpenWorld.uproject 우클릭 → "Generate Visual Studio project files"
3. Visual Studio / Rider 에서 빌드
4. 에디터 실행 후 Play
```

> GPT 연동 기능 사용 시 `GPTSettings` (Project Settings) 에 API 키 설정이 필요합니다.

---

## 📑 Commit Message Convention

| Type | Description |
|------|------|
| feature | 새로운 기능 추가 |
| improve | 기존 기능 개선 |
| fix | 버그 수정 |
| docs | 문서 수정 |
| style | 코드 스타일 변경 (기능 변경 없음) |
| refactor | 코드 리팩토링 |
| test | 테스트 코드 추가/수정 |
| chore | 빌드 · 설정 등 기타 작업 |

```bash
git commit -m "feature: Pal 운반 커맨드 Executor 추가"
```

---

## 📞 Contact

🧑‍💻 **이은구** — UE5 / C++ 게임 클라이언트 개발자 지망
- 📧 skysea6055@gmail.com
- 🐙 [github.com/leeeungu](https://github.com/leeeungu)
