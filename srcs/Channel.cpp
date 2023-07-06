#include "Channel.hpp"
/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Channel::Channel() : _topic("") {}

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
		if ((*iter)->GetNickname() == user->GetNickname()) // 이미 채널에 유저가 있을경우
			return 0;
	}
	if (!InviteCheck(*user))
	{
		ERR_INVITEONLYCHAN(*user, _channelName);
		throw "";
	}
	else if (!_mode.KeyCheck(password))
	{
		ERR_BADCHANNELKEY(*user, _channelName);
		throw "";
	}
	else if (!_mode.LimiteCheck(_users.size()))
	{
		ERR_CHANNELISFULL(*user, _channelName);
		throw "";
	}	
	_users.push_back(user);
	std::string str = ":" + user->GetNickHostmask() + " JOIN :" + GetName();
	SendUsers(str);

	return 1;
}

void Channel::LeaveUser(int fd)
{
	for (std::vector<User *>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if ((*iter)->GetFd() == fd)
		{
			if (_mode.OperUserCheck((*iter)->GetNickname()))
				_mode.operatorUser.erase(std::find(_mode.operatorUser.begin(), _mode.operatorUser.end(), (*iter)->GetNickname()));
			_users.erase(iter);
			return;
		}
	}
}

void Channel::KickUser(User &user, std::string username, std::string message)
{
	if (!_mode.OperUserCheck(user.GetNickname()))
	{
		ERR_CHANOPRIVSNEEDED(user, _channelName);
		throw "";
	}
	SendUsers(message);
	for (std::vector<User *>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if (!(*iter)->GetNickname().compare(username))
		{
			(*iter)->leaveChannel(_channelName);
			return;
		}
	}
}

void Channel::SendUsers(std::string &message, User &user)
{
	for(std::size_t i = 0; i < this->_users.size(); i++)
	{
		if (_users[i]->GetNickname().compare(user.GetNickname()))
			Send(_users[i]->GetFd(), message);
	}
}

void Channel::SendUsers(std::string &message)
{
	for(std::size_t i = 0; i < this->_users.size(); i++)
	{
		Send(_users[i]->GetFd(), message);
	}
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

bool Channel::isUser(std::string nickName)
{
	for (std::vector<User *>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if (!(*iter)->GetNickname().compare(nickName))
			return 1;
	}
	return 0;
}

int Channel::ModeInvite(User &user, bool flag)
{
	if (!_mode.OperUserCheck(user.GetNickname()))
	{
		ERR_CHANOPRIVSNEEDED(user, _channelName);
		throw "";
	}
	_mode.inviteFlag = flag;
	return 0;
}

int Channel::ModeTopic(User &user, bool flag)
{
	if (!_mode.OperUserCheck(user.GetNickname()))
	{
		ERR_CHANOPRIVSNEEDED(user, _channelName);
		throw "";
	}
	_mode.topicSetFlag = flag;
	return 0;
}

int Channel::ModeLimite(User &user, bool flag, std::string limiteNum)
{
	if (!_mode.OperUserCheck(user.GetNickname()))
	{
		ERR_CHANOPRIVSNEEDED(user, _channelName);
		throw "";
	}
	std::stringstream ss(limiteNum);

	int num;
	ss >> num;
	_mode.limiteFlag = flag;
	if (flag == ADD && num > 0)
	{
		if (ss.fail() || !ss.eof())
		{
			ERR_NEEDMOREPARAMS(user, "Parameter is not a number");
			throw "";
		}
		_mode.limite = num;
	}
	return 0;
}

int Channel::ModeKey(User &user, bool flag, std::string key)
{
	if (!_mode.OperUserCheck(user.GetNickname()))
	{
		ERR_BADCHANNELKEY(user, _channelName);
		throw "";
	}
	if (_mode.keyFlag == false || _mode.KeyCheck(key) || !key.empty() || key.compare(""))
	{
		_mode.keyFlag = flag;
		_mode.key = key;
	}
	else
		throw "";
	return 0;
}

int Channel::ModeOperator(User &user, bool flag, std::string userName)
{
	if (!_mode.OperUserCheck(user.GetNickname()))
	{
		ERR_CHANOPRIVSNEEDED(user, _channelName);
		throw "";
	}

	if (isUser(userName))
	{
		std::vector<std::string>::iterator find = std::find(_mode.operatorUser.begin(), _mode.operatorUser.end(), userName);
		if (find == _mode.operatorUser.end())
		{
			if (flag == ADD)
				_mode.operatorUser.push_back(userName);
		}
		else
		{
			if (flag == REMOVE)
				_mode.operatorUser.erase(find);
		}
		return 0;
	}
	else
	{
		ERR_NOSUCHNICK(user, userName);
		throw "";
	}
}

bool Channel::InviteCheck(User &user)
{
	if (!_mode.inviteFlag)
		return true;
	
	std::vector<std::string> invited = user.GetInvitedChannels();
	std::vector<std::string>::iterator iter = std::find(invited.begin(), invited.end(), _channelName);
	return iter != invited.end();
}



void Channel::InviteUser(User &inviter, std::string invitee)
{
	if (!UserChannelController::Instance().isNick(invitee))
	{
		ERR_NOSUCHNICK(inviter, invitee);
		throw ("");
	}
	if (_mode.OperUserCheck(inviter.GetNickname()))
	{
		User &user = UserChannelController::Instance().FindUser(invitee);
		std::vector<std::string>::iterator find = std::find(user.GetInvitedChannels().begin(),user.GetInvitedChannels().end(), _channelName);
		if (isUser(user.GetNickname())) //(이미 채널에 들어와 있을떄)
		{
			ERR_USERONCHANNEL(inviter, invitee, _channelName);
			throw ("");
		}
		else if (find == user.GetInvitedChannels().end())
			UserChannelController::Instance().FindUser(invitee).SetInvitedChannel(_channelName);
	}
	else
	{
		ERR_CHANOPRIVSNEEDED(inviter, _channelName);
		throw ("");
	}
}

void Channel::SetOper(User &user)
{
	_mode.operatorUser.push_back(user.GetNickname());
}


void Channel::SetTopic(std::string topic, User &user)
{
	if (_mode.topicSetFlag == true)
	{
		if (_mode.OperUserCheck(user.GetNickname()))
			_mode.topic = topic;
		else
		{
			ERR_CHANOPRIVSNEEDED(user, _channelName);
			throw "";
		}
	}
	else
		_mode.topic = topic;
}

std::string Channel::GetModeFlags()
{
	std::string str("");
	if (_mode.keyFlag)
		str = 'k';
	if (_mode.inviteFlag)
		str += 'i';
	if (_mode.limiteFlag)
		str += 'l';
	if (_mode.topicSetFlag)
		str += 't';
	if (str.empty())
		return str;
	return '+' + str;
}

std::vector<int> Channel::GetFdlist()
{
	std::vector<int> tmp;
	for (size_t i = 0; i < _users.size(); i++)
	{
		tmp.push_back(_users[i]->GetFd());
	}
	return (tmp);
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */