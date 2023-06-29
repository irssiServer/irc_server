#ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include <vector>
# include "CommandHandler.hpp"
# include "UserChannelController.hpp"

struct s_UserInfo
{
	int fd;
	std::string nickname;
	std::string username;
	std::string hostname;
	std::string realname;
	std::string	buf;
	std::vector<std::string> invitedChannels;
	int buf_fd;
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
		void SetInvitedChannel(std::string invitedChannel) { _userInfo.invitedChannels.push_back(invitedChannel); };
		std::vector<std::string> GetInvitedChannels() const { return _userInfo.invitedChannels; };
		void Setbuf(std::string buf) { _userInfo.buf = buf; };
		void Setbuf_fd(int buf) { _userInfo.buf_fd = buf; };
		std::string Getbuf();
		int	Getbuf_fd();
		std::string GetNickHostmask();



		void JoinChannel(Channel *channel, std::string password);
		void leaveChannel(int id);
		void leaveChannel(std::string &str);
		void AllLeaveChannels();
		Channel &FindChannel(std::string channel);

	private:
		t_UserInfo _userInfo;
		std::vector<Channel *> _channels;
};


#endif /* ************************************************************ USER_H */