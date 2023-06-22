NAME= ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
RM= rm -f

SOURCE=	main.cpp Channel.cpp User.cpp UserChannelController.cpp CommandHandler.cpp KqueueEventManager.cpp
SRCDIR= ./srcs

SOURCES = $(addprefix $(SRCDIR)/,$(SOURCE))
OBJECTS = $(SOURCES:.cpp=.o)

$(NAME) : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJECTS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	$(RM) $(OBJECTS)
fclean : clean
	$(RM) $(NAME)
all : $(NAME)
re : fclean all
.PHONY: clean fclean all re
