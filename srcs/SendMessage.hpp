#ifndef SENDMESSAGE_HPP
# define SENDMESSAGE_HPP

# include <iostream>
# include <string>
# include <sys/socket.h> 
# include "User.hpp"

void Send(int fd, std::string str);


// 성공 메세지
void RPL_WELCOME(User &user);



// 실패 메세지
void ERR_NOUSERMODE(User &user);
void ERR_CHANNELISFULL(User &user);
void ERR_INVITEONLYCHAN(User &user);
void ERR_BADCHANNELKEY(User &user);


#endif /* ***************************************************** SENDMESSAGE_H */