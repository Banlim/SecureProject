#include "common.c"
#define CERTFILE "client.pem"

SSL_CTX *setup_client_ctx(void)
{
	SSL_CTX *ctx; 

	ctx=SSL_CTX_new(SSLv23_method()); //SSL_CTX 객체 SSLv23 방식으로 생성 
	if(SSL_CTX_use_certificate_chain_file(ctx,CERTFILE)!=1) //인증서 로딩
		int_error("파일로부터의 인증서 로딩에 오류가 발생했습니다\n");
	if(SSL_CTX_use_PrivateKey_file(ctx,CERTFILE,SSL_FILETYPE_PEM)!=1)//개인키 로딩
		int_error("파일로부터의 개인키 로딩에 오류가 발생했습니다\n");
	return ctx;
} //SSL 연결 위한 SSL_CTX 생성, 생성된 SSL_CTX 리턴

int do_client_loop(SSL *ssl)
{
	int err,nwritten;
	char buf[80];

	for(;;)
	{
		if(!fgets(buf,sizeof(buf),stdin))
			break;
		for(nwritten=0;nwritten<sizeof(buf);nwritten+=err)
		{
			err=SSL_write(ssl,buf+nwritten,sizeof(buf)-nwritten);
			if(err<=0)
				return 0;
		}
	}
	return 1;
}

int main(int argc,char *argv[])
{
	BIO *conn; //input, output handling용
	SSL *ssl;
	SSL_CTX *ctx;

	init_OpenSSL(); //SSL_load_errors_strings(); 들어있음-에러코드 연관 데이터 로드-
	seed_prng();

	ctx=setup_client_ctx(); //SSL 연결 위한 SSL_CTX생성, ctx에 저장

	conn=BIO_new_connect(SERVER":"PORT); //초기화, check PORT,SERVER, CLINET in common.h
	//위 셋 define 되어있음 ->("localhost"":""16001")
	//connect 성공시 BIO 구조체의 주소값 리턴, 실패시 NULL 리턴
	if(!conn)
		int_error("Error creating connection BIO");
	
	if(BIO_do_connect(conn)<=0) //연결 시도
		int_error("Error connecting to remote machine");
	
	if(!(ssl=SSL_new(ctx)))
		int_error("Error creating an SSL context");
	SSL_set_bio(ssl,conn,conn);
	if(SSL_connect(ssl)<=0)
		int_error("Error connecting SSL obgect");

	fprintf(stderr,"SSL Connection opende\n");
	if(do_client_loop(ssl))
		SSL_shutdown(ssl);
	else 
		SSL_clear(ssl);
	fprintf(stderr,"SSL Connection closed\n");

	SSL_free(ssl);
	SSL_CTX_free(ctx);
	return 0;
}
