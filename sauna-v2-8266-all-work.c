//  упрощаем написание
#define wremya_d (sensors_param.cfgdes[0]) // время начала дневной
#define wremya_n (sensors_param.cfgdes[1]) // время начала ночной
#define temp_d (sensors_param.cfgdes[2]) // темп. дневная
#define temp_n (sensors_param.cfgdes[3]) // темп. ночная
#define trejim (sensors_param.cfgdes[4])   // режим 0/1/2 - выкл//день-ночь//постоянно по дневной 
#define wremya_s_on (sensors_param.cfgdes[5]) // время разрешения работы сауны
#define wremya_s_off (sensors_param.cfgdes[6]) // время запрешения работы сауны
// сауна
uint8_t sauna = 9; //номер гпио выход sauna
uint8_t shour_start; // время (часы) начала работы 
uint8_t smin_start;  //  минуты начала работы 
uint8_t shour_stop;  //  время (часы) завершения работы 
uint8_t smin_stop;   //  минуты завершения работы 
uint8_t flag_dop = 0; // кол-во доп. включений
bool flag_work = 0;   // флаг "сегодя включалось"
uint8_t flag_zapret = 0; // всё, нельзя продлить
bool timer_on = 0;
uint16_t timer_prodleniya = 0;  //  время, когда можно продлить (1 час)
const char* s_on_of; // текст для подстановки на главную  sauna
const char* s_fnt;// цвет для подстановки на главную  sauna
// термостат
int32_t tempSave=0;
int32_t tempZal;
uint16_t vremia_oshibki=0;
bool flag_error = 0;
bool tmzone=0; //  день/ночь
uint8_t nasos = 4; //номер гпио выход nasos
//индикаторы
uint8_t s_ledr = 14;  // номер GPIO красный св.д.
uint8_t s_ledb = 16;  // номер GPIO голубой св.д.
			// 14+16 = зелёный св.д.
uint8_t n_ledr = 15;  // номер GPIO красный св.д.
uint8_t n_ledb = 2;  // номер GPIO голубой св.д.
			// 15+2 = зелёный св.д.
// кнопки
uint8_t n_button = 13;  // номер GPIO кнопки насоса
uint8_t s_button = 12;  // номер GPIO кнопки сауны
bool sost_s = 1;
bool sost_n = 1;
bool ruchnoy = 0;
uint16_t tik_s = 0; // считаю время нажатия
uint16_t tik_n = 0;
uint8_t flag_s = 0;
uint8_t flag_n = 0;
uint8_t swkl = 0;
uint8_t nwkl = 0;
int8_t sclic = 0;
int8_t nclic = 0;
const char* t_on_of; // текст для подстановки на главную
const char* t_fnt;// цвет для подстановки на главную
const char* t_rj; // режим для подстановки на главную termostat
const char* o_f = "ВЫКЛЮЧЕН";
const char* o_n = "ВКЛЮЧЕН";
const char* o_r = "РУЧНОЙ";
const char* d_n = "День-Ночь";
const char* w_d = "Всегда-День";
const char* crl = "coral";
const char* elw = "yellow";
const char* ds = "disabled";
const char* en = "";
const char* bt_wkl;
const char* bt_ust;
const char* bt_dop;

static os_timer_t esp_timer1;
void read_kod(){
		if (digitalRead(n_button) != sost_n)//ловлю изменение гпио
	{
		flag_n ++;
		sost_n = digitalRead(n_button);//запоминаю состояние гпио
	}
	if (flag_n == 1 && sost_n == 0)//если нажата кнопка, считаю время нажатия
	{
		tik_n ++;
	}
	if (flag_n > 1)//если дребезг или отпустили кнопку
	{
		if(sost_n == 1)//если кнопка отпущена 
		{
			if(tik_n > 30 && tik_n < 1500)//если время нажатия больше 30мс и меньше 1,5сек
			{//то это короткое нажатие
		nwkl = 1;// перекладываем для обработки в основном цикле
		tik_n = 0;//обнуляем переменные
		flag_n = 0;//обнуляем переменные
			}
			if(tik_n >= 1500)// больше 1,5сек
			{//то это длинное нажатие
		nwkl = 2;// перекладываем для обработки в основном цикле
		tik_n = 0;//обнуляем переменные
		flag_n = 0;//обнуляем переменные
			}
		}
		tik_n = 0;//если меньше 30 мс , значит это дребезг
		flag_n = 0;//обнуляем переменные
	}
///////////
		if (digitalRead(s_button) != sost_s)//ловлю изменение гпио
	{
		flag_s ++;
		sost_s = digitalRead(s_button);//запоминаю состояние гпио
	}
		if (flag_s == 1 && sost_s == 0)//если нажата кнопка, считаю время нажатия
	{
		tik_s ++;
	}
		if (flag_s > 1)//если дребезг или отпустили кнопку
	{
		if(sost_s == 1)//если кнопка отпущена 
		{
			if(tik_s > 30 && tik_s < 1500)//если время нажатия больше 30мс и меньше 1,5сек
			{//то это короткое нажатие
		swkl = 1;// перекладываем для обработки в основном цикле
		tik_s = 0;//обнуляем переменные
		flag_s = 0;//обнуляем переменные
			}
			if(tik_s >= 1500)// больше 1,5сек
			{//то это длинное нажатие
		swkl = 100;// перекладываем для обработки в основном цикле
		tik_s = 0;//обнуляем переменные
		flag_s = 0;//обнуляем переменные
			}
		}
		tik_s = 0;//если меньше 30 мс , значит это дребезг
		flag_s = 0;//обнуляем переменные
	}
}

void ICACHE_FLASH_ATTR startfunc(){
              digitalWrite(sauna,0);
              digitalWrite(nasos,0);
		flag_dop = 0;
		flag_work = 0;
		flag_zapret = 0;
		timer_prodleniya = 0;
		s_on_of = o_f;
		s_fnt = elw;
		bt_wkl = en;
		bt_ust = en;
		bt_dop = ds;
	ets_timer_disarm(&esp_timer1);
	ets_timer_setfn(&esp_timer1, read_kod, NULL);
	ets_timer_arm(&esp_timer1, 1, 1);// 1мс , постоянная работа
}
void ICACHE_FLASH_ATTR timerfunc(uint32_t timersrc){
	    ///	
	    tempZal = vsens[0][0];//датчик температуры dsw1 (делить на 100)
		if ( trejim == 0 ){ //выкл постоянно
		digitalWrite(n_ledr,0);
		digitalWrite(n_ledb,0);
		}
		if ( trejim == 1 ){ //термостат по времени (зелёный)
		digitalWrite(n_ledr,1);
		digitalWrite(n_ledb,1);
		}
		if ( trejim == 2 ){ //постоянно по дневной (красный)
		digitalWrite(n_ledr,1);
		digitalWrite(n_ledb,0);
		}
//  обработчик кнопок   
	if ( swkl == 1 ){ // ловим физ. кнопку "сауна"
	sclic++;
	if ( sclic < 0 || sclic > 3){ 
		sclic = 0 ; }//закольцовка нажатий 
		swkl = 0;
	if ( sclic == 1 ){ 
	valdes[0] = 1;
	}
	if ( sclic == 2 ){ 
	valdes[0] = 20;
	}
	if ( sclic == 3 ){ 
	valdes[0] = 20;
	}
	}
	if ( swkl == 100 ){
		valdes[0] = 100;
		swkl = 0;
		}
	if ( nwkl == 1 ){// ловим физ. кнопку "насос"
	nclic++;
	if ( nclic < 0 || 1 < nclic ){ 
		nclic = 0 ; }  //закольцовка нажатий 
		nwkl = 0;
	if ( nclic == 1 ){ 
	 ruchnoy = 1;
	}else{
	 ruchnoy = 0;
	}
	}
	///////// >>>> termostat begin
   if ( wremya_d < 0 || 23 < wremya_d ){ 
		wremya_d = 0 ;} //проверка часы
    if ( wremya_n < 0 || 23 < wremya_n ) {
		wremya_n = 0 ;} //проверка часы
    if ( trejim < 0 || 2 < trejim ){ 
		trejim = 0 ; }//проверка режим
    if ( tempZal != 0 && tempZal != 850 && tempZal != 2550 ) { // ошибка ds18b20
        tempSave = tempZal;
        flag_error = 0;
        vremia_oshibki = 0;
    } else { 
    if (flag_error == 0){
        vremia_oshibki++ ; 
    if ( 600 <= vremia_oshibki ) //если датчик оборвало, работаем 10 минут по сохраненному значению
        flag_error = 1; //если не восстановится, отключаемся
        } 
    }
    if ( wremya_d < wremya_n ) {
        if ( wremya_d <= time_loc.hour && time_loc.hour < wremya_n ) {
            tmzone = 0 ; //день
        } else { 
            tmzone =1 ; //ночь
        }
    }
		if (ruchnoy == 1){
		digitalWrite(nasos,1);//вкл постоянно "вручную" (голубой)
		digitalWrite(n_ledr,0);
		digitalWrite(n_ledb,1);
		t_rj = o_r;
		t_on_of = o_n;
		t_fnt = crl;
	}else{
		if ( trejim == 0 || flag_error == 1){ //выкл постоянно
		digitalWrite(nasos,0);
		t_rj = o_f;
		t_on_of = o_f;
		t_fnt = elw;
		}else{
		if (trejim == 1){
		t_rj = d_n;
		}
		if (trejim == 2){
		t_rj = w_d;
		}		
		if ((trejim == 1 && tmzone == 0) || ( trejim == 2 )){ //термостат по времени день или постоянно по дневной температуре
		if (digitalRead(nasos) == 1 && tempSave > temp_d){ 
		digitalWrite(nasos,0);
		t_on_of = o_f;
		t_fnt = elw;
        }
		if (digitalRead(nasos) == 0 && tempSave < (temp_d-3)){
		digitalWrite(nasos,1);
		t_on_of = o_n;
		t_fnt = crl;
            } 
    } //  день
		if (trejim == 1 &&  tmzone == 1 ){ //термостат по времени ночь
		if (digitalRead(nasos) == 1 && tempSave > temp_n){
		digitalWrite(nasos,0);
		t_on_of = o_f;
		t_fnt = elw;
        }
            if (digitalRead(nasos) == 0 && tempSave < (temp_n-3)){
		digitalWrite(nasos,1);
		t_on_of = o_n;
		t_fnt = crl;
            } 
	    } //термостат по времени ночь
     }
  } ///////<<<< termostat end
    ///////////////  sauna begin 

	if ((time_loc.hour == wremya_s_on) && (time_loc.min == 0)){// сброс по утру
              valdes[0] = 0;
              valdes[1] = 0;
              valdes[2] = 0;
		flag_dop = 0;
		flag_work = 0;
		flag_zapret = 0;
		timer_on = 0;
		timer_prodleniya = 0;
		shour_start = 0;
		smin_start = 0;
                shour_stop  = 0;
		smin_stop = 0;
		bt_wkl = en;
		bt_ust = en;
		bt_dop = ds;
    
		}
     if ((flag_work == 0) && (digitalRead(sauna) == 1))
		{
		digitalWrite(sauna,0); // сброс джет запроса
		}
          if ((wremya_s_on <= time_loc.hour) && (time_loc.hour < wremya_s_off))
		{  // можно включать >>  
     if ((flag_work == 0) && (0 < valdes[0] && valdes[0] <= 3) && (valdes[1] == 0) && (valdes[2] == 0)) //проверка - сегодня не включалась
	   {// >>>>> включить сейчас 
           if (valdes[0] == 1) {  //  вкл. на 2ч.
		shour_start = time_loc.hour;
		smin_start = time_loc.min;
//                shour_stop  = (time_loc.hour + 2);
		shour_stop  = (time_loc.hour);
		smin_stop = (time_loc.min+3);
                   }
	   if (valdes[0] == 2)
		{ 
		shour_start = time_loc.hour;
		smin_start = time_loc.min;
		if(time_loc.min >= 30)  {   //  вкл. на 2ч.30мин.
		smin_stop = (time_loc.min - 30);
                shour_stop = (time_loc.hour + 3);
		  } else {
		shour_stop = (time_loc.hour + 2);
		smin_stop = (time_loc.min + 30);
		}
		}
            if (valdes[0] == 3){ //  вкл. на 3ч.
		shour_start = time_loc.hour;
		smin_start = time_loc.min;
                shour_stop  = (time_loc.hour + 3);
		smin_stop = time_loc.min;
		}
		digitalWrite(sauna,1);
                flag_work = 1;
                valdes[0] = 0;
                swkl = 0;
		digitalWrite(s_ledr,1);
		digitalWrite(s_ledb,0);
		s_on_of = o_n;
		s_fnt = crl;
		bt_wkl = ds;
		bt_ust = ds;
		bt_dop = en;
                   }// <<<<< включить сейчас
	   // >>>>>  установка времени готовности сауны
	   if ((flag_work == 0) && (valdes[1] != 0) && (valdes[2] != 0) && (0 < valdes[0] && valdes[0] <= 3)) // вызов + проверка - сегодня не включалась
	   { 
	   shour_stop = valdes[1];
	   smin_stop = valdes[2];
	   
	   if (valdes[0] == 1)
		{ 
		shour_start = (shour_stop - 2);
		smin_start = smin_stop;
		}
           if (valdes[0] == 2)
                 {  //  вкл. на 2ч30м.
	   if (smin_stop < 30)
		 {
		shour_start = (shour_stop - 3);
		smin_start = (30 + smin_stop);
                   }else{
		shour_start = (shour_stop - 2);
		smin_start = (smin_stop - 30);
                   }
		   }
            if (valdes[0] == 3) //  вкл. на 3ч.
		{ 
		shour_start = (shour_stop - 3);
		smin_start = smin_stop;
		}
	   if ((shour_start >= wremya_s_on) && (shour_start < wremya_s_off))  //проверка - вр. готовности < время запрета
	   {  // alert ?
	   // здесь код провекрки - не поздно?
		flag_work = 1; // если можно ставим флаг и ждём время
		bt_wkl = ds;
		bt_ust = ds;
		bt_dop = ds;
	   }
		} // <<<<< установка времени готовности сауны
	if ((flag_work == 1) && (time_loc.hour <= shour_start) && (time_loc.min < smin_start))
		{  // если стоит флаг - ждём время и мигаем св.диодом
		if (digitalRead(s_ledb)==1){
		digitalWrite(s_ledb,0);
		}
		if (timersrc%5==0){
		digitalWrite(s_ledb,1);
		}
		}
		
	if ((flag_work == 1) && (time_loc.hour == shour_start) && (time_loc.min == smin_start))
		{  // если стоит флаг - ждём время и включаем
		digitalWrite(sauna,1);
                valdes[0] = 0;
                swkl = 0;
		digitalWrite(s_ledr,1);
		digitalWrite(s_ledb,0);
		s_on_of = o_n;
		s_fnt = crl;
		bt_wkl = ds;
		bt_ust = ds;
		bt_dop = en;
		}

	   // >>>> доп. время
	if ((flag_work == 1) && ((valdes[0] == 20) || (valdes[0] == 30)) && (flag_dop < 2) && (flag_zapret == 0))
		{// добавляем доп. время
		if ((time_loc.hour < shour_stop) || ((time_loc.hour == shour_stop) && (time_loc.min < smin_stop)))
		{// ещё включена
		if ((smin_stop + valdes[0]) < 60)
		{
		smin_stop = (smin_stop + valdes[0]);
		}else{
		shour_stop  = (shour_stop + 1);
		smin_stop = (valdes[0] + smin_stop - 60);
		}
		}else{// если уже отключилась
		if (time_loc.min >= 30)
		{
                shour_stop = (time_loc.hour + 1);
		smin_stop = (valdes[0] + time_loc.min - 60);
		}else{
		shour_stop = time_loc.hour;
		smin_stop = (time_loc.min + valdes[0]);
		}
		}
              digitalWrite(sauna,1);
		flag_dop++;
		timer_on = 0;
                 valdes[0] = 0;
                 swkl = 0;
		digitalWrite(s_ledr,1);
		digitalWrite(s_ledb,0);
		s_on_of = o_n;
		s_fnt = crl;
		} // <<< доп. время
	      }  // <<< можно включать
	  /// выключение >>>
                  if ((digitalRead(sauna) == 1) && (shour_stop <= time_loc.hour) && (smin_stop <= time_loc.min ))
		{ //если вкл., и если время вышло
		digitalWrite(sauna,0); // если вкл., и время больше, и доп.время -0, то выключаем
		digitalWrite(s_ledr,1);
		digitalWrite(s_ledb,1);
		timer_on = 1;
		s_on_of = o_f;
		s_fnt = elw;
		} // <<< выключение 
		if ((valdes[0] == 100) && (flag_work == 1)){// выключаем кнопкой
		flag_zapret++;
		valdes[0] = 0;
		swkl = 0;
		if (flag_zapret > 3){ // + три нажатия - снять запрет
		flag_zapret = 0;
		 }
		}
		 if (flag_zapret > 0){ // выключаем кнопкой
		digitalWrite(sauna,0);
		digitalWrite(s_ledr,0);
		digitalWrite(s_ledb,0);
		s_on_of = o_f;
		s_fnt = elw;
		}
	  if ((timer_on == 1) && (flag_work == 1) && (flag_zapret == 0))
		{// доп. таймер
                  timer_prodleniya++;
              if (timer_prodleniya>=3600){ // пока тикает - можно продлить
                   flag_zapret = 1;
                           }  // досчитал -> на сегодня хватит
                  } // <<< время, когда можно продлить
            valdes[0] = 0;
                   //////  sauna end
  ///   отладка

//	printf("flag = %d %d %d %d %d %d %d\n\n",digitalRead(sauna),flag_work,flag_zapret,flag_dop,timer_on,timer_prodleniya);			
			
		

 delay(1000); // обязательная строка для RTOS, минимальное значение - 10мс
    }
void webfunc(char *pbuf)  { //веб интерфейс
os_sprintf(HTTPBUFF,"<div><style>.i1{font-size:1.3em;text-align:center;}.bt{height: 35px;width:100%;}.br{float:right;}.tbl{width:100%;align:center;margin:0 auto;text-align:center;float:center} th{width:25%;}.panel{display:none;}</style><div class='h' style='background:#778899'> Сауна . <font color='yellow'>НАГРЕВ : </font><b><font color='%s'> %s</font> . </b></div><div><table class='tbl'><tr style='background:#A6EECE'><th></th><th>Греть</th><th>Вр.Выкл.</th><th>Вр.Выкл.</th></tr><tr><td><button %s class='bt' type='button' form='tww' onclick='dataSelect(this.form)'>Включить</button></td><td><form id='tww'><select %s class='bt' name='v0'><option value='1' selected>2 Ч. 00 М.</option><option value='2'>2 Ч. 30 М.</option><option value='3'>3 Ч. 00 М.</option></select></td></form><td><input disabled class='i1' size='2' name='strt' value='%02d:%02d'></td><td><input disabled class='i1' size='2' name='stp' value='%02d:%02d'></td></tr><tr><form id='dm'><td><button %s class='bt' type='button'  onclick='dataSelect(this.form)'>Добавить.</button></td><td><select %s class='bt' name='v0'><option value='20' selected>20 Минут</option><option value='30'>30 М.</option></select></td></form><td><button %s class='bt' type='button' onclick='sts()'>Нагреть ко&nbsp;времени</button></td><form id='dm'><td><button name='v0' value='100' class='bt' type='button' onclick='dataSelect(this.form)'>Выключить.</button></td></form></tr></table></div><div id='sts' class='panel'> <div>&nbsp;<u><h2>Задать время готовности.</h2></u><table class='tbl'><tbody><form id='data'><tr><td>Часы:</td><td> <select class='bt' name='th'><option value='08'>8</option><option value='09'>9</option><option value='10'>10</option><option value='11'>11</option><option value='12'>12</option><option value='13'>13</option><option value='14'>14</option><option value='15'>15</option><option value='16'>16</option><option value='17'>17</option><option selected value='18'>18</option><option value='19'>19</option><option value='20'>20</option><option value='21'>21</option><option value='22'>22</option><option value='23'>23</option><option value='24'>24</option></select></td><td>Минуты:</td><td><select class='bt' name='tm'><option selected value='00'>00</option><option value='05'>05</option><option value='10'>10</option><option value='15'>15</option><option value='20'>20</option><option value='25'>25</option><option value='30'>30</option><option value='35'>35</option><option value='40'>40</option><option value='45'>45</option><option value='50'>50</option><option value='55'>55</option></select></td></form><td><button type='button' form='data' class='bt' onclick='dataSelect(this.form)'>Установить</button></td><td><button class='bt' onclick='stg()'>&nbsp;&nbsp;Отмена&nbsp;&nbsp;</button></td></tr></tbody></table></div></div><br><div class='h' style='background:#1c87d9'>Режим:<b><font color='yellow'> %s</font></b> .<br>Насос:<b><font color='%s'> %s</font></b></div><table class='tbl'><tr style='background-color:#de8887'><th> День </th><th> Ночь </th><th> Т_День </th><th> Т_Ночь</th></tr><form action=configdes><tr><td>c <input class='i1' size='1' name='cfg1' value='%02d'> ч.</td><td>с <input class='i1' size='1' name='cfg2' value='%02d'> ч.</td><td><input class='i1' size='1' name='cfg3' value='%s'>°C</td>",s_fnt,s_on_of,bt_wkl,bt_wkl,shour_start,smin_start,shour_stop,smin_stop,bt_dop,bt_dop,bt_ust,t_rj,t_fnt,t_on_of,wremya_d,wremya_n,fltostr(temp_d));
os_sprintf(HTTPBUFF+os_strlen(HTTPBUFF),"<td><input class='i1' size='1' name='cfg4' value='%s'>°C</td></tr><input type='hidden' name='cfg5' value='%d'><input type='hidden' name='cfg6' value='%02d'><input type='hidden' name='cfg7' value='%02d'><input type='hidden' name='st' value='3'><tr><td>&nbsp;<button  class='bt' onclick='tmp(cfg3);tmp(cfg4);md(cfg5,0)'>Отключить<br>насос</button></td><td>&nbsp;<button  class='bt' onclick='tmp(cfg3);tmp(cfg4);md(cfg5,1)'>Режим&nbsp;по времени</button></td><td>&nbsp;<button  class='bt' onclick='tmp(cfg3);tmp(cfg4);md(cfg5,2)'>Режим<br>постоянно</button></td><td>&nbsp;<button class='bt' onclick='tmp(cfg3);tmp(cfg4)'>Установить<br>значения</button></td></tr></form></table><script>function sts() {document.getElementById('sts').style.display = 'block';}function stg(){document.getElementById('sts').style.display = 'none';}function dataSelect(f){if(f.v0){v = f.v0.value;(document.createElement('img')).src=(location.href+'valdes?int=1&set='+v);};if(f.th && f.tm){h = f.th.value;m = f.tm.value;(document.createElement('img')).src=(location.href+'valdes?int=2&set='+h);(document.createElement('img')).src=(location.href+'valdes?int=3&set='+m);stg()}};function tmp(x){x.value = x.value*10};function md(x,y){x.value=y};</script></div>",fltostr(temp_n),trejim,wremya_s_on,wremya_s_off);
}


//  время день. ,время ночь ,темп.день. ,темп.ночь ,режим ,сауна начало ,сауна конец
// 3 valdes