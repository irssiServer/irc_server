#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include <iostream>
# include <string>
# include <sstream>
# include "User.hpp"
# include "UserChannelController.hpp"

#define NICKNUM 1
#define USERNUM 2
#define PASSNUM 3
#define PRIVMSGNUM 4

class CommandHandler
{

	public:
		CommandHandler();
		~CommandHandler();
		
		static int CommandRun(std::string str, User &user);
		static void NICK(std::vector<std::string> &params, User &user);
		static void PASS(std::vector<std::string> &params, User &user);
		static void USER(std::vector<std::string> &params, User &user);
		static void PRIVMSG(std::vector<std::string> &params, User &user);
		
		
		static void PRIVMSG();
		
		


	private:
		// map과 함수포인터를 결합해서 사용
		void CommandInit(std::map<std::string, void(*)(std::vector<std::string> &param, User &user)> &commandMap);
		void CommandNumInit(std::map<std::string, int> &commandNum);
		static std::map<std::string, void(*)(std::vector<std::string> &param, User &user)> _commandMap;
		static std::map<std::string, int> _commandNum;
		// static std::map<std::string, int> _messageNumbers;

};


#endif /* ************************************************** COMMANDHANDLER_H */