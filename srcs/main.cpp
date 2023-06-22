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
    
    std::string nickname;
    std::string username;
    std::string hostname;
    std::string realname;
    std::string data;
    
	void    Push_data(std::string str)
    {
        data = data + str;
    }
    std::string Get_command()
    {
        std::size_t len = data.find("\n");
		std::size_t size = data.size();
        if (len == std::string::npos)
			return ("");
		std::string tmp;
		tmp.resize(len);
		
        for(std::size_t i = 0; i < len; i++)
            tmp[i] = data[i];
		for(std::size_t i = 0; i < size; i++)
			data[i] = data[i + len + 1];
		data.resize(size - len - 1);
		return (tmp);
    }



    s_MandatoryClientInit() : userFlag(0), nickFlag(0), passwordFlag(0),  nickname(""), username(""), hostname(""), realname(""), data("") {}
} typedef t_MandatoryClientInit;









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
                            clients[currEvent->ident].Push_data(str);
                            str = clients[currEvent->ident].Get_command();
                            if (!str.empty())
                            {
                                std::cout << "client " << currEvent->ident << " : |"  << str <<"|"<< std::endl;
                                std::stringstream ss(str);
                                std::string tmp;
                                try
                                {
                                    CommandHandler::CommandRun(UserChannelController::Instance().FindUser(currEvent->ident), str);
                                    User test;
                                    int commandNum = CommandHandler::CommandRun(test, str);
                                    ss >> tmp;
                                    if (commandNum == NICKNUM)
                                    {
                                        ss >> tmp;
                                        clients[currEvent->ident].nickFlag = true;
                                        clients[currEvent->ident].nickname = tmp;
                                        std::cout << "NICK OK!\n";
                                    }
                                    else if (commandNum == USERNUM)
                                    {
                                        int commandNum = CommandHandler::CommandRun(UserChannelController::Instance().FindUser(currEvent->ident), str);
                                        ss >> tmp;
                                        clients[currEvent->ident].userFlag = true;
                                        clients[currEvent->ident].username = tmp;
                                        ss >> tmp;
                                        clients[currEvent->ident].hostname = tmp;
                                        ss >> tmp;
                                        ss >> tmp;
                                        std::size_t len = str.find(":");
                                        if (len == std::string::npos)
                                            clients[currEvent->ident].realname = tmp;
                                        else
                                        {
                                            std::string tmp1;
                                            tmp1.resize(str.size() - len - 1);
                                            size_t i = 0;
                                            while (i < str.size() - len - 1)
                                            {
                                                tmp1[i] = str[len + 1 + i];
                                                i++;
                                            }
                                            clients[currEvent->ident].realname = tmp1;
                                        }
                                        std::cout << clients[currEvent->ident].realname << "|"<<std::endl;
                                        std::cout << "USER OK!\n";
                                    }
                                    else if (commandNum == PASSNUM)
                                    {
                                        ss >> tmp;
                                        if (!tmp.compare(password))
                                        {
                                            clients[currEvent->ident].passwordFlag = true;
                                            std::cout << "PASS OK!\n";
                                        }
                                        else
                                            write(currEvent->ident, "Password incorrect\n", strlen("Password incorrect\n"));
                                    }
                                    if (clients[currEvent->ident].nickFlag && clients[currEvent->ident].userFlag && clients[currEvent->ident].passwordFlag)
                                    {
                                        std::cout << "make USER\n";
                                        UserChannelController::Instance().AddUser(currEvent->ident, clients[currEvent->ident].nickname,
                                        clients[currEvent->ident].username, clients[currEvent->ident].hostname, clients[currEvent->ident].realname);
                                    }
                                }
                                catch(const char *str)
                                {
                                    write(currEvent->ident, str, strlen(str));
                                    write(currEvent->ident, "\n", 1);
                                }
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