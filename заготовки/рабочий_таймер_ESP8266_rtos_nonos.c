

void ICACHE_FLASH_ATTR read_esp(){
// код, работающий по таймеру
}

static os_timer_t esp_timer; // глобально объявим таймер esp_timer

os_timer_disarm(&esp_timer);
os_timer_setfn(&esp_timer, (os_timer_func_t *)read_esp, NULL); // read_esp -функция, вызваемая по таймеру.
os_timer_arm(&esp_timer, 1000, 1); // запуск таймера:// 1000 миллисекунд. 1 - многократно. 0 -однократно.







