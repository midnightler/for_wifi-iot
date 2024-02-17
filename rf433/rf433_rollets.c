uint16_t half_cycle  = 50;// прикрыть/приоткрыть
uint16_t count_cycle;// кол-во циклов для роллеты (время откр/закр)
uint16_t count1_cycle  = 100;// полное откр/закр - 200мс*100=20сек.
uint8 count2_cycle = 0; // кол-во циклов для групп
uint16_t tik1_cycle = 0; //циклы для роллеты
uint8 tik2_cycle = 0; // циклы для групп
uint8 flag_run = 0; //флаг работы
uint16_t ngpio = 0; //номер gpio
bool lgpio; //уровень 1/0 gpio
uint8 nmbr = 0; //номер элемента из массива
int  roll[] = {140,141,142,143,144}; //массив номеров gpio

static timer_t esp_timer_rc;

void read_kod() {
		if (flag_run == 0 && valdes[0] != 0) // ловлю нажатие
		{
		ngpio = valdes[0]/10;
		lgpio = valdes[0]%10;
		}
		if (flag_run == 0 && valdes[0] >= 1450 && valdes[0] <= 1601) //для реле
		{
		digitalWrite(ngpio,lgpio);//отправляю код
		valdes[0] = 0;
		}	
		if (flag_run == 0 && valdes[0] >= 1400 && valdes[0] <= 1441) //для ролет по одной
		{
		flag_run = 1;
		count_cycle = count1_cycle;
		}
	if (flag_run == 1)
	{
		digitalWrite(ngpio,lgpio);//отправляю код
		tik1_cycle ++;//считаю циклы
		if(tik1_cycle >= count_cycle)//если больше
		{// обнуляем для нового цикла
		tik1_cycle = 0;
		valdes[0] = 0;
		flag_run = 0;
		}
	}
	if (flag_run == 0 && valdes[0] >= 9010 && valdes[0] <= 9061) //роллеты группами
	{		
		flag_run = 2;
		if (ngpio == 901) //street full open/glosed
		{
		count_cycle  = count1_cycle;
		count2_cycle = 2;
		}
		if (ngpio == 902) //street half open/glosed
		{
		count_cycle  = half_cycle;
		count2_cycle = 2;
		}
		if (ngpio == 903) //all open/glosed
		{
		count_cycle  = count1_cycle;
		count2_cycle = 4;
		}
	}
	if (flag_run == 2)
	{
		ngpio = roll[nmbr];//читаю из массива
		digitalWrite(ngpio,lgpio);//отправляю код
		tik1_cycle ++;//считаю циклы
		if(tik1_cycle > count_cycle)//если больше
		{
		tik1_cycle = 0;// обнуляем 
		tik2_cycle ++; ////считаю циклы циклов
		nmbr+=1;// прибавляю номер ячейки
		delay(3);//??
		if(tik2_cycle > count2_cycle)//если больше
		{// обнуляем для нового цикла
		valdes[0] = 0;
		flag_run = 0;
		tik2_cycle = 0;
		nmbr = 0;
		}
		}
	}
	}
			
void startfunc() {
	ets_timer_disarm(&esp_timer_rc);
	ets_timer_setfn(&esp_timer_rc, read_kod, NULL);
	ets_timer_arm(&esp_timer_rc, 200, 1);//  200мс, постоянная работа
		if (sensors_param.cfgdes[1] != 20)
			{
		count1_cycle = sensors_param.cfgdes[1]*5 ;//циклы > время сек
			}
		if (sensors_param.cfgdes[2] != 10)
			{
		half_cycle = sensors_param.cfgdes[2]*5 ;// полуциклы > время сек
			}
}

void timerfunc(uint32_t timersrc) { 
vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

void webfunc(char *pbuf) { //тут обьяснялок не будет
os_sprintf(HTTPBUFF,"<style>.tab{border:1px solid #ccb;background-color:#f1f1f1;}.tab button{background-color:inherit;padding:4px;font-size:19px;width:33%%;}.tab button.active{background-color:#ccc;}.tbc{padding:6px 12px;border:12px solid #ccc;animation:fadeEffect 0.5s;}@keyframes fadeEffect{from{opacity:0;}to{opacity:1;}}button{font-size:20px;padding:20px;width:50%%;}em {color:magenta;}en {font-size: 28px;}</style><div class=\"tab\"><button class=\"tbl active\" onclick=\"oTb(event, \'T1\')\" id=\"defOp\">Роллеты<br>группами.</button><button class=\"tbl\" onclick=\"oTb(event, \'T2\')\">Роллеты<br>по одной.</button><button class=\"tbl\" onclick=\"oTb(event, \'T3\')\">Освещение<br>Двор.</button></div><div id=\"T1\" class=\"tbc\" style=\"display: block;\"><en>Все окна:</en><div><button type=\"button\" onclick=\"fn(9031)\">Все<br>Открыть.</button><button type=\"button\" onclick=\"fn(9030)\">Все<br>Закрыть.</button></div><br><hr><en>Уличная сторона:</en><div><button type=\"button\" onclick=\"fn(9011)\">Все<br>Открыть.</button><button type=\"button\" onclick=\"fn(9010)\">Все<br>Закрыть.</button></div><br><hr></p></div><div id=\"T2\" class=\"tbc\" style=\"display: none;\"><en>Спальня 1:</en><div><button type=\"button\" onclick=\"fn(1401)\">Лев.<br>Открыть. </button><button type=\"button\" onclick=\"fn(1411)\">Прав.<br>Открыть.</button></div><p></p><div><button type=\"button\" onclick=\"fn(1400)\">Лев.<br>Закрыть.  </button><button type=\"button\" onclick=\"fn(1410)\">Прав.<br>Закрыть.  </button></div><br><hr><en>Спальня 2:</en><div><button type=\"button\" onclick=\"fn(1421)\">Открыть.<p></p></button><button type=\"button\" onclick=\"fn(1420)\">Закрыть.<p></p></button></div><br><hr><en>Спальня 3:</en><div><button type=\"button\" onclick=\"fn(1431)\">Открыть.<p></p></button><button type=\"button\" onclick=\"fn(1430)\">Закрыть.<p></p></button></div><br><hr><en>Спальня 4:</en><div><button type=\"button\" onclick=\"fn(1441)\">Открыть.<p></p></button><button type=\"button\" onclick=\"fn(1440)\">Закрыть.<p></p></button></div><br><hr></div><div id=\"T3\" class=\"tbc\" style=\"display: none;\"><en>Освещение улицы:</en><div><button type=\"button\" onclick=\"fn(1451)\">Включить.</button><button type=\"button\" onclick=\"fn(1450)\">Отключить.</button></div></div><script type=\"text/javascript\" charset=\"utf-8\">function oTb(evt, TbName) {var i, tbc, tbl;tbc = document.getElementsByClassName(\"tbc\");for (i = 0; i < tbc.length; i++) {tbc[i].style.display = \"none\";}tbl = document.getElementsByClassName(\"tbl\");for (i = 0; i < tbl.length; i++) {tbl[i].className = tbl[i].className.replace(\" active\", \"\");}document.getElementById(TbName).style.display = \"block\";evt.currentTarget.className += \" active\";}document.getElementById(\"defOp\").click();function fn(x){(document.createElement('img')).src=(location.href+'valdes?int=1&set='+x);}</script> ");
}



//  Глобальные переменные 2
//  Количество настроек
//  Режим, вр.закрытия, вр.прикрытия, -1
//  Имя вкладки настроек
//  433-Set


