#ifndef _POINT_H_
#define _POINT_H_

struct Point {
	Point() : x(0), y(0) {}
	Point(int x, int y) : x(x), y(y) {}
	int x;
	int y;
};

#endif // !_POINT_H_