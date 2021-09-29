#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_sntp.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/apps/sntp.h"
#include "lwip/apps/sntp_opts.h"
#include <string.h>
#include "lib/com/eth/listener.h"
#include "lib/com/eth/tcpsocket.h"
#include "lib/com/eth/udpsocket.h"
#include "lib/protocol/client.h"
#include "lib/protocol/discovery.h"
#include "driver/gpio.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"

#define SSID "Koole Controls"
#define PSWD "K@u5tGD!8Ug&X!rc"


JBV::Client client;


extern "C" {
   void app_main();
}



void PrintHEX(void* data, size_t size)
{
	for (int i = 0; i < size; i++)
		printf("%02x ", ((uint8_t*)data)[i]);
	printf("\n");
}


esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}





ResponseFrame* OnMessageReceived(JBV::Client* client, RequestFrame* rx)
{
	ResponseFrame* response = NULL; 
	ESP_LOGI("Main", "Received CMD '%s'", (char*)rx->Data);
	
	if (!strcmp((char *)rx->Data, "LED 0"))
	{
		gpio_set_level(GPIO_NUM_2, 0);
		response = ResponseFrame::ASCII(rx->SrcAddress, rx->FrameID, "OKE");
	}
	else if (!strcmp((char *)rx->Data, "LED 1"))
	{
		gpio_set_level(GPIO_NUM_2, 1);
		response = ResponseFrame::ASCII(rx->SrcAddress, rx->FrameID, "OKE");
	}
	
	
	if(response == NULL)
		response = ResponseFrame::ASCII(rx->SrcAddress, rx->FrameID, "Command not supported");
	
	return response;
}


void app_main(void)
{
	nvs_flash_init();
	tcpip_adapter_init();
	ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

	wifi_config_t sta_config = {};
	memcpy(sta_config.sta.ssid, SSID, strlen(SSID));
	memcpy(sta_config.sta.password, PSWD, strlen(PSWD));
	sta_config.sta.bssid_set = false;
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
	ESP_ERROR_CHECK( esp_wifi_start() );
	ESP_ERROR_CHECK( esp_wifi_connect() );

	setenv("TZ", "UTC-1UTC,M3.5.0,M10.5.0/3", 1);
	tzset();
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
	
	//Setup client
	client.SID = SoftwareID::TestApp;
	client.myAddress = 0;
	esp_read_mac((uint8_t*)&client.myAddress, ESP_MAC_WIFI_STA);
	client.OnRequestFrame.Bind(&OnMessageReceived);

	//Add TCP listener
	TCPListener listener;
	client.AddListener(&listener);
	
	//Add UDP Broadcast socket
	UDPSocket sock;
	sock.Type = IConnection::ConnectionTypes::Broadcast;
	sock.Connect("255.255.255.255", 51100);
	client.AddConnection(&sock);
	

	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_2, 0);
	

	while (1)
	{
		
		
		//ESP_LOGI("Main", "xPortGetFreeHeapSize() = %08x", xPortGetFreeHeapSize());
		
		
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	
	/*
	UDPSocket sock;
	
	
	

	return;
	
	
	
	ESPNOW espnow;
	espnow.Init();
	espnow.OnBroadcast.Bind(&ESPNOWBroadcast);
	
	
	
	
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_2, 1);
	
	const char c1 = '1';
	const char c0 = '0';
	
	bool wap = false;
	while (1)
	{
		
		if (wap)
			espnow.SendBroadcast((uint8_t *)&c1, 1);
		else
			espnow.SendBroadcast((uint8_t *)&c0, 1);
			
		
		wap = !wap;
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		
		
	}
	*/
}










