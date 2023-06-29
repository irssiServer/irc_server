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
	if (!_mode.InviteCheck(user->GetNickname()))
		throw "irc.local 473 asdf #1 :Cannot join channel (invite only)";
	else if (!_mode.KeyCheck(password))
		throw ":irc.local 475 asdf #1 :Cannot join channel (incorrect channel key)";
	else if (!_mode.LimiteCheck(_users.size()))
		throw ":irc.local 471 asdf #1 :Cannot join channel (channel is full)";
	_users.push_back(user);
	std::string str = user->GetNickHostmask() + " JOIN :" + GetName();
	SendUsers(str);
	return 1;
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

void Channel::KickUser(User &user, std::string username, std::string comment)
{
	if (!_mode.OperUserCheck(user.GetNickname()))
		throw "No permission to Kick command";
	for (std::vector<User *>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if (!(*iter)->GetNickname().compare(username))
		{
			(*iter)->leaveChannel(_channelName);
			write(user.GetFd(), comment.c_str(),comment.size());
			return;
		}
	}
}

void Channel::SendUsers(std::string &message)
{
	for(std::size_t i = 0; i < this->_users.size(); i++)
	{
		std::cout << _users[i]->GetNickname() << std::endl;
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
	if (_mode.OperUserCheck(user.GetNickname()))
		throw "You must have channel op access or above to set channel mode i";
	_mode.inviteFlag = flag;
	return 0;
}

int Channel::ModeTopic(User &user, bool flag)
{
	if (_mode.OperUserCheck(user.GetNickname()))
		throw "You must have channel op access or above to set channel mode t";
	_mode.topicSetFlag = flag;
	return 0;
}

int Channel::ModeLimite(User &user, bool flag, int limiteNum)
{
	if (_mode.OperUserCheck(user.GetNickname()))
		throw "You must have channel op access or above to set channel mode l";
	_mode.limiteFlag = flag;
	if (flag == ADD && limiteNum > 0)
		_mode.limite = limiteNum;
	return 0;
}

int Channel::ModeKey(User &user, bool flag, std::string key)
{
	if (_mode.OperUserCheck(user.GetNickname()))
		throw "You must have channel op access or above to set channel mode k";
	if (_mode.keyFlag == false || _mode.KeyCheck(key))
	{
		_mode.keyFlag = flag;
		_mode.key = key;
	}
	else
		throw "467 gyyu #1 :Channel key already set";
	return 0;
}

int Channel::ModeOperator(User &user, bool flag, std::string userName)
{

	if (_mode.OperUserCheck(user.GetNickname()))
		throw "You must have channel op access or above to set channel mode t";

	if (isUser(userName))
	{
		std::vector<std::string>::iterator find = std::find(_mode.operatorUser.begin(),_mode.operatorUser.end(), userName);
		if (find == _mode.operatorUser.end())
		{
			if (flag == ADD)
				_mode.operatorUser.push_back(*find);
		}
		else
		{
			if (flag == REMOVE)
				_mode.operatorUser.erase(find);
		}
		return 0;
	}
	else
		throw "401 gyyu vd :No such nick";
}

void Channel::InviteUser(User &inviter, std::string invitee)
{
	if (!UserChannelController::Instance().isNick(invitee))
		throw ("401 gyyu vd :No such nick");
	if (_mode.OperUserCheck(inviter.GetNickname()))
	{
		if (!_mode.InviteCheck(invitee))
			SetInvitedUser(invitee);
	}
	else
		throw ("No operator permissions");
}

void Channel::SetOper(User &user)
{
	_mode.operatorUser.push_back(user.GetNickname());
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */