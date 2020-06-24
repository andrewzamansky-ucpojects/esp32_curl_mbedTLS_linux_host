#include <stdio.h>

#define USE_OUR_SOCKET_API
#if !defined(USE_OUR_SOCKET_API)
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
#else
	#include "sys/socket.h"
	#include "dev_management_api.h"
	#include "ESP8266_api.h"
#endif

#include <string.h> /* memcpy, memset */
#include <unistd.h>
#include <uart_api.h>

void error(char const *msg)
{
	printf("%s\n", msg);
}


extern  struct dev_desc_t *esp8266_dev;
extern  struct dev_desc_t *esp8266_uart_tx_wrap_dev;
extern  struct dev_desc_t *esp8266_uart_dev;

int main( void )
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	int total;
	int total_sent;
	int bytes_sent;
	struct hostent*  p_hostent;
	char    *h_addr;
	uint32_t server_ip;
	fd_set fds_read;
	fd_set fds_write;
	fd_set fds_err;

	char const *message =
			"GET / HTTP/1.1\r\n"
			"Host: www.example.com\r\n"
			"Accept: */*\r\n"
			"\r\n";

#if defined(USE_OUR_SOCKET_API)
//	#define TEST_UART
	#if defined(TEST_UART)
		uint32_t interface_device_speed  = 921600;
		DEV_IOCTL(esp8266_uart_dev,
				IOCTL_UART_SET_BAUD_RATE, &interface_device_speed);

		DEV_IOCTL_0_PARAMS(esp8266_uart_tx_wrap_dev, IOCTL_DEVICE_START);
		while(1)
		{
			static int cnt = 0;
			static char test_str[] = "test_xxxxxx";

			if (0 == (cnt %2))
			{
				interface_device_speed  = 921600;
			}
			else
			{
				interface_device_speed  = 115200;
			}
			DEV_IOCTL(esp8266_uart_dev,
					IOCTL_UART_SET_BAUD_RATE, &interface_device_speed);
			snprintf(test_str, 12, "test_%d\n", cnt++);
			DEV_WRITE(esp8266_uart_tx_wrap_dev, (uint8_t*)test_str, strlen(test_str));
			sleep(5);
		}
	#endif
	DEV_IOCTL_0_PARAMS(esp8266_dev, IOCTL_DEVICE_START);
//	DEV_IOCTL_1_PARAMS(esp8266_dev,
//			IOCTL_ESP8266_SET_SSID_NAME, (void*)"WECA-NTCA");
//	DEV_IOCTL_1_PARAMS(esp8266_dev,
//			IOCTL_ESP8266_SET_SSID_PSWRD, (void*)"4wecantca");
	DEV_IOCTL_1_PARAMS(esp8266_dev,
			IOCTL_ESP8266_SET_SSID_NAME, (void*)"slow_wifi");
	DEV_IOCTL_1_PARAMS(esp8266_dev,
			IOCTL_ESP8266_SET_SSID_PSWRD, (void*)"keyforwifi");

	set_esp8266_pdev_for_socket_api(esp8266_dev);
#endif

	printf("request will be sent : \n--start--\n%s", message);
	printf("--end--\n");

	portno = 80;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		error("ERROR opening socket");
		return 1;
	}

	p_hostent = gethostbyname("www.example.com");
	//p_hostent = gethostbyname("www.google.com");
	h_addr = p_hostent->h_addr_list[0];
    memcpy(&serv_addr.sin_addr, h_addr, sizeof(struct in_addr));

    server_ip = (uint32_t)serv_addr.sin_addr.S_un.S_addr;
    printf("dbg : resolved address :  %d.%d.%d.%d\n",
    		server_ip  & 0xff, (server_ip >> 8)  & 0xff,
    		(server_ip >> 16)  & 0xff, (server_ip >> 24)  & 0xff );
    //    printf("dbg : resolved address :  %s\n", inet_ntoa(serv_addr.sin_addr));

	serv_addr.sin_family = AF_INET;
	/*  93.184.216.34  is for example.com */
	//inet_aton("93.184.216.34", &serv_addr.sin_addr);
	serv_addr.sin_port = htons(portno);


	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		error("ERROR connecting");
		close(sockfd);
		return 1;
	}
	printf("socket connected\n");

    total = strlen(message);
    total_sent = 0;
    do {
        //bytes_sent = write(sockfd, message + total_sent, total - total_sent);
    	bytes_sent = send(sockfd, message + total_sent, total - total_sent, 0);
        if (bytes_sent < 0)
        {
            error("ERROR writing message to socket");
        	close(sockfd);
        }
        if (bytes_sent == 0)
            break;
        total_sent+=bytes_sent;
    } while (bytes_sent < total);

	printf("socket write finished\n");

	FD_ZERO(&fds_read);
	FD_ZERO(&fds_write);
	FD_ZERO(&fds_err);
	while (1)
	{
		FD_SET(sockfd, &fds_read);
		select(sockfd + 1, &fds_read, &fds_write, &fds_err, NULL);

		//n = read(sockfd, buffer, 255);
		n = recv(sockfd, buffer, 255, 0);
		if (0 == n)
		{
			//break;
		}

		if (n < 0)
		{
			 error("ERROR reading from socket");
			close(sockfd);
		}
		buffer[n] = 0;
		write(1, buffer, n);
		//sleep(1);
	}
	printf("socket read finished\n");

	while(1)
	{
		sleep(5);
	}

	close(sockfd);

	return 0;
}
