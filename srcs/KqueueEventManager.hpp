#ifndef KQUEUEEVENTMANAGER_HPP
# define KQUEUEEVENTMANAGER_HPP

# include <iostream>
# include <string>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <map>
# include <vector>
#include "UserChannelController.hpp"
#include <netinet/in.h>

struct s_MandatoryClientInit
{
    bool userFlag;
    bool nickFlag;
    bool passwordFlag;
    bool pass;
    
    std::string nickname;
    std::string username;
    std::string hostname;
    std::string realname;
    std::string data;
    
	void Push_data(std::string str)
    {
        data = data + str;
    }

    std::string Get_command()
    {
        std::size_t len = data.find("\r\n");
        if (len == std::string::npos)
			return ("");
		std::string tmp = data.substr(0, len);
        data.erase(0, len + 2);
		return (tmp);
    }
    s_MandatoryClientInit() : userFlag(0), nickFlag(0), passwordFlag(0),  nickname("*"), username(""), hostname(""), realname(""), data("") {}
} typedef t_MandatoryClientInit;


void	Init_socket(int port, int &connectSocket);
void	Init_event(std::vector<struct kevent> &changeList, int &kq, int &connectSocket);
void	AcceptUser(int connectSocket, std::vector<struct kevent> &changeList, std::map<int, t_MandatoryClientInit> &clients);
void	AuthenticateUserAccess(int fd, std::map<int, t_MandatoryClientInit> &clients, std::string &password, std::string &message);

#endif /* ********************************************** KQUEUEEVENTMANAGER_H */