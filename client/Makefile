CLIENT = client
SERVER = server
BINARY = binary

all:
	g++ ${CLIENT}.cpp ${SERVER}.cpp -o ${BINARY}

clean:
	rm ${BINARY}

run: all
	./${BINARY}
