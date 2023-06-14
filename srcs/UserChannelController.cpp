#include "UserChannelController.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

UserChannelController::UserChannelController() {}
UserChannelController::UserChannelController( const UserChannelController & src ) { *this = src; }
UserChannelController::~UserChannelController() {}
UserChannelController &UserChannelController::operator=( UserChannelController const & rhs )
{
	if ( this != &rhs )
		*this = rhs;
	return *this;
}

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



/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */