/* tcpserver.c */
/* Programmed by Adarsh Sethi */
/* Sept. 13, 2018 */

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, bind, listen, accept */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include "message.h"

#define STRING_SIZE 1024

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_TCP_PORT 65000
void print_struct(struct message mess);

int main(void) {

   int sock_server;  /* Socket on which server listens to clients */
   int sock_connection;  /* Socket on which server exchanges data with client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned int server_addr_len;  /* Length of server address structure */
   unsigned short server_port;  /* Port number used by server (local port) */

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */

   char sentence[STRING_SIZE];  /* receive message */
   char modifiedSentence[STRING_SIZE]; /* send message */
   unsigned int msg_len;  /* length of message */
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */


   /*account variables*/
   int checking_acct=0;
   int savings_acct=0;
   struct message MessageA;
   /* open a socket */


if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Server: can't open stream socket");
      exit(1);
   }

   /* initialize server address information */

   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */
   server_port = SERV_TCP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }

   /* listen for incoming requests from clients */

OPEN_SOCKET:   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   printf("\nI am here to listen ... on port %hu\n\n", server_port);

   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */

   for (;;) {

      sock_connection = accept(sock_server, (struct sockaddr *) &client_addr,
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
      if (sock_connection < 0) {
         perror("Server: accept() error\n");
         close(sock_server);
         exit(1);
      }
   
      /* receive the message */
	  MessageA.error = 0;
while(1){
      bytes_recd = recv(sock_connection, &MessageA, 20, 0);
      if(bytes_recd>0){
        printf("\n");
        printf("Bytes Received: %d",bytes_recd);
        printf("\n");
      }
       else{
	printf("There was an error receiving the message.");
      }
      if(MessageA.error == (42) || MessageA.error == (420) ){
	         goto BOTTOM;
      }
     
        printf("\nMessage Received:");
	print_struct(MessageA);
      //action based on the message contents
      if(MessageA.trans_type=='c'){//if wants to check account balance
        if(MessageA.account=='s'){//if savings acct
          MessageA.balance_before=savings_acct;
          MessageA.balance_after=savings_acct;
        }
        else if(MessageA.account=='c'){//if checking acct
          MessageA.balance_before=checking_acct;
          MessageA.balance_after=checking_acct;
        }
      }
      else if(MessageA.trans_type=='d'){ //wants to deposit money
        if(MessageA.account=='s'){//if savings account
          MessageA.balance_before=savings_acct;
          savings_acct+=MessageA.amount;
          MessageA.balance_after=savings_acct;
        }
        else if(MessageA.account=='c'){//if checking account
          MessageA.balance_before=checking_acct;
          checking_acct+=MessageA.amount;
          MessageA.balance_after=checking_acct;
        }
      }
      else if(MessageA.trans_type=='w'){//wants to withdraw money
        MessageA.balance_before=checking_acct;
        if(MessageA.amount<=checking_acct){//if enough money in checking acct
          if(MessageA.amount%20==0){//if amount is a multiple of 20
            checking_acct-=MessageA.amount;
          }
          else{
            MessageA.error=20;
          }//if not in 20's
        }//if enough in acct
        else{
          MessageA.error=-1;
        }//if not enough
        MessageA.balance_after=checking_acct;
      }
      else if(MessageA.trans_type=='t'){//wants to transfer money
        if(MessageA.account=='s'){
          MessageA.balance_before=checking_acct;
          if(MessageA.amount<=savings_acct){
            savings_acct-=MessageA.amount;
            checking_acct+=MessageA.amount;
          }//if enough in account
          else{
            MessageA.error=-1;
          }
          MessageA.balance_after=checking_acct;
        }//if savings acct
        else if(MessageA.account=='c'){
          MessageA.balance_before=savings_acct;
          if(MessageA.amount<=checking_acct){
            checking_acct-=MessageA.amount;
            savings_acct+=MessageA.amount;
          }//if enough in acct
          else{
            MessageA.error=-1;
          }
          MessageA.balance_after=savings_acct;
        }//if checking acct
      }

         /* send message */

         bytes_sent = send(sock_connection, &MessageA, 20, 0);
         if(bytes_sent>0){
           printf("\n");
           printf("Bytes Sent: %d", bytes_sent);
           printf("\n");
         }
         else{
		printf("There was an error sending the message.");
	}
        printf("\nMessage Sent:");
	print_struct(MessageA);
   }
   }
      /* close the socket */



BOTTOM: if(MessageA.error==42){
	 printf("\nClosed Connection\n");
	 MessageA.error = 0;
	 MessageA.trans_type = ' ';
	 MessageA.account = ' ';
	 MessageA.amount = 0;
	 MessageA.balance_before = 0;
	 MessageA.balance_after = 0;
         savings_acct = 0;
         checking_acct = 0;
         print_struct(MessageA);
         goto OPEN_SOCKET;
		
   }
close(sock_connection);
 
   
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
