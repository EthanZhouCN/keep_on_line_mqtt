#ifndef _CJSON_UTILS_H_
#define _CJSON_UTILS_H_


#include "stdint.h"

extern uint16_t DoubleTypeDataPoint2String(unsigned char *buff, char *id, double value, int *datetime);

extern uint16_t DataPoint2Json(unsigned char *buff, char *id_1, double value_1, char *id_2, unsigned char value_2);


#endif



