#include "UserChannelController.hpp"
#include "KqueueEventManager.hpp"
#include "CommandHandler.hpp"
#include <sstream>

void ErrorPrintExit(std::string str)
{
    std::cout << str << std::endl;
    exit(true);
}

void ArrayToPort(char *str, int &port)
{
    std::stringstream ss;

    ss.str(str);
    ss >> port;    
    if (!ss.eof() || ss.fail() || port < 0 || port > 65535)
        ErrorPrintExit("Error: port is wrong");
}

void  ArrayToPass(char *str, std::string &password)
{
    std::stringstream ss;

    ss.str(str);
    ss >> password;
    if (!ss.eof() || ss.fail())
        ErrorPrintExit("Error: password is wrong");
}

struct s_MandatoryClientInit
{
    bool userFlag;
    bool nickFlag;
    bool passwordFlag;
    
    std::string username;
    std::string nickname;
    std::string hostname;
    std::string realname;
    std::string data;
    
	void    Push_data(std::string str)
    {
        data = data + str;
    }
    std::string Get_command()
    {
        std::size_t len = data.find("\r\n");
		int size = data.size();
        if (len == std::string::npos)
			return (NULL);
		std::string tmp;
		tmp.resize(len);
		
        for(int i = 0; i < len; i++)
            tmp[i] = data[i];
		for(int i = 0; i < size; i++)
			data[i] = data[i + len + 2];
		data.resize(size - len - 2);
		return (tmp);
    }
    s_MandatoryClientInit() : userFlag(0), nickFlag(0), passwordFlag(0), username(""), nickname(""), hostname(""), realname(""), data("") {}
} typedef t_MandatoryClientInit;

int main(int argc, char **argv)
{
    std::string password;
    int port;

    if (argc != 3)
        ErrorPrintExit("Error: arguments ex) ./program port password");

    ArrayToPort(argv[1], port);
    ArrayToPass(argv[2], password);
    int connectSocket;
    struct sockaddr_in server_addr;
    // 접속요청을 수락할 소켓을 만든다. (ipv4, TCP, 기본프로토콜)
    connectSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (connectSocket == -1)
        ErrorPrintExit("Error: not create socket");
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

    int kq;
    if ((kq = kqueue()) == -1) // kqueue를 커널에 생성한다.
        ErrorPrintExit("Error: not create Kqueue");


    //클라이언트를 map으로 저장한다
    // key : 클라이언트의 소켓, val : 클라이언트가 보낸 문자열
    std::map<int, t_MandatoryClientInit> clients;

    //kevent에 등록할 이벤트들
    std::vector<struct kevent> changeList;
    
    // kevent에서 반환된(이벤트가 발생한) event들을 저장할 리스트 최대갯수를 8로 지정
    struct kevent eventList[8];
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

    int invokedEvnets;
    while (1)
    {
        // kevent 함수는 kevent를 등록과 반환을 한번에 하는 함수이다.
        // 1번째 매개변수는 이벤트들을 등록, 이벤트의 발생을 알릴 kqueue를 지정한다.
        // 2번째 매개변수는 등록할 event배열의 첫번째 주소를 넘겨준다.
        // 3번째 매개변수는 등록할 event들의 갯수를 넘겨준다.
        // 4번째 매개변수는 첫번째 kqueue에서 발생한 이벤트들을 담을 kevent배열의 주소를 넘겨준다.
        // 5번째 매개변수는 발생한 이벤트들을 반환할때의 최대수를 지정한다.
        // 6번째 매개변수는 해당 함수에서 이벤트가 발생할때까지 기다리는 timeout을 지정한다.
        // 반환값은 반환받은 이벤트의 갯수이다. 5번째의 최대갯수랑은 차이가 있다.
        invokedEvnets = kevent(kq, &changeList[0], changeList.size(), eventList, 8, NULL);
        if (invokedEvnets < 0)
            ErrorPrintExit("Error: Kevent");

        // 위의 kevent에서 등록한 이벤트들은 지운다. 지워도 되는 이유는 kevent안에 이미 저장되어있다.
        changeList.clear();

        // 포인터로 하는 이유는 객체의 복사에대한 자원보다 포인터의 복사가 자원적으로 더 최적화기 때문
        struct kevent* currEvent;
        for (int i = 0; i < invokedEvnets; i++)
        {
            currEvent = &eventList[i];
            // flags가 비트연산으로 처리된다. 헤더에서 플래그들을 확인해보자, EV_ERROR가 켜져있으면 에러처리
            if (currEvent->flags & EV_ERROR)
            {
                // 외부에서 접속을 받는 소켓이 에러났을때는 중대한 사항이라고 판단
                if (static_cast<int>(currEvent->ident) == connectSocket)
                    ErrorPrintExit("Error: Server socket");
                else
                {
                    std::cout << "client disconnected: " << currEvent->ident << std::endl;
                    close(currEvent->ident);
                    // 클라이언트와 연결이 끊겼을때의 처리
                }
            }
            else if (currEvent->filter == EVFILT_READ)
            {
                if (static_cast<int>(currEvent->ident) == connectSocket)
                {
                    int clientSocket;
                    struct sockaddr clientAddr;
                    socklen_t clientAddrLen;
                    // accept는 연결 요청을 보낸 클라이언트를 수락하는 함수이다.
                    // 첫번째 매개변수는 연결 요청을 기다리는 소켓
                    // 두번째 매개변수는 요청한 클라이언트의 주소 정보를 담아준다.
                    // 세번째 매개변수는 addr 변수의 크기를 담아준다.
                    // 반환값은 성공시 소켓을 생성하고, 해당 소켓의 fd를 반환한다.
                    if ((clientSocket = accept(connectSocket, &clientAddr, &clientAddrLen)) == -1)
                    {
                        std::cout << clientAddr.sa_data << " is fail to accept" << std::endl;
                    }
                    else
                    {
                        //connectSocket랑 똑같이 클라이언트와 통신한 소켓도 논블록킹으로 설정해준다.
                        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                        // 클라이언트소켓또한 kevent에 등록하기 위해 changeList에 담아준다. 다음 순회때 kevent에 등록될것이다.
                        struct kevent temp_event;
                        EV_SET(&temp_event, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                        changeList.push_back(temp_event);
                        clients.insert(std::pair<int, t_MandatoryClientInit>(clientSocket, t_MandatoryClientInit()));
                    }
                }
                else
                {
                    std::string str;
                    str.resize(currEvent->data); // string의 주소에 직접쓰기 때문에 공간을 미리 할당해줘야한다.
                    // read는 주소를 write할 주소를 가져가기 때문에 &str로 가져가면 string의 첫번째가아닌 string 포인터자체에 write하기 때문에
                    // string에서 실질적으로 문자열을 담는 str[0]의 주소값을 넘겨줘야 string에서 문자열로 인식할 수 있다.
                    if (read(currEvent->ident, &str[0], currEvent->data) <= 0)
                    {
                        std::cout << "client disconnected: " << currEvent->ident << std::endl;
                        close(currEvent->ident);
                    }
                    else
                    {
                        std::cout << "client " << currEvent->ident << " : "  << str;
                        if (clients[currEvent->ident].nickFlag && clients[currEvent->ident].userFlag && clients[currEvent->ident].userFlag)
                        {
                            // 메시지처리 진행
                        }
                        else
                        {
                            std::stringstream ss(str);
                            std::string tmp;
                            ss >> tmp;
                            if (!tmp.compare("NICK"))
                            {
                                ss >> tmp;
                                if (ss.fail() || !ss.eof())
                                    std::cout << "check commad\n";
                                else if (!CommandHandler::NICK(tmp))
                                {
                                    clients[currEvent->ident].nickFlag = true;
                                    clients[currEvent->ident].nickname = tmp;
                                }
                                else
                                    write(currEvent->ident, "nickname is already", strlen("nickname is already"));
                            }
                            else if (!tmp.compare("USER"))
                            {
                                ss >> tmp;
                                if (ss.fail() || !ss.eof())
                                    std::cout << "check commad\n";
                                else if (!CommandHandler::NICK(tmp))
                                {
                                    clients[currEvent->ident].nickFlag = true;
                                    clients[currEvent->ident].nickname = tmp;
                                }
                                else
                                    write(currEvent->ident, "nickname is already", strlen("nickname is already"));
                            }
                            else if (!tmp.compare("PASS"))
                            {
                                ss >> tmp;
                                if (ss.fail() || !ss.eof())
                                    std::cout << "check commad\n";
                                else if (tmp == password)
                                    clients[currEvent->ident].passwordFlag = true;
                                else
                                    write(currEvent->ident, "Password incorrect", strlen("Password incorrect"));
                            }
                            // 기본적인 id를 지정할 정보를 주지않은 상태임으로 NICK, USER, PASS 이외의 어떤 명령어도 처리하지 않아야된다.
                            // 
                        }
                    }
                    std::cout << std::flush;
                }
            }
            else if (currEvent->filter == EVFILT_WRITE)
            {
                std::cout << "EVFILT_WRITE" << std::endl;
            }
        }
    }
}