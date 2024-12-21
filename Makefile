# Nome degli eseguibili
CLIENT_TARGET = Client.out
SERVER_TARGET = Server.out

# Directory
SRC_DIR = .
CLIENT_DIR = $(SRC_DIR)/Client
SERVER_DIR = $(SRC_DIR)/Server
UTILS_DIR = $(SRC_DIR)/Utils

# File sorgenti
CLIENT_SRC = $(CLIENT_DIR)/Client.c $(UTILS_DIR)/ClientUtils.c 
SERVER_SRC = $(SERVER_DIR)/Server.c $(UTILS_DIR)/ServerUtils.c

# File oggetto
CLIENT_OBJ = $(CLIENT_DIR)/obj/Client.o $(UTILS_DIR)/obj/ClientUtils.o
SERVER_OBJ = $(SERVER_DIR)/obj/Server.o $(UTILS_DIR)/obj/ServerUtils.o

# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra -I$(CLIENT_DIR) -I$(SERVER_DIR) -I$(UTILS_DIR)

LIB_PATH = /usr/lib/ssl
LIB_NAME = ssl
OPENSSL = -L$(LIB_PATH) -l$(LIB_NAME) -lcrypto


# Regola principale per compilare entrambi gli eseguibili
all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Regola per il Client
$(CLIENT_TARGET): $(CLIENT_OBJ)
	if [ ! -d "$(CLIENT_DIR)/bin" ]; then \
		mkdir $(CLIENT_DIR)/bin; \
	fi
	$(CC) $(CLIENT_OBJ) -o $(CLIENT_DIR)/bin/$(CLIENT_TARGET)

# Regola per il Server
$(SERVER_TARGET): $(SERVER_OBJ)
	if [ ! -d "$(SERVER_DIR)/bin" ]; then \
		mkdir $(SERVER_DIR)/bin; \
	fi
	$(CC) $(SERVER_OBJ) $(OPENSSL) -o $(SERVER_DIR)/bin/$(SERVER_TARGET)



# Regole per i file oggetto del Client
$(CLIENT_DIR)/obj/Client.o: $(CLIENT_DIR)/Client.c $(CLIENT_DIR)/Client.h $(UTILS_DIR)/ClientUtils.h
	if [ ! -d "$(CLIENT_DIR)/obj" ]; then \
		mkdir $(CLIENT_DIR)/obj; \
	fi
	$(CC) $(CFLAGS) -c $(CLIENT_DIR)/Client.c -o $(CLIENT_DIR)/obj/Client.o

$(UTILS_DIR)/obj/ClientUtils.o: $(UTILS_DIR)/ClientUtils.c $(UTILS_DIR)/ClientUtils.h
	if [ ! -d "$(UTILS_DIR)/obj" ]; then \
		mkdir $(UTILS_DIR)/obj; \
	fi
	$(CC) $(CFLAGS) -c $(UTILS_DIR)/ClientUtils.c -o $(UTILS_DIR)/obj/ClientUtils.o



# Regole per i file oggetto del Server
$(SERVER_DIR)/obj/Server.o: $(SERVER_DIR)/Server.c $(SERVER_DIR)/Server.h $(UTILS_DIR)/ServerUtils.h
	if [ ! -d "$(SERVER_DIR)/obj" ]; then \
		mkdir $(SERVER_DIR)/obj; \
	fi
	$(CC) $(CFLAGS) -c $(SERVER_DIR)/Server.c -o $(SERVER_DIR)/obj/Server.o

$(UTILS_DIR)/obj/ServerUtils.o: $(UTILS_DIR)/ServerUtils.c $(UTILS_DIR)/ServerUtils.h
	if [ ! -d "$(UTILS_DIR)/obj" ]; then \
		mkdir $(UTILS_DIR)/obj; \
	fi
	$(CC) $(CFLAGS) -c $(UTILS_DIR)/ServerUtils.c -o $(UTILS_DIR)/obj/ServerUtils.o



# Per sha256
$(UTILS_DIR)/obj/sha256.o: $(UTILS_DIR)/sha256.c $(UTILS_DIR)/sha256.h
	if [ ! -d "$(UTILS_DIR)/obj" ]; then \
		mkdir $(UTILS_DIR)/obj; \
	fi
	$(CC) $(CFLAGS) -c $(UTILS_DIR)/sha256.c -o $(UTILS_DIR)/obj/sha256.o



# Pulizia
clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT_TARGET) $(SERVER_TARGET)
