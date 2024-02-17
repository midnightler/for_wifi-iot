
//  пример кода мигания светодиодом
// зажигание на 1сек. каждые 5сек.
	if (условие)
		{  // если выполняется - мигаем св.диодом
		if (digitalRead(s_ledb)==1){
		digitalWrite(s_ledb,0);
		}
		if (timersrc%5==0){
		digitalWrite(s_ledb,1);
		}
		}


