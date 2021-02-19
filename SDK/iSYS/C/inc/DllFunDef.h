/*
iSYS C_Client DLL 导出接口函数
*/

#pragma once

#include "DataStructure.h"

enum  euUpdateType
{
	ELEMENT_TREE = 1,//对象树
	USER,	     //用户/用户组
	TRANSFER	 //接口
};

//订阅请求枚举
typedef enum _SubscriptionEnum
{
	TagReqNULL = 0,				//未进行任何订阅请求
	TagRealUpdata = 1,			//位号实时更新订阅请求
	TagAlarm = 2,				//位号报警状态变化订阅
	TagStopUpdata = 4,			//位号停止更新通知
	TagRecoveryDone = 8,		//历史数据恢复完成通知
	TagDataChange = 16,			//位号数据变化订阅
	TagReqAll = -1				//所有订阅请求的组合
}SubscriptionEnum;

//组态更新回调函数
typedef void (*CallbackMessageFunc) (HANDLE handle, euUpdateType Type, FILETIME ftUpdateTime);

//位号数据更新通知回调函数
typedef long (*CallbackTagValueChange)(HANDLE handle,		//连接句柄，标识是哪个客户端的订阅通知消息
									DWORD dwCount,			//数据更新通知的位号个数
									WORD* wNotifyType,		//位号数据更新通知类型数组，长度为dwCount，为SubscriptionEnum枚举值之一或其组合
									TAGVALSTATE* pValue);	//位号值数组，长度为dwCount

//位号停止更新通知回调函数
typedef long (*CallbackTagStopUpdate) (HANDLE handle,		//连接句柄，标识是哪个客户端的订阅通知消息
									DWORD dwCount,			//数据更新通知的位号个数
									HTAG *hTags);			//位号句柄数组

//历史数据恢复完成通知回调函数
typedef long (*CallbackTagRecoveryDone)(HANDLE handle,		//连接句柄，标识是哪个客户端的订阅通知消息
									DWORD dwCount,			//声明接下去的位号个数
									HTAG* hTags,			//位号句柄数组
									FILETIME* pftBegins,	//位号数据恢复起始时间数组
									FILETIME* pftEnds);		//位号数据恢复结束时间数组

//回调函数结构定义
typedef struct  _CALLBACKINFO
{
private:
	DWORD cb;										//该成员必须是CALLBACK_INFO结构的长度，这是为了保证未来能够扩充新的回调而不必修改接口
public:
	//构造函数
	_CALLBACKINFO()
	{
		cb = sizeof(CALLBACKINFO);
		pfOnTagValueChange = NULL;
		pfOnTagStopUpdata = NULL;
		pfOnTagRecoveryDone = NULL;
	}
	CallbackTagValueChange pfOnTagValueChange;		//位号数据更新通知回调函数指针
	CallbackTagStopUpdate pfOnTagStopUpdata;		//位号停止更新通知回调函数指针
	CallbackTagRecoveryDone pfOnTagRecoveryDone;	//历史数据恢复完成通知回调函数指针
} CALLBACKINFO;



//获取服务命令的版本号
DWORD	 _stdcall GetVersionNumber( HANDLE handle );
//获取实时数据库软件时间
HRESULT _stdcall GetRTDBTime( HANDLE handle, FILETIME & ftRTDBTime );

//连接服务器
HANDLE	_stdcall Connect( LPCWSTR lpServerIPAddr, USHORT usServerPort );
//获取异常错误值的具体解释
HRESULT _stdcall GetLastException();
//获取异常错误值的具体解释
HRESULT	_stdcall GetErrorMgs( HANDLE handle, HRESULT hr, LPWSTR* pch );
//断开连接
BOOL    _stdcall DisConnect( HANDLE handle );
//获得超时时间
DWORD   _stdcall GetWaitTime();
void    _stdcall SetWaitTime( DWORD dw );
//登陆
HRESULT _stdcall LogIn( HANDLE handle, LPCWSTR lpUserName, LPCWSTR lpPassword );
//注销
HRESULT _stdcall LogOut( HANDLE handle );
//设置客户端名称
HRESULT _stdcall SetClientName(HANDLE handle, LPCWSTR wszClientName);
//请求组态更新提示
HRESULT _stdcall ReqUpdateMessage( HANDLE handle, CallbackMessageFunc fpMsgFun, BOOL bFlag );

//增加一组虚位号
HRESULT _stdcall AddVirtualTag( HANDLE handle, DWORD dwCount,
							   VIRTUALTAGDEF* pVirDef, HRESULT** ppRes );
//删除一组虚位号
HRESULT _stdcall DelVirtualTag( HANDLE handle, DWORD dwCount, 
							   HTAG* hTags, HRESULT** ppRes );
//枚举虚位号
HRESULT _stdcall EnumVirtualTag( HANDLE handle, DWORD& dwCount, 
								VIRTUALTAGDEF** ppVirDef );
//获取一组虚位号组态信息
HRESULT _stdcall GetVirtualTagInfo( HANDLE handle, DWORD dwCount,
								   HTAG* hTags, HRESULT** ppRes, 
								   VIRTUALTAGDEF** ppVirDef );
//设置一组虚位号的组态信息
HRESULT _stdcall SetVirtualTagInfo( HANDLE handle, DWORD dwCount,
								   VIRTUALTAGDEF* pVirDef, HRESULT** ppRes );
//获取一个虚位号的脚本内容
HRESULT _stdcall GetVirtualTagScript( HANDLE handle, HTAG hTag, LPWSTR* pszScript );
//设置一个虚位号的脚本内容
HRESULT _stdcall SetVirtualTagScript( HANDLE handle, HTAG hTag, LPCWSTR lpScript );

//根据一个位号ID获取其名字
HRESULT _stdcall GetTagIDByName( HANDLE handle, LPCWSTR lpName, HTAG& hTag);
//根据一个位号名字获取ID
HRESULT _stdcall GetTagNameByID( HANDLE handle, HTAG hTag, LPWSTR* pszName );
//根据一个位号路径获得ID
HRESULT _stdcall GetTagIDByPath( HANDLE handle, LPCWSTR lpPath, HTAG& hTag );
//根据一个位号ID获取路径
HRESULT _stdcall GetTagPathByID( HANDLE handle, HTAG hTag, LPWSTR* pszPath );
//移动一个位号在对象树上的位置
HRESULT _stdcall MoveTag( HANDLE handle, HTAG hTag, LPCWSTR wszFatherPath );

//增加一个区域
HRESULT _stdcall AddRegion( HANDLE handle, REGION_DEF* pRegionDef );
//删除一个区域
HRESULT _stdcall DelRegion( HANDLE handle, LPCWSTR wszRegionPath );
//根据一组区域路径获取区域组态信息
HRESULT _stdcall GetRegionInfoByPath( HANDLE handle, DWORD dwCount, LPWSTR* pszPath,
									 HRESULT** ppRes, REGION_ATTRIBUTE** ppRegion );
//设置一组区域组态信息
HRESULT _stdcall SetRegionInfo( HANDLE handle, DWORD dwCount, REGION_ATTRIBUTE* pRegions, 
								   HRESULT** ppRes );
//移动一个区域，该方法兼做重命名
HRESULT _stdcall MoveRegion( HANDLE handle, LPCWSTR wszRegionPath, LPCWSTR lpDesPath );
//枚举一个指定区域下的子区域
HRESULT _stdcall EnumChildRegion( HANDLE handle, LPCWSTR wszRegionPath, 
								 DWORD &dwCount, REGION_ATTRIBUTE** ppRegion, WORD wdRecursive );
//枚举一个指定区域下的位号    
HRESULT _stdcall EnumTagsInRegion( HANDLE handle, LPCWSTR wszRegionPath, 
								 DWORD& dwRealCount, REALTAGDEF** ppReal, DWORD& dwVirCount, VIRTUALTAGDEF** ppVirtual, WORD wdRecursive );
//枚举指定区域下所有对象类型、对象及其属性
HRESULT _stdcall Snapshot( HANDLE handle, LPCWSTR wszRegionPath, DWORD dwTypeCount, HELEMENTTYPE* pElementType,
								 REGION_TREE** ppRegionTree);

//增加一组实位号
HRESULT _stdcall AddRealTag( HANDLE handle, DWORD dwCount, 
							REALTAGDEF* pRealDef, HRESULT** ppRes );
//删除一组实位号
HRESULT _stdcall DelRealTag( HANDLE handle, DWORD dwCount, 
							HTAG* hTags, HRESULT** ppRes );
//枚举实位号
HRESULT _stdcall EnumRealTag( HANDLE handle, DWORD& dwCount, REALTAGDEF** ppRealDef );
//根据一个接口名字枚举其位号
HRESULT _stdcall EnumRealTagByTransfer(HANDLE handle, LPCWSTR lpInterfaceName,
									   DWORD& dwCount, REALTAGDEF** ppRealDef );
//获取一组实位号组态信息
HRESULT _stdcall GetRealTagInfo( HANDLE handle, DWORD dwCount, HTAG* hTags,
								HRESULT** ppRes, REALTAGDEF** ppRealDef );
//设置一组实位号的组态信息
HRESULT _stdcall SetRealTagInfo( HANDLE handle, DWORD dwCount, 
								REALTAGDEF* pRealDef, HRESULT** ppRes );

//读取一批位号的实时数据
HRESULT _stdcall ReadTagsValue( HANDLE handle, DWORD dwCount, HTAG *hTags, 
							   HRESULT** ppRes, TAGVALSTATE** ppValue );
//写入一批位号的实时数据
HRESULT _stdcall WriteTagsValue( HANDLE handle, DWORD dwCount, HTAG *hTags, VARIANT *pvtValue,
								HRESULT** ppRes);

//写入一批位号的实时数据
HRESULT _stdcall WriteTagsValueEx( HANDLE handle, DWORD dwCount, HTAG *hTags, VARIANT *pvtValue, 
								  FILETIME *pftTime, WORD *pwQuality, HRESULT** ppRes);
//写入一个位号一批历史数据
HRESULT _stdcall WriteTagBulkHisData(HANDLE handle, HTAG hTag, DWORD dwCount, VARIANT *pvtValue, 
									FILETIME *pftTime, WORD *pwQuality);

//读取单个位号所有的内存历史数据
HRESULT _stdcall ReadTagAllMemHis( HANDLE handle, HTAG hTag, 
								  DWORD &dwCount, TAGVALSTATE** ppValue );
//读取一批位号在某个时刻的内存历史值
HRESULT _stdcall ReadTagsMemHisAtTime( HANDLE handle, DWORD dwCount, HTAG* hTags, FILETIME ftTime, DWORD wFlag,
									  DWORD dwTimeInterval,  HRESULT** ppRes, TAGVALSTATE** ppValue );
//读取单个位号在某个时间段内的内存历史值
HRESULT _stdcall ReadTagMemHisInTime(  HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag,
									 DWORD &dwCount, TAGVALSTATE** ppValue );
//采样读取一个位号内存历史值
HRESULT _stdcall ReadTagMemHisSampling( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD dwInterval, 
									   DWORD wFlag, DWORD dwTimeDiff, DWORD &dwCount, TAGVALSTATE** ppValue );

//读取一批位号在某个时刻的磁盘历史值
HRESULT _stdcall ReadTagsDiskHisAtTime( HANDLE handle, DWORD dwCount, HTAG* hTags, FILETIME ftTime, DWORD wFlag,
									 DWORD dwTimeInterval,  HRESULT** ppRes, TAGVALSTATE** ppValue );
//读取单个位号在某个时间段内的磁盘历史值
HRESULT _stdcall ReadTagDiskHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag,
									  DWORD &dwCount, TAGVALSTATE** ppValue );
//采样读取一个位号磁盘历史值
HRESULT _stdcall ReadTagDiskHisSampling( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD dwInterval, 
									   DWORD wFlag, DWORD dwTimeDiff, DWORD &dwCount, TAGVALSTATE** ppValue );
//读取一批位号在某时刻的历史值，如果位号有内存历史值则读取内存历史值，否则读取磁盘历史值
HRESULT _stdcall ReadTagsHisAtTime( HANDLE handle, DWORD dwCount, HTAG* hTags, FILETIME ftTime, DWORD wFlag,
								   DWORD dwTimeInterval, HRESULT** ppRes, TAGVALSTATE** ppValue );
//读取单个位号在某时间段内的历史值，如果位号有内存历史值则读取内存历史，否则读取磁盘历史值
HRESULT _stdcall ReadTagHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag/*读取策略*/, 
								  DWORD &dwCount, TAGVALSTATE** ppValue );
//采样读取一个位号历史值
HRESULT _stdcall ReadTagHisSampling( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD dwInterval, 
									   DWORD wFlag, DWORD dwTimeDiff, DWORD &dwCount, TAGVALSTATE** ppValue );
//读取单个位号在某个时间段内的磁盘历史值的最大值
HRESULT _stdcall ReadMaxDiskHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pMaxValue);

//读取单个位号在某个时间段内的磁盘历史值的最小值
HRESULT _stdcall ReadMinDiskHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pMinValue);

//读取单个位号在某个时间段内的磁盘历史值的和值
HRESULT _stdcall ReadSumDiskHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pSumValue);

//读取单个位号在某个时间段内的磁盘历史值的平均值
HRESULT _stdcall ReadAvgDiskHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pAvgValue);

//增加一个用户
HRESULT _stdcall AddUser(HANDLE handle, USER_DEF* pUser);
//删除一个用户		
HRESULT _stdcall DelUser(HANDLE handle, LPCWSTR lpUserName);
//获取一个用户信息
HRESULT _stdcall GetUserInfo(HANDLE handle, LPCWSTR lpUserName, USER_DEF** ppUser);
//设置一个用户信息		
HRESULT _stdcall SetUserDef(HANDLE handle, USER_DEF* pUser);
//获取用户隶属的用户组
HRESULT _stdcall GetGroupsInUser(HANDLE handle, LPCWSTR lpUserName, DWORD& dwCount, USERGROUP_DEF** lpUserGroups );
//枚举用户	
HRESULT _stdcall EnumUser(HANDLE handle, DWORD& dwCount, USER_DEF** ppUser);
//增加一个用户组
HRESULT _stdcall AddUserGroup(HANDLE handle, USERGROUP_DEF* pUserGroup);
//删除一个用户组		
HRESULT _stdcall DelUserGroup(HANDLE handle, LPCWSTR lpUserGroup);
//获取一个用户组信息		
HRESULT _stdcall GetUserGroupDef(HANDLE handle, LPCWSTR lpUserGroup, USERGROUP_DEF** ppUserGroup);
//设置一个用户组信息		
HRESULT _stdcall SetUserGroupDef(HANDLE handle, USERGROUP_DEF* pUserGroup);
//向一个用户组增加一个用户	
HRESULT _stdcall AddUserToUserGroup(HANDLE handle, LPCWSTR lpUserGroup, LPCWSTR lpUserName);
//从一个用户组删除一个用户
HRESULT _stdcall RemoveUserFromUserGroup(HANDLE handle, LPCWSTR lpUserGroup, LPCWSTR lpUserName);
//枚举用户组		
HRESULT _stdcall EnumUserGroup(HANDLE handle, DWORD& dwCount, USERGROUP_DEF** ppUserGroup);
//枚举一个指定用户的权限列表
HRESULT _stdcall EnumUserACL(HANDLE handle, LPCWSTR lpUserName, DWORD& dwCount, ACEHEADER** ppACL);
//枚举一个指定用户组的权限列表
HRESULT _stdcall EnumUserGroupACL(HANDLE handle, LPCWSTR lpUserGroup, DWORD& dwCount, ACEHEADER** ppACL);
//验证一个用户/用户组的操作权限
HRESULT _stdcall Authenticate(HANDLE handle, LPCWSTR lpUserName, LPCWSTR wszPath,  DWORD dwCount, LPWSTR* pszRight, HRESULT** ppRes);
//调整一个用户/用户组的操作权限	
HRESULT _stdcall AdjustPrivilege(HANDLE handle, LPCWSTR lpUserName, LPCWSTR wszPath,
								 BOOL bAdd,
								 BOOL bGrand,
								 DWORD dwCount, 
								 LPWSTR* pszRight, 
								 HRESULT** ppRes);
//获取所有类型以及隶属于类型的权限
HRESULT _stdcall GetAllTypeRight(HANDLE handle, DWORD &dwCount, TYPE_INFO** ppTypeInfo);
//在线保存所有组态信息		
HRESULT _stdcall OnlineSave(HANDLE handle);

//增加一个接口	
HRESULT _stdcall AddTransfer(HANDLE handle, TRANSFER_DEF* pTransDef, DWORD& hTransferID);
//删除一个接口
HRESULT _stdcall DelTransfer(HANDLE handle, LPCWSTR lpTransName);
//枚举接口
HRESULT _stdcall EnumTransfer(HANDLE handle, DWORD& dwCount, TRANSFER_DEF** ppTransDef);
//获取一个指定接口的组态信息	
HRESULT _stdcall GetTransferDef(HANDLE handle, LPCWSTR lpTransName, TRANSFER_DEF** ppTransDef);
//设置一个指定接口的组态信息	
HRESULT _stdcall SetTransferDef (HANDLE handle, TRANSFER_DEF* pTransDef);
//获取所有已经注册的接口软件的列表	
HRESULT _stdcall GetAllRegisteredTransers(HANDLE handle, DWORD& dwCount, TRANSFER_INFO** ppTransRegInfo);
//获取一个指定接口所支持的属性列表
HRESULT _stdcall QueryTransferAvailableProperties(HANDLE handle, LPCWSTR lpTransName, DWORD& dwCount, TRANSFER_PROPERTYDEF** ppProperDef);
//获取指定接口的一组属性值	
HRESULT _stdcall GetPropertiesVal(HANDLE handle, LPCWSTR lpTransName, DWORD& dwCount, TRANSFER_PROPERTYVALUE** ppProperty);
//设置指定接口的一组属性值	
HRESULT _stdcall SetPropertiesVal(HANDLE handle, LPCWSTR lpTransName, DWORD dwCount,TRANSFER_PROPERTYVALUE* pProperty, HRESULT** ppRes);


///**********实时数据库5.1新增接口 add at 2015/9/9 ***************///
//设置订阅回调函数
HRESULT _stdcall SetCallbackInfo(HANDLE handle, CALLBACKINFO & callbackInfo);
//添加订阅位号（包括TagRealUpdata、TagAlarm、TagStopUpdata、TagRecoveryDone其一或组合订阅方式）
HRESULT _stdcall AddNotifyTags(HANDLE handle,				//连接句柄
							DWORD dwCount,			//声明接下去的位号个数
							HTAG *hTags,			//位号句柄数组
							WORD* wSubscriptionType,//位号订阅请求类型数组，长度为dwCount，为SubscriptionEnum枚举值之一或其组合，但不应包含TagDataChange
							HRESULT** ppRes);		//返回结果数组，大小为dwCount，值为S_OK表明位号添加成功

//取消订阅位号
HRESULT _stdcall CancelNotifyTags(HANDLE handle,				//连接句柄
							DWORD dwCount,			//声明接下去的位号个数
							HTAG *hTags,			//位号句柄数组
							WORD* wSubscriptionType = NULL);	//要取消的订阅请求类型数组，若为NULL则代表取消所有hTags中的位号的所有订阅类型

//添加数据变化通知订阅位号
HRESULT _stdcall AddDataChangeNotifyTags(HANDLE handle,		//连接句柄
							DWORD dwCount,			//声明接下去的位号个数
							HTAG *hTags,			//位号句柄数组
							double* dblChange,		//数据变化绝对值数组，长度为dwCount，代表位号值变化超过了多少给予通知，布尔型与字符串型位号忽略该参数
							HRESULT** ppRes);		//返回结果数组，大小为dwCount，值为S_OK表明位号添加成功
//获取指定连接已建立订阅通知的位号信息
HRESULT _stdcall GetNotifyTagInfos(HANDLE handle,			//连接句柄
							DWORD & dwCount,		//声明接下去的位号个数
							HTAG ** pphTags,		//返回位号句柄数组
							WORD** ppSubscripType,	//位号订阅请求类型数组，长度为dwCount，为SubscriptionEnum枚举值之一或其组合
							double** ppdblChange);	////数据变化绝对值数组，长度为dwCount，代表位号值变化超过了多少给予通知，布尔型与字符串型位号忽略该参数
//清空所有已建立的订阅位号
HRESULT _stdcall ClearNotifyTags(HANDLE handle);			//连接句柄

//获取是否启用自动连接标识
HRESULT _stdcall GetAutoRelink(HANDLE handle, bool & bIsAutoRelink);
//设置是否启用自动连接标识
HRESULT _stdcall SetAutoRelink(HANDLE handle, bool bIsAutoRelink);
//判断指定连接句柄是否已经连接到数据库
HRESULT _stdcall IsConnected(HANDLE handle, bool & bIsConnected);

//根据批量位号ID获取其名称
HRESULT _stdcall GetTagsNameByIDs( HANDLE handle,DWORD dwCount, HTAG* hTag, LPWSTR** ppszName, HRESULT** ppRes );
//根据批量位号名称获取其ID
HRESULT _stdcall GetTagsIDByNames( HANDLE handle, DWORD dwCount,LPWSTR* lpNames, HTAG** pphTags, HRESULT** ppRes);
//根据批量位号路径获取其ID
HRESULT _stdcall GetTagsIDByPaths( HANDLE handle, DWORD dwCount,LPWSTR* lpPath, HTAG** pphTags, HRESULT** ppRes );
//根据批量位号ID获取其路径
HRESULT _stdcall GetTagsPathByIDs( HANDLE handle, DWORD dwCount, HTAG* hTag, LPWSTR** ppPaths, HRESULT** ppRes );
//读取单个位号在某个时间段内的磁盘历史值的最大值(返回最大值所在时间点)
HRESULT _stdcall ReadMaxDiskHisInTimeEx(HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pMaxValue, FILETIME* pftTime, WORD* pwQuliaty);
//读取单个位号在某个时间段内的磁盘历史值的最小值(返回最小值所在时间点)
HRESULT _stdcall ReadMinDiskHisInTimeEx(HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pMinValue, FILETIME* pftTime, WORD* pwQuliaty);

//在所有位号范围下模糊查找实位号，支持“*、?”等基于正则表达式的模糊查找。
HRESULT _stdcall FindRealTags(HANDLE handle,LPCWSTR wszFilter, LPCWSTR wszRegionPath, LPCWSTR wszTransferName,  DWORD dwVtCount, VARTYPE* pVtDataTypes, DWORD* pdwCount, HTAG** pphTags);
//在所有位号范围下模糊查找虚位号，支持“*、?”等基于正则表达式的模糊查找。
HRESULT _stdcall FindVirtualTags(HANDLE handle,LPCWSTR wszFilter, LPCWSTR wszRegionPath, DWORD dwVtCount, VARTYPE* pVtDataTypes, DWORD* pdwCount, HTAG** pphTags);
