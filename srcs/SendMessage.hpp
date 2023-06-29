#ifndef SENDMESSAGE_HPP
# define SENDMESSAGE_HPP

# include <iostream>
# include <string>
# include <sys/socket.h> 
# include "User.hpp"

void Send(int fd, std::string str);


// 성공 메세지
void RPL_WELCOME(std::string &serverName, User &user);



// 실패 메세지
void ERR_CHANNELISFULL(std::string &serverName, User &user);
void ERR_INVITEONLYCHAN(std::string &serverName, User &user);
void ERR_BADCHANNELKEY(std::string &serverName, User &user);


#endif /* ***************************************************** SENDMESSAGE_H */