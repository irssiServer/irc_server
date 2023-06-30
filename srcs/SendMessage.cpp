#include "SendMessage.hpp"

/*
** --------------------------------- METHODS ----------------------------------
*/

void Send(int fd, std::string str)
{
	send(fd, str.c_str(), str.size(), 0);	
	send(fd, "\n", 1, 0);	
}

void RPL_WELCOME(User &user)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 001 " + user.GetNickname() + " :Welcome to the 42IRCnet Network," + user.GetNickHostmask();
	Send(user.GetFd(), str);
}


void RPL_CHANNELMODEIS(User &user, Channel &channel)
{
	std::string str;
	//:irc.local 324 b #1 :+nt
	str = ":" + UserChannelController::Instance().GetServerName() + " 324 " + user.GetNickname() + " " + channel.GetName() + " :"; //+nt;
	Send(user.GetFd(), str);
}











void ERR_NEEDMOREPARAMS(User &user, std::string command)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 461 " + user.GetNickname() + " " + command +" :Not enough parameters";
	Send(user.GetFd(), str);
}

void ERR_ALREADYREGISTERED(User &user)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 462 " + user.GetNickname() + " :You may not reregister";
	Send(user.GetFd(), str);
}

void ERR_NICKNAMEINUSE(User &user, std::string nick)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 433 " + user.GetNickname() + " " + nick + " :Nickname is already in use";
	Send(user.GetFd(), str);
}

void ERR_ERRONEUSNICKNAME(User &user, std::string nick)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 432 " + user.GetNickname() + " " + nick + " :Erroneus nickname";
	Send(user.GetFd(), str);
}

void ERR_NOTREGISTERED(User &user)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 451 " + user.GetNickname() + " :You have not registered";
	Send(user.GetFd(), str);
}

void ERR_CANNOTSENDTOCHAN(User &user, std::string channel)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 404 " + user.GetNickname() + " " + channel + " :Cannot send to channel";
	Send(user.GetFd(), str);
}

void ERR_NOSUCHCHANNEL(User &user, std::string channel)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 403 " + user.GetNickname() + " " + channel + " :No such channel";
	Send(user.GetFd(), str);
}

void ERR_NOSUCHNICK(User &user, std::string nick)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 401 " + user.GetNickname() + " " + nick + " :No such nick/channel";
	Send(user.GetFd(), str);
}

void RPL_NOTOPIC(User &user, std::string channel)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 331 " + user.GetNickname() + " " + channel + " :No topic is set";
	Send(user.GetFd(), str);
}

void ERR_NOTONCHANNEL(User &user, std::string channel)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 442 " + user.GetNickname() + " " + channel + " :You're not on that channel";
	Send(user.GetFd(), str);
}

void ERR_NOORIGIN(User &user)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 409 " + user.GetNickname() + " :No origin specified";
	Send(user.GetFd(), str);
}

void ERR_NOUSERMODE(User &user)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 470 " + user.GetNickname() + " :Does not Support NICK MODE";
	Send(user.GetFd(), str);
}

void ERR_UNKNOWNMODE(User &user, char mode)
{
	std::string str;
	
	str = ":" + UserChannelController::Instance().GetServerName() + " 472 " + user.GetNickname() + " " + mode + " :is not a recognised channel mode.";
}

void ERR_CHANOPRIVSNEEDED(User &user, std::string channel)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 482 " + user.GetNickname() + " " + channel +  " :You're not channel operator";
	Send(user.GetFd(), str);
}

void ERR_USERONCHANNEL(User &user, std::string nick, std::string channel)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 443 " + user.GetNickname() + " " + nick + " " + channel +  " :is already on channel";
	Send(user.GetFd(), str);
}

void RPL_INVITING(User &user, std::string nick, std::string channel)
{
	std::string str;

	str = ":" + UserChannelController::Instance().GetServerName() + " 341 " + user.GetNickname() + " " + nick + " :" + channel;
	Send(user.GetFd(), str);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */