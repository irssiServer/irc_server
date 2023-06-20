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
    std::cout << "check |" << str <<"|" << "\n"; 
    std::cout <<  UserChannelController::Instance().isNick(str) << "\n";
    if (!UserChannelController::Instance().isNick(str))
        return false;
    return true;
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */