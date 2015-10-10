/**
 *start from the very beginning,and to create greatness
 *@author: LinChuangwei 
 *@E-mail：979951191@qq.com
 *@brief：配置文件解析类实现文件
 */

 #include "parseconfig.h"

parseconfig* parseconfig::__self = NULL;//static必须在类外初始化

/**
 *parseconfig - 构造函数，主要完成各个数据成员的初始化
 */
parseconfig::parseconfig():pasv_active(1),port_active(1),listen_port(21),
						max_clients(2000),max_per_ip(50),accept_timeout(60),
						connect_timeout(60),idle_session_timeout(300),data_connection_timeout(300),
						local_umask(077),upload_max_rate(0),download_max_rate(0),listen_address(NULL)

{
}

/**
 *instance - 返回该单例对象
 */
parseconfig& parseconfig::instance()
{
	if (__self == NULL)
	{
		__self = new parseconfig();
		atexit(destroy);//在程序结束的时候会自动调用destroy
		//参考muduo库的,但是这里貌似也不能这么用
	}
	return *(__self);//返回引用
}

/**
 *destroy - 释放内存
 */
void parseconfig::destroy()
{
	delete __self;
	__self = NULL;
	LCWFTPD_LOG(DEBUG,"**************parseconfig::destroy()**************");
}

/**
 *loadfile - 加载配置文件将参数存入
 *成功返回0，失败返回1
 */
int parseconfig::loadfile()
{
	FILE* fp = NULL;
	int linenumber = 0;//这个参数用于记录配置文件的行数，便于在配置文件出错时打印提醒
	char* line;//行指针
	char key[128] = {0};//保存命令的key
	char value[128] = {0};//保存命令的value
	char linebuf[MAX_CONF_LEN+1];//保存读取到的行

	LCWFTPD_LOG(DEBUG,"%s is loading......",CONF_FILE);
	//以只读方式打开文件CONF_FILE，文件必须已经存在
	if ((fp = fopen(CONF_FILE,"r")) == NULL)
	{
		LCWFTPD_LOG(ERROR,"Can't load the configure file %s",CONF_FILE);
		return 1;
	}
	while(fgets(linebuf,MAX_CONF_LEN+1,fp) != NULL)//读取一行到linebuf
	{
		++linenumber;//行数++
		line = lcwftpstr.delspace(linebuf);//去除行首和行尾的空格、\r\n
		if (line[0] == '#' || line[0] == '\0')
		{
			continue;//注释或空行直接跳过
		}
		lcwftpstr.str_split(line,key,value,'=');//根据等号切割
		strcpy(key,lcwftpstr.delspace(key));//去除空格
		strcpy(value,lcwftpstr.delspace(value));//去除空格
		if(0 == strlen(value))
		{//某些key没有配置value,提示key和行号
			LCWFTPD_LOG(DEBUG,"missing value in %s for %s locate line %d",CONF_FILE,key,linenumber);
		}
		//这里只会在程序执行的时候执行一次，比较次数也不算多，姑且用if else 
		//strcasecmp忽略大小写比较字符串
		if(strcasecmp(key, "pasv_enable") == 0)//被动模式
		{ 
            pasv_active = handle_pasv_port(key,value,linenumber);
            LCWFTPD_LOG(DEBUG,"value for pasv_enable is %d",pasv_active);
        }
        else if(strcasecmp(key, "port_enable") == 0)//主动模式
        {
        	port_active = handle_pasv_port(key,value,linenumber); 
        	LCWFTPD_LOG(DEBUG,"value for port_enable is %d",port_active);
        }
        else if(strcasecmp(key, "local_umask") == 0)//权限掩码
        {//8进制字符串转化为unsigned int
        	local_umask = lcwftpstr.str_octal_to_uint(value);
        	LCWFTPD_LOG(DEBUG,"value for local_umask is %u(Decimal system)",local_umask);
        }
        else if(strcasecmp(key, "listen_address") == 0)//ip地址
        {
        	listen_address = strdup(value);
        	LCWFTPD_LOG(DEBUG,"value for listen_address is %s",listen_address);
        }
        else
        {
            //配置所有unsigned int 类型的命令，这个函数一定要放在最后
        	//因为如果再匹配不到就认为是不支持的命令
        	configuint(key,value,linenumber);
        }
	}  


	fclose(fp);
	return 0;
}

/**
 *handle_pasv_port - 一个pasv和port配置的处理函数
 *@key:key值
 *@value:value值
 *@linenumber:行号
 *返回解析后的设置布尔值，true或者false
 */
bool parseconfig::handle_pasv_port(char* key,char* value,int linenumber)
{
	 bool trueorfalse;
	 if (strcasecmp(value,"YES") == 0 || strcasecmp(value,"TRUE") == 0 || 
	    	strcasecmp(value,"1") == 0)
	 {//为真
	  	trueorfalse = true;
	 }
	 else if (strcasecmp(value,"NO") == 0 || strcasecmp(value,"FALSE") == 0 ||
	  	strcasecmp(value,"0") == 0)
	 {//为假
	   trueorfalse = false;
	 }
	 else//没有配置可以使用默认值，配置错了程序最好退出
	 {
	  	LCWFTPD_LOG(ERROR,"value in %s [line: %d] for %s not support",CONF_FILE,linenumber,key);
	 }
	 return trueorfalse;
}

/**
 *configuint - 一个配置unsigned int类型配置项的函数
 *@key:key值
 *@value:value值
 *@linenumber:行号
 */
void parseconfig::configuint(char* key,char* value,int linenumber)
{//在这里定义一个key和value的数组，到时候遍历即可，避免写太多的if else
	struct parseconf_uint_setting
	{
		const char* set_key;
		unsigned int set_value;
	}
	parseconfig_uint_array[9] =
	{
		{"listen_port",listen_port},
        {"max_clients",max_clients},
        {"max_per_ip",max_per_ip},
        {"accept_timeout",accept_timeout},
        {"connect_timeout",connect_timeout},
        {"idle_session_timeout",idle_session_timeout},
        {"data_connection_timeout",data_connection_timeout},
        {"upload_max_rate",upload_max_rate},
        {"download_max_rate",download_max_rate},
	};
	int i;
	for (i = 0; i < 9; ++i)
	{//key和数组中的每个结构体的第一个成员比较比较
		if (strcasecmp(key,parseconfig_uint_array[i].set_key) == 0)
		{
			parseconfig_uint_array[i].set_value = atoi(value);
			LCWFTPD_LOG(DEBUG,"value for %s is %u",key,parseconfig_uint_array[i].set_value);
			return;//找到了就返回
		}
	}
	//不知道的指令
	LCWFTPD_LOG(ERROR,"Bad directive in %s [line:%d] %s:Unknown directive",CONF_FILE,linenumber,key);
}
