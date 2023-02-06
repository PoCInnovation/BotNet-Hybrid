SRC		 	=	src/client.cpp			\
				src/server.cpp
SRC_UPNP 	=	src/upnp.cpp			\
				src/upnp_init.cpp		\
				src/upnp_actions.cpp
OBJ			=	$(SRC:.cpp=.o)
OBJ_UPNP	=	$(SRC_UPNP:.cpp=.o)
NAME 		= 	client_server
_NAME 		= 	upnp

CPP_FLAGS	=	-std=c++20
UPNP_FLAGS	=	-lnpupnp -lupnpp
INCLUDE		=	include/

all:
	g++ -c $(SRC) -I $(INCLUDE) $(CPP_FLAGS)
	mv *.o src
	g++ $(OBJ) -o $(NAME)

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
