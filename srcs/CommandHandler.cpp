#include "CommandHandler.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CommandHandler::CommandHandler() {}
CommandHandler::~CommandHandler() {}


/*
** --------------------------------- METHODS ----------------------------------
*/

bool CommandHandler::NICK(std::string str)
{
    std::stringstream ss(str);
    std::string tmp;

    ss >> tmp;
    tmp.clear();
    ss >> tmp;
    if (ss.fail() && tmp.empty() && UserChannelController::Instance().isNick(tmp))
        return false;
    return true;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */