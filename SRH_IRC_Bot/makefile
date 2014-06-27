# Libs 
LIBS = -L../libircclient-1.6/src/ -lircclient -lpthread -lnsl
INCLUDES = -I../libircclient-1.6/include/

Bot: SRH_IRC_Bot.c
	gcc -o Bot SRH_IRC_Bot.c -lsqlite3 $(LIBS) $(INCLUDES)
