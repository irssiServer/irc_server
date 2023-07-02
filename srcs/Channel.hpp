#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <unistd.h>
# include <iostream>
# include <string>
# include <vector>
# include "UserChannelController.hpp"


struct s_ChannelMode
{
	bool inviteFlag;
	bool topicSetFlag;
	bool keyFlag;
	bool limiteFlag;
	std::vector<std::string> operatorUser;
	std::string topic;
	std::string key;
	int limite;

	s_ChannelMode() :inviteFlag(0), topicSetFlag(0), keyFlag(0), limiteFlag(0), limite(0) {}


	bool KeyCheck(std::string password)
	{
		if (!keyFlag)
			return true;

		return password.compare(key);
	}

	bool LimiteCheck(int userNum)
	{
		if (!limiteFlag)
			return true;
			
		return userNum < limite;
	}

	bool OperUserCheck(std::string user)
	{
		for (std::vector<std::string>::iterator iter = operatorUser.begin(); iter != operatorUser.end(); iter++)
		{
			if (!iter->compare(user))
				return true;
		}
		return false;
	}

} typedef t_ChannelMode;

class Channel
{

	public:
		Channel();
		Channel(int id, std::string channelName);
		Channel(int id, std::string channelName, t_ChannelMode mode);
		~Channel();
		void SetId(int id) { _id = id; };
		int GetId() const { return _id; };
		void SetName(std::string channelName) { _channelName = channelName; };
		std::string GetName() const { return _channelName; };
		void SetTopic(std::string topic, User &user);
		std::string GetTopic() const { return _mode.topic; };
		void SetOper(User &user);
		int GetUserSize() { return _users.size(); };

		void SendUsers(std::string &message, User &user);
		void SendUsers(std::string &message);
		int EnterUser(User *user, std::string password);
		void LeaveUser(int fd);
		void KickUser(User &user, std::string username);
		bool isUser(User user);
		bool isUser(std::string nickName);
		std::string GetModeFlags();

		// mode change function
		int ModeInvite(User &user, bool flag);
		int ModeTopic(User &user, bool flag);
		int ModeLimite(User &user, bool flag, int limiteNum);
		int ModeKey(User &user, bool flag, std::string key);
		int ModeOperator(User &user, bool flag, std::string userName);
		
		bool InviteCheck(User &user);
		void InviteUser(User &inviter, std::string invitee);


	private:
		bool operator==(const Channel& rhs) const;

		int _id;
		std::string _channelName;
		t_ChannelMode _mode;
		std::vector<User *> _users;
		std::string _topic;

};

#endif /* ********************************************************* CHANNEL_H */