
int onemon[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


typedef struct time
{

	int year;
	int month;
	int date;
	
	int hour;
	int min;
	int sec;
	
}Time;


int modYear(Time* t, int plus)
{
	t->year += plus;
	
	if(t->year < 0) t->year = 0;
	return t->year;
}

int modMonth(Time* t, int plus)
{
	t->month += plus;
	
	if(t->month < 1) t->month = 1;
	else if(12 < t->month) 
	{
		t->month = 1;
		modYear(t, 1);
	}

	return t->month;
}


int modDate(Time* t, int plus)
{
	t->date += plus;
	
	if(t->date < 1) t->date = 1;

	if(t->month != 2)
	{
		if(onemon[t->month] < t->date) t->date = onemon[t->month];
		
		return t->date;
	}

	if((t->year % 100 !=0 && t->year % 4 == 0) || t->year % 400 == 0)
	{
		if(29 < t->date) t->date = 29;
			
	}
	else
	{
		if(28 < t->date) t->date = 28;
	}

	return t->date;
}


int modHour(Time* t, int plus)
{
	t->hour += plus;
	
	if(t->hour < 0) t->hour = 23;
	else if(23 < t->hour) 
	{
		t->hour -= 24;
		modDate(t, 1);
	}

	return t->hour;
}


int modMin(Time* t, int plus)
{
	t->min += plus;
	
	if(t->min < 0) 
	{
		t->min = 59;
		modHour(t, -1);
	}
	else if(59 < t->min) 
	{
		t->min -= 60;
		modHour(t, 1);
	}

	return t->min;
}

int modSec(Time* t, int plus)
{
	t->sec += plus;
	
	if(t->sec < 0)
	{
		t->sec = 59;
		modMin(t, -1);
	}
	else if(59 < t->sec) 
	{
		t->sec = 0;
		modMin(t, 1);
	}

	return t->sec;
}


void timecpy(Time *t1, Time *t2)
{

	t1->year = t2->year;
	t1->month = t2->month;
	t1->date = t2->date;
	t1->hour = t2->hour;
	t1->min = t2->min;
	t1->sec = t2->sec;

}

