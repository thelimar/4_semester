
struct boards_info 
{
	double left;
	double right;
	double delta;
};

#define KEEPCNT 3 // ������������ ���������� �������� �������� ����������
#define KEEPIDLE 2 // ����� (� ��������), � ������� �������� ���������� ������ ���������� �������������� �� ����, ��� TCP ������ ���������� �������� ����������
#define KEEPINTVL 1 // ����� (� ��������) ����� ���������� ���������� ����������

#define SERVERPORT 4901 
#define CLIENTPORT 4902 

#define LISTEN_BACKLOG 100

#define ACCEPT_TIMEOUT_SEC  0       
#define ACCEPT_TIMEOUT_USEC 1000  

#define COMPUTE_TIMEOUT_SEC  40   
#define COMPUTE_TIMEOUT_USEC 0  