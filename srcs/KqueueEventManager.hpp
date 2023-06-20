#ifndef KQUEUEEVENTMANAGER_HPP
# define KQUEUEEVENTMANAGER_HPP

# include <iostream>
# include <string>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <map>
# include <vector>

class KqueueEventManager
{

	public:
		KqueueEventManager();
		~KqueueEventManager();

	private:

};
void	Init_socket(int port, int &connectSocket);
void	Init_event(std::vector<struct kevent> changeList, int &kq, int &connectSocket);

#endif /* ********************************************** KQUEUEEVENTMANAGER_H */