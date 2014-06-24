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

irc_callbacks_t callbacks;
irc_session_t *s;
irc_ctx_t ctx;

bool help;

int main(int argc, char** argv)
{
	Options(argc, argv);

	if (user == null || chan == null || server == null || password == null)
		Help();

	if (help)
		return 0;

	memset($callbacks, 0, sizeof(callbacks));

	IrcEvents();

	ctx.nick = user;
	ctx.password = password;
	ctx.channel = chan;

	s = irc_create_session(&callbacks);

	if (!s)
	{
		printf("Error!\n");
		return 1;
	}

	irc_set_ctx(s, &ctx);
	irc_option_set(s, LIBIRC_OPTION_STRIPNICKS);

	// Connecting with Server

	if (irc_connect(s, server, 6667, 0, user, 0, 0))
	{
		printf("Could not connect!\n");
		return 1;
	}

	irc_run(s);

	return 0;
}

// Setting options

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

// Helpfunction

void Help()
{
	help = true;
	printf("Instructions: \n");
	printf("\t-u User \n");
	printf("\t-c Channel \n");
	printf("\t-s Server \n");
	printf("\t-p Password \n");
}

//IRC Events

void IrcEvents()
{
	callbacks.event_connect = event_connect;
	callbacks.event_join = event_join;
	callbacks.event_part = event_part;
	callbacks.event_nick = event_nick;
	callbacks.event_privmsg = event_privmsg;
	callbacks.event_topic = event_topic;
	callbacks.event_channel = event_channel;
}

// Connection

void event_connect(irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	irc_ctx_t* ctx = (irc_ctx_t*)irc_get_ctx(session);
	irc_cmd_join(session, ctx->channel, 0);
}