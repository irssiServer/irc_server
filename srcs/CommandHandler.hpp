#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include <iostream>
# include <string>
# include <sstream>
# include "UserChannelController.hpp"

class CommandHandler
{

	public:

		static void CommandRun(std::string str);
		static void NICK(std::vector<std::string> &params);

	private:
		CommandHandler();
		~CommandHandler();

		// map과 함수포인터를 결합해서 사용
		void CommandInit(std::map<std::string, void(*)(std::vector<std::string> &param)> &commandMap);
		static std::map<std::string, void(*)(std::vector<std::string> &param)> _commandMap;

};

#endif /* ************************************************** COMMANDHANDLER_H */