SRC		 	=	main.cpp	\
				TypeGuesser.cpp	\
				upnp.cpp	\
				upnp_init.cpp	\
				upnp_actions.cpp	\
				Bot.cpp

SRC_UPNP 	=	upnp.cpp			\
				upnp_init.cpp		\
				upnp_actions.cpp
SRC := $(addprefix src/, $(SRC))
SRC_UPNP := $(addprefix src/, $(SRC))
OBJ			=	$(SRC:.cpp=.o)
OBJ_UPNP	=	$(SRC_UPNP:.cpp=.o)
NAME 		= 	client_server
_NAME 		= 	upnp

CPP_FLAGS	=	-std=c++20
UPNP_FLAGS	=	-lnpupnp -lupnpp
INCLUDE		=	include/

%.o: %.cpp
	g++ -c $< $(CPP_FLAGS) -I $(INCLUDE) -o $@


all: $(OBJ)
	g++ $(OBJ) -o $(NAME) $(UPNP_FLAGS)

upnp:
	g++ -c $(SRC_UPNP) -I $(INCLUDE) $(CPP_FLAGS)
	mv *.o src
	g++ $(OBJ_UPNP) -o $(_NAME) $(UPNP_FLAGS)

clean:
	rm -f $(OBJ)
	rm -f $(OBJ_UPNP)

fclean: clean
	rm -f $(NAME)
	rm -f $(_NAME)

re: fclean all
