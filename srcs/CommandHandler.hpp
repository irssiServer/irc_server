#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include <iostream>
# include <string>
# include <sstream>
# include "UserChannelController.hpp"

#define NICKNUM 1;
#define USERNUM 2;
#define PASSNUM 3;

class CommandHandler
{

	public:

		static int CommandRun(std::string str);
		static void NICK(std::vector<std::string> &params);

	private:
		CommandHandler();
		~CommandHandler();

		// map과 함수포인터를 결합해서 사용
		void CommandInit(std::map<std::string, void(*)(std::vector<std::string> &param)> &commandMap);
		static std::map<std::string, void(*)(std::vector<std::string> &param)> _commandMap;
		// static std::map<std::string, int> _messageNumbers;

};

#endif /* ************************************************** COMMANDHANDLER_H */