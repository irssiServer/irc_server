#include "SendMessage.hpp"

/*
** --------------------------------- METHODS ----------------------------------
*/

void Send(int fd, std::string str)
{
	send(fd, str.c_str(), str.size(), 0);	
	send(fd, "\n", 1, 0);	
}

void RPL_WELCOME(std::string serverName, User &user)
{
	std::string str;

	str = ":" + serverName + " 001 " + user.GetNickname() + " :Welcome to the 42IRCnet Network," + user.GetNickHostmask();
	Send(user.GetFd(), str);
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */