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

	// (void)serverName;
	str = ":" + serverName + " 001 " + user.GetHostname() + " :Welcome to the 42IRCnet Network," + user.GetNickHostmask();
	Send(user.GetFd(), str);
	// Send(user.GetFd(), ":irc.local 001 gyyu :Welcome to the Localnet IRC Network gyyu!root@127.0.0.1");
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */