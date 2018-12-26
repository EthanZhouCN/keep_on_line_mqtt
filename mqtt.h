#ifndef __MQTT_H
#define __MQTT_H	 

#include <string.h>
#include "stdint.h"
#define		MQTT_TypeCONNECT							1//请求连接  
#define		MQTT_TypeCONNACK							2//请求应答  
#define		MQTT_TypePUBLISH							3//发布消息  
#define		MQTT_TypePUBACK								4//发布应答  
#define		MQTT_TypePUBREC								5//发布已接收，保证传递1  
#define		MQTT_TypePUBREL								6//发布释放，保证传递2  
#define		MQTT_TypePUBCOMP							7//发布完成，保证传递3  
#define		MQTT_TypeSUBSCRIBE							8//订阅请求  
#define		MQTT_TypeSUBACK								9//订阅应答  
#define		MQTT_TypeUNSUBSCRIBE						10//取消订阅  
#define		MQTT_TypeUNSUBACK							11//取消订阅应答  
#define		MQTT_TypePINGREQ							12//ping请求  
#define		MQTT_TypePINGRESP							13//ping响应  
#define		MQTT_TypeDISCONNECT 						14//断开连接  
 
#define		MQTT_StaCleanSession						1	//清理会话 
#define		MQTT_StaWillFlag							0	//遗嘱标志
#define		MQTT_StaWillQoS								0	//遗嘱QoS连接标志的第4和第3位。
#define		MQTT_StaWillRetain							0	//遗嘱保留
#define		MQTT_StaUserNameFlag						1	//用户名标志 User Name Flag
#define		MQTT_StaPasswordFlag						1	//密码标志 Password Flag
#define		MQTT_KeepAlive								120
#define		MQTT_ClientIdentifier   					"505342358"						//客户端标识符 Client Identifier
#define		MQTT_WillTopic								""								//遗嘱主题 Will Topic
#define		MQTT_WillMessage							""								//遗嘱消息 Will Message
#define		MQTT_UserName								"193203"						//用户名 User Name
#define		MQTT_Password								"uBJZGOHsvRD8sDttT2uPzF2BbJE="	//密码 Password


typedef struct{
	unsigned char PacketType;
	unsigned int RemainingLength;
}FixedHeader_t;

typedef struct{
	unsigned char UserFlag;
	unsigned char PasswordFlag;
	unsigned char WillRetainFlag;
	unsigned char WillQosFlag;
	unsigned char WillFlag;
	unsigned char CleanSessionFlag;
	unsigned char Reserve;
}ConnectFlag_t;

typedef struct{
	unsigned char ProtocolNameLength;
	unsigned char ProtocolName[10];
	unsigned char ProtocolLevel;
	ConnectFlag_t ConnectFlag;
	unsigned short KeepAlive; 
}VariableHeader_t;


unsigned char SetMQTTPacketType(unsigned char MesType,unsigned char DupFlag,unsigned char QosLevel,unsigned char Retain);
uint16_t GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg);//获取发布消息的数据包		 	
void GetDataSUBSCRIBE(unsigned char *buff,const char *dat,unsigned int Num,unsigned char RequestedQoS);//订阅主题的数据包 Num:主题序号 RequestedQoS:服务质量要求0,1或2
void GetDataDisConnet(unsigned char *buff);//获取断开连接的数据包
//uint16_t GetDataConnet(unsigned char *buff);//获取连接的数据包正确连接返回20 02 00 00
uint16_t GetDataConnet(unsigned char *buff, char *ProtocolName, unsigned char ProtocolLevel, unsigned char UserFlag, unsigned char PasswordFlag, unsigned char WillRetainFlag, unsigned char WillQosFlag, unsigned char WillFlag, unsigned char CleanSessionFlag, unsigned short KeepAlive, char *ClientIdentifier, char *UserName, char *UserPassword);//获取连接的数据包正确连接返回20 02 00 00

uint16_t GetDataPINGREQ(unsigned char *buff);//心跳请求的数据包成功返回d0 00
uint16_t GetDataPointPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,unsigned short packetid, const char *msg);//获取发布消息的数据包
//void PlatfromPUBLISHAnalysis(unsigned char *buff, FixedHeader_t *FixedHeader, VariableHeader_t *VariableHeader, unsigned char *payload);
void PlatfromPUBLISHAnalysis(unsigned char *buff, FixedHeader_t *FixedHeader, unsigned short *topicnamelen, unsigned char *topicname, unsigned short *payloadlen, unsigned char *payload);
void PlatfromCmdPUBLISHRsp(unsigned char *buff, FixedHeader_t *FixedHeader, unsigned short *topicnamelen, unsigned char *topicname, unsigned short *payloadlen, unsigned char *payload);


	

#endif
