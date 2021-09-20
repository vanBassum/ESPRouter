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
#include "driver/gpio.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"




extern "C" {
   void app_main();
}



/*
#define SECTORSIZE 4096
const esp_partition_t* partBoot;
const esp_partition_t* partRunning;
const esp_partition_t* partUpdate;

size_t erasePointer = 0;

esp_err_t Prep()
{
	partBoot = esp_ota_get_boot_partition();
	partRunning = esp_ota_get_running_partition();
	partUpdate = esp_ota_get_next_update_partition(NULL);


	ESP_LOGI("MAIN", "Boot partition    = '%s'. Address 0x%08x Size 0x%08x Encrypted = %d", partBoot->label, partBoot->address, partBoot->size, partBoot->encrypted);
	ESP_LOGI("MAIN", "Running partition = '%s'. Address 0x%08x Size 0x%08x Encrypted = %d", partRunning->label, partRunning->address, partRunning->size, partRunning->encrypted);
	ESP_LOGI("MAIN", "Update partition  = '%s'. Address 0x%08x Size 0x%08x Encrypted = %d", partUpdate->label, partUpdate->address, partUpdate->size, partUpdate->encrypted);

	if (partUpdate == NULL)
		return ESP_FAIL;
	
	erasePointer = 0;
	return ESP_OK;
}

esp_err_t WriteOwn(size_t address, void* data, uint32_t size)
{
	esp_err_t err = ESP_FAIL;
	uint32_t max = address + size;
	while (max > erasePointer)
	{
		err = esp_partition_erase_range(partUpdate, erasePointer, SECTORSIZE);
		if (err == ESP_OK)
			erasePointer += SECTORSIZE;
		else
		{
			ESP_LOGE("MAIN", "esp_partition_erase_range (%s)", esp_err_to_name(err));
			return err;
		}
	}

	err = esp_partition_write(partUpdate, address, data, size);

	if (err != ESP_OK)
		ESP_LOGE("MAIN", "esp_partition_write (%s)", esp_err_to_name(err));

	return err;
}

esp_err_t End()
{
	esp_err_t err = esp_ota_set_boot_partition(partUpdate);

	return err;
}



enum class Commands : uint8_t
{
	LedON = 1,
	LedOFF = 2,
	UpdateStart = 3,
	UpdateData = 4,
	UpdateEnd = 5,
};

#pragma pack(1)
struct CmdPackage
{
	Commands cmd;
	
};
*/

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



void SocketDisconnected(TCPSocket* socket)
{
	ESP_LOGI("MAIN", "TCP Disconnected");
	delete socket;
}


JBV::Client client;

void SocketAccepted(TCPSocket* socket)
{
	socket->OnDisconnect.Bind(&SocketDisconnected);
	client.SetConnection(socket);
	ESP_LOGI("MAIN", "Accepted TCP socket");
}


void OnMessageReceived(JBV::Client* client, uint8_t src[6], uint8_t* data, uint16_t dataSize)
{
	ESP_LOGI("Main", "Received CMD '%s'", (char*)data);
	
	if (!strcmp((char *)data, "LED 0"))
	{
		gpio_set_level(GPIO_NUM_2, 0);
		client->SendMessage(src, (uint8_t*)"OKE", 3);
	}
	else if (!strcmp((char *)data, "LED 1"))
	{
		gpio_set_level(GPIO_NUM_2, 1);
		client->SendMessage(src, (uint8_t*)"OKE", 3);
	}
	
	
	
	
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
	memcpy(sta_config.sta.ssid, "vanBassum", 10);
	memcpy(sta_config.sta.password, "pedaalemmerzak", 15);
	sta_config.sta.bssid_set = false;
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
	ESP_ERROR_CHECK( esp_wifi_start() );
	ESP_ERROR_CHECK( esp_wifi_connect() );

	setenv("TZ", "UTC-1UTC,M3.5.0,M10.5.0/3", 1);
	tzset();
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();

	//UDPSocket sock;
	//sock.OnDataReceived.Bind(&UDPRecieved);

	TCPListener listener;
	listener.OnSocketAccepted.Bind(&SocketAccepted);
	client.OnMessageReceived.Bind(&OnMessageReceived);

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










