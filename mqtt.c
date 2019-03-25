#include "mqtt.h"
#include "stdio.h"


unsigned char SetMQTTPacketType(unsigned char MesType,unsigned char DupFlag,unsigned char QosLevel,unsigned char Retain)
{
	unsigned char dat = 0;
	dat = (MesType & 0x0f) << 4;
	dat |= (DupFlag & 0x01) << 3;
	dat |= (QosLevel & 0x03) << 1;
	dat |= (Retain & 0x01);
	return dat;
}
uint16_t GetDataConnet(unsigned char *buff, char *ProtocolName, unsigned char ProtocolLevel, unsigned char UserFlag, unsigned char PasswordFlag, unsigned char WillRetainFlag, unsigned char WillQosFlag, unsigned char WillFlag, unsigned char CleanSessionFlag, unsigned short KeepAlive, char *ClientIdentifier, char *UserName, char *UserPassword)//获取连接的数据包正确连接返回20 02 00 00
{
	int RemainedLength = 0;

	unsigned short _ProtocolName_len 		= strlen(ProtocolName);
	unsigned short _ClientIdentifier_len 	= strlen(ClientIdentifier);
	unsigned short _UserName_len 			= strlen(UserName);
	unsigned short _UserPassword_len 		= strlen(UserPassword);

	printf("ProtocolName = %s\n", ProtocolName);
	printf("ClientIdentifier = %s\n", ClientIdentifier);
	printf("UserName = %s\n", UserName);
	printf("UserPassword = %s\n", UserPassword);
	
	/* FixedHeader */
	unsigned char _FixedHeader_len = 0;
	
	*buff = SetMQTTPacketType(MQTT_TypeCONNECT,0,0,0);
	_FixedHeader_len += 1;

	RemainedLength += 2; 					//_ProtocolName_len
	RemainedLength += _ProtocolName_len; 	//_ProtocolName
	RemainedLength += 1; 					//Protocol Level
	RemainedLength += 1; 					//Connect Flag
	RemainedLength += 2; 					//KeepAlive

	RemainedLength += 2; 					//Client Identifier len
	RemainedLength += _ClientIdentifier_len;//Client Identifier
	RemainedLength += 2;					//UserName len
	RemainedLength += _UserName_len;		//UserName
	RemainedLength += 2;					//UserPassword len
	RemainedLength += _UserPassword_len;	//UserPassword

	do{
		_FixedHeader_len += 1;
		if(RemainedLength >= 0x80)
		{
			*++buff = (0x80 | (RemainedLength % 0x80));
			RemainedLength = RemainedLength / 0x80;
		}
		else
		{
			*++buff = RemainedLength;
		}
	}while(RemainedLength >= 0x80);


	/* VariableHeader */
	*++buff 	= _ProtocolName_len>>8;
	*++buff 	= _ProtocolName_len&0xff;
	
	memcpy(++buff, ProtocolName, _ProtocolName_len);
	buff += _ProtocolName_len;

	*buff = ProtocolLevel;

	*++buff = (((UserFlag & 0x01) << 7) | ((PasswordFlag & 0x01) << 6) | ((WillRetainFlag & 0x01) << 5) | ((WillQosFlag & 0x03) << 3) | ((WillFlag & 0x01) << 2)  | ((CleanSessionFlag & 0x01) << 1));
	
	*++buff = KeepAlive>>8;
	*++buff = KeepAlive&0xff;

	/* Payload */
	*++buff = _ClientIdentifier_len >> 8;
	*++buff = _ClientIdentifier_len & 0xff;
	memcpy(++buff, ClientIdentifier, _ClientIdentifier_len);
	buff += _ClientIdentifier_len;

	*buff = _UserName_len >> 8;
	*++buff = _UserName_len & 0xff;
	memcpy(++buff, UserName, _UserName_len);
	buff += _UserName_len;
	
	*buff = _UserPassword_len >> 8;
	*++buff = _UserPassword_len & 0xff;
	memcpy(++buff, UserPassword, _UserPassword_len);
	buff += _UserPassword_len;

	return _FixedHeader_len + RemainedLength;

}


void GetDataDisConnet(unsigned char *buff)//获取断开连接的数据包
{
	buff[0] = 0xe0;
	buff[1] = 0;
}
uint16_t GetDataPINGREQ(unsigned char *buff)//心跳请求的数据包成功返回d0 00
{
	buff[0] = 0xc0;
	buff[1] = 0;
	return 2;
}
/*
	成功返回90 0x 00 Num RequestedQoS
*/
void GetDataSUBSCRIBE(unsigned char *buff,const char *dat,unsigned int Num,unsigned char RequestedQoS)//订阅主题的数据包 Num:主题序号 RequestedQoS:服务质量要求0,1或2
{
	unsigned int i,len = 0,lennum = 0; 
	buff[0] = 0x82;
	len = strlen(dat);
	buff[2] = Num>>8;
	buff[3] = Num;
	buff[4] = len>>8;
	buff[5] = len;
	for(i = 0;i<len;i++)
	{
		buff[6+i] = dat[i];
	}
	lennum = len;
	buff[6 + lennum ] = RequestedQoS;
	buff[1] = lennum + 5;
}
uint16_t GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg)//获取发布消息的数据包
{
	unsigned int i,len=0,lennum=0;
	buff[0] = SetMQTTPacketType(MQTT_TypePUBLISH,dup,qos,retain);
	
	len = strlen(topic);
	buff[2] = len>>8;
	buff[3] = len;
	for(i = 0;i<len;i++)
	{
		buff[4+i] = topic[i];
	}
	lennum = len;
	len = strlen(msg);
	for(i = 0;i<len;i++)
	{
		buff[4+i+lennum] = msg[i];
	}
	lennum += len;
	buff[1] = lennum + 2;
	return buff[1];
	
}


uint16_t GetDataPointPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,unsigned short packetid, const char *msg)//获取发布消息的数据包
{
	unsigned int i;
	int len_size = 1;
	*buff = SetMQTTPacketType(MQTT_TypePUBLISH,dup,qos,retain);

	int remainedLength = 0;
	
	int topic_len = strlen(topic);
	int msg_len = strlen(msg);

	remainedLength += 2; //topic_len bytes
	remainedLength += topic_len;
	remainedLength += 2; // PacketIdentifier bytes

	remainedLength += 1; //type bytes
	remainedLength += 2; //msg_len bytes
	remainedLength += msg_len;

	if(remainedLength>=0x80)
	{
		
		*++buff = (0x80|(remainedLength%0x80));
		*++buff = remainedLength/0x80;
		len_size += 2;
	}
	else
	{
		*++buff = remainedLength;
		len_size += 1;
	}
	
	
	*++buff = topic_len>>8;
	*++buff = topic_len;
	
	
	for(i = 0;i<topic_len;i++)
	{
		
		*++buff = topic[i];
		
	}
	
	*++buff = packetid>>8;
	*++buff = packetid & 0xff;


	*++buff = 0x03;				//数据格式
	
	*++buff = msg_len>>8;
	*++buff = msg_len;

	
	for(i = 0;i<msg_len;i++)
	{
		
		*++buff = msg[i];
	}
	
	return remainedLength + len_size;
	
}


#if 0
void PlatfromPUBLISHAnalysis(unsigned char *buff, FixedHeader_t *FixedHeader, VariableHeader_t *VariableHeader, unsigned char *payload)
{
	
	FixedHeader->PacketType = *buff>>4;

	uint8_t multiplier = 1;
	uint32_t len = 0;

	
	do{
		len += (*++buff & 0x7f) * multiplier;
		multiplier *= 0x80;
	}while((*buff & 0x80) != 0);

	FixedHeader->RemainingLength = len;

	VariableHeader->ProtocolNameLength = *++buff;
	
	memcpy(VariableHeader->ProtocolName, ++buff, VariableHeader->ProtocolNameLength);

	buff += VariableHeader->ProtocolNameLength;
	VariableHeader->ConnectFlag.UserFlag = (*buff>>7) & 0x01;
	VariableHeader->ConnectFlag.PasswordFlag = (*buff>>6) & 0x01;
	VariableHeader->ConnectFlag.WillRetainFlag = (*buff>>5) & 0x01;
	VariableHeader->ConnectFlag.WillQosFlag = (*buff>>2) & 0x03;
	VariableHeader->ConnectFlag.WillFlag = (*buff>>1) & 0x01;
	VariableHeader->ConnectFlag.CleanSessionFlag = *buff & 0x01;

	VariableHeader->KeepAlive = *++buff;
	
	memcpy(payload, ++buff, VariableHeader->ProtocolNameLength-10);
	
}
#endif


void PlatfromPUBLISHAnalysis(unsigned char *buff, FixedHeader_t *FixedHeader, unsigned short *topicnamelen, unsigned char *topicname, unsigned short *payloadlen, unsigned char *payload)
{
	if(buff == NULL)
	{
		return ;
	}
	
	if(FixedHeader != NULL)
	{
		FixedHeader->PacketType = *buff>>4;
	}
	uint8_t _multiplier = 1;
	uint32_t _len = 0;
	uint16_t _topic_len = 0;
	uint16_t _payload_len = 0;
	
	do{
		_len += (*++buff & 0x7f) * _multiplier;
		_multiplier *= 0x80;
	}while((*buff & 0x80) != 0);

	if(FixedHeader != NULL)
	{	
		FixedHeader->RemainingLength = _len;
	}
	//printf("FixedHeader->RemainingLength = %d.\n", FixedHeader->RemainingLength);

	_topic_len = (*++buff)<<8;
	_topic_len |= (*++buff);
	//printf("_topic_len = %d.\n", _topic_len);
	if(topicnamelen != NULL)
	{
		*topicnamelen = _topic_len;
	}
	

	if(topicname != NULL)
	{
		memcpy(topicname, ++buff, _topic_len);	
	}
	
	buff += _topic_len;
	_payload_len = _len - _topic_len - 2;  //_topic_len byte
	if(payloadlen != NULL)
	{
		*payloadlen = _payload_len;
	}

	if(payload != NULL)
	{
		memcpy(payload, buff, _payload_len);
	}
	
	
}



void PlatfromCmdPUBLISHRsp(unsigned char *buff, FixedHeader_t *FixedHeader, unsigned short *topicnamelen, unsigned char *topicname, unsigned short *payloadlen, unsigned char *payload)
{
	if((buff == NULL) || (topicname == NULL))
	{
		return ;
	}
	
	unsigned char *_topicname_ptr = NULL;
	
	if(FixedHeader != NULL)
	{
		FixedHeader->PacketType = *buff>>4;
	}
	uint8_t _multiplier = 1;
	uint32_t _len = 0;
	uint16_t _topic_len = 0;
	uint16_t _payload_len = 0;
	
	do{
		_len += (*++buff & 0x7f) * _multiplier;
		_multiplier *= 0x80;
	}while((*buff & 0x80) != 0);

	if(FixedHeader != NULL)
	{	
		FixedHeader->RemainingLength = _len;
	}
	//printf("FixedHeader->RemainingLength = %d.\n", FixedHeader->RemainingLength);

	_topic_len = (*++buff)<<8;
	_topic_len |= (*++buff);
	//printf("_topic_len = %d.\n", _topic_len);
	if(topicnamelen != NULL)
	{
		*topicnamelen = _topic_len;
	}
	
	_topicname_ptr = ++buff;

	if(topicname != NULL)
	{
		memcpy(topicname, _topicname_ptr, _topic_len);
		topicname[3] = 's';
		topicname[4] = 'p';
		memcpy(_topicname_ptr, topicname, _topic_len);	
	}
	
	buff += _topic_len;
	_payload_len = _len - _topic_len - 2;  //_topic_len byte
	if(payloadlen != NULL)
	{
		*payloadlen = _payload_len;
	}

	if(payload != NULL)
	{
		memcpy(payload, buff, _payload_len);
	}
	
	
}


