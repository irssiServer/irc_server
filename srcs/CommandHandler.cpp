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
        while (!ss.fail())
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