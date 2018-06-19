/*
 * Bitmap.h
 *
 *  Created on: Jun 14, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_DATA_BITMAP_H_
#define INCLUDE_DATA_BITMAP_H_
#include <cstddef>
#include <generic/util.h>
#include <generic/programming.h>

class Bitmap{
public:
	Bitmap(size_t bitmapSize);
	DELETE_COPY(Bitmap);
	~Bitmap();

	bool value(size_t i)const;
	void value(size_t i,bool v);
	// 返回SIZE_MAX表明失败
	size_t findUnset();

private:
	size_t * _mem;
	size_t   _bitmapSize;
	size_t   _memEntryNum;
};




#endif /* INCLUDE_DATA_BITMAP_H_ */
