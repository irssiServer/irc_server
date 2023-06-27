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
    commandMap["KICK"] = CommandHandler::KICK;
    commandMap["MODE"] = CommandHandler::MODE;
    // commandMap["INVITE"] = CommandHandler::INVITE;
    commandMap["TOPIC"] = CommandHandler::TOPIC;
    // commandMap["QUIT"] = CommandHandler::QUIT;
    commandMap["PING"] = CommandHandler::PING;
    // commandMap["PONG"] = CommandHandler::PONG;
    // commandMap["WHO"] = CommandHandler::WHO;
    // commandMap["LIST"] = CommandHandler::LIST;
    // commandMap["ERROR"] = CommandHandler::ERROR; 구현하기 애매 서버->클라 로  메세지를 보내는 경우가 크게 없음

    /////:irc.local 451 * PART :You have not registered. error 451 -> 계정 생성전 NICK PASS USER 를 제외한 명령어 사용때 발생 NICK 명령 후에는 * -> 입력한 NICK
    /////:irc.local 461 a PART :Not enough parameters. error 461 -> 파라미터 부족할때 무조건 우선순위인 에러코드 451보다 우선시됨
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
            if (tmp[0] == ':')
            {
                std::size_t len = str.find(':');
                std::string param;
                param.resize(str.size() - len - 1);
                for (std::size_t i = 0; i < str.size() - len - 1; i++)
                    param[i] = str[len + i + 1];
                params.push_back(param);
                break;
            }
            else
            {
                if (ss.fail())
                    break;
                params.push_back(tmp);
            }
            tmp.clear();
        }
        _commandMap[command](user, params);
        return (_commandNum[command]);
    }
    catch(const char *str)
    {
        throw str;
    }

    return (0);
}

void CommandHandler::USER(User &user, std::vector<std::string> &params)
{
    std::string tmp;
    if (params.size() < 4)
    {
        tmp = ":irc.local 461 " + user.Getbuf() + " USER :Not enough parameters.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() != -1)
    {
        tmp = ":irc.local 462 " + user.Getbuf() + " :You may not reregister";
        user.send(tmp);
        return ;
    }
}

void CommandHandler::NICK(User &user, std::vector<std::string> &params)
{
    std::string tmp;
    if (params.size() < 1)
    {
        tmp = ":irc.local 461 " + user.Getbuf() + " NICK :Not enough parameters.";
        user.send(tmp);
        return ;
    }
    if (UserChannelController::Instance().isNick(params[0]))
    {
        tmp = ":irc.local 433 * " + params[0] + " :Nickname is already in use.";
        user.send(tmp);
        return ;
    }
    if (!Check_nick(params[0]))
    {
        tmp = tmp = ":irc.local 433 * " + params[0] + " :Erroneous Nickname.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() > 0)
        user.SetNickname(params[0]);
    user.Setbuf(params[0]);
    
}


// 주체설정
void CommandHandler::JOIN(User &user, std::vector<std::string> &params)
{
    std::string tmp;
    if (params.size() < 1)
    {
        tmp = ":irc.local 461 " + user.Getbuf() + " JOIN :Not enough parameters.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() < 0)
    {
        tmp = ":irc.local 451 " + user.Getbuf() + " JOIN :You have not registered.";
        user.send(tmp);
        return ;
    }
    std::vector<std::string> channelName = Split(params[0], ',');
    std::vector<std::string> channelPass = Split(params[1], ',');
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
    std::string tmp;
    if (params.size() == 0)
    {
        tmp = ":irc.local 461 " + user.Getbuf() + " PASS :Not enough parameters.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() < 0)
    {
        tmp = ":irc.local 451 " + user.Getbuf() + " PASS :You have not registered.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() != -1)
    {
        tmp = ":irc.local 462 " + user.Getbuf() + " :You may not reregister";
        user.send(tmp);
        return ;
    }
}

void CommandHandler::MSG(int fd, std::string &message)
{
    write(fd, message.c_str(), message.size());
    write(fd, "\n", 1);
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
    std::string tmp;
    if (params.size() < 2)
    {
        std::string tmp;
        tmp = ":irc.local 461 " + user.Getbuf() + " PRIVMSG :Not enough parameters.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() < 0)
    {
        tmp = ":irc.local 451 " + user.Getbuf() + " PRIVMSG :You have not registered.";
        user.send(tmp);
        return ;
    }
    
    recv = Split(params[0], ',');
    params[1] = ":" + params[1];
    std::string tmp1;
    tmp1 = ":" + user.GetNickname() + "!" + user.GetUsername() + "@" + "127.0.0.1 PRIVMSG ";
    for (std::size_t i = 0; i < recv.size(); i++)
    {
        tmp1 = tmp1 + recv[i] + " " + params[1];
        if (recv[i][0] == '#')
        {
            if (UserChannelController::Instance().isChannel(recv[i]))
            {
                if (UserChannelController::Instance().FindChannel(recv[i]).isUser(user))
                    UserChannelController::Instance().FindChannel(recv[i]).send(tmp1);
                else
                {
                    tmp = ":irc.local 404 " + user.GetNickname() + recv[i] +  ":You cannot send external messages to this channel whilst the +n (noextmsg) mode is set.";
                    user.send(tmp);
                }
            }
            else
            {
                tmp = ":irc.local 404 " + user.GetNickname() + recv[i] + " :No such channel.";
                user.send(tmp);
            }
        }
        else
        {
            if (UserChannelController::Instance().isNick(recv[i]))
                UserChannelController::Instance().FindUser(recv[i]).send(tmp1);
            else
            {
                tmp = ":irc.local 401 " + user.GetNickname() + " " + params[1] + " :No such nick.";
                user.send(tmp);
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
    std::string tmp;
    if (params.size() == 0)
    {
        tmp = ":irc.local 461 " + user.Getbuf() + " PART :Not enough parameters.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() < 0)
    {
        tmp = ":irc.local 451 " + user.Getbuf() + " PART :You have not registered.";
        user.send(tmp);
        return ;
    }
    std::vector<std::string> channelNames = Split(params[0], ',');

    for (std::vector<std::string>::iterator iter = channelNames.begin(); iter != channelNames.end(); iter++)
    {
        user.leaveChannel(*iter);
    }
}

void CommandHandler::KICK(User &user, std::vector<std::string> &params)
{
    std::string tmp;
    if (params.size() < 1)
    {
        tmp = ":irc.local 461 " + user.Getbuf() + " KICK :Not enough parameters.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() < 0)
    {
        tmp = ":irc.local 451 " + user.Getbuf() + " KICK :You have not registered.";
        user.send(tmp);
        return ;
    }
    // <channel> <user> [<comment>]
    std::string channelName = params[0];
    std::string userName = params[1];
    std::string comment = params[2];

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
    int flag = ADD;
    int modes = 1; // [0] = 채널명, [1] = 설정할모드 플래그들, [2 ~] = 모드설정에 필요한 파라미터들
    int paramNum = 2;
    Channel channel;

    std::string mode = params[1];
    std::string tmp;
    if (params.size() == 0)
    {
        tmp = ":irc.local 461 " + user.Getbuf() + " MODE :Not enough parameters.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() < 0)
    {
        tmp = ":irc.local 451 " + user.Getbuf() + " MODE :You have not registered.";
        user.send(tmp);
        return ;
    }
    try
    {
        try
        {
            // channel = UserChannelController::Instance().FindChannel(channelName);
            channel = user.FindChannel(channelName);
        }
        catch(const char *str)
        {
            throw str;
        }
        
        for (size_t i = 0; i < params[modes].size(); i++)
        {
            if (params[modes][i] == '+')
                flag = ADD;
            else if (params[modes][i] == '-')
                flag = REMOVE;
            else
            {
                // 파라미터가 필수인 모드들
                if (params[modes][i] == 'i' || params[modes][i] == 't' || (params[modes][i] == 'l' && flag == REMOVE))
                {
                    if (params[modes][i] == 'i')
                        channel.ModeInvite(user, flag);
                    else if (params[modes][i] == 't')
                        channel.ModeTopic(user, flag);
                    else if (params[modes][i] == 'l')
                        channel.ModeLimite(user, flag, -1);
                }
                // 파라미터가 필요없는 모드들
                else if (params[modes][i] == 'o' || params[modes][i] == 'k' || (params[modes][i] == 'l' && flag == ADD))
                {
                    std::stringstream ss(params[paramNum]);

                    if (params[modes][i] == 'o')
                        channel.ModeOperator(user, flag, params[paramNum]);
                    else if (params[modes][i] == 'k')
                        channel.ModeKey(user, flag, params[paramNum]);
                    else if (params[modes][i] == 'l')
                        channel.ModeOperator(user, flag, params[paramNum]);
                    paramNum++;
                }
                else
                    throw "error: It's not mode";
            }
        } 
    }
    catch(const char *str)
    {
        std::cerr << str << std::endl;
    }
}

void CommandHandler::TOPIC(User &user, std::vector<std::string> &params)
{
    std::string tmp;
    std::vector<std::string> recv;
    if (params.size() == 0)
    {
        tmp = ":irc.local 461 " + user.Getbuf() + " TOPIC :Not enough parameters.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() < 0)
    {
        tmp = ":irc.local 451 " + user.Getbuf() + " TOPIC :You have not registered.";
        user.send(tmp);
        return ;
    }
    if (!UserChannelController::Instance().isChannel(params[0]))
    {
        tmp = ":irc.local 403 " + user.GetNickname() + params[0] + " :No such channel.";
        user.send(tmp);
        return ;
    }
    if (params.size() == 1)
    {
        if (UserChannelController::Instance().FindChannel(params[0]).GetTopic() == "")
        {
            tmp = ":irc.local 331 " + user.GetNickname() + params[0]  + " :No topic is set.";
            user.send(tmp);
        }
        else
        {
            // :irc.local 332 test #11 :hello
            // :irc.local 333 test #11 test!test1@127.0.0.1 :1687519622
            tmp = ":test 332 " + user.GetNickname() + " " + params[0] + " " + UserChannelController::Instance().FindChannel(params[0]).GetTopic();
            user.send(tmp);
        }
    }
    else
    { 
        if (UserChannelController::Instance().FindChannel(params[0]).isUser(user))
        {
            std::cout << "topic : |"<< params[1] << "|\n";
            UserChannelController::Instance().FindChannel(params[0]).SetTopic(params[1]);
            if (params[1][0] != ':')
                params[1] = ":" + params[1];
            tmp = ":" + user.GetNickname() + "!" + user.GetUsername() + "@" + "127.0.0.1 TOPIC " + params[0] + " " + params[1];
            UserChannelController::Instance().FindChannel(params[0]).send(tmp);
        }
        else
        {
            //보내는이가 채널안에없음 
            tmp = ":irc.local 442 " + user.GetNickname() + params[0] + " :You're not on that channel!";
            user.send(tmp);
        }
    }
}

void CommandHandler::PING(User &user, std::vector<std::string> &params)
{
    std::string tmp;
    if (params.size() == 0)
    {
        tmp = ":irc.local 461 " + user.Getbuf() + " PING :Not enough parameters.";
        user.send(tmp);
        return ;
    }
    if (user.GetFd() < 0)
    {
        tmp = ":irc.local 451 " + user.Getbuf() + " PING :You have not registered.";
        user.send(tmp);
        return ;
    }
    if (params[0] == "")
    {
        tmp = ":irc.local 409 " + user.GetNickname() + " :No origin specified.";
        user.send(tmp);
        return ;
    }
    tmp = "PONG " + params[0];
    user.send(tmp);
}

// void CommandHandler::QUIT(User &user, std::vector<std::string> &params)
// {
// }

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */