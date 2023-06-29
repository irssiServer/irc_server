#ifndef SENDMESSAGE_HPP
# define SENDMESSAGE_HPP

# include <iostream>
# include <string>
# include <sys/socket.h> 
# include "User.hpp"
# include "UserChannelController.hpp"
void Send(int fd, std::string str);


// 성공 메세지
void RPL_WELCOME(User &user);



// 실패 메세지
void ERR_CHANNELISFULL(User &user);
void ERR_INVITEONLYCHAN(User &user);
void ERR_BADCHANNELKEY(User &user);
void ERR_NEEDMOREPARAMS(User &user, std::string command); //461
void ERR_ALREADYREGISTERED(User &user); //462
void ERR_NICKNAMEINUSE(User &user, std::string nick); //433
void ERR_ERRONEUSNICKNAME(User &user, std::string nick); //432
void ERR_NOTREGISTERED(User &user); //451
void ERR_CANNOTSENDTOCHAN(User &user, std::string channel); //404
void ERR_NOSUCHCHANNEL(User &user, std::string channel); //403
void ERR_NOSUCHNICK(User &user, std::string nick); //401
void RPL_NOTOPIC(User &user, std::string channel); // 331
void ERR_NOTONCHANNEL(User &user, std::string channel); //442
void ERR_NOORIGIN(User &user); //409

#endif /* ***************************************************** SENDMESSAGE_H */
