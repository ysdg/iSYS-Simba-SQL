#pragma  once 
#include <Windows.h>
#include <winsock2.h>
#include "oaidl.h"

//位号部分数据结构

typedef DWORD HTAG;
typedef DWORD HELEMENT;
typedef WORD  HELEMENTTYPE;
typedef DWORD HRIGHTTYPE;

//定义位号值的定义，包括数据、时间等。
typedef struct 
{
	HTAG		hTag;			//位号的全局句柄
	FILETIME    ftTimeStamp;	//时间戳
	WORD        wQuality;		//质量码
	WORD		wAlarmState;	//报警状态
	VARIANT     vEng_value;		//数据值
}TAGVALSTATE;

//位号的共同属性,即无论是实位号还是虚位号都共享的信息
typedef struct 
{
	//基本信息
	LPWSTR  szTagName;				//位号名称
	LPWSTR  szTagAlias;				//位号别名
	LPWSTR  szTagDescr;				//位号描述
	LPWSTR	 szPath;				//位号所在路径，该属性在权限管理系统中至关重要
	LPWSTR	 szUnit;				//位号的单位
	HTAG        hTag;               //位号的客户端句柄
	VARTYPE     vtDataType;         //位号的请求数据类型
	WORD        bReadOnly;          //位号只读
	//工程上下限
	float  fEngHigh;
	float  fEngLow;
	//内存历史信息
	WORD        bStore;             //是否进入历史数据库标志
	DWORD       dwPoolLenth;        //位号的内存历史长度
	//报警信息
	double		dbHihi_limit;		//高高限
	double		dbHi_limit;			//高限
	double		dbLolo_limit;		//低低限
	double		dbLo_limit;			//低限
	WORD		bAlarmFlag;		    //报警使能
	//磁盘历史数据库所需要的属性
	WORD		bArchive;			//是否存数据文档
	WORD        bCompress;          //是否压缩
	WORD        wCompMax;           //最大压缩间隔
	WORD        wCompMin;           //最小压缩间隔
	double       dbCompDev;          //压缩偏移量
	DWORD     dwBufferCount;      //磁盘历史位号缓存数据个数
	WORD        wFloatPrecision;     //浮点精度
} BASETAGDEF;

typedef struct//实位号的数据结构
{
	//公共部分
	BASETAGDEF baseDef;
	//接口层信息
	LPWSTR szTransferName;			//位号所在接口软件的名称
	LPWSTR szItemID;				//位号在底层的名称
	WORD        bScan;              //接口层是否扫描
	DWORD		dwScanSec;			//接口层扫描周期
	DWORD       dwInterfaceFlag;    //接口层标志位
} REALTAGDEF;

typedef struct//虚拟位号定义
{
	//公共部分
	BASETAGDEF baseDef;
	//脚本执行信息
	DWORD       dwTriggerFlag;       //虚拟位号的触发条件
	DWORD       dwInterval;          //如果是定时触发,则该参数代表触发周期
	LPWSTR  szTimerScript;			 //虚拟位号对应的定时执行脚本名称
	//LPWSTR  szTimerScriptText;  
	LPWSTR  szReadScript;			 //虚拟位号对应的读取执行脚本名称
	//LPWSTR  szReadScriptText;
	LPWSTR  szWriteScript;			 //虚拟位号对应的写入执行脚本名称
	//LPWSTR  szWriteScriptText;  
	LPWSTR  szScriptText; 
} VIRTUALTAGDEF;


//有关组态部分数据结构
typedef struct tagREGION_DEF//对象定义，包括区域和叶节点
{
	LPWSTR wszParentPath;	        //父节点路径
	LPWSTR wszName;					//对象名
	LPWSTR wszDesc;					//对象描述
	VARIANT  vCustomData;			//对象的自定义信息
} REGION_DEF;

typedef struct tagREGION_ATTRIBUTE//对象属性
{
	LPWSTR wszPath;					//对象全路径
	LPWSTR wszDesc;					//对象描述
	VARIANT  vCustomData;			//对象的自定义信息
}REGION_ATTRIBUTE;

typedef struct tagUSERGROUP_DEF//用户组定义
{
	LPWSTR wszUserGroupName;			//用户组名称
	LPWSTR wszUserGroupDesc;			//用户组描述
	VARIANT  vCustomData;			   //用户组的自定义数据
} USERGROUP_DEF;

typedef struct tagUSER_DEF//用户定义
{
	LPWSTR wszUserName;        //用户名称
	LPWSTR wszUserPassword;    //用户密码
	LPWSTR wszUserDesc;        //用户描述
	VARIANT  vCustomData;				//用户的自定义数据
} USER_DEF;

typedef struct tagACEHEADER//权限访问条目
{
	LPWSTR wszElement;			//对象路径
	LPWSTR wszRightName;		//权限名称
	USHORT bGrant;				//授予权限/拒绝权限
}ACEHEADER;

typedef struct tagTYPEDEF//类型属性
{
	LPWSTR wszTypeName;   //对象类型的名称
	LPWSTR wszTypeAlias; //对象类型别名
	LPWSTR wszTypeDescr; //对象类型描述
	HELEMENTTYPE hElementType;    //对象类型句柄
}TYPE_DEF;

typedef struct tagRIGHTDEF//权限定义
{
	LPWSTR wszRightName;  //权限的名称	
	LPWSTR wszRightAlias;//权限类型别名
	LPWSTR wszRightDescr;//权限类型描述
	HRIGHTTYPE hRight;			  //权限的句柄
}RIGHT_DEF;

struct REGION_TREE //区域树信息
{   
	REGION_ATTRIBUTE RegionAttr;		//区域属性
	DWORD dwRealTagCount;				//该区域第一层下实位号的个数
	REALTAGDEF* pRealTags;				//该区域第一层下实位号属性数组
	DWORD dwVirtualTagCount;			//该区域第一层下虚位号的个数
	VIRTUALTAGDEF* pVirtualTags;		//该区域第一层下虚位号属性数组
	DWORD dwChildRegionCount;			//该区域第一层下的子区域个数
	REGION_TREE* pChildRegions;			//该区域第一层下的子区域信息
};

typedef struct tagTYPEINFO  //对象类型信息
{
	LPWSTR wszTypeName;				//对象类型的名称，可以是Global，Region，Tag
	LPWSTR wszTypeAlias;			//对象类型的别名
	LPWSTR wszTypeDescr;			//对象类型的描述，该属性可以为空
	HELEMENTTYPE hElementType;    	//对象类型的句柄
	DWORD dwRightCount;				//隶属于该对象的权限个数
	RIGHT_DEF* pRightDef;			//隶属于该对象的权限数组
}TYPE_INFO;

//有关接口部分数据结构
typedef struct//接口基础信息定义
{
	//基本信息
	DWORD hTransfer; //接口软件ID
	LPWSTR szTransferName; //接口软件名称
	LPWSTR szTransferProgID; //接口软件的ProgID
	LPWSTR szTransferDescription; //接口软件描述
}TRANSFER_DEF;

typedef struct tagTransferInfo  //已注册接口基本信息
{
	LPWSTR wszTransferProgID;
	LPWSTR wszTransferDescription;
} TRANSFER_INFO;



typedef struct  
{
	//基本属性
	LPWSTR wszName;			//属性的名称，该字符串不会为空，属性的名字可以依赖于另外一个元属性而变化
	LPWSTR wszDescription;  //属性的描述， 注意：该字符串可以是空 NULL
	BOOL	bReadOnly; //是否是只读属性，如果是只读属性，那么组态的时候不需要让用户去调整，但是可以通过该属性反映服务器内部状态
	VARTYPE   vt;   //属性的数据类型
	VARIANT   varDefault; //缺省值，如果该VARIANT类型不是VT_EMPY说明该位号有缺省值
	//标志位，采用什么方式对属性的值进行校验，
	//可以是0(不校验)， PROP_HI(校验上限)， PROP_LO(校验下限)， PROP_HILO(校验上下限)， PROP_REG(通过正则表达式校验)
	WORD      wLimitType;
	//属性的上下限
	double    dbUpLimit; //属性的上限，仅对数值类型有效
	double    dbLowLimit; //属性的下限，仅对数值类型有效
	//对于一些特殊的输入，比如枚举类型等，可以采用正则表达式来校验，这样适用性是最广泛的
	//注意：如果不是采用正则表达式校验，该字符串可以是空 NULL
	LPWSTR wszRegularExpression; //正则表达式
} TRANSFER_PROPERTYDEF;

typedef struct  //属性值
{
	LPWSTR wszName; //属性的名称
	VARIANT varPropty; //属性的值
} TRANSFER_PROPERTYVALUE;

#define INVALID_ID  0xFFFFFFFF