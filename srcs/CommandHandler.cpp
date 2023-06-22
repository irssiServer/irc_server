#include "CommandHandler.hpp"

std::map<std::string, void(*)(std::vector<std::string> &param, User &user)> CommandHandler::_commandMap;
std::map<std::string, int> CommandHandler::_commandNum;

int Check_nick(std::string nick)
{
    if (!isalpha(nick[0]))
        return (0);
    for (int i = 0; i < nick.size(); i++)
    {
        if (!isalpha(nick[i]) && !isdigit(nick[i]))
            return (0);
    }
    return (1);
}

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CommandHandler::CommandHandler() 
{
    CommandInit(_commandMap);
    CommandNumInit(_commandNum);
}

CommandHandler::~CommandHandler() {}


/*
** --------------------------------- METHODS ----------------------------------
*/

void CommandHandler::CommandNumInit(std::map<std::string, int> &commandNum)
{
    commandNum["NICK"] = NICKNUM;
    commandNum["PASS"] = PASSNUM;
    commandNum["USER"] = USERNUM;
    // commandNum["JOIN"] = JOINNUM;
    // commandNum["PART"] = PARTNUM;
    commandNum["PRIVMSG"] = PRIVMSGNUM;
    // commandNum["KICK"] = KICKNUM;
    // commandNum["MODE"] = MODENUM;
    // commandNum["INVITE"] = INVITENUM;
    // commandNum["TOPIC"] = TOPICNUM;
    // commandNum["QUIT"] = QUITNUM;
    // commandNum["PING"] = PINGNUM;
    // commandNum["PONG"] = PONGNUM;
    // commandNum["WHO"] = WHONUM;
    // commandNum["LIST"] = LISTNUM;
    // commandNum["ERROR"] = ERRORNUM;
}
void CommandHandler::CommandInit(std::map<std::string, void(*)(std::vector<std::string> &param, User &user)> &commandMap)
{
    commandMap["NICK"] = CommandHandler::NICK;
    commandMap["PASS"] = CommandHandler::PASS;  
    commandMap["USER"] = CommandHandler::USER;
    // commandMap["JOIN"] = CommandHandler::NICK;
    // commandMap["PART"] = CommandHandler::NICK;
    commandMap["PRIVMSG"] = CommandHandler::PRIVMSG;
    // commandMap["KICK"] = CommandHandler::NICK;
    // commandMap["MODE"] = CommandHandler::NICK;
    // commandMap["INVITE"] = CommandHandler::NICK;
    // commandMap["TOPIC"] = CommandHandler::NICK;
    // commandMap["QUIT"] = CommandHandler::NICK;
    // commandMap["PING"] = CommandHandler::NICK;
    // commandMap["PONG"] = CommandHandler::NICK;
    // commandMap["WHO"] = CommandHandler::NICK;
    // commandMap["LIST"] = CommandHandler::NICK;
    // commandMap["ERROR"] = CommandHandler::NICK;
    
   
    



}

int CommandHandler::CommandRun(std::string str, User &user)
{
    std::stringstream ss(str);
    std::vector<std::string> params;
    std::string command;

    ss >> command;
    if (ss.fail() || command.empty())
        throw "";
    if (_commandMap[command] == NULL)
    {
        throw "command not found";
    }
    try
    {
        while (!ss.fail() && !ss.eof())
        {
            std::string tmp;
            ss >> tmp;
            params.push_back(tmp);
            tmp.clear();
        }
        _commandMap[command](params, user);
        return (_commandNum[command]);
    }
    catch(const char *str)
    {
        std::cerr << str << '\n';
    }
    return (0);
}

void CommandHandler::USER(std::vector<std::string> &params, User &user)
{
    if (user.GetFd() != -1)
    {
        //ERROR 462;
        return;
    }
    if (params.size() < 4)
        throw "Insufficient argvs";
}

void CommandHandler::NICK(std::vector<std::string> &params, User &user)
{
    if (params.size() < 1)
        throw "not found nickname";
    if (UserChannelController::Instance().isNick(params[0]))
        throw "already nickname";
}

void CommandHandler::PASS(std::vector<std::string> &params, User &user)
{
    if (user.GetFd() != -1)
    {
        //ERROR 462;
        return;
    }
    if (params.size() != 1)
        throw "check PASS arg";
}

void CommandHandler::PRIVMSG(std::vector<std::string> &params, User &user)
{
    std::string msg;
    std::vector<std::string> recv;
    if (params.size() < 2)
        throw "Not enough parameters";
    recv = Split(params[0]);
    msg = params[1];
    for (int i = 0; i < recv.size(); i++)
    {
        if (Check_nick(recv[i]))
        {
            //ERROR NICK
            return ;
        }
    }
    // if (send[0] == '#')
    // else
    // 클라이언트가 프라이빗메시지로 보냈을때 확인
    // 누가 누구에게 메시지
    // user.
    // channel.
}




/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */