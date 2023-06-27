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

int main(int argc, char **argv)
{
    CommandHandler asd;
    std::string password;
    int port;
    int connectSocket;
    std::vector<struct kevent> changeList;
    int kq;
    if (argc != 3)
        ErrorPrintExit("Error: arguments ex) ./program port password");
    ArrayToPort(argv[1], port);
    ArrayToPass(argv[2], password);
    Init_socket(port, connectSocket);
    Init_event(changeList, kq, connectSocket);
    int invokedEvnets;
    struct kevent eventList[8];
    std::map<int, t_MandatoryClientInit> clients;
    User test;
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
                    AcceptUser(connectSocket, changeList, clients);
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
                        clients[currEvent->ident].Push_data(str);
                        str = clients[currEvent->ident].Get_command();
                        std::cout << "client " << currEvent->ident << " : "  << str;
                        if (clients[currEvent->ident].nickFlag && clients[currEvent->ident].userFlag && clients[currEvent->ident].passwordFlag)
                        {
                            try
                            {
                                if (!str.empty())
                                    CommandHandler::CommandRun(UserChannelController::Instance().FindUser(currEvent->ident), str);
                            }
                            catch (const char *str)
                            {
                                std::cerr << str << std::endl;
                            }
                        }
                        else
                        {
                            if (!str.empty())
                            {
                                AuthenticateUserAccess(currEvent->ident, clients, password, str);
                            }
                        }
                    }
                }
                std::cout << std::flush;
            }
            else if (currEvent->filter == EVFILT_WRITE)
            {
                std::cout << "EVFILT_WRITE" << std::endl;
            }
        }
    }
}