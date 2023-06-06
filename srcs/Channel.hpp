#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include "UserChannelController.hpp"

struct s_ChannelMode // 좀 더 확인해봐야할듯
{
	bool Invite;
	std::string Topic;
	std::string key;
	std::string _operator;
	std::string limite;
} typedef t_ChannelMode;

class Channel
{

	public:

		Channel();
		Channel(int id, std::string channelName);
		Channel( Channel const & src );
		~Channel();
		Channel &operator=( Channel const & rhs );

		void SetId(int id) { _id = id; };
		int getId() { return _id; };

		int EnterUser(User *user);
		void LeaveUser(int id);

	private:
		int _id;
		std::string _channelName;
		t_ChannelMode _mode;
		std::vector<User *> _users;

};

#endif /* ********************************************************* CHANNEL_H */