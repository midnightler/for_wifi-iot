// рабочий пример таймера для ESP32
static timer_t esp_timer_name; // глобально объявим таймер esp_timer_name

void read_kod() { // код, который будет вызываться по таймеру
	}
			
void startfunc() {
	ets_timer_disarm(&esp_timer_name);
	ets_timer_setfn(&esp_timer_name, read_kod, NULL);  // read_kod -функция, которую нужно вызвать по таймеру.
	ets_timer_arm(&esp_timer_name, 200, 1);//  200мс, 1-постоянная работа , 0 -однократно.
}

void timerfunc(uint32_t timersrc) { 
vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

void webfunc(char *pbuf) { //тут html код
os_sprintf(HTTPBUFF,"бла-бла-бла ");
}



