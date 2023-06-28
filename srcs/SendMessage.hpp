#ifndef SENDMESSAGE_HPP
# define SENDMESSAGE_HPP

# include <iostream>
# include <string>
# include <sys/socket.h> 
# include "User.hpp"

void Send(int fd, std::string str);

void RPL_WELCOME(std::string serverName, User &user);


#endif /* ***************************************************** SENDMESSAGE_H */