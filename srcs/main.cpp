#include "UserChannelController.hpp"
#include "KqueueEventManager.hpp"
int main(int argc, char **argv)
{
    std::string password;
    int port;

    if (argc != 3)
        exit(1);

    password = argv[1];
    port = atoi(argv[2]);

    int connectSocket;
    struct sockaddr_in server_addr;
    // 접속요청을 수락할 소켓을 만든다. (ipv4, TCP, 기본프로토콜)
    if (connectSocket = socket(PF_INET, SOCK_STREAM, 0) == -1)
        exit(1);
    memset(&connectSocket, 0, sizeof(connectSocket));
    server_addr.sin_family = AF_INET; // ipv4로 설정
    // ip주소를 설정함 htonl = 호스트 바이트 순서로 네트워크를 변환하는 함수
    // INADDR_ANY는 0.0.0.0으로 모든 주소를 의미, 어떤 네트워크 인터페이스에서도 클라이언트 연결을 수락할 수 있음을 설정
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(6667);
}