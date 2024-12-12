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
SERVER_SRC = $(SERVER_DIR)/Server.c $(UTILS_DIR)/ServerUtils.c $(UTILS_DIR)/sha256.c

# File oggetto
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o)

# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra -I$(CLIENT_DIR) -I$(SERVER_DIR) -I$(UTILS_DIR)

# Regola principale per compilare entrambi gli eseguibili
all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Regola per il Client
$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) -o $(CLIENT_DIR)/$(CLIENT_TARGET)

# Regola per il Server
$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o $(SERVER_DIR)/$(SERVER_TARGET)

# Regole per i file oggetto del Client
$(CLIENT_DIR)/Client.o: $(CLIENT_DIR)/Client.c $(CLIENT_DIR)/Client.h $(UTILS_DIR)/ClientUtils.h
	$(CC) $(CFLAGS) -c $(CLIENT_DIR)/Client.c -o $(CLIENT_DIR)/Client.o

$(UTILS_DIR)/ClientUtils.o: $(UTILS_DIR)/ClientUtils.c $(UTILS_DIR)/ClientUtils.h
	$(CC) $(CFLAGS) -c $(UTILS_DIR)/ClientUtils.c -o $(UTILS_DIR)/ClientUtils.o

# Regole per i file oggetto del Server
$(SERVER_DIR)/Server.o: $(SERVER_DIR)/Server.c $(SERVER_DIR)/Server.h $(UTILS_DIR)/ServerUtils.h $(UTILS_DIR)/sha256.h
	$(CC) $(CFLAGS) -c $(SERVER_DIR)/Server.c -o $(SERVER_DIR)/Server.o

$(UTILS_DIR)/ServerUtils.o: $(UTILS_DIR)/ServerUtils.c $(UTILS_DIR)/ServerUtils.h
	$(CC) $(CFLAGS) -c $(UTILS_DIR)/ServerUtils.c -o $(UTILS_DIR)/ServerUtils.o


# Per sha256
$(UTILS_DIR)/sha256.o: $(UTILS_DIR)/sha256.c $(UTILS_DIR)/sha256.h
	$(CC) $(CFLAGS) -c $(UTILS_DIR)/sha256.c -o $(UTILS_DIR)/sha256.o



# Pulizia
clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT_TARGET) $(SERVER_TARGET)
