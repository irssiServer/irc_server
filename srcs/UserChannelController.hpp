#ifndef USERCHANNELCONTROLLER_HPP
# define USERCHANNELCONTROLLER_HPP

class User;
class Channel;

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
		int _userID;
		int _channelID;
		std::map<int, User> _users;
		std::map<int, Channel> _channels;
};


#endif /* ******************************************* USERCHANNELCONTROLLER_H */