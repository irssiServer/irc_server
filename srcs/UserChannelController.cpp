#include "UserChannelController.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

UserChannelController::UserChannelController() {}
UserChannelController::~UserChannelController() {}


/*
** --------------------------------- METHODS ----------------------------------
*/

UserChannelController &UserChannelController::Instance()
{
	static UserChannelController _userChannelController;
	return _userChannelController;	
}

void UserChannelController::AddUser(int fd, std::string nickname, std::string username, std::string hostname)
{
	User user(fd, nickname, username, hostname);
	_users.insert(std::pair<int, User>(fd, user));
}

void UserChannelController::RemoveUser(int id)
{
	_users.erase(id);
}

void UserChannelController::AddChannel(std::string channelName, t_ChannelMode mode)
{
	_channelID++;
	Channel channel(_channelID, channelName, mode);
	_channels.insert(std::pair<int, Channel>(_channelID, channel));
}

void UserChannelController::RemoveChannel(int id) 
{
	_channels.erase(id);
}

bool UserChannelController::isNick(std::string nick)
{
	for (std::map<int, User>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if (!iter->second.GetNickname().compare(nick))
			return true;
	}
	return false;
}

bool UserChannelController::isChannel(std::string channel)
{
	for (std::map<int, Channel>::iterator iter = _channels.begin(); iter != _channels.end(); iter++)
	{
		if (!iter->second.GetName().compare(channel))
			return true;
	}
	return false;
}

User &UserChannelController::FindUser(int fd)
{
	return _users[fd];
}

Channel &UserChannelController::FindChannel(std::string channel)
{
	for (std::map<int, Channel>::iterator iter = _channels.begin(); iter != _channels.end(); iter++)
	{
		if (!iter->second.GetName().compare(channel))
			return iter->second;
	}
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */