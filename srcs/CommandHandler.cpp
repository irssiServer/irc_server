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
    commandMap["JOIN"] = CommandHandler::JOIN;

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
    return 0;// command num
}

void CommandHandler::NICK(std::vector<std::string> &params)
{
    if (params.size() != 1)
        throw "not found nickname";
    if (UserChannelController::Instance().isNick(params[0]))
        throw "already nickname";
}


// 주체설정
void CommandHandler::JOIN(std::vector<std::string> &params)
{
    User user; // 임시생성 매개변수로 받아올것
    if (params.size() < 1)
        throw "paramiter is short";
    std::vector<std::string> channelName = Split(params[0], ',');
    std::vector<std::string> channelPass = Split(params[1], ',');
    std::map<std::string, std::string> channelMap;

    std::vector<std::string>::iterator passIter = channelPass.begin();
    for (std::vector<std::string>::iterator iter = channelName.begin(); iter != channelName.end(); iter++)
    {
        if (passIter != channelPass.end())
        {
            channelMap[*iter] = *passIter++;
        }
        else
            channelMap[*iter];
    }
    if (channelName.empty())
        throw "paramiter is short";

    for (std::map<std::string, std::string>::iterator iter = channelMap.begin(); iter != channelMap.end(); iter++)
        {
            if (UserChannelController::Instance().isChannel(iter->first))
            {
                // 생성된 방에 입장
            }
            else
            {
                // 방을 생성후 입장(운영자 권한부여)
                UserChannelController::Instance().AddChannel(iter->first, t_ChannelMode());
                user.JoinChannel(&UserChannelController::Instance().FindChannel(iter->first), iter->second);
            }
        }
    }
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */