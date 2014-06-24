/*
	Projectname:	SRH_IRC_Bot
	Author:			Christian Hauck
	Subject:		Linux programming
*/

#include "SRH_IRC_Bot.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <unistd.h>
#include "libircclient.h"

struct sqlite3* Database;

static char global_origin[50];

char* user;
char* chan;
char* server;
char* password;




bool help;

int main(int argc, char** argv)
{
	Options(argc, argv);

	if (user == null || chan == null || server == null || password == null)
		Help));

	if (help)
		return 0;
}

void Options(int argc, char** argv)
{
	int opt = 0;
	while ((opt = getopt(argc, argv, "u:c:s:p:h::")) != -1)
	{
		switch (opt)
		{
		case 'u': user = optarg; break;
		case 'c': chan = optarg; break;
		case 's': server = optarg; break;
		case 'p': password = optarg; break;
		case 'h': Help(); break;
		}
	}
}

void Help()
{
	help = true;
	printf("Instructions: \n");
	printf("\t-u User \n");
	printf("\t-c Chan \n");
	printf("\t-s Server \n");
	printf("\t-p Password \n");
}