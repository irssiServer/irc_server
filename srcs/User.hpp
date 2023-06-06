#ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include <vector>
# include "UserChannelController.hpp"

struct s_UserInfo
{
	int id;
	int fd;
	std::string nickname;
	std::string username;
	std::string hostname;
} typedef t_UserInfo;


class User
{
	public:
		User();
		User(int id, int fd, std::string nickname, std::string username, std::string hostname);
		User( User const & src );
		~User();
		User &operator=( User const & rhs );

		void SetId(int id) { _userInfo.id = id; };
		int getId() { return _userInfo.id; };
		void SetFd(int fd) { _userInfo.fd = fd; };
		int getFd() { return _userInfo.fd; };
		void SetNickname(int nickname) { _userInfo.nickname = nickname; };
		std::string getNickname() { return _userInfo.nickname; };
		void SetUsername(int username) { _userInfo.username = username; };
		std::string getUsername() { return _userInfo.username; };
		void SetHostname(int hostname) { _userInfo.hostname = hostname; };
		std::string getHostname() { return _userInfo.hostname; };

		void JoinChannel(Channel *channel);
		void leaveChannel(int id);

	private:
		t_UserInfo _userInfo;
		std::vector<Channel *> _channels;
};


#endif /* ************************************************************ USER_H */