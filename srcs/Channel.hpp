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
	std::vector<std::string> invitedUser;
	std::string topic;
	std::string key;
	int limite;

	s_ChannelMode() :inviteFlag(0), topicSetFlag(0), keyFlag(0), limiteFlag(0), limite(0) {}

	bool InviteCheck(std::string nickname)
	{
		if (!inviteFlag)
			return true;
		
		// 해당 유저의 당위성 체크는 좀 더 고민해야 될듯(nickname으로 체크하면 로그아웃하고 나서 같은 이름의 유저도 처리될듯)
		std::vector<std::string>::iterator iter
				= std::find(invitedUser.begin(), invitedUser.end(), nickname);
		return iter != invitedUser.end();
	}

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
		void SetTopic(std::string topic) { _topic = topic; };
		std::string GetTopic() const { return _topic; };
		void SetOper(User &user);
		std::vector<std::string> GetInvitedUser() const { return _mode.invitedUser; };
		void SetInvitedUser(std::string nickName) { _mode.invitedUser.push_back(nickName); };

		void send(std::string &message);
		int EnterUser(User *user, std::string password);
		void LeaveUser(int fd);
		void KickUser(User &user, std::string username, std::string comment);
		bool isUser(User user);
		bool isUser(std::string nickName);

		// mode change function
		int ModeInvite(User &user, bool flag);
		int ModeTopic(User &user, bool flag);
		int ModeLimite(User &user, bool flag, int limiteNum);
		int ModeKey(User &user, bool flag, std::string key);
		int ModeOperator(User &user, bool flag, std::string userName);
		
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