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
	_userInfo.flag = 0;
}
User::~User() 
{
}
User::User(int fd, std::string nickname, std::string username, std::string hostname, std::string realname) 
{
	_userInfo.fd = fd;
	_userInfo.nickname = nickname;
	_userInfo.username = username;
	_userInfo.hostname = hostname;
	_userInfo.realname = realname;

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
	try
	{
		if (channel->EnterUser(this, password))
			_channels.push_back(channel);
	}
	catch(const std::string str)
	{
		throw str;
	}
}

void User::leaveChannel(std::string str)
{
	int id;
	for (std::vector<Channel *>::iterator iter = _channels.begin(); iter != _channels.end(); iter++)
	{
		if ((*iter)->GetName() == str)
		{
			id = (*iter)->GetId();
			(*iter)->LeaveUser(GetFd());
			_channels.erase(iter);
			if (UserChannelController::Instance().FindChannel(str).GetUserSize() == 0)
				UserChannelController::Instance().RemoveChannel(id);
			return ;
		}
	}
}

Channel &User::FindChannel(std::string channel)
{
	for (std::vector<Channel *>::iterator iter = _channels.begin(); iter != _channels.end(); iter++)
	{
		if (!(*iter)->GetName().compare(channel))
			return *(*iter);
	}
	Channel *tmp = NULL;

	return *tmp;
}

void User::AllLeaveChannels()
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		leaveChannel(_channels[i]->GetName());
	}
}

std::string User::GetNickHostmask()
{
	return GetNickname() + "!" + GetUsername() + "@" + GetHostname();
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */