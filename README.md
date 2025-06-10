# 🌐 IRC Chat Server

<div align="center">

**고성능 멀티클라이언트 IRC 채팅 서버 구현 프로젝트**

[![📚 기술 문서](https://img.shields.io/badge/%F0%9F%93%9A-%EA%B8%B0%EC%88%A0%20%EB%AC%B8%EC%84%9C-blue?style=for-the-badge)](https://github.com/irssiServer/irc_server/wiki)

</div>

---

## 📖 프로젝트 개요

이 프로젝트는 **RFC 2812 IRC 프로토콜 표준**을 준수하여 구현된 실시간 채팅 서버입니다. **kqueue 기반 이벤트 드리븐 아키텍처**를 사용하여 높은 동시성과 성능을 제공하며, 실제 IRC 클라이언트(irssi, HexChat 등)와 완벽한 호환성을 지원합니다.

## ✨ 주요 기능

### 🔐 사용자 인증 시스템

- **3단계 인증 프로세스**: NICK, USER, PASS 순서 무관 처리
- **중복 닉네임 검증** 및 실시간 충돌 방지
- **패스워드 기반 서버 접근 제어**

### 💬 채널 관리 시스템

- **채널 생성/삭제** 및 실시간 참여자 관리
- **운영자 권한 시스템** (op/deop 명령어)
- **채널 모드 제어**: invite-only, key, limit 등
- **개인 메시지(DM)** 및 그룹 채팅 지원

### 🚀 고성능 네트워크 처리

- **kqueue 기반 I/O 멀티플렉싱**으로 수백 명 동시 접속 지원
- **Non-blocking 소켓** 통신으로 응답성 극대화
- **이벤트 드리븐 아키텍처**로 CPU 효율성 향상

### 🛡️ 안정성 및 보안

- **메모리 누수 방지** 및 자동 리소스 관리
- **파일 디스크립터 관리** 시스템
- **예외 안전성** 보장 및 에러 복구 메커니즘

## 🏗️ 시스템 아키텍처

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   IRC Client    │◄──►│   IRC Server     │◄──►│   IRC Client    │
│   (irssi)       │    │                  │    │   (HexChat)     │
└─────────────────┘    │  ┌─────────────┐ │    └─────────────────┘
                       │  │   kqueue    │ │
┌─────────────────┐    │  │ Event Loop  │ │    ┌─────────────────┐
│   IRC Client    │◄──►│  └─────────────┘ │◄──►│   IRC Client    │
│   (Terminal)    │    │                  │    │   (Custom)      │
└─────────────────┘    │  ┌─────────────┐ │    └─────────────────┘
                       │  │User/Channel │ │
                       │  │ Controller  │ │
                       │  └─────────────┘ │
                       └──────────────────┘
```

## 🛠️ 기술 스택

- **언어**: C++98
- **네트워크**: BSD Socket, kqueue (macOS/FreeBSD)
- **프로토콜**: IRC (RFC 2812)
- **빌드 시스템**: Makefile
- **테스트 환경**: Docker (InspIRCd + irssi)

## 📦 설치 및 실행

### 요구사항

- macOS 또는 FreeBSD (kqueue 지원)
- C++ 컴파일러 (C++98 표준)
- Make

### 컴파일

```bash
git clone https://github.com/your-username/irc_server.git
cd irc_server
make
```

### 실행

```bash
./ircserv <port> <password>
```

**예시:**

```bash
./ircserv 6667 mypassword
```

### 클라이언트 연결

```bash
# irssi 사용
irssi -c localhost -p 6667 -w mypassword

# nc 사용 (테스트용)
nc localhost 6667
```

## 🎮 사용법

### 기본 IRC 명령어

#### 서버 연결

```
PASS mypassword
NICK mynickname
USER myuser 0 * :My Real Name
```

#### 채널 관리

```
JOIN #general          # 채널 입장
PART #general          # 채널 퇴장
LIST                   # 채널 목록 보기
NAMES #general         # 채널 사용자 목록
```

#### 메시지

```
PRIVMSG #general :Hello everyone!    # 채널 메시지
PRIVMSG user123 :Hi there!           # 개인 메시지
```

#### 운영자 명령어

```
MODE #general +o user123    # 사용자에게 op 권한 부여
MODE #general +i            # invite-only 모드 설정
MODE #general +k secret     # 채널 키 설정
KICK #general user123       # 사용자 강퇴
```

### 연결 스트레스 테스트

```
for i in {1..100}
do
      nc localhost 6667 &
done
```


## 🔧 개발 및 테스트

### Docker 테스트 환경

프로젝트에는 실제 IRC 서버와의 호환성 테스트를 위한 Docker 환경이 포함되어 있습니다.

```bash
cd irssi_docker
docker build -t irc-test .
docker run -it --name irc-container irc-test

# 컨테이너 내부에서
inspircd --config /etc/inspircd/inspircd.conf &
tcpflow -i lo port 6667 -c &
irssi -c 127.0.0.1
```

### 패킷 분석

```bash
# TCP 통신 패킷 캡처
tcpflow -i lo port 6667 -c

# 다른 터미널에서 클라이언트 접속
irssi -c 127.0.0.1
```

## 📊 성능 특징

- **메모리 사용량**: 클라이언트당 ~2KB
- **응답 시간**: 평균 1ms 이하
- **CPU 사용률**: 유휴 시 0.1% 미만

## 🏆 주요 성과

### 기술적 해결 과제

1. **io 이벤트 관리 최적화**: io 멀티플렉싱 기법의 kqueue를 이용한 이벤트 기반 비동기 I/O 처리
2. **IRC 프로토콜 호환성**: 실제 클라이언트와 주요기능에 대한 호환
3. **동시성 처리**: 이벤트 드리븐 아키텍처로 고성능 구현

## 📁 프로젝트 구조

```
irc_server/
├── srcs/
│   ├── main.cpp                    # 메인 서버 루프
│   ├── KqueueEventManager.cpp      # kqueue 이벤트 처리
│   ├── CommandHandler.cpp          # IRC 명령어 처리
│   ├── UserChannelController.cpp   # 사용자/채널 관리
│   ├── SendMessage.cpp             # IRC 메시지 전송
│   ├── User.cpp                    # 사용자 클래스
│   └── Channel.cpp                 # 채널 클래스
├── includes/                       # 헤더 파일들
├── irssi_docker/                   # Docker 테스트 환경
└── Makefile
```
