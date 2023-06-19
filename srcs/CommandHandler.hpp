#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include <iostream>
# include <string>
# include <sstream>
# include "UserChannelController.hpp"

class CommandHandler
{

	public:

		static bool NICK(std::string str);

	private:
		CommandHandler();
		~CommandHandler();
};

#endif /* ************************************************** COMMANDHANDLER_H */