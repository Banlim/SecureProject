#include "common.h"
void do_client_read(BIO *conn){
	int err, nread;
	char buf[80];

	for(nread = 0; nread < sizeof(buf); nread += err)
	{
		err = BIO_read(conn, buf+nread, sizeof(buf) - nread);
		if(err <= 0)
			break;
	}
	printf("[server] : %s", buf);
}
void do_client_loop(BIO *conn)
{
	int err, nwritten;
	char buf[80];

		if(!fgets(buf, sizeof(buf), stdin)){
			return;
		}
		for(nwritten = 0; nwritten <sizeof(buf); nwritten += err)
		{
				err = BIO_write(conn, buf + nwritten, sizeof(buf) - nwritten);
			if( err <= 0 )
				return;
		}
		if(!strcmp(buf, "quit\n")){
			fprintf(stderr, "Connection Closed.\n");
			exit(0);
		}

}

int main(int argc, char *argv[])
{
	BIO *conn;
	init_OpenSSL();

	conn = BIO_new_connect(SERVER ":" PORT);

	if(!conn)
		int_error("Error creating connection BIO");

	if(BIO_do_connect(conn) <= 0)
		int_error("Error connecting to remote machine");

	fprintf(stderr, "Connection opened\n");
	while(1){
		do_client_loop(conn);
		do_client_read(conn);
	}
	fprintf(stderr, "Connection closed\n");

	BIO_free(conn);
	return 0;
}

