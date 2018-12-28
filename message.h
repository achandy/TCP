#ifndef Message
#define Message

struct message
{
	char trans_type;
	char account;
	int amount;
	int balance_before;
	int balance_after;
	char error;
	
};


#endif 