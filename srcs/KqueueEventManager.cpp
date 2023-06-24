#include "KqueueEventManager.hpp"

void ErrorPrintExit(std::string str);

void	Init_socket(int port, int &connectSocket)
{
    struct sockaddr_in server_addr;
    // 접속요청을 수락할 소켓을 만든다. (ipv4, TCP, 기본프로토콜)
    connectSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (connectSocket == -1)
        ErrorPrintExit("Error: not create socket");
    int reuse = 1;
    setsockopt(connectSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // ipv4로 설정

    // ip주소를 설정함 htonl = 호스트 바이트 순서로 네트워크를 변환하는 함수
    // INADDR_ANY는 0.0.0.0으로 모든 주소를 의미, 어떤 네트워크 인터페이스에서도 클라이언트 연결을 수락할 수 있음을 설정
    // htonl, htons는 해당 프로토콜에 맞게 변환해주는 함수이다. 
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port); // port로 대체

    // server_addr을 처음부터 sockaddr로 선언하고 초기화하지 않는 이유는
    // sockaddr은 ipv4 외에도 다양한 네트워크를 담을 수 있게 만들어져있어
    // sockaddr_in(ipv4 네트워크 특화 struct)보다 설정하기가 까다롭다.
    // 그렇기 때문에 설정하기 편한 sockaddr_in으로 설정하고 bind에 넘겨줄때 형변환을 한다.
    // sockaddr 과 sockaddr_in은 동일한 크기, 메모리 레이아웃이 호환돼서 형변환을 해도 알맞게 변환이 이루어진다.
    // 메모리 레이아웃이 호환된다는 의미는 메모리 저장방식이 같다는 의미로, 저장방식과 크기가 같기 때문에 강제 형변환이 이루어져도 정상적으로 변환이 된다.
    if (bind(connectSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        ErrorPrintExit("Error: not binding");
    
    if (listen(connectSocket, 20) == -1)// 20에 해당하는 부분은 연결 대기의 최대수이다.
        ErrorPrintExit("Error: not listening");

    // fcntl은 fd의 속성을 제어하는 함수이다.
    // 1번째 매개변수는 fd
    // 2번쨰 매개변수는 어떤 일을 하는지 지정하는데, fd를 복사하거나 상태를 set,get 하거나 플래그를 set,get하는 일중 어떤 걸 할지 정한다.
    // 3번째 매개변수는 2번째에서 정한 작업에 설정할 플래그이다.
    // 이 코드는 클라이언트가 접속을 요청할 소켓의 플래그를 변경할건데 NONBLOCK으로 변경한다는 의미이다.
    fcntl(connectSocket, F_SETFL, O_NONBLOCK);
}

void	Init_event(std::vector<struct kevent> &changeList, int &kq, int &connectSocket)
{
	if ((kq = kqueue()) == -1) // kqueue를 커널에 생성한다.
        ErrorPrintExit("Error: not create Kqueue");

    //클라이언트를 map으로 저장한다
    // key : 클라이언트의 소켓, val : 클라이언트가 보낸 문자열
    //kevent에 등록할 이벤트들
    
    // kevent에서 반환된(이벤트가 발생한) event들을 저장할 리스트 최대갯수를 8로 지정
    struct kevent temp_event;

    // EV_SET는 kevent를 한번에 초기화하는 함수이다.
    // kevent, ident, filter, flags, fflags, data, udata
    // kevent는 초기화할 kevent를, ident는 이벤트가 발생할 소켓을
    // filter는 어떤 이벤트가 발생했을때 알릴지를(READ, WRITE, VNODE)
    // flags는 이벤트를 어떤 형태로 등록할지 (ADD, DELETE)
    // fflags는 filter에 대한 추가적인 플래그를 지정
    // data는 filter에 대한 추가적인 데이터(read, write일 경우 읽거나 쓸때의 최대 바이트 수, timer의 경우 몇 번만에 만료되는지 등)
    // udata는 사용자 정의 데이터를 저장할때 사용. 말 그대로 추가적으로 해당 이벤트에 대한 정보를 사용자가 직접 저장, 해당 이벤트가 발생했을때 읽을 수 있게 void*형으로 어떤 데이터도 허용가능함
    // connectSocket에 read할 수 있을때 알림이 오게 되는 struct kevent를 kqueue에 add,enable로 등록한다.
    // 추가적인 플래그들은 read 할때는 필요 없기 때문에 설정하지 않고 따로 설명도 필요없기 때문에 udata도 NULL로 설정한다.
    EV_SET(&temp_event, connectSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    changeList.push_back(temp_event);
}

/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */