#include "Channel.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Channel::Channel()
{
}
Channel::Channel(int id, std::string channelName)
{
	_id = id;
	_channelName = channelName;
}

Channel::Channel( const Channel & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Channel::~Channel()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Channel &				Channel::operator=( Channel const & rhs )
{
	if ( this != &rhs )
	{
	}
	return *this;
}



/*
** --------------------------------- METHODS ----------------------------------
*/

int Channel::EnterUser(User *user)
{
	// if (user가 채널에 들어올 수 있는지 확인)
	// {
		_users.push_back(user);
		return 1;
	// }
	// else
	// 	return 0;
}

void Channel::LeaveUser(int id)
{
	for (std::vector<User *>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if ((*iter)->GetFd() == id)
		{
			_users.erase(iter);
			break;
		}
	}
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */