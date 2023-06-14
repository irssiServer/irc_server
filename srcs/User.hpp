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
} typedef t_UserInfo;

class User
{
	public:
		User();
		User(int fd, std::string nickname, std::string username, std::string hostname);
		User( User const & src );
		~User();

		User &operator=( User const & rhs );
		bool operator==(const User& rhs) const;

		void SetFd(int fd) { _userInfo.fd = fd; };
		int GetFd() const { return _userInfo.fd; };
		void SetNickname(int nickname) { _userInfo.nickname = nickname; };
		std::string GetNickname() const { return _userInfo.nickname; };
		void SetUsername(int username) { _userInfo.username = username; };
		std::string GetUsername() const { return _userInfo.username; };
		void SetHostname(int hostname) { _userInfo.hostname = hostname; };
		std::string GetHostname() const { return _userInfo.hostname; };

		void JoinChannel(Channel *channel, std::string password);
		void leaveChannel(int id);

	private:
		t_UserInfo _userInfo;
		std::vector<Channel *> _channels;
};


#endif /* ************************************************************ USER_H */