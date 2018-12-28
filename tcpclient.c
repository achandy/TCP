/* tcp_ client.c */ 
/* Programmed by Adarsh Sethi */
/* Sept. 13, 2018 */     

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include "message.h"
#define STRING_SIZE 1024

void print_struct(struct message mess);

int main(void) {

 int sock_client;  /* Socket used by client */
   char loop = 'y';
   char connection = ' ';
  char stay_connection = ' ';
 
   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
   char server_hostname[STRING_SIZE]; /* Server's hostname */
   unsigned short server_port;  /* Port number used by server (remote port) */

   char sentence[STRING_SIZE];  /* send message */
   char modifiedSentence[STRING_SIZE]; /* receive message */
   unsigned int msg_len;  /* length of message */                      
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
  
   /* open a socket */

  Open_Connection: if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
   }

   /* Note: there is no need to initialize local client address information 
            unless you want to specify a specific local port
            (in which case, do it the same way as in udpclient.c).
            The local address initialization and binding is done automatically
            when the connect function is called later, if the socket has not
            already been bound. */

   /* initialize server address information */

   printf("Enter hostname of server: ");
   scanf("%s", server_hostname);
   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname");
      close(sock_client);
      exit(1);
   }

   printf("Enter port number for server: ");
   scanf("%hu", &server_port);

   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);

    /* connect to the server */
 		
    if (connect(sock_client, (struct sockaddr *) &server_addr, 
                                    sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
   }
   
   struct message Message1;
   struct message Message2;
  
   /* user interface */
   
  UI: while(loop == 'y'){
	 Begin:
	 Message1.error = 0;
	 Message1.trans_type = ' ';
	 Message1.account = ' ';
	 Message1.amount = 0;
	 Message1.balance_before = 0;
	 Message1.balance_after = 0;
	 
 	 printf("\nHello, what type of transaction would you like to make?\n");
	   printf("w -> withdrawal\nd -> deposit\nc -> check balance\nt -> transfer funds\n");
	   scanf(" %c",&Message1.trans_type);
	
	   if(Message1.trans_type == 'w'){
		   printf("\nPlease enter the amount you would like to withdraw\n");
		   scanf(" %d", &Message1.amount);
	   }
	   
	   else if(Message1.trans_type == 'd'){
		   printf("\nWhich account would you like to deposit into?\n");
	       printf("s -> savings\nc -> checkings\n");
	       scanf(" %c", &Message1.account);
		   
		   if(Message1.account != 's' && Message1.account != 'c'){
			   goto Error;
		   }
			
			printf("\nPlease enter the amount you would like to deposit\n");
			scanf(" %d", &Message1.amount);
	   }
	   
	   else if(Message1.trans_type == 'c'){
		   printf("\nWhich account would you like to check?\n");
	       printf("s -> savings\nc -> checkings\n");
	       scanf(" %c",&Message1.account);
           if(Message1.account != 's' && Message1.account != 'c'){
			   goto Error;
		   }

	   }
	   
	   else if(Message1.trans_type == 't'){
		   printf("\nWhich account would you like to transfer from?\n");
	       printf("s -> savings\nc -> checkings\n");
	       scanf(" %c", &Message1.account);
		   
		   if(Message1.account != 's' && Message1.account != 'c'){
			   goto Error;
		   }
		   
			printf("\nPlease enter the amount you would like to transfer\n");
			scanf(" %d", &Message1.amount);
	   }
	   
	  Error:if(Message1.trans_type != 'w' && Message1.trans_type != 'd' && Message1.trans_type != 'c' && Message1.trans_type != 't') {
		   printf("\nYou have entered an invalid answer, would you like to start a new transaction?\n");
		   printf("y -> yes\nn-> no\n");
		   scanf(" %c", &loop);
		   
		   if(loop = 'y'){
			   goto Begin;
		   }
		   if(loop = 'n'){
			   break;
		   }
		   else{
			   goto Error;
		   }
	   }  
	   
	   printf("\nMessage Sent: \n");
           bytes_sent = send(sock_client, &Message1, 20, 0);
	   print_struct(Message1);

	   printf("\nMessage Received: \n");	
	   bytes_recd = recv(sock_client, &Message2 , 20, 0);
	   print_struct(Message2);

	   if(Message2.error == 0){	   
		   if(Message2.trans_type == 'w'){
			   printf("\nYou have successfully withdrawn $")
               printf(" %d", Message2.amount);
			   print(" from checkings\n");
		   }
		   else if(Message2.trans_type == 'd'){
			   printf("\nYou have successfully deposited $")
               printf(" %d", Message2.amount);
			   printf(" from");
			   if(Message2.account == 'c'){
				   printf(" checkings\n");
			   }
			   else if(Message2.account == 's'){
				   printf(" checkings\n");
			   }
		   }
		   else if(Message2.trans_type == 'c'){
			   printf("\nYour balance is $")
			   printf(" %d", Message2.balance_after);
			   if(Message2.account == 'c'){
				   printf(" in your checkings account\n");
			   }
			   else if(Message2.account == 's'){
				   printf(" in your savings account\n");
			   }
		   }
		   else if(Message2.trans_type == 't'){
			   printf("\nYou have transfered $")
			   printf(" %d", Message2.amount);
			   if(Message2.account == 'c'){
				   printf(" from your checkings account to your savings account\n");
			   }
			   else if(Message2.account == 's'){
				   printf(" from your savings account to your checkings account\n");
			   }
		   
		   }
		   
		   
		   printf("\nWould you like to make another transaction?\n");
		   printf("y -> yes\nn -> no\n");
		   scanf(" %c", &loop);
		   if(loop == 'y'){
			   goto Begin;
		   }
		   if(loop == 'n'){
			   break;
		   }
		   else{
			   goto Error;
		   }
	   }
	   
      else if(Message2.error == 20){
		  printf("\nYou have entered an invalid withdrawal amount, would you like to start a new transaction?\n");
		   printf("y -> yes\nn -> no\n");
		   scanf(" %c", &loop);
		   if(loop == 'y'){
			   goto Begin;
		   }
		   if(loop == 'n'){
			   break;
		   }
		   else{
			   goto Error;
		   }
	  }
	  
	   
      else if(Message2.error == -1){
		  printf("\nYour account has insufficient funds, would you like to start a new transaction?\n");
		   printf("y -> yes\nn -> no\n");
		   scanf(" %c", &loop);
		   if(loop == 'y'){
			   goto Begin;
		   }
		   if(loop == 'n'){
			   break;
		   }
		   else{
			   goto Error;
		   }
	  }
	   
   }
   
	printf("\nWould you like to send more messages on this connection?\n");
	printf("y -> yes\nn -> no\n");
        scanf(" %c", &stay_connection);
	if (stay_connection == 'y'){
		loop = 'y';
		goto UI; 
	}
	else if(stay_connection == 'n'){
		printf("Closed Connection\n");
	}
	
	
	printf("\nWould you like to open a new connection?\n");
	printf("y -> yes\nn -> no\n");
    scanf(" %c", &connection);
        Message1.trans_type='z';
	if (connection == 'y'){
		Message1.error = 42;
		bytes_sent = send(sock_client, &Message1, 20, 0);
	        print_struct(Message1);
                loop = 'y';
		close (sock_client);
		goto Open_Connection; 
	}
	else if(connection == 'n'){
		Message1.error = 420;
		bytes_sent = send(sock_client, &Message1, 20, 0);
	    print_struct(Message1);
	}
    close (sock_client);
}

void print_struct(struct message mess){
	printf("\nThe transaction type is: ");
    printf(" %c", mess.trans_type);
	printf("\nThe account type is: ");
    printf(" %c", mess.account);
	printf("\nThe amount entered is: ");
    printf(" %d", mess.amount);
	printf("\nYour balance originally had: ");
    printf(" %d", mess.balance_before);
    printf("\nYour new balance is now: ");
    printf(" %d", mess.balance_after);
    printf("\nThe error is: ");
    printf(" %d\n", mess.error);
}
