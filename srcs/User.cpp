#include "User.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

User::User()
{
}

User::User( const User & src )
{
	*this = src;
}

User::User(int fd, std::string nickname, std::string username, std::string hostname)
{
	_userInfo.fd = fd;
	_userInfo.nickname = nickname;
	_userInfo.username = username;
	_userInfo.hostname = hostname;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

User::~User()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

User &User::operator=( User const & rhs )
{
	if ( this != &rhs )
	{
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void User::JoinChannel(Channel *channel)
{
	if (channel->EnterUser(this))
		_channels.push_back(channel);
	else
		std::cout << "채널에 입장하지 못했습니다." << std::endl;
}

void User::leaveChannel(int id)
{
	for (std::vector<Channel *>::iterator iter = _channels.begin(); iter != _channels.end(); iter++)
	{
		if ((*iter)->getId() == id)
		{
			_channels.erase(iter);
			break;
		}
	}
}



/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */