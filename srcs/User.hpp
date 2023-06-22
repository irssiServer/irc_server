#ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include <vector>
# include "UserChannelController.hpp"

struct s_UserInfo
{
	int fd;
	std::string nickname;
	std::string username;
	std::string hostname;
	std::string realname;
} typedef t_UserInfo;

class User
{
	public:
		User();
		User(int fd, std::string nickname, std::string username, std::string hostname, std::string realname);
		~User();

		bool operator==(const User& rhs) const;

		void SetFd(int fd) { _userInfo.fd = fd; };
		int GetFd() const { return _userInfo.fd; };
		void SetNickname(std::string nickname) { _userInfo.nickname = nickname; };
		std::string GetNickname() const { return _userInfo.nickname; };
		void SetUsername(std::string username) { _userInfo.username = username; };
		std::string GetUsername() const { return _userInfo.username; };
		void SetHostname(std::string hostname) { _userInfo.hostname = hostname; };
		std::string GetHostname() const { return _userInfo.hostname; };
		void Setrealname(std::string realname) { _userInfo.realname = realname; };
		std::string Getrealname() const { return _userInfo.realname; };
		void send(User &recv, std::vector &message);

		void JoinChannel(Channel *channel, std::string password);
		void leaveChannel(int id);
		void leaveChannel(std::string &str);
		Channel &FindChannel(std::string channel);

	private:
		t_UserInfo _userInfo;
		std::vector<Channel *> _channels;
};


#endif /* ************************************************************ USER_H */