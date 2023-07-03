#include "KqueueEventManager.hpp"

void ErrorPrintExit(std::string str);

void	Init_socket(int port, int &connectSocket)
{
    struct sockaddr_in server_addr;

    connectSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (connectSocket == -1)
        ErrorPrintExit("Error: not create socket");
    int reuse = 1;
    setsockopt(connectSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // ipv4로 설정

    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port); // port로 대체

    if (bind(connectSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        ErrorPrintExit("Error: not binding");
    
    if (listen(connectSocket, 20) == -1)// 20에 해당하는 부분은 연결 대기의 최대수이다.
        ErrorPrintExit("Error: not listening");

    fcntl(connectSocket, F_SETFL, O_NONBLOCK);
}

void	Init_event(std::vector<struct kevent> &changeList, int &kq, int &connectSocket)
{
	if ((kq = kqueue()) == -1)
        ErrorPrintExit("Error: not create Kqueue");

    struct kevent temp_event;

    EV_SET(&temp_event, connectSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    changeList.push_back(temp_event);
}

void AcceptUser(int connectSocket, std::vector<struct kevent> &changeList, std::map<int, t_MandatoryClientInit> &clients)
{
    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;

    if ((clientSocket = accept(connectSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1)
    {
        std::cout << ((struct sockaddr *)&clientAddr)->sa_data << " is fail to accept" << std::endl;
    }
    else
    {
        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
        struct kevent temp_event;
        EV_SET(&temp_event, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        changeList.push_back(temp_event);
        clients.insert(std::pair<int, t_MandatoryClientInit>(clientSocket, t_MandatoryClientInit()));
        clients[clientSocket].hostname = inet_ntoa(clientAddr.sin_addr);
    }
}

void AuthenticateUserAccess(int fd, std::map<int, t_MandatoryClientInit> &clients, std::string &password, std::string &message)
{
    User test;
    test.SetNickname(clients[fd].nickname);
    test.SetFd(fd);
    test.SetHostname(clients[fd].hostname);
    test.SetFlag(1);

    std::stringstream ss(message);
    std::string tmp;
    try
    {
        int commandNum = CommandHandler::CommandRun(test, message);
        ss >> tmp;
        if (commandNum == NICKNUM)
        {
            ss >> tmp;
            clients[fd].nickFlag = true;
            clients[fd].nickname = tmp;
            std::cout << "NICK OK!\n";
        }
        else if (commandNum == USERNUM)
        {
            ss >> tmp;
            clients[fd].userFlag = true;
            clients[fd].username = tmp;
            ss >> tmp;
            // clients[fd].hostname = tmp;
            ss >> tmp;
            ss >> tmp;
            std::size_t len = message.find(":");
            if (len == std::string::npos)
                clients[fd].realname = tmp;
            else
            {
                std::string tmp1;
                tmp1.resize(message.size() - len - 1);
                size_t i = 0;
                while (i < message.size() - len - 1)
                {
                    tmp1[i] = message[len + 1 + i];
                    i++;
                }
                clients[fd].realname = tmp1;
            }
            std::cout << "USER OK!\n";
        }
        else if (commandNum == PASSNUM)
        {
            ss >> tmp;
            clients[fd].passwordFlag = true;
            if (!tmp.compare(password))
                clients[fd].pass = true;
            else
                clients[fd].pass = false;
        }
        if (clients[fd].nickFlag && clients[fd].userFlag && clients[fd].passwordFlag)
        {
            if (UserChannelController::Instance().isNick(clients[fd].nickname))
            {
                clients[fd].nickFlag = false;
                test.SetNickname("*");  
                ERR_NICKNAMEINUSE(test, clients[fd].nickname);
                throw "Nickname is already in use";
            }
            if (!clients[fd].pass)
            {  
                Send(fd, "ERROR :Closing link: (" + test.GetNickname() + "@" + test.GetHostname() + ")"+ "[Access denied by configuration]");
                clients.erase(fd);
                close (fd);

                throw "ERROR :Closing link: (a@127.0.0.1) [Access denied by configuration]";
            }
            UserChannelController::Instance().AddUser(fd, clients[fd].nickname,
                clients[fd].username, clients[fd].hostname, clients[fd].realname);
            std::cout << "client " << clients[fd].nickname << "!" << clients[fd].username << "@" << clients[fd].hostname  << " is connected" << std::endl;
        }
    }
    catch(const char *message)
    {
        std::cout << message << std::endl;
    }
}

/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */