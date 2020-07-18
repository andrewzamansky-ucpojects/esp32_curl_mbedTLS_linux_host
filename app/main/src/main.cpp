
#include "project_config.h"
#include <stdio.h>

#if !defined(CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION)
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
#else
	#include "dev_management_api.h"
	#include "ESP8266_api.h"
	#include "file_descriptor_manager_api.h"
	#include "std_net_functions_api.h"
	#include <uart_api.h>
#endif

#include <string.h> /* memcpy, memset */
#include <unistd.h>
#include "curl/curl.h"

extern "C" {
	#define DEBUG
	#include "PRINTF_api.h"

	void mbedtls_port_init();

}

//#define TEST_HTTP
//#define TEST_HTTPS
#define TEST_HTTP2

#if defined(TEST_HTTP)
	#if defined(TEST_HTTPS) || defined(TEST_HTTP2)
		#error "only one test can be choosen "
	#endif
#endif
#if defined(TEST_HTTPS)
	#if defined(TEST_HTTP) || defined(TEST_HTTP2)
		#error "only one test can be choosen "
	#endif
#endif
#if defined(TEST_HTTP2)
	#if defined(TEST_HTTP) || defined(TEST_HTTPS)
		#error "only one test can be choosen "
	#endif
#endif

void error(char const *msg)
{
	printf("%s\n", msg);
}


extern  struct dev_desc_t *esp8266_dev;
extern  struct dev_desc_t *esp8266_uart_tx_wrap_dev;
extern  struct dev_desc_t *esp8266_uart_dev;

int my_trace(CURL *handle, curl_infotype type,
             char *data, size_t size, void *userp)
{
  const char *text;
  (void)handle; /* prevent compiler warning */
  (void)userp; /* prevent compiler warning */

  printf("%s-----\n", __FUNCTION__);
  switch (type)
  {
  case CURLINFO_TEXT:
	  PRINTF_DBG("== Info: ");
	  PRINT_DATA_DBG(data, size);
	  PRINTF_DBG("\n");// to flush stdout in linux
  default: /* in case a new one is introduced to shock us */
    return 0;
  case CURLINFO_HEADER_OUT:
    text = "=> Send header";
  //  return 0;
    break;
  case CURLINFO_DATA_OUT:
    text = "=> Send data";
  //  return 0;
    break;
  case CURLINFO_SSL_DATA_OUT:
    text = "=> Send SSL data";
    PRINTF_DBG("%s", text);
    PRINTF_DBG("\n");// to flush stdout in linux
    return 0;
   break;
  case CURLINFO_HEADER_IN:
    text = "<= Recv header";
    break;
  case CURLINFO_DATA_IN:
    text = "<= Recv data";
    break;
  case CURLINFO_SSL_DATA_IN:
    text = "<= Recv SSL data";
    PRINTF_DBG("%s", text);
    PRINTF_DBG("\n");// to flush stdout in linux
    return 0;
   break;
  }

  PRINTF_DBG("%s", text);
  PRINT_DATA_DBG(data, size);
  PRINTF_DBG("\n");// to flush stdout in linux
  return 0;
}


size_t receive_data(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize;

	realsize = size * nmemb;
	PRINT_DATA_DBG(contents, realsize);

	return realsize;
}


int main( void )
{
	uint8_t esp8266_ready;
	CURL *curl;
	CURLcode res;

	PRINTF_API_init();
	mbedtls_port_init();

#if defined(CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION)
	file_descriptor_manager_api_init();
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

//	DEV_IOCTL_1_PARAMS(esp8266_dev,
//			IOCTL_ESP8266_SET_SSID_NAME, (void*)"WECA-NTCA");
//	DEV_IOCTL_1_PARAMS(esp8266_dev,
//			IOCTL_ESP8266_SET_SSID_PSWRD, (void*)"4wecantca");
	DEV_IOCTL_1_PARAMS(esp8266_dev,
			IOCTL_ESP8266_SET_SSID_NAME, (void*)"slow_wifi");
	DEV_IOCTL_1_PARAMS(esp8266_dev,
			IOCTL_ESP8266_SET_SSID_PSWRD, (void*)"keyforwifi");
	DEV_IOCTL_0_PARAMS(esp8266_dev, IOCTL_DEVICE_START);
	esp8266_ready = 0;
	while (0 == esp8266_ready)
	{
		DEV_IOCTL_1_PARAMS(esp8266_dev,
				IOCTL_ESP8266_IS_INITIALIZED, &esp8266_ready);
		while(PRINTF_API_print_from_debug_buffer(64));
		sleep(1);
	}


	file_descriptor_manager_api_register_INET_device(esp8266_dev);
	std_net_functions_api_register_net_device(esp8266_dev);
#endif

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (NULL == curl)
	{
		CRITICAL_ERROR("failed to init curl");
	}

#if defined(TEST_HTTP)
	curl_easy_setopt(curl, CURLOPT_URL, "http://example.com/");
#elif defined(TEST_HTTPS) || defined(TEST_HTTP2)
	curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");
	#if defined(TEST_HTTP2)
		curl_easy_setopt(curl,
			CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
	#endif
#else
	#error "no test is selected"
#endif

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receive_data);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	res = curl_easy_perform(curl);

	if(res != CURLE_OK)
	{
		PRINTF_DBG("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();
	while(1)
	{
		while(PRINTF_API_print_from_debug_buffer(64));
		sleep(1);
	}


	return 0;
}


extern "C" {
	int custom_rand_generate(void) {return 0;}
}
