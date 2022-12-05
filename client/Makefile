SRC = src/
CLIENT = client
SERVER = server
BINARY = binary

all:
	g++ ${SRC}${CLIENT}.cpp ${SRC}${SERVER}.cpp -o ${BINARY} -I include/

clean:
	rm ${BINARY}

run: all
	./${BINARY}
