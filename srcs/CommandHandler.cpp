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
    commandMap["PASS"] = CommandHandler::PASS;  
    commandMap["USER"] = CommandHandler::NICK; 
    commandMap["JOIN"] = CommandHandler::NICK;
    commandMap["PART"] = CommandHandler::NICK;
    commandMap["PRIVMSG"] = CommandHandler::PRIVMSG;
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
    return (1);
}

void CommandHandler::USER(std::vector<std::string> &params)
{
    if (params.size() < 4)
        throw "Insufficient argvs";
    // if (params[2] != server_name)
    //     throw "check server";
    if (params[3][0] != ':')
        throw "check realname";
}

void CommandHandler::NICK(std::vector<std::string> &params)
{
    if (params.size() != 1)
        throw "not found nickname";
    if (UserChannelController::Instance().isNick(params[0]))
        throw "already nickname";
}

void CommandHandler::PASS(std::vector<std::string> &params)
{
    if (params.size() != 1)
        throw "check PASS arg";
}

void CommandHandler::PRIVMSG(std::vector<std::string> &params)
{
    if (params.size() < 1)
        throw ""

}
/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */