# LiarGame

Qt 6로 구현한 멀티플레이 라이어 게임입니다. TCP 서버가 회원과 방 상태를 MySQL에 저장하고 게임 진행을 관리하며, 데스크톱 클라이언트는 로그인·로비·채팅·게임·투표 화면을 제공합니다.

## 주요 기능

- 회원가입, ID·이메일 중복 확인, 로그인/로그아웃
- 이메일을 이용한 ID 찾기, 비밀번호 및 닉네임 변경
- 방 생성·삭제·입장·퇴장과 참가자 목록 갱신
- 로비 일반 채팅과 순서 기반 게임 힌트 채팅
- 17개 카테고리의 내장 단어 목록에서 문제 무작위 선택
- 라이어 무작위 지정, 라이어 최종 답변, 참가자 투표
- 무승부 시 다음 라운드 진행 및 승패 판정
- Qt Resource System을 이용한 배경 이미지 포함

## 구성

```text
LiarGame/
├─ LiarGameClient/
│  ├─ client_mainwindow.cpp/.h/.ui  # 클라이언트 UI와 요청·응답 처리
│  ├─ selectCategoryDialog.*        # 카테고리 선택과 투표 다이얼로그
│  ├─ images/                       # 배경 이미지
│  ├─ images.qrc, wallpaper.qrc     # Qt 리소스
│  └─ CMakeLists.txt
└─ LiarGameServer/
   ├─ server_mainwindow.cpp/.h/.ui  # TCP/DB 서버와 게임 상태 머신
   ├─ game_data.h                   # 카테고리·단어 제공 클래스
   ├─ db_information.h              # MySQL 접속 정보
   └─ main.cpp
```

서버 폴더에는 현재 `CMakeLists.txt`가 포함되어 있지 않습니다. 서버는 Qt Creator에서 프로젝트를 구성하거나 아래의 요구 모듈을 포함하는 CMake 파일을 추가해야 빌드할 수 있습니다.

## 아키텍처와 프로토콜

```text
Qt Client ── TCP :9806 ── Qt Server ── QMYSQL ── MySQL(liargame_db)
```

클라이언트와 서버는 JSON 객체를 교환합니다. 각 패킷은 Qt `QDataStream` 버전 6.0으로 직렬화하며, `quint32` 블록 크기 다음에 JSON `QByteArray`가 옵니다. JSON의 `type` 필드가 `login`, `create_room`, `start_game`, `hint_message`, `vote` 같은 요청과 그 응답을 구분합니다.

게임 상태는 `game_states.state_data` JSON에 현재 라운드, 차례, 라이어, 일반 단어와 라이어 단어 등을 저장합니다. 접속 소켓과 사용자·방의 실시간 매핑은 서버 메모리에서도 함께 관리합니다.

## 요구 사항

- C++17 이상 권장
- CMake 3.19 이상
- Qt 6: Core, Widgets, Network, Sql
- MySQL/MariaDB
- Qt MySQL 드라이버(`QMYSQL`)

## 설정

### 서버 DB

`LiarGameServer/db_information.h`에서 `DB_HOST_NAME`, `DB_DATABASE_NAME`, `DB_USER_NAME`, `DB_PASSWORD`를 실제 DB에 맞게 변경합니다. 현재 비밀번호가 소스에 하드코딩되어 있으므로 공유·배포 전에 외부 설정으로 분리하는 것이 안전합니다.

서버 코드가 사용하는 테이블은 다음과 같습니다.

| 테이블 | 용도와 주요 컬럼 |
|---|---|
| `users` | `user_id`, `password`, `nickname`, `email` |
| `rooms` | `room_id`, `room_name`, `creator_id`, `status` |
| `room_members` | `room_id`, `user_id`, `join_time` |
| `game_states` | `room_id`, `current_turn_user_id`, `state_data` |

저장소에 스키마 SQL은 포함되어 있지 않으므로 위 컬럼과 서버 쿼리를 기준으로 테이블·PK·FK를 준비해야 합니다. 비밀번호는 서버에서 SHA-256 해시 문자열로 저장·비교합니다.

### 클라이언트 접속 주소

`LiarGameClient/client_mainwindow.cpp` 생성자의 `serverIp`와 `serverPort`를 서버 환경에 맞게 변경합니다. 현재 값은 개발망 IP `10.10.21.122`, 포트 `9806`입니다. 서버는 모든 인터페이스의 `9806` 포트에서 수신합니다.

## 빌드 및 실행

클라이언트:

```bash
cmake -S LiarGameClient -B build/client
cmake --build build/client --config Release
```

서버 프로젝트에는 빌드 파일이 없으므로 Qt Creator에서 Qt Widgets 프로젝트로 열어 `main.cpp`, `server_mainwindow.cpp/.h/.ui`, `game_data.h`, `db_information.h`를 포함하고 `Core`, `Widgets`, `Network`, `Sql` 모듈을 링크합니다.

실행 순서는 다음과 같습니다.

1. MySQL과 `liargame_db` 스키마를 준비합니다.
2. 서버를 실행하고 UI에서 **서버 시작**을 눌러 DB 연결과 포트 수신을 시작합니다.
3. 클라이언트를 한 개 이상 실행하여 회원가입·로그인합니다.
4. 방을 만들고 참가자를 모은 뒤 방장이 카테고리를 선택해 게임을 시작합니다.

## 게임 진행

1. 서버가 카테고리에서 서로 다른 두 단어를 고르고 참가자 중 라이어를 정합니다.
2. 시민에게 일반 단어를, 라이어에게 역할 정보를 개별 전송합니다.
3. 정해진 순서대로 참가자가 힌트를 입력합니다.
4. 라이어의 답변 또는 참가자 투표 단계로 전환합니다.
5. 투표 결과와 라이어의 정답 여부로 승자를 결정합니다. 무승부이면 다음 라운드를 시작합니다.

## 참고 및 제한 사항

- 클라이언트와 서버의 `QDataStream::Qt_6_0` 및 패킷 프레이밍은 반드시 같아야 합니다.
- 서버 메모리 상태와 DB 상태를 함께 사용하므로 비정상 종료 뒤 남은 방·참가자 레코드를 정리할 운영 절차가 필요합니다.
- 이메일 인증 메일 발송은 없으며 입력된 이메일은 계정 조회용 식별자로 사용됩니다.
- 현재 코드와 리소스에는 개발 환경별 Qt Creator 사용자 설정 파일이 포함되어 있습니다.
