#include "kol_utils.h"

rent_config_t g_rent_config;


void wait_ack(u32 timeout_ms)
{
	struct timespec abstime;
	struct timeval now;
	u32 nsec = 0;
	
	pthread_mutex_t mutex_wait_ack;
	pthread_cond_t cond_wait_ack;

	pthread_mutex_init(&mutex_wait_ack, NULL); 
	pthread_cond_init(&cond_wait_ack, NULL);
	
	gettimeofday(&now, NULL);
	nsec = now.tv_usec * 1000 + (timeout_ms % 1000) * 1000000;
	abstime.tv_nsec = nsec % 1000000000;
	abstime.tv_sec = now.tv_sec + nsec / 1000000000 + timeout_ms / 1000;
	pthread_mutex_lock(&mutex_wait_ack);  
	pthread_cond_timedwait(&cond_wait_ack, &mutex_wait_ack, &abstime);  
	pthread_mutex_unlock(&mutex_wait_ack); 
}



void pri_journal(uint8 *title, uint8 *pkg, uint16 position)
{
	uint16 tt_index = 0; 
	PRINTF("%s",title);
	for(; tt_index<position; tt_index++){
		PRINTF("%02X", *(pkg+tt_index));
	}
	PRINTF("\n");
}


uint8 rent_check_bcc(uint8 *pkg, uint16 len)
{
	uint16 index = 0;
	uint8  bcc_code = 0;
	
	for(index=2; index<len-1; index++){
		bcc_code ^= *(pkg+index);
	}
	
	if(*(pkg+len-1) == bcc_code)
	{
		return SET;
	}else{
		return CLR;
	}
}


int getfirstfile(char *basePath,char *filename)
{
	DIR *dir;
	struct dirent *ptr;
	if ((dir=opendir(basePath)) == NULL)
		return 2;
	while ((ptr=readdir(dir)) != NULL){
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
			continue;
		else if(ptr->d_type == 8){///file
			strcpy(filename,basePath);
			strcat(filename,ptr->d_name);
			closedir(dir);
			return 1;
		}else{
			continue;
		}
	}
	closedir(dir);
	return 0;
}




static u8 get_bits_of_digit(u32 tmpdigit)
 {
	u8t n;
	if(!tmpdigit)
		return 1;
	n=0;
	while(tmpdigit){
		n++;
		tmpdigit /=10;
	}
	return n;
}


/*
	功能：分时参数设置
	参考：现国标代码
	整合：周振强
	日期：18-08-05
*/
void rent_set_config()
{

	u8 strline[500] = {0};
	u16 tmpl = 0;

	strcpy((char *)strline+tmpl,(char *)"server_ip=");
	tmpl +=strlen((char *)"server_ip=");
	strcpy((char *)strline+tmpl, (char *)g_rent_config.server_ip);
	tmpl+=strlen((char *)g_rent_config.server_ip);
	strline[tmpl++]='\n';


	strcpy((char *)strline+tmpl,(char *)"server_port=");
	tmpl +=strlen((char *)"server_port=");
	sprintf((char *)strline+tmpl, "%d", g_rent_config.server_port);
	tmpl +=get_bits_of_digit(g_rent_config.server_port);
	strline[tmpl++]='\n';
	
	strcpy((char *)strline+tmpl,(char *)"hb_interval=");
	tmpl +=strlen((char *)"hb_interval=");
	sprintf((char *)strline+tmpl,"%d",g_rent_config.hb_interval);
	tmpl +=get_bits_of_digit(g_rent_config.hb_interval);
	strline[tmpl++]='\n';

	FILE *fp = NULL;
	fp=fopen(RENT_CONFIG_DIR,"w");
	fwrite(strline, 1, strlen((char *)strline), fp);
	fclose(fp);
	
}

/*
	功能：分时功能配置信息解析
	参考：国标代码
	整合：周振强
	日期：2018-08-04
*/
static void rent_params_handle(u8t *config_key, u8t *config_value, u8t *str)
{
	u8t key_last_position=0;
	u8t value_last_position=0;
	u16t i=0;
	
	if(strlen((char *)str)<2)
		return;
	for(i=0; i<RENT_CONFIG_LINE_SIZE; i++){
		if('='==str[i]){
			key_last_position=i;
			break;
		}		
	}
	
	if (0 == key_last_position)
		return;

	for(; i<RENT_CONFIG_LINE_SIZE; i++)
	{
		if(('\r'==str[i]) ||('\n'==str[i]))
		{
			value_last_position=i;
			break;
		}		
	}
	
	if (0 == value_last_position)
		return;
		
	memcpy(config_key, str, key_last_position);
	memcpy(config_value, str+key_last_position+1, value_last_position-key_last_position-1);
	
	if(!strcmp((const char *)config_key,"server_ip"))
	{
		memset(g_rent_config.server_ip,0,sizeof(g_rent_config.server_ip));
		strcpy((char *)g_rent_config.server_ip,(const char *)config_value);
	}
	else if(!strcmp((const char *)config_key,"server_port"))
	{
		g_rent_config.server_port=atoi((const char *)config_value);
	}
	else if(!strcmp((const char *)config_key,"hb_interval"))
	{
		g_rent_config.hb_interval=atoi((const char *)config_value);
	}

}

void rent_get_config(void)
{//读取配置信息
	FILE *fp;
	u8t strline[RENT_CONFIG_LINE_SIZE];
	u8t config_key[RENT_CONFIG_LINE_SIZE/2]		={0};
	u8t config_value[RENT_CONFIG_LINE_SIZE/2]	={0};

	memset(strline,0,sizeof(strline));
	fp=fopen(RENT_CONFIG_DIR,"r");
	if(NULL == fp)//打开配置文件失败
		return ;
		
	while(!feof(fp)){
		if(fgets((char *)strline, RENT_CONFIG_LINE_SIZE, fp));
		
		memset(config_key, 0, RENT_CONFIG_LINE_SIZE/2);
		memset(config_value, 0, RENT_CONFIG_LINE_SIZE/2);
		
		rent_params_handle(config_key, config_value, strline);
	}
	
	fclose(fp);
}


void init_rent_dir()
{
	uint8 dir[256] = {0};
	memcpy(dir, "mkdir ", 6);
	memcpy(dir+6, RENT_RESSUE_JOURNAL_FILE_DIR, strlen(RENT_RESSUE_JOURNAL_FILE_DIR));
	memcpy(dir+6+strlen(RENT_RESSUE_JOURNAL_FILE_DIR), " -p", 3);
	if(system((const char *)dir));
}


