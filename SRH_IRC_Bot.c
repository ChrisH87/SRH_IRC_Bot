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

char* username;
char* channel;
char* server;
char* password;

irc_callbacks_t callbacks;
irc_session_t *s;
irc_ctx_t ctx;

FILE *log_file;


typedef struct
{
	char* channel;
	char* nick;
}

bool help;

bool daemon;

int main(int argc, char** argv)
{
	Options(argc, argv);

	if (username == null || channel == null || server == null || password == null)
		Help();

	if (help)
		return 0;

	if (daemon)
		Daemon();

	sqlite3_open("ircbot_db.db", &dDatabase);

	memset($callbacks, 0, sizeof(callbacks));

	IrcEvents();

	ctx.nick = username;
	ctx.password = password;
	ctx.channel = channel;

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
	while ((opt = getopt(argc, argv, "u:c:s:p:d:h::")) != -1)
	{
		switch (opt)
		{
		case 'u': username = optarg; break;
		case 'c': channel = optarg; break;
		case 's': server = optarg; break;
		case 'p': password = optarg; break;
		case 'd': daemon = true; break;
		case 'h': Help(); break;
		}
	}
}

// Helpfunction

void Help()
{
	help = true;
	printf("Instructions: \n");
	printf("\t-u Username \n");
	printf("\t-c Channel \n");
	printf("\t-s Server \n");
	printf("\t-p Password \n");
	printf("\t-d Daemon \n");
}

//Daemon

void Daemon()
{
	pid_t pid, sid;
	if (getppid() == 1) return;
	pid = fork();
	if (pid<0) exit(1);

	if (pid>0) exit(0);

	umask(0);
	sid = setsid();
	if (sid < 0) exit(1);

	if ((chdir("/") < 0)) exit(1);

	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);
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

//Join Event

void event_join(irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	irc_cmd_msg(session, params[0], "Bot loaded");
	char str_join[200];
	sprintf(str_join, "INSERT INTO join_irc (Channel, User, Joindate, Jointime) Values ('%s', '%s', date('now'), time('now'))", params[0], origin);
	sqlite_exec(Database, str_join, 0, 0, 0);
	printf("User: '%s', has signed in\n", origin);
}


//Part Event


void event_part (irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	char str_quit[500];
	printf("User: '%s', has signed off\n", origin);
	sprintf(str_quit, "INSERT INTO part_irc (user, channel, reason, partdate, parttime) VALUES('%s', '%s', '%s', date('now'), time('now'))", origin, params[0], params[1]);
	sqlite3_exec(Database, str_quit, 0, 0, 0);
}

// Nick Event

void event_nick(irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	Com_Printf("IRC: Nick event, from %s to %s\n", params[0], params[1]);
}


//PM Event


void event_privmsg(irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	printf("'%s' said '%s' to you\n", origin, params[1]);

	if (strcmp(params[1], "-hi") == 0)
	{
		printf("Private Message!\n");
		irc_cmd_msg(session, origin, "I respond to private messages");
	}

	if (strcmp(params[1], "-login") == 0)
	{
		strcpy(global_origin, origin);
		sqlite3_exec(Database, "Select partdate from join_irc order by rowid desc limit 1", SQLCallback, 0, 0);
		printf(global_origin);
		fflush(stdout);
	}

	if (strcmp(params[1], "-logfile") == 0)
	{
		log_file = fopen("log.txt", "w");
		sqlite3_exec(Database, "Select * From join_irc", SQLCallback_File, 0, 0);
		fclose(log_file);
	}

	if (strcmp(params[1], "-help") == 0)
	{
		irc_cmd_msg(session, origin, "-hi : response");
		irc_cmd_msg(session, origin, "-login : show Days");
		irc_cmd_msg(session, origin, "-help : Show this message");
		irc_cmd_msg(session, origin, "-logfile : Creates a Log File");
	}
}


//Channel Event


void event_channel(irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	char str_insert[10000];
	sprintf(str_insert, "INSERT INTO log_irc (User, Channel, Message, InsertDate, InsertTime) Values ('%s', '%s', '%s', date('now'), time('now'))", origin, params[0]], params[1]);
	sqlite3_exec(Database, str_insert, 0, 0, 0);
	printf("'%s' said in Channel %s this: %s\n",
		origin ? origin : "One of the Channel",
		params[0], params[1]);
}


//Topic Event


void event_topic(irc_session_t * session, const char * event, const char * origin, const char** params, unsigned int count)
{
	if (count > 1) {
		Com_Printf("IRC: %s changes topic on %s to %s\n", origin, params[0], params[1]);
	}
	else {
		Com_Printf("IRC: %s changes topic on %s\n", origin, params[0]);
	}
}


//Output with sqlite3


static int SQLCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for (i = 0; i < argc; i++)
	{
		char str_sql[500];
		sprintf(str_sql, "%s : %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		irc_cmd_msg(s, global_origin, str_sql);
	}
	printf("\n");
	fflush(stdout);
	return 0;
}


//Output with Filesystem


static int SQLCallback_File(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for (i = 0; i < argc; i++)
	{
		fprintf(log_file, "%s : %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		printf("%s : %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}