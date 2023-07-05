NAME= ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g #-fsanitize=address -g3
RM= rm -f

SOURCE=	main.cpp Channel.cpp User.cpp UserChannelController.cpp CommandHandler.cpp KqueueEventManager.cpp Utility.cpp SendMessage.cpp
SRCDIR= ./srcs

SOURCES = $(addprefix $(SRCDIR)/,$(SOURCE))
OBJECTS = $(SOURCES:.cpp=.o)

$(NAME) : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJECTS) -g

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	$(RM) $(OBJECTS)
fclean : clean
	$(RM) $(NAME)
all : $(NAME)
re : fclean all
up : all
	./ircserv 6667 123
rup : re
	make clean
	./ircserv 6667 123
.PHONY: clean fclean all re
