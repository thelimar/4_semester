
struct boards_info 
{
	double left;
	double right;
	double delta;
};

#define KEEPCNT 3 // Максимальное количество запросов проверки активности
#define KEEPIDLE 2 // Время (в секундах), в течение которого соединение должно оставаться бездействующим до того, как TCP начнет отправлять проверки активности
#define KEEPINTVL 1 // Время (в секундах) между отдельными проверками активности

#define SERVERPORT 4901 
#define CLIENTPORT 4902 

#define LISTEN_BACKLOG 100

#define ACCEPT_TIMEOUT_SEC  0       
#define ACCEPT_TIMEOUT_USEC 1000  

#define COMPUTE_TIMEOUT_SEC  40   
#define COMPUTE_TIMEOUT_USEC 0  