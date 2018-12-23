#include "cjson_utils.h"
#include "string.h"
#include "cJSON.h"
uint16_t DoubleTypeDataPoint2String(unsigned char *buff, char *id, double value)
{
	uint16_t buff_len = 0;
	
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
    cJSON_AddItemToObject(obj,"at",cJSON_CreateString("2018-12-10 17:09:38"));
    cJSON_AddItemToObject(obj,"value",cJSON_CreateNumber(value));
    
    //清理工作
    buff_len = strlen(cJSON_Print(json));
    memset(buff, 0, sizeof((char *)buff));
    memcpy(buff, cJSON_Print(json), buff_len);

    cJSON_Delete(json);

    return buff_len;

} 

