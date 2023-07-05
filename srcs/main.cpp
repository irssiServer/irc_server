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
    int invokedEvnets;
    struct kevent eventList[8];
    std::map<int, t_MandatoryClientInit> clients;

    if (argc != 3)
        ErrorPrintExit("Error: arguments ex) ./program port password");
    ArrayToPort(argv[1], port);
    ArrayToPass(argv[2], password);
    Init_socket(port, connectSocket);
    Init_event(changeList, kq, connectSocket);
    UserChannelController::Instance().SetServerName(argv[0]);
    try
    {
        while (1)
        {
            invokedEvnets = kevent(kq, &changeList[0], changeList.size(), eventList, 8, NULL);
            if (invokedEvnets < 0)
                ErrorPrintExit("Error: Kevent");

            changeList.clear();
            struct kevent* currEvent;
            for (int i = 0; i < invokedEvnets; i++)
            {
                currEvent = &eventList[i];
                if (currEvent->flags & EV_ERROR)
                {
                    if (static_cast<int>(currEvent->ident) == connectSocket)
                        ErrorPrintExit("Error: Server socket");
                    else
                    {
                        std::cout << "client disconnected: " << currEvent->ident << std::endl;
                        UserChannelController::Instance().RemoveUser(currEvent->ident);
                        clients.erase(currEvent->ident);
                    }
                }
                else if (currEvent->filter == EVFILT_READ)
                {
                    if (static_cast<int>(currEvent->ident) == connectSocket)
                        AcceptUser(connectSocket, changeList, clients);
                    else
                    {
                        std::string str;
                        str.resize(currEvent->data);
                        if (recv(currEvent->ident, &str[0], currEvent->data, 0) <= 0)
                        {
                            std::cout << "client disconnected: " << currEvent->ident << std::endl;
                            UserChannelController::Instance().RemoveUser(currEvent->ident);
                            clients.erase(currEvent->ident);
                        }
                        else
                        {
                            clients[currEvent->ident].Push_data(str);
                            while ((str = clients[currEvent->ident].Get_command()).compare(""))
                            {
                                std::cout << "client " << currEvent->ident << " : "  << str << std::endl;
                                try
                                {
                                    if (clients[currEvent->ident].nickFlag && clients[currEvent->ident].userFlag && clients[currEvent->ident].passwordFlag)
                                    {
                                        if (!str.empty())
                                            if (CommandHandler::CommandRun(UserChannelController::Instance().FindUser(currEvent->ident), str) == QUITNUM)
                                                clients.erase(currEvent->ident);
                                    }
                                    else
                                        AuthenticateUserAccess(currEvent->ident, clients, password, str);
                                }
                                catch (const char *str)
                                {
                                    std::cerr << str << std::endl;
                                }
                                catch (const std::string str)
                                {
                                    std::cerr << str << std::endl;
                                }
                            }
                        }
                    }
                }
                else if (currEvent->filter == EVFILT_WRITE)
                {
                    std::cout << "EVFILT_WRITE" << std::endl;
                }
            }
        }
    }
    catch(const std::string str)
    {
        std::cerr << "string : " << str << std::endl;
    }
    catch(const char * str)
    {
        std::cerr << "char * : " << str << std::endl;
    }
}