#include <time.h>
#include "common.h"


void do_send(BIO *conn){
	int err, nwrite;
	char buf[80];
	FILE *file_pointer;
	file_pointer = fopen("output.txt", "a+");

	if(!fgets(buf, sizeof(buf), stdin)){
		return;
	}
	for(nwrite = 0; nwrite < sizeof(buf); nwrite += err)
	{
		err = BIO_write(conn, buf+nwrite, sizeof(buf) - nwrite);
		if(err <= 0)
			return;
	}
	fprintf(file_pointer, "[server] : %s", buf);
	fclose(file_pointer);

}
void do_server_loop(BIO *conn)
{
	int err, nread;
	char buf[80];
	FILE *file_pointer;
	file_pointer = fopen("output.txt", "a+");

//	do
//	{
		for(nread = 0; nread < sizeof(buf); nread += err)
		{
			err = BIO_read(conn, buf+nread, sizeof(buf) - nread);
			
			if(err <= 0)
				break;
		}
		fprintf(file_pointer, "[client] : %s", buf);
		printf("[client] : %s", buf);
//	}
//	while(err > 0);

	fclose(file_pointer);

	if(!strcmp(buf, "quit\n")){
		fprintf(stderr, "Connection closed. \n");
		exit(0);
	}
}

void THREAD_CC server_thread(void *arg)
{
	BIO *client = (BIO *)arg;
	time_t cur;
	struct tm *date;
	cur = time(NULL);
	date = localtime(&cur);

#ifndef WIN32
	pthread_detach(pthread_self( ));
#endif
	fprintf(stderr, "Connection opened. \n");

	FILE *file_pointer;
	file_pointer = fopen("output.txt", "a+");
	fprintf(file_pointer,"%d-%0d-%0d %0d:%0d\n", date->tm_year + 1900, date->tm_mon + 1, date->tm_mday, date->tm_hour, date->tm_sec);
	fclose(file_pointer);

	while(1){
		do_server_loop(client);
		do_send(client);
	}

	fprintf(stderr, "Connection closed. \n");
	BIO_free(client);
	ERR_remove_state(0);
#ifdef WIN32
	_endthread( );
#endif
}

int main(int argc, char *argv[])
{
	BIO *acc, *client;
	THREAD_TYPE tid;

	init_OpenSSL( );

	acc = BIO_new_accept(PORT);

	if(!acc)
		int_error("Error creating server socket");

	if(BIO_do_accept(acc) <= 0)
		int_error("Error binding server socket");

	for(;;)
	{
		if(BIO_do_accept(acc) <= 0)
			int_error("Error accepting connection");

		client = BIO_pop(acc);
		THREAD_CREATE(tid, (void *)server_thread, client);
	}

	BIO_free(acc);
	return 0;

}

