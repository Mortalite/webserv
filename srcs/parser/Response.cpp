#include "parser/Response.hpp"

Response::Response() {
	timeBuffer.reserve(100);
}

Response::~Response() {

}

size_t Response::isLearYear(int year) {
	return (!(year % 4) && (year % 100 || !(year % 400)));
}

size_t Response::yearSize(int year) {
	if (isLearYear(year))
		return (366);
	return (365);
}

std::string Response::getDate() {
	static int year0 = 1900;
	static int epoch_year = 1970;
	static size_t secs_day = 24*60*60;
	static size_t _ytab[2][12] =
			{
					{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
					{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
			};

	struct tm tm;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	int year = epoch_year;
	time_t time = tv.tv_sec;
	size_t dayclock =  time % secs_day;
	size_t dayno = time / secs_day;

	tm.tm_sec = dayclock % 60;
	tm.tm_min = (dayclock % 3600) / 60;
	tm.tm_hour = dayclock / 3600;
	tm.tm_wday = (dayno + 4) % 7;
	while (dayno >= yearSize(year))
	{
		dayno -= yearSize(year);
		year++;
	}
	tm.tm_year = year - year0;
	tm.tm_yday = dayno;
	tm.tm_mon = 0;
	while (dayno >= _ytab[isLearYear(year)][tm.tm_mon])
	{
		dayno -= _ytab[isLearYear(year)][tm.tm_mon];
		tm.tm_mon++;
	}
	tm.tm_mday = dayno + 1;
	tm.tm_isdst = 0;
	strftime(&timeBuffer[0], 100, "%a, %d %b %Y %H:%M:%S GMT", &tm);
	std::cout << "strftime = " << &timeBuffer[0] << std::endl;
	return (&timeBuffer[0]);
}
