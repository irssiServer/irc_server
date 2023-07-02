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
    commandNum["QUIT"] = QUITNUM;
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
    commandMap["INVITE"] = CommandHandler::INVITE;
    commandMap["TOPIC"] = CommandHandler::TOPIC;
    commandMap["QUIT"] = CommandHandler::QUIT;
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
        // :irc.local 421 asdf ASDF :Unknown command
        throw "command not found";
    }
    try
    {
        while (!ss.fail() && !ss.eof())
        {
            std::string tmp;
            tmp.clear();
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
        }
        _commandMap[command](user, params);
        return (_commandNum[command]);
    }
    catch(const std::string str)
    {
        std::cout << str << std::endl;
        return -2;
    }

    return (0);
}

void CommandHandler::USER(User &user, std::vector<std::string> &params)
{
    std::string tmp;
    if (params.size() < 4)
    {
        ERR_NEEDMOREPARAMS(user, "USER");
        throw "";
    }
    if (user.GetFlag() != 1)
    {
        ERR_ALREADYREGISTERED(user);
        throw "";
    }
}

void CommandHandler::NICK(User &user, std::vector<std::string> &params)
{
    if (params.size() < 1)
    {
        ERR_NEEDMOREPARAMS(user, "NICK");
        throw "";
    }
    if (UserChannelController::Instance().isNick(params[0]))
    {
        ERR_NICKNAMEINUSE(user, params[0]);
        throw "";
    }
    if (!Check_nick(params[0]))
    {
        ERR_ERRONEUSNICKNAME(user, params[0]);
        throw "";
    }
    user.SetNickname(params[0]);
}

void CommandHandler::JOIN(User &user, std::vector<std::string> &params)
{
    if (params.size() < 1)
    {
        ERR_NEEDMOREPARAMS(user, "JOIN");
        throw "";
    }
    if (user.GetFlag() == 1)
    {
        ERR_NOTREGISTERED(user);
        throw "";
    }
    std::vector<std::string> channelName = Split(params[0], ',');
    std::vector<std::string> channelPass = Split(params[1], ',');
    std::map<std::string, std::string> channelMap; // first = 채널이름, second = 채널 비밀번호

    std::vector<std::string>::iterator passIter = channelPass.begin();
    for (std::vector<std::string>::iterator iter = channelName.begin(); iter != channelName.end(); iter++) // 채널명 = key, 비밀번호 = val
    {
        if (passIter != channelPass.end())
            channelMap[*iter] = *passIter++;
        else
            channelMap[*iter];
    }
    for (std::map<std::string, std::string>::iterator iter = channelMap.begin(); iter != channelMap.end(); iter++)
    {
        try
        {
            if (!UserChannelController::Instance().isChannel(iter->first)) // 채널이 없을때는 채널 생성 및 권한 부여 
            {
                UserChannelController::Instance().AddChannel(iter->first, t_ChannelMode());
                user.JoinChannel(&UserChannelController::Instance().FindChannel(iter->first), iter->second);
                UserChannelController::Instance().FindChannel(iter->first).SetOper(user);
            }
            else
                user.JoinChannel(&UserChannelController::Instance().FindChannel(iter->first), iter->second);
        }
        catch(const std::string str)
        {
            throw str;
        }
    }
}

void CommandHandler::PASS(User &user, std::vector<std::string> &params)
{
    if (params.size() == 0)
    {
        ERR_NEEDMOREPARAMS(user, "PASS");
        throw "";
    }
    if (user.GetFlag() != 1)
    {
        ERR_ALREADYREGISTERED(user);
        throw "";
    }
}

void CommandHandler::PRIVMSG(User &user, std::vector<std::string> &params)
{
    std::vector<std::string> recv;
    std::string buf;
    int flag = 0;
    if (params.size() < 2)
    {
        ERR_NEEDMOREPARAMS(user, "PRIVMSG");
        throw "";
    }
    if (user.GetFlag() == 1)
    {
        ERR_NOTREGISTERED(user);
        throw "";
    }
    
    recv = Split(params[0], ',');
    params[1] = ":" + params[1];
    std::string tmp1;
    tmp1 = user.GetNickHostmask() + " PRIVMSG ";
    for (std::size_t i = 0; i < recv.size(); i++)
    {
        try
        {
            tmp1 = tmp1 + recv[i] + " " + params[1];
            if (recv[i][0] == '#')
            {
                if (UserChannelController::Instance().isChannel(recv[i]))
                {
                    if (UserChannelController::Instance().FindChannel(recv[i]).isUser(user))
                        UserChannelController::Instance().FindChannel(recv[i]).SendUsers(tmp1, user);
                    else
                    {
                        ERR_CANNOTSENDTOCHAN(user, recv[i]);
                        throw "";
                    }
                }
                else
                {
                    ERR_NOSUCHCHANNEL(user, recv[i]);
                    throw "";
                }
            }
            else
            {
                if (UserChannelController::Instance().isNick(recv[i]))
                    Send(UserChannelController::Instance().FindUser(recv[i]).GetFd(), tmp1);
                else
                {
                    ERR_NOSUCHNICK(user, recv[i]);
                    throw "";
                }
            }
        }
        catch(const std::string str)
        {
            flag = 1;
        }
    }
    if (flag == 1)
        throw "";
}


void CommandHandler::PART(User &user, std::vector<std::string> &params)
{
    if (params.size() == 0)
    {
        ERR_NEEDMOREPARAMS(user, "PART");
        throw "";
    }
    if (user.GetFlag() == 1)
    {
        ERR_NOTREGISTERED(user);
        throw "";
    }
    std::vector<std::string> channelNames = Split(params[0], ',');

    for (std::vector<std::string>::iterator iter = channelNames.begin(); iter != channelNames.end(); iter++)
    {
        if (!UserChannelController::Instance().isChannel(*iter))
        {
            ERR_CANNOTSENDTOCHAN(user, *iter);
            throw "";
        }
        if (!UserChannelController::Instance().FindChannel(*iter).isUser(user))
        {
            ERR_NOTONCHANNEL(user, *iter);
            throw "";
        }
        //:gyyu!root@127.0.0.1 PART :#1
        std::string str = ":gyyu!root@127.0.0.1 PART :#1";
        UserChannelController::Instance().FindChannel(*iter).SendUsers(str);
        user.leaveChannel(*iter);
        return;
    }
}

void CommandHandler::KICK(User &user, std::vector<std::string> &params)
{
    int flag = 0;
    if (params.size() < 2)
    {
        ERR_NEEDMOREPARAMS(user, "KICK");
        throw "";
    }
    if (user.GetFlag() == 1)
    {
        ERR_NOTREGISTERED(user);
        throw "";
    }
    if (!UserChannelController::Instance().isChannel(params[0]))
    {
        ERR_NOSUCHCHANNEL(user, params[0]);
        throw "";
    }
    std::vector<std::string> recv;
    recv = Split(params[1], ',');
    for (std::size_t i = 0; i < recv.size(); i++)
    {
        try
        {
            if (!UserChannelController::Instance().FindChannel(params[0]).isUser(recv[i]))
            {
                ERR_NOSUCHNICK(user, recv[i]);
                throw "";
            }
            else
            {
                if (!UserChannelController::Instance().FindChannel(params[0]).isUser(user.GetNickname()))
                {
                    ERR_NOTONCHANNEL(user, params[0]);
                    throw "";
                }
                else
                {
                    std::string tmp;
                    tmp = user.GetNickHostmask() + " KICK " + params[0] + " " + recv[i] + " :";
                    user.FindChannel(params[0]).KickUser(user, recv[i]);
                    if (params.size() == 2)
                        tmp = tmp + user.GetNickname();
                    else
                        tmp = tmp + params[2];
                    UserChannelController::Instance().FindChannel(params[0]).SendUsers(tmp);
                }
            }
        }
        catch(const std::exception& e)
        {
            flag = 1;
        }
    }
    if (flag == 1)
        throw "";
    
}


void CommandHandler::MODE(User &user, std::vector<std::string> &params)
{
    if (params.size() == 0)
    {
        ERR_NEEDMOREPARAMS(user, "MODE");
        throw "";
    }
    if (user.GetFlag() == 1)
    {
        ERR_NOTREGISTERED(user);
        throw "";
    }
    if (!UserChannelController::Instance().isChannel(params[0]))
    {
        ERR_NOSUCHCHANNEL(user, params[0]);
        throw "";
    }
    if (params.size() == 1)
    {
        RPL_CHANNELMODEIS(user, UserChannelController::Instance().FindChannel(params[0]));
        return ;
    }
    std::string channelName = params[0];
    int flag = ADD;
    int modes = 1; // [0] = 채널명, [1] = 설정할모드 플래그들, [2 ~] = 모드설정에 필요한 파라미터들
    int paramNum = 2;
	bool errorFlag = false;

    std::string mode = params[1];
	if (UserChannelController::Instance().isNick(channelName))
	{
		ERR_NOUSERMODE(user);
		return ;
	}
		Channel &channel = UserChannelController::Instance().FindChannel(channelName);
	for (size_t i = 0; i < params[modes].size(); i++)
	{
		if (params[modes][i] == '+')
			flag = ADD;
		else if (params[modes][i] == '-')
			flag = REMOVE;
		else
		{
			// 파라미터가 필요없는 모드들
			if (params[modes][i] == 'i' || params[modes][i] == 't' || (params[modes][i] == 'l' && flag == REMOVE))
			{
				if (params[modes][i] == 'i')
					channel.ModeInvite(user, flag);
				else if (params[modes][i] == 't')
					channel.ModeTopic(user, flag);
				else if (params[modes][i] == 'l')
					channel.ModeLimite(user, flag, -1);
			}
			// 파라미터가 필수인 모드들
			else if (params[modes][i] == 'o' || params[modes][i] == 'k' || (params[modes][i] == 'l' && flag == ADD))
			{
				if (params[modes][i] == 'o')
					channel.ModeOperator(user, flag, params[paramNum]);
				else if (params[modes][i] == 'k')
					channel.ModeKey(user, flag, params[paramNum]);
				else if (params[modes][i] == 'l')
					channel.ModeOperator(user, flag, params[paramNum]);
				paramNum++;
			}
			else
			{
				ERR_UNKNOWNMODE(user, params[modes][i]);
				errorFlag = true;
			}
		}
	} 
	if (errorFlag == true)
		throw "";
}

void CommandHandler::TOPIC(User &user, std::vector<std::string> &params)
{
    std::string tmp;
    std::vector<std::string> recv;
    if (params.size() == 0)
    {
        ERR_NEEDMOREPARAMS(user, "TOPIC");
        throw "";
    }
    if (user.GetFlag() == 1)
    {
        ERR_NOTREGISTERED(user);
        throw "";
    }
    if (!UserChannelController::Instance().isChannel(params[0]))
    {
        ERR_NOSUCHCHANNEL(user, params[0]);
        throw "";
    }

    if (params.size() == 1)
    {
        if (UserChannelController::Instance().FindChannel(params[0]).GetTopic() == "")
        {
            RPL_NOTOPIC(user, params[0]);
            throw "";
        }
        else
        {
            tmp = ":"+ UserChannelController::Instance().GetServerName() + " "
            + user.GetNickname() + " " + params[0] + " "
            + UserChannelController::Instance().FindChannel(params[0]).GetTopic();
            Send(user.GetFd(), tmp);
        }
    }
    else
    { 
        if (UserChannelController::Instance().FindChannel(params[0]).isUser(user))
        {
            UserChannelController::Instance().FindChannel(params[0]).SetTopic(params[1], user);
            if (params[1][0] != ':')
                params[1] = ":" + params[1];
            tmp = user.GetNickHostmask() + " TOPIC " + params[0] + " " + params[1];
            UserChannelController::Instance().FindChannel(params[0]).SendUsers(tmp);
        }
        else
        {
            ERR_NOTONCHANNEL(user, params[0]);
            throw "";
        }
    }
}

void CommandHandler::PING(User &user, std::vector<std::string> &params)
{
    std::string tmp;
    if (params.size() == 0)
    {
        ERR_NEEDMOREPARAMS(user, "PING");
        throw "";
    }
    if (user.GetFlag() == 1)
    {
        ERR_NOTREGISTERED(user);
        throw "";
    }
    if (params[0] == "")
    {
        ERR_NOORIGIN(user);
        throw "";
    }
    tmp = ":" + UserChannelController::Instance().GetServerName() + " PONG " + params[0];
    Send(user.GetFd(), tmp);
}

void CommandHandler::QUIT(User &user, std::vector<std::string> &params)
{
    (void)params;
    UserChannelController::Instance().RemoveUser(user.GetFd());
}

void CommandHandler::INVITE(User &user, std::vector<std::string> &params)
{
    if (user.GetFlag() == 1)
    {
        ERR_NOTREGISTERED(user);
        throw "";
    }
    if (params.size() < 2)
    {
        RPL_INVITELIST(user);
        RPL_ENDOFINVITELIST(user);
        throw "";
    }
    if (!UserChannelController::Instance().isChannel(params[1]))
    {
        ERR_NOSUCHCHANNEL(user, params[1]); //403
        throw "";
    }
    if (!UserChannelController::Instance().isNick(params[0]))
    {
        ERR_NOSUCHNICK(user, params[0]);
        throw "";
    }
    if (!UserChannelController::Instance().FindChannel(params[1]).isUser(user))
    {
        ERR_NOTONCHANNEL(user, params[1]);
        throw "";
    }
    if (UserChannelController::Instance().FindChannel(params[1]).isUser(params[0]))
    {
        ERR_USERONCHANNEL(user, params[0], params[1]);
        throw "";
    }
    try
    {
        UserChannelController::Instance().FindChannel(params[1]).InviteUser(user, params[0]);
        RPL_INVITING(user, params[0], params[1]);
        Send(UserChannelController::Instance().FindUser(params[0]).GetFd(), user.GetNickHostmask() + " INVITE " + params[0] + " :" + params[1]);
    }
    catch(const std::string str)
    {
        throw "";
    }
    
    //서버에 있던사람 :irc.local NOTICE #1 :*** b invited c into the channel
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */