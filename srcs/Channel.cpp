#include "Channel.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Channel::Channel() {}
Channel::~Channel() {}
Channel::Channel(int id, std::string channelName) : _id(id), _channelName(channelName), _mode(t_ChannelMode()) {}
Channel::Channel(int id, std::string channelName, t_ChannelMode mode) : _id(id), _channelName(channelName), _mode(mode) {}



bool Channel::operator==(const Channel& rhs) const
{
	return _id == rhs.GetId();
}

/*
** --------------------------------- METHODS ----------------------------------
*/

int Channel::EnterUser(User *user, std::string password)
{
	for (std::vector<User *>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if ((*iter)->GetNickname() == user->GetNickname())
			return 0;
	}
	if (_mode.InviteCheck(user->GetNickname()) && _mode.KeyCheck(password) && _mode.LimiteCheck(_users.size()))
	{
		_users.push_back(user);
		return 1;
	}
	else
		return 0;
}

void Channel::LeaveUser(int fd)
{
	for (std::vector<User *>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if ((*iter)->GetFd() == fd)
		{
			_users.erase(iter);
			return;
		}
	}
}

void Channel::send(std::vector<std::string> &message)
{
	for(std::size_t i = 0; i < this->_users.size(); i++)
		CommandHandler::MSG(_users[i]->GetFd(), message);
}

bool Channel::isUser(User user)
{
	for (std::vector<User *>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if ((*iter)->GetFd() == user.GetFd())
			return 1;
	}
	return 0;
}
/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */