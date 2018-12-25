#include "cjson_utils.h"
#include "string.h"
#include "cJSON.h"
#include "time.h"

static void get_time(struct tm *_tm)
{
	time_t now;
	struct tm *timenow;
	time(&now);
	timenow =localtime(&now);
	
	_tm->tm_year 	= timenow->tm_year+1900;
	_tm->tm_mon 	= timenow->tm_mon+1;
	_tm->tm_mday 	= timenow->tm_mday;
	_tm->tm_hour 	= timenow->tm_hour;
	_tm->tm_min 	= timenow->tm_min;
	_tm->tm_sec 	= timenow->tm_sec;
}


uint16_t DoubleTypeDataPoint2String(unsigned char *buff, char *id, double value)
{
	uint16_t buff_len = 0;
	uint8_t tm_array[20] = {0};
	struct tm current_tm;
	get_time(&current_tm);
	
	sprintf(tm_array, "%04d-%02d-%02d %02d:%02d:%02d", current_tm.tm_year, current_tm.tm_mon, current_tm.tm_mday,
													   current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec);
	
	//先创建空对象
    cJSON *json = cJSON_CreateObject();
    //添加数组
    cJSON *datastreams_array = NULL;
    cJSON_AddItemToObject(json,"datastreams", datastreams_array=cJSON_CreateArray());
    //在数组上添加对象
    cJSON *obj = NULL;
    cJSON_AddItemToArray(datastreams_array, obj=cJSON_CreateObject());
    cJSON_AddStringToObject(obj,"id", (char *)id);
	cJSON *datapoints_array = NULL;
	cJSON_AddItemToObject(obj, "datapoints", datapoints_array=cJSON_CreateArray());
    //在对象上添加键值对
    cJSON_AddItemToArray(datapoints_array, obj=cJSON_CreateObject());
    cJSON_AddItemToObject(obj,"at",cJSON_CreateString(tm_array));
    cJSON_AddItemToObject(obj,"value",cJSON_CreateNumber(value));
    
    //清理工作
    buff_len = strlen(cJSON_Print(json));
    memset(buff, 0, sizeof((char *)buff));
    memcpy(buff, cJSON_Print(json), buff_len);

    cJSON_Delete(json);

    return buff_len;

} 

