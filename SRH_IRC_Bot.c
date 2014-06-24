/*
	Projectname:	SRH_IRC_Bot
	Author:			Christian Hauck
	Subject:		Linux Programming
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
	ParseOptions(argc, argv);

}