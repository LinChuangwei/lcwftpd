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
		
		//strcasecmp忽略大小写比较字符串
		if (strcasecmp(key, "pasv_enable") == 0) 
		{ 
            pasv_active = handle_pasv_port(value,key,linenumber);
            std::cout<<"pasv_active:"<<pasv_active<<std::endl;;
        } 
		

	}  


	fclose(fp);
	return 0;
}

/**
 *handle_pasv_port - 一个pasv和port配置的处理函数
 *@value:value值
 *返回解析后的设置布尔值，true或者false
 */
bool parseconfig::handle_pasv_port(char* value,char* key,int linenumber)
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
	  	LCWFTPD_LOG(ERROR,"value in %s for %s locate line %d not support",CONF_FILE,key,linenumber);
	 }
	 return trueorfalse;
}