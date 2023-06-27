#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include <iostream>
# include <string>
# include <unistd.h>
# include <sstream>
# include "User.hpp"
# include "UserChannelController.hpp"
# include "Utility.hpp"

#define NICKNUM 1
#define USERNUM 2
#define PASSNUM 3
#define PRIVMSGNUM 4

class CommandHandler
{
	public:

		CommandHandler();
		~CommandHandler();
		
		// parsing
		static void NICK(User &user, std::vector<std::string> &params);
		static void PASS(User &user, std::vector<std::string> &params);
		static void USER(User &user, std::vector<std::string> &params);
		static void PRIVMSG(User &user, std::vector<std::string> &params);
		static void JOIN(User &user, std::vector<std::string> &params);
		static void PART(User &user, std::vector<std::string> &params);
		static void KICK(User &user, std::vector<std::string> &params);
		static void MODE(User &user, std::vector<std::string> &params);
		static void TOPIC(User &user, std::vector<std::string> &params);
		static void PING(User &user, std::vector<std::string> &params);
		// static void QUIT(User &user, std::vector<std::string> &params);
		static std::string MakeMessage(std::string mod, std::string path);
		static std::string MakeMessage(std::string mod, std::string path1, std::string path2);
		// excute
		static int CommandRun(User &user, std::string str);
		static void MSG(int fd, std::string message);

	private:
		
		// map과 함수포인터를 결합해서 사용
		void CommandInit(std::map<std::string, void(*)(User &user, std::vector<std::string> &param)> &commandMap);
		void CommandNumInit(std::map<std::string, int> &commandNum);
		void CommandMsgInit(std::map<std::string, std::string> &message);
		static std::map<std::string, void(*)(User &user, std::vector<std::string> &param)> _commandMap;
		static std::map<std::string, int> _commandNum;
		static std::map<std::string, std::string>  _message;

};


#endif /* ************************************************** COMMANDHANDLER_H */