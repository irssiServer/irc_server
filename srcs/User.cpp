#include "User.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

User::User()
{
	_userInfo.fd = -1;
	_userInfo.hostname = "";
	_userInfo.nickname = "";
	_userInfo.realname = "";
	_userInfo.username = "";
}
User::~User() {}
User::User(int fd, std::string nickname, std::string username, std::string hostname, std::string realname) 
{
	_userInfo.fd = fd;
	_userInfo.nickname = nickname;
	_userInfo.username = username;
	_userInfo.hostname = hostname;
	_userInfo.realname = realname;

}

void User::send(User &recv, std::vector &message)
{
	
}



bool User::operator==( User const & rhs) const
{
	return GetFd() == rhs.GetFd();
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void User::JoinChannel(Channel *channel, std::string password)
{
	if (channel->EnterUser(this, password))
		_channels.push_back(channel);
	else
		std::cout << "채널에 입장하지 못했습니다." << std::endl;
}

void User::leaveChannel(int id)
{
	for (std::vector<Channel *>::iterator iter = _channels.begin(); iter != _channels.end(); iter++)
	{
		if ((*iter)->GetId() == id)
		{
			(*iter)->LeaveUser(GetFd());
			_channels.erase(iter);
			break;
		}
	}
}

void User::leaveChannel(std::string &str)
{
	for (std::vector<Channel *>::iterator iter = _channels.begin(); iter != _channels.end(); iter++)
	{
		if ((*iter)->GetName() == str)
		{
			(*iter)->LeaveUser(GetFd());
			_channels.erase(iter);
			break;
		}
	}
}

Channel &User::FindChannel(std::string channel)
{
	for (std::vector<Channel *>::iterator iter = _channels.begin(); iter != _channels.end(); iter++)
	{
		if (!(*iter)->GetName().compare(channel))
		{
			return *(*iter);
		}
	}
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */