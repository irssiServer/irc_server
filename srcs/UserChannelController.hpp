#ifndef USERCHANNELCONTROLLER_HPP
# define USERCHANNELCONTROLLER_HPP

class User;
class Channel;

struct s_ChannelMode;
typedef struct s_ChannelMode t_ChannelMode;

struct s_UserInfo;
typedef struct s_UserInfo t_UserInfo;

# include <iostream>
# include <string>
# include <map>
# include "User.hpp"
# include "Channel.hpp"

class UserChannelController
{

	public:

		UserChannelController();
		UserChannelController( UserChannelController const & src );
		~UserChannelController();
		UserChannelController &operator=( UserChannelController const & rhs );

		// add, remove Method
		void AddUser(int fd, std::string nickname, std::string username, std::string hostname);
		void RemoveUser(int id);
		void AddChannel(std::string channelName, t_ChannelMode mode);
		void RemoveChannel(int id);

	private:
		int _channelID;
		std::map<int, User> _users;
		std::map<int, Channel> _channels;
};


#endif /* ******************************************* USERCHANNELCONTROLLER_H */