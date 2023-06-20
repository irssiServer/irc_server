#include "CommandHandler.hpp"


std::map<std::string, void(*)(std::vector<std::string> &param)> CommandHandler::_commandMap;


/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CommandHandler::CommandHandler() 
{
    CommandInit(_commandMap);
}

CommandHandler::~CommandHandler() {}


/*
** --------------------------------- METHODS ----------------------------------
*/



void CommandHandler::CommandInit(std::map<std::string, void(*)(std::vector<std::string> &param)> &commandMap)
{
    commandMap["NICK"] = CommandHandler::NICK;
    commandMap["PASS"] = CommandHandler::NICK;  
    commandMap["USER"] = CommandHandler::NICK; 
//    -----------------------------------------
    commandMap["JOIN"] = CommandHandler::NICK;
    commandMap["PART"] = CommandHandler::NICK;
    commandMap["PRIVMSG"] = CommandHandler::NICK; 
    commandMap["KICK"] = CommandHandler::NICK;   
    commandMap["MODE"] = CommandHandler::NICK; 
    commandMap["INVITE"] = CommandHandler::NICK; 
    commandMap["TOPIC"] = CommandHandler::NICK; 
    commandMap["QUIT"] = CommandHandler::NICK; 
    commandMap["PING"] = CommandHandler::NICK; 
    commandMap["PONG"] = CommandHandler::NICK;
    commandMap["WHO"] = CommandHandler::NICK; 
    commandMap["LIST"] = CommandHandler::NICK; 
    commandMap["ERROR"] = CommandHandler::NICK; 
   
    



}

int CommandHandler::CommandRun(std::string str)
{
    std::stringstream ss(str);
    std::vector<std::string> params;
    std::string command;

    ss >> command;
    if (ss.fail() || command.empty())
        throw "";
    if (_commandMap[command] == NULL) 
        throw "command not found";
    try
    {
        while (!ss.fail() || !ss.eof())
        {
            std::string tmp;
            ss >> tmp;
            params.push_back(tmp);
            tmp.clear();
        }
        _commandMap[command](params);
    }
    catch(const std::string& str)
    {
        std::cerr << str << '\n';
    }
}

void CommandHandler::NICK(std::vector<std::string> &params)
{
    if (params.size() != 1)
        throw "not found nickname";
    if (UserChannelController::Instance().isNick(params[0]))
        throw "already nickname";
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */