int32_t tempSave=1;
int32_t tempVhod;
uint8_t vremia_oshibki=0;
bool tmzone=0;
uint8_t gpio=12; //номер гпио выход
uint16_t tik1_button = 0; //если нажали кнопку считаю время нажатия
uint8_t flag_button = 0;
bool sost_button = 1;
bool short_button = 0;
uint8_t mode_clicks = 0;


static os_timer_t esp_timer1;

read_kod()
{
		if(sensors_param.cfgdes[4] < 0 || sensors_param.cfgdes[4] > 2 ){
		sensors_param.cfgdes[4] = 0;
	}
		mode_clicks = sensors_param.cfgdes[4];
	
	if (digitalRead(2) != sost_button)//ловлю изменение гпио
	{
		flag_button ++;
		sost_button = digitalRead(2);//запоминаю состояние гпио
	}
	if (flag_button == 1 && sost_button == 0)//если нажата кнопку считаю время нажатия
	{
		tik1_button ++;
	}
	if (flag_button > 1)//если дребезг или отпустили кнопку
	{
		if(sost_button == 1)//если кнопка отпущена 
		{
			if(tik1_button > 30 && tik1_button < 1500)//если время нажатия больше 30мс и меньше 1,5сек
			{
				short_button = 1;//то это короткое нажатие
			}
		}
		tik1_button = 0;//обнуляем переменные если время составило меньше 30 мс , значит это дребезг
		flag_button = 0;//обнуляем переменные
	}	
	if(short_button == 1)//если зафиксировано нажатие
	{
		mode_clicks += 1;// суммируем нажатия
		if(mode_clicks > 2)
		{
		mode_clicks = 0;
		}
		sensors_param.cfgdes[4] = mode_clicks;// перекладываем в переменную для обработки в основном цикле
		short_button = 0;//обнуляем переменные
		}
	}
	
void startfunc() {
	os_timer_disarm(&esp_timer1);
	os_timer_setfn(&esp_timer1, (os_timer_func_t *)read_kod, NULL);
	os_timer_arm(&esp_timer1, 1, 1);// 1мс , постоянная работа
}

void timerfunc(uint32_t timersrc) { 

    tempVhod = data1wire[0] ;//датчик температуры dsw1 (делить на 10)

   if ( sensors_param.cfgdes[0] < 0 || 23 < sensors_param.cfgdes[0] ) 
		sensors_param.cfgdes[0] = 0 ; //проверка часы

    if ( sensors_param.cfgdes[1] < 0 || 23 < sensors_param.cfgdes[1] ) 
		sensors_param.cfgdes[1] = 0 ; //проверка часы
	
    if ( sensors_param.cfgdes[4] < 0 || 2 < sensors_param.cfgdes[4] ) 
		sensors_param.cfgdes[4] = 0 ; //проверка режим
	
    if ( tempVhod != 0 && tempVhod != 850 && tempVhod != 2550 ) { 
        tempSave = tempVhod/10 ; 
    } else { 
        vremia_oshibki++ ; 
    }
	
    if ( 600 <= vremia_oshibki ){ //если датчик оборвало работаем минуту (10) по сохраненному значению,если он не восстановится ,тогда уже станем в ошибкy
        tempSave = tempVhod/10 ; 
        vremia_oshibki = 0 ; 
    }
	
    if ( sensors_param.cfgdes[0] < sensors_param.cfgdes[1] ) {
        if ( sensors_param.cfgdes[0] <= time_loc.hour && time_loc.hour < sensors_param.cfgdes[1] ) {
            tmzone = 1 ; //работает
        } else { 
            tmzone =0 ; //не работает
        }
    } else {
        if ( ( sensors_param.cfgdes[0] <= time_loc.hour && time_loc.hour < 24 ) || ( 0 <= time_loc.hour && time_loc.hour < sensors_param.cfgdes[1] ) ) { 
            tmzone = 1 ; 
        } else { 
            tmzone = 0 ; 
        }
    }
	
    if ( sensors_param.cfgdes[4] == 0 ) 
		digitalWrite(gpio,0);//выкл постоянно
    if ( sensors_param.cfgdes[4] == 1 ){ //термостат по времени
     if ( tmzone == 0 ){
		digitalWrite(gpio,0);
}  else {
            if ( sensors_param.cfgdes[3] < tempSave ) 
				digitalWrite(gpio,0);
            if ( tempSave < sensors_param.cfgdes[2] ) 
				digitalWrite(gpio,1);
        }
    } 
	
    if ( sensors_param.cfgdes[4] == 2 ){//термостат работает постоянно 
            if ( sensors_param.cfgdes[3] < tempSave ) 
				digitalWrite(gpio,0);
            if ( tempSave < sensors_param.cfgdes[2] ) 
				digitalWrite(gpio,1);
        }

 delay(1000); // обязательная строка, минимальное значение - 10мс
    }
void webfunc(char *pbuf) { //тут обьяснялок не будет
    if ( sensors_param.cfgdes[4] == 1 && tmzone == 0 ) {
        os_sprintf(HTTPBUFF,"<br><div class='h' style='background: #73c140 '> РЕЖИМ :<b><font color='yellow'>ПО ВРЕМЕНИ</font></b> НАГРЕВ : <font color='blue'>ВЫКЛЮЧЕН ПО ВРЕМЕНИ</font> . &nbsp;&nbsp;");
    } else {

    if ( GPIO_ALL_GET(gpio)!=0 && sensors_param.cfgdes[4] == 1 ) {
        os_sprintf(HTTPBUFF,"</div><br><div class='h' style='background: #73c140 '> РЕЖИМ :<b><font color='yellow'>ПО ВРЕМЕНИ</font></b> НАГРЕВ : <font color='red'>ВКЛЮЧЕН </font> . &nbsp;&nbsp;");
    } 
    if ( GPIO_ALL_GET(gpio)==0 && sensors_param.cfgdes[4] == 1 ) {
        os_sprintf(HTTPBUFF,"</div><br><div class='h' style='background: #73c140 '> РЕЖИМ :<b><font color='yellow'>ПО ВРЕМЕНИ</font></b> НАГРЕВ : <font color='yellow'>ВЫКЛЮЧЕН</font> . &nbsp;&nbsp;");
    } 
}
    if ( GPIO_ALL_GET(gpio)!=0 &&  sensors_param.cfgdes[4] == 2 ) {
        os_sprintf(HTTPBUFF,"</div><br><div class='h' style='background: #73c140 '> РЕЖИМ :<b><font color='red'> ПОСТОЯННО </font></b> НАГРЕВ : <font color='red'>ВКЛЮЧЕН </font> . &nbsp;&nbsp;");
    }	
    if ( GPIO_ALL_GET(gpio)==0 &&  sensors_param.cfgdes[4] == 2 ) {
        os_sprintf(HTTPBUFF,"</div><br><div class='h' style='background: #73c140 '> РЕЖИМ :<b><font color='red'> ПОСТОЯННО </font></b> НАГРЕВ : <font color='yellow'>ВЫКЛЮЧЕН </font> . &nbsp;&nbsp;");
    }		
    if ( sensors_param.cfgdes[4] == 0  ) {
        os_sprintf(HTTPBUFF,"</div><br><div class='h' style='background: #73c140 '> РЕЖИМ :<b><font color='black'> ВЫКЛЮЧЕН</font></b> НАГРЕВ : <font color='black'>ВЫКЛЮЧЕН</font> . &nbsp;");
    }
		os_sprintf(HTTPBUFF,"</div><div class='c'><div class='main'><form action='configdes'><pre><table border='0'class='catalogue'><tr style='background-color: yellow'><td> Вкл.С. </td><td> Вкл.До. </td><td> Темп.ВКЛ. </td><td> Темп.ВЫКЛ.</td></tr><tr><td> <INPUT size=1 NAME='cfg1' value='%02d'> ч.</td><td> <INPUT size=1 NAME='cfg2' value='%02d'> ч.</td><td>ниже <INPUT size=1 NAME='cfg3' value='%s'>°C</td>",sensors_param.cfgdes[0],sensors_param.cfgdes[1], fltostr(sensors_param.cfgdes[2]));
    os_sprintf(HTTPBUFF+os_strlen(HTTPBUFF),"</div><td>выше <INPUT size=1 NAME='cfg4' value='%s'>°C</td></tr></table><br><input type='hidden' name='cfg5' value='%d'><input type='hidden' name='st' value=3><input type=submit value='принять' onclick='pb(cfg3);pb(cfg4)'> <input type=submit value='режим' onclick='pb(cfg3);pb(cfg4);eb(cfg5)'></pre></form></div><script>function pb(x){x.value = x.value*10};function eb(x){x.value = Number(x.value)+1}</script>",fltostr(sensors_param.cfgdes[3]),sensors_param.cfgdes[4]);

}





