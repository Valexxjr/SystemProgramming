/*
* SPLPv1.c
* The file is part of practical task for System programming course.
* This file contains validation of SPLPv1 protocol.
*/


/*
Валай Александр Александрович
№ группы 13
*/



/*
---------------------------------------------------------------------------------------------------------------------------
# |      STATE      |         DESCRIPTION       |           ALLOWED MESSAGES            | NEW STATE | EXAMPLE
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
1 | INIT            | initial state             | A->B     CONNECT                      |     2     |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
2 | CONNECTING      | client is waiting for con-| A<-B     CONNECT_OK                   |     3     |
|                 | nection approval from srv |                                       |           |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
3 | CONNECTED       | Connection is established | A->B     GET_VER                      |     4     |
|                 |                           |        -------------------------------+-----------+----------------------
|                 |                           |          One of the following:        |     5     |
|                 |                           |          - GET_DATA                   |           |
|                 |                           |          - GET_FILE                   |           |
|                 |                           |          - GET_COMMAND                |           |
|                 |                           |        -------------------------------+-----------+----------------------
|                 |                           |          GET_B64                      |     6     |
|                 |                           |        ------------------------------------------------------------------
|                 |                           |          DISCONNECT                   |     7     |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
4 | WAITING_VER     | Client is waiting for     | A<-B     VERSION ver                  |     3     | VERSION 2
|                 | server to provide version |          Where ver is an integer (>0) |           |
|                 | information               |          value. Only a single space   |           |
|                 |                           |          is allowed in the message    |           |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
5 | WAITING_DATA    | Client is waiting for a   | A<-B     CMD data CMD                 |     3     | GET_DATA a GET_DATA
|                 | response from server      |                                       |           |
|                 |                           |          CMD - command sent by the    |           |
|                 |                           |           client in previous message  |           |
|                 |                           |          data - string which contains |           |
|                 |                           |           the following allowed cha-  |           |
|                 |                           |           racters: small latin letter,|           |
|                 |                           |           digits and '.'              |           |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
6 | WAITING_B64_DATA| Client is waiting for a   | A<-B     B64: data                    |     3     | B64: SGVsbG8=
|                 | response from server.     |          where data is a base64 string|           |
|                 |                           |          only 1 space is allowed      |           |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
7 | DISCONNECTING   | Client is waiting for     | A<-B     DISCONNECT_OK                |     1     |
|                 | server to close the       |                                       |           |
|                 | connection                |                                       |           |
---------------------------------------------------------------------------------------------------------------------------

IN CASE OF INVALID MESSAGE THE STATE SHOULD BE RESET TO 1 (INIT)

*/


#include "splpv1.h"


enum STATE {
	INIT,
	CONNECTING,
	CONNECTED,
	WAITING_VER,
	WAITING_DATA,
	WAITING_B64_DATA,
	DISCONNECTING
};
// this variable stores info about current state of protocol
enum STATE state = INIT;

/* FUNCTION:  validate_message
*
* PURPOSE:
*    This function is called for each SPLPv1 message between client
*    and server
*
* PARAMETERS:
*    msg - pointer to a structure which stores information about
*    message
*
* RETURN VALUE:
*    MESSAGE_VALID if the message is correct
*    MESSAGE_INVALID if the message is incorrect or out of protocol
*    state
*/
enum test_status validate_message(struct Message *msg)
{
	if (!msg->direction) {
		if (!strcmp(msg->text_message, "CONNECT") && state == INIT) {
			state = CONNECTING;
			return MESSAGE_VALID;
		}
		if (!strcmp(msg->text_message, "GET_VER") && state == CONNECTED) {
			state = WAITING_VER;
			return MESSAGE_VALID;
		}
		if (!strcmp(msg->text_message, "GET_DATA") && state == CONNECTED) {
			state = WAITING_DATA;
			return MESSAGE_VALID;
		}
		if (!strcmp(msg->text_message, "GET_FILE") && state == CONNECTED) {
			state = WAITING_DATA;
			return MESSAGE_VALID;
		}
		if (!strcmp(msg->text_message, "GET_COMMAND") && state == CONNECTED) {
			state = WAITING_DATA;
			return MESSAGE_VALID;
		}
		if (!strcmp(msg->text_message, "GET_B64") && state == CONNECTED) {
			state = WAITING_B64_DATA;
			return MESSAGE_VALID;
		}
		if (!strcmp(msg->text_message, "DISCONNECT") && state == CONNECTED) {
			state = DISCONNECTING;
			return MESSAGE_VALID;
		}
	}

	if (msg->direction) {
		if (!strcmp(msg->text_message, "CONNECT_OK") && state == CONNECTING) {
			state = CONNECTED;
			return MESSAGE_VALID;
		}
		if (!strcmp(msg->text_message, "DISCONNECT_OK") && state == DISCONNECTING) {
			state = INIT;
			return MESSAGE_VALID;
		}
		if (!strncmp(msg->text_message, "VERSION ", 8) && state == WAITING_VER) {
			state = CONNECTED;
			char* buf = calloc(50, sizeof(char));
			strcpy(buf, &msg->text_message[9]);
			if (atoi(buf) > 0) {
				free(buf);
				return MESSAGE_VALID;
			}
			free(buf);
		}
		if (!strncmp(msg->text_message, "GET_DATA ", 8) && state == WAITING_DATA) {
			state = CONNECTED;
			char *buf1 = calloc(10, sizeof(char));
			strcpy(buf1, &msg->text_message[strlen(msg->text_message) - 9]);
			if (!strcmp(buf1, " GET_DATA")) {
				char *buf = calloc(1000, sizeof(char));
				strcpy(buf, "");
				strncpy(buf, &msg->text_message[9], strlen(msg->text_message) - 18);
				if (isstr(buf)) {
					free(buf);
					return MESSAGE_VALID;
				}
				free(buf);
			}
			free(buf1);
		}
		if (!strncmp(msg->text_message, "GET_FILE ", 8) && state == WAITING_DATA) {
			state = CONNECTED;
			char *buf1 = calloc(10, sizeof(char));
			strcpy(buf1, &msg->text_message[strlen(msg->text_message) - 9]);
			if (!strcmp(buf1, " GET_FILE")) {
				char *buf = calloc(1000, sizeof(char));
				strncpy(buf, &msg->text_message[9], strlen(msg->text_message) - 18);
				if (isstr(buf)) {
					free(buf);
					return MESSAGE_VALID;
				}
				free(buf);
			}
			free(buf1);
		}
		if (!strncmp(msg->text_message, "GET_COMMAND ", 11) && state == WAITING_DATA) {
			state = CONNECTED;
			char *buf1 = calloc(15, sizeof(char));
			strcpy(buf1, &msg->text_message[strlen(msg->text_message) - 12]);
			if (!strcmp(buf1, " GET_COMMAND")) {
				char *buf = calloc(1000, sizeof(char));
				strncpy(buf, &msg->text_message[12], strlen(msg->text_message) - 24);
				if (isstr(buf)) {
					free(buf);
					return MESSAGE_VALID;
				}
				free(buf);
			}
			free(buf1);
		}
	}
	if (!strncmp(msg->text_message, "B64: ", 5) && state == WAITING_B64_DATA) {
		state = CONNECTED;
		char* buf = calloc(14000, sizeof(char));
		strcpy(buf, &msg->text_message[5]);
		if (isbase64(buf)) {
			free(buf);
			return MESSAGE_VALID;
		}
		free(buf);
	}
	state = INIT;
	return MESSAGE_INVALID;
}

int isstr(char* str) {
	char* mass = calloc(256, sizeof(char));
	memset(mass, 0, 256 * sizeof(char));
	mass['.'] = 1;
	for (char i = '0'; i != ':'; i++)
		mass[i] = 1;
	for (char i = 'a'; i != '{'; i++)
		mass[i] = 1;
	for (int i = 0; i < strlen(str); i++)
		if (!mass[str[i]]) {
			free(mass);
			return 0;
		}
	free(mass);
	return 1;
}

int isbase64(char* str) {
	int lng = strlen(str);
	if (lng % 4 != 0)
		return 0;
	char* mass = calloc(256, sizeof(char));
	memset(mass, 0, 256 * sizeof(char));
	for (char i = '0'; i != ':'; i++)
		mass[i] = 1;
	for (char i = 'a'; i != '{'; i++)
		mass[i] = 1;
	for (char i = 'A'; i != '['; i++)
		mass[i] = 1;
	mass['+'] = 1;
	mass['/'] = 1;
	for (int i = 0; i < lng - 2; i++)
		if (!mass[str[i]]) {
			free(mass);
			return 0;
		}
	if (str[lng - 2] != '=' && !mass[str[lng - 2]])
		return 0;
	if (str[lng - 1] != '=' && !mass[str[lng - 1]])
		return 0;
	free(mass);
	return 1;
}