#include "CommandHandler.hpp"

std::map<std::string, void(*)(User &user, std::vector<std::string> &param)> CommandHandler::_commandMap;
std::map<std::string, int> CommandHandler::_commandNum;

int Check_nick(std::string nick)
{
    if (!isalpha(nick[0]))
        return (0);
    for (std::size_t i = 0; i < nick.size(); i++)
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
void CommandHandler::CommandInit(std::map<std::string, void(*)(User &user, std::vector<std::string> &param)> &commandMap)
{
    commandMap["NICK"] = CommandHandler::NICK;
    commandMap["PASS"] = CommandHandler::PASS;  
    commandMap["USER"] = CommandHandler::USER;
    commandMap["JOIN"] = CommandHandler::JOIN;
    commandMap["PART"] = CommandHandler::PART;
    commandMap["PRIVMSG"] = CommandHandler::PRIVMSG;
    commandMap["KICK"] = CommandHandler::NICK;
    commandMap["MODE"] = CommandHandler::MODE;
    // commandMap["INVITE"] = CommandHandler::NICK;
    // commandMap["TOPIC"] = CommandHandler::NICK;
    // commandMap["QUIT"] = CommandHandler::NICK;
    // commandMap["PING"] = CommandHandler::NICK;
    // commandMap["PONG"] = CommandHandler::NICK;
    // commandMap["WHO"] = CommandHandler::NICK;
    // commandMap["LIST"] = CommandHandler::NICK;
    // commandMap["ERROR"] = CommandHandler::NICK;
}

int CommandHandler::CommandRun(User &user, std::string str)
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
        _commandMap[command](user, params);
        return (_commandNum[command]);
    }
    catch(const char *str)
    {
        std::cerr << str << '\n';
        throw str;
    }

    return (0);
}

void CommandHandler::USER(User &user, std::vector<std::string> &params)
{
    if (user.GetFd() != -1)
    {
        //ERROR 462;
        return;
    }
    if (params.size() < 4)
        throw "Insufficient argvs";
    // user.SetHostname();
    // user.SetUsername();
    // user.Setrealname();
}

void CommandHandler::NICK(User &user, std::vector<std::string> &params)
{
    if (params.size() < 1)
        throw "not found nickname";
    if (UserChannelController::Instance().isNick(params[0]))
        throw "already nickname";
    if (user.GetFd() > 0)
    {
        if (!Check_nick(params[0]))
        {
            //닉네임 유효성
            return ;
        }
        user.SetNickname(params[0]);
    }
}


// 주체설정
void CommandHandler::JOIN(User &user, std::vector<std::string> &params)
{
    if (params.size() < 1)
        throw "paramiter is short";
    std::vector<std::string> channelName = ft_Split(params[0], ',');
    std::vector<std::string> channelPass = ft_Split(params[1], ',');
    std::map<std::string, std::string> channelMap; // first = 채널이름, second = 채널 비밀번호

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
        if (!UserChannelController::Instance().isChannel(iter->first)) // 채널이 없을때는 채널 생성
            UserChannelController::Instance().AddChannel(iter->first, t_ChannelMode());
        
        user.JoinChannel(&UserChannelController::Instance().FindChannel(iter->first), iter->second);
    }
}

void CommandHandler::PASS(User &user, std::vector<std::string> &params)
{
    if (user.GetFd() != -1)
    {
        //ERROR 462; 적용되지않음
        return;
    }
    if (params.size() != 1)
        throw "check PASS arg";
}

void CommandHandler::MSG(int fd, std::vector<std::string> &message)
{
    for (std::size_t i = 0; i < message.size() ; i++)
        write(fd, message[i].c_str(), message[i].size());
}

// PRIVMSG 채널 메세지 보내기
                // 클라이언트가 ㅁ 채널에 "a" 를 전달
                // 서버 ㅁ 채널의 유저들에게 "a"를 전달해야됌
                // ㅁ 채널 찾는다. PRIVMSG(a ,ㅁ, "a");
                //ㅁ.sendMessage("a");

                //ㅁ.sendMessage()
                // {
                //     while ()
                //         a.send()
                // }

void CommandHandler::PRIVMSG(User &user, std::vector<std::string> &params)
{
    std::vector<std::string> recv;
    if (params.size() < 2)
        throw "Not enough parameters";
    recv = ft_Split(params[0], ',');
    std::vector<std::string> msg;
    std::string tmp;
    tmp = ":" + user.GetNickname() + "!" + user.GetUsername() + "@" + "127.0.0.1 PRIVMSG ";
    msg.push_back(tmp);
    msg.push_back(params[1]);
    for (std::size_t i = 0; i < recv.size(); i++)
    {
        if (recv[i][0] == '#')
        {
            if (UserChannelController::Instance().isChannel(recv[i]))
            {
                if (UserChannelController::Instance().FindChannel(recv[i]).isUser(user))
                    UserChannelController::Instance().FindChannel(recv[i]).send(msg);
                else
                {
                    //보내는이가 채널안에없음 
                    std::cout << recv[i] << "->" << user.GetNickname() << " : 채널안에 유저가없다\n";
                }
            }
            else
            {
                //메세지를 받는 채널이 없음
                std::cout << recv[i] << " : 메세지를 받을 채널이 없다\n";
            }
        }
        else
        {
            if (UserChannelController::Instance().isNick(recv[i]))
                user.send(UserChannelController::Instance().FindUser(recv[i]), msg);
            else
            {
                user.send()
                //받는이가없음
                std::cout << recv[i] <<  " : 받는이가 없다\n";
            } 
        }
    }
    // if (send[0] == '#')
    // else
    // 클라이언트가 프라이빗메시지로 보냈을때 확인
    // 누가 누구에게 메시지
    // user.
    // channel.
}


void CommandHandler::PART(User &user, std::vector<std::string> &params)
{
    if (params.size() < 1)
        throw "Not enough parameters";

    std::vector<std::string> channelNames = Split(params[0], ',');

    for (std::vector<std::string>::iterator iter = channelNames.begin(); iter != channelNames.end(); iter++)
    {
        user.leaveChannel(*iter);
    }
}

void CommandHandler::KICK(User &user, std::vector<std::string> &params)
{
// <channel> <user> [<comment>]
    std::string channelName = params[0];
    std::string userName = params[1];
    std::string comment = params[2];

    if (params.size() < 1)
        throw "Not enough parameters";

    try
    {
        user.FindChannel(channelName).KickUser(user, userName, comment);    
    }
    catch(const char *str)
    {
        std::cout << str << std::endl;
    }
    
}

void CommandHandler::MODE(User &user, std::vector<std::string> &params)
{
    std::string channelName = params[0];
    std::string mode = params[1];
    
    try
    {
        Channel &channel = UserChannelController::Instance().FindChannel(params[0]);
        // if (params[0])
    }
    catch(const char *str)
    {
        std::cerr << str << std::endl;
    }
    
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */