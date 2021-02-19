/*
iSYS C_Client DLL �����ӿں���
*/

#pragma once

#include "DataStructure.h"

enum  euUpdateType
{
	ELEMENT_TREE = 1,//������
	USER,	     //�û�/�û���
	TRANSFER	 //�ӿ�
};

//��������ö��
typedef enum _SubscriptionEnum
{
	TagReqNULL = 0,				//δ�����κζ�������
	TagRealUpdata = 1,			//λ��ʵʱ���¶�������
	TagAlarm = 2,				//λ�ű���״̬�仯����
	TagStopUpdata = 4,			//λ��ֹͣ����֪ͨ
	TagRecoveryDone = 8,		//��ʷ���ݻָ����֪ͨ
	TagDataChange = 16,			//λ�����ݱ仯����
	TagReqAll = -1				//���ж�����������
}SubscriptionEnum;

//��̬���»ص�����
typedef void (*CallbackMessageFunc) (HANDLE handle, euUpdateType Type, FILETIME ftUpdateTime);

//λ�����ݸ���֪ͨ�ص�����
typedef long (*CallbackTagValueChange)(HANDLE handle,		//���Ӿ������ʶ���ĸ��ͻ��˵Ķ���֪ͨ��Ϣ
									DWORD dwCount,			//���ݸ���֪ͨ��λ�Ÿ���
									WORD* wNotifyType,		//λ�����ݸ���֪ͨ�������飬����ΪdwCount��ΪSubscriptionEnumö��ֵ֮һ�������
									TAGVALSTATE* pValue);	//λ��ֵ���飬����ΪdwCount

//λ��ֹͣ����֪ͨ�ص�����
typedef long (*CallbackTagStopUpdate) (HANDLE handle,		//���Ӿ������ʶ���ĸ��ͻ��˵Ķ���֪ͨ��Ϣ
									DWORD dwCount,			//���ݸ���֪ͨ��λ�Ÿ���
									HTAG *hTags);			//λ�ž������

//��ʷ���ݻָ����֪ͨ�ص�����
typedef long (*CallbackTagRecoveryDone)(HANDLE handle,		//���Ӿ������ʶ���ĸ��ͻ��˵Ķ���֪ͨ��Ϣ
									DWORD dwCount,			//��������ȥ��λ�Ÿ���
									HTAG* hTags,			//λ�ž������
									FILETIME* pftBegins,	//λ�����ݻָ���ʼʱ������
									FILETIME* pftEnds);		//λ�����ݻָ�����ʱ������

//�ص������ṹ����
typedef struct  _CALLBACKINFO
{
private:
	DWORD cb;										//�ó�Ա������CALLBACK_INFO�ṹ�ĳ��ȣ�����Ϊ�˱�֤δ���ܹ������µĻص��������޸Ľӿ�
public:
	//���캯��
	_CALLBACKINFO()
	{
		cb = sizeof(CALLBACKINFO);
		pfOnTagValueChange = NULL;
		pfOnTagStopUpdata = NULL;
		pfOnTagRecoveryDone = NULL;
	}
	CallbackTagValueChange pfOnTagValueChange;		//λ�����ݸ���֪ͨ�ص�����ָ��
	CallbackTagStopUpdate pfOnTagStopUpdata;		//λ��ֹͣ����֪ͨ�ص�����ָ��
	CallbackTagRecoveryDone pfOnTagRecoveryDone;	//��ʷ���ݻָ����֪ͨ�ص�����ָ��
} CALLBACKINFO;



//��ȡ��������İ汾��
DWORD	 _stdcall GetVersionNumber( HANDLE handle );
//��ȡʵʱ���ݿ����ʱ��
HRESULT _stdcall GetRTDBTime( HANDLE handle, FILETIME & ftRTDBTime );

//���ӷ�����
HANDLE	_stdcall Connect( LPCWSTR lpServerIPAddr, USHORT usServerPort );
//��ȡ�쳣����ֵ�ľ������
HRESULT _stdcall GetLastException();
//��ȡ�쳣����ֵ�ľ������
HRESULT	_stdcall GetErrorMgs( HANDLE handle, HRESULT hr, LPWSTR* pch );
//�Ͽ�����
BOOL    _stdcall DisConnect( HANDLE handle );
//��ó�ʱʱ��
DWORD   _stdcall GetWaitTime();
void    _stdcall SetWaitTime( DWORD dw );
//��½
HRESULT _stdcall LogIn( HANDLE handle, LPCWSTR lpUserName, LPCWSTR lpPassword );
//ע��
HRESULT _stdcall LogOut( HANDLE handle );
//���ÿͻ�������
HRESULT _stdcall SetClientName(HANDLE handle, LPCWSTR wszClientName);
//������̬������ʾ
HRESULT _stdcall ReqUpdateMessage( HANDLE handle, CallbackMessageFunc fpMsgFun, BOOL bFlag );

//����һ����λ��
HRESULT _stdcall AddVirtualTag( HANDLE handle, DWORD dwCount,
							   VIRTUALTAGDEF* pVirDef, HRESULT** ppRes );
//ɾ��һ����λ��
HRESULT _stdcall DelVirtualTag( HANDLE handle, DWORD dwCount, 
							   HTAG* hTags, HRESULT** ppRes );
//ö����λ��
HRESULT _stdcall EnumVirtualTag( HANDLE handle, DWORD& dwCount, 
								VIRTUALTAGDEF** ppVirDef );
//��ȡһ����λ����̬��Ϣ
HRESULT _stdcall GetVirtualTagInfo( HANDLE handle, DWORD dwCount,
								   HTAG* hTags, HRESULT** ppRes, 
								   VIRTUALTAGDEF** ppVirDef );
//����һ����λ�ŵ���̬��Ϣ
HRESULT _stdcall SetVirtualTagInfo( HANDLE handle, DWORD dwCount,
								   VIRTUALTAGDEF* pVirDef, HRESULT** ppRes );
//��ȡһ����λ�ŵĽű�����
HRESULT _stdcall GetVirtualTagScript( HANDLE handle, HTAG hTag, LPWSTR* pszScript );
//����һ����λ�ŵĽű�����
HRESULT _stdcall SetVirtualTagScript( HANDLE handle, HTAG hTag, LPCWSTR lpScript );

//����һ��λ��ID��ȡ������
HRESULT _stdcall GetTagIDByName( HANDLE handle, LPCWSTR lpName, HTAG& hTag);
//����һ��λ�����ֻ�ȡID
HRESULT _stdcall GetTagNameByID( HANDLE handle, HTAG hTag, LPWSTR* pszName );
//����һ��λ��·�����ID
HRESULT _stdcall GetTagIDByPath( HANDLE handle, LPCWSTR lpPath, HTAG& hTag );
//����һ��λ��ID��ȡ·��
HRESULT _stdcall GetTagPathByID( HANDLE handle, HTAG hTag, LPWSTR* pszPath );
//�ƶ�һ��λ���ڶ������ϵ�λ��
HRESULT _stdcall MoveTag( HANDLE handle, HTAG hTag, LPCWSTR wszFatherPath );

//����һ������
HRESULT _stdcall AddRegion( HANDLE handle, REGION_DEF* pRegionDef );
//ɾ��һ������
HRESULT _stdcall DelRegion( HANDLE handle, LPCWSTR wszRegionPath );
//����һ������·����ȡ������̬��Ϣ
HRESULT _stdcall GetRegionInfoByPath( HANDLE handle, DWORD dwCount, LPWSTR* pszPath,
									 HRESULT** ppRes, REGION_ATTRIBUTE** ppRegion );
//����һ��������̬��Ϣ
HRESULT _stdcall SetRegionInfo( HANDLE handle, DWORD dwCount, REGION_ATTRIBUTE* pRegions, 
								   HRESULT** ppRes );
//�ƶ�һ�����򣬸÷�������������
HRESULT _stdcall MoveRegion( HANDLE handle, LPCWSTR wszRegionPath, LPCWSTR lpDesPath );
//ö��һ��ָ�������µ�������
HRESULT _stdcall EnumChildRegion( HANDLE handle, LPCWSTR wszRegionPath, 
								 DWORD &dwCount, REGION_ATTRIBUTE** ppRegion, WORD wdRecursive );
//ö��һ��ָ�������µ�λ��    
HRESULT _stdcall EnumTagsInRegion( HANDLE handle, LPCWSTR wszRegionPath, 
								 DWORD& dwRealCount, REALTAGDEF** ppReal, DWORD& dwVirCount, VIRTUALTAGDEF** ppVirtual, WORD wdRecursive );
//ö��ָ�����������ж������͡�����������
HRESULT _stdcall Snapshot( HANDLE handle, LPCWSTR wszRegionPath, DWORD dwTypeCount, HELEMENTTYPE* pElementType,
								 REGION_TREE** ppRegionTree);

//����һ��ʵλ��
HRESULT _stdcall AddRealTag( HANDLE handle, DWORD dwCount, 
							REALTAGDEF* pRealDef, HRESULT** ppRes );
//ɾ��һ��ʵλ��
HRESULT _stdcall DelRealTag( HANDLE handle, DWORD dwCount, 
							HTAG* hTags, HRESULT** ppRes );
//ö��ʵλ��
HRESULT _stdcall EnumRealTag( HANDLE handle, DWORD& dwCount, REALTAGDEF** ppRealDef );
//����һ���ӿ�����ö����λ��
HRESULT _stdcall EnumRealTagByTransfer(HANDLE handle, LPCWSTR lpInterfaceName,
									   DWORD& dwCount, REALTAGDEF** ppRealDef );
//��ȡһ��ʵλ����̬��Ϣ
HRESULT _stdcall GetRealTagInfo( HANDLE handle, DWORD dwCount, HTAG* hTags,
								HRESULT** ppRes, REALTAGDEF** ppRealDef );
//����һ��ʵλ�ŵ���̬��Ϣ
HRESULT _stdcall SetRealTagInfo( HANDLE handle, DWORD dwCount, 
								REALTAGDEF* pRealDef, HRESULT** ppRes );

//��ȡһ��λ�ŵ�ʵʱ����
HRESULT _stdcall ReadTagsValue( HANDLE handle, DWORD dwCount, HTAG *hTags, 
							   HRESULT** ppRes, TAGVALSTATE** ppValue );
//д��һ��λ�ŵ�ʵʱ����
HRESULT _stdcall WriteTagsValue( HANDLE handle, DWORD dwCount, HTAG *hTags, VARIANT *pvtValue,
								HRESULT** ppRes);

//д��һ��λ�ŵ�ʵʱ����
HRESULT _stdcall WriteTagsValueEx( HANDLE handle, DWORD dwCount, HTAG *hTags, VARIANT *pvtValue, 
								  FILETIME *pftTime, WORD *pwQuality, HRESULT** ppRes);
//д��һ��λ��һ����ʷ����
HRESULT _stdcall WriteTagBulkHisData(HANDLE handle, HTAG hTag, DWORD dwCount, VARIANT *pvtValue, 
									FILETIME *pftTime, WORD *pwQuality);

//��ȡ����λ�����е��ڴ���ʷ����
HRESULT _stdcall ReadTagAllMemHis( HANDLE handle, HTAG hTag, 
								  DWORD &dwCount, TAGVALSTATE** ppValue );
//��ȡһ��λ����ĳ��ʱ�̵��ڴ���ʷֵ
HRESULT _stdcall ReadTagsMemHisAtTime( HANDLE handle, DWORD dwCount, HTAG* hTags, FILETIME ftTime, DWORD wFlag,
									  DWORD dwTimeInterval,  HRESULT** ppRes, TAGVALSTATE** ppValue );
//��ȡ����λ����ĳ��ʱ����ڵ��ڴ���ʷֵ
HRESULT _stdcall ReadTagMemHisInTime(  HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag,
									 DWORD &dwCount, TAGVALSTATE** ppValue );
//������ȡһ��λ���ڴ���ʷֵ
HRESULT _stdcall ReadTagMemHisSampling( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD dwInterval, 
									   DWORD wFlag, DWORD dwTimeDiff, DWORD &dwCount, TAGVALSTATE** ppValue );

//��ȡһ��λ����ĳ��ʱ�̵Ĵ�����ʷֵ
HRESULT _stdcall ReadTagsDiskHisAtTime( HANDLE handle, DWORD dwCount, HTAG* hTags, FILETIME ftTime, DWORD wFlag,
									 DWORD dwTimeInterval,  HRESULT** ppRes, TAGVALSTATE** ppValue );
//��ȡ����λ����ĳ��ʱ����ڵĴ�����ʷֵ
HRESULT _stdcall ReadTagDiskHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag,
									  DWORD &dwCount, TAGVALSTATE** ppValue );
//������ȡһ��λ�Ŵ�����ʷֵ
HRESULT _stdcall ReadTagDiskHisSampling( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD dwInterval, 
									   DWORD wFlag, DWORD dwTimeDiff, DWORD &dwCount, TAGVALSTATE** ppValue );
//��ȡһ��λ����ĳʱ�̵���ʷֵ�����λ�����ڴ���ʷֵ���ȡ�ڴ���ʷֵ�������ȡ������ʷֵ
HRESULT _stdcall ReadTagsHisAtTime( HANDLE handle, DWORD dwCount, HTAG* hTags, FILETIME ftTime, DWORD wFlag,
								   DWORD dwTimeInterval, HRESULT** ppRes, TAGVALSTATE** ppValue );
//��ȡ����λ����ĳʱ����ڵ���ʷֵ�����λ�����ڴ���ʷֵ���ȡ�ڴ���ʷ�������ȡ������ʷֵ
HRESULT _stdcall ReadTagHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag/*��ȡ����*/, 
								  DWORD &dwCount, TAGVALSTATE** ppValue );
//������ȡһ��λ����ʷֵ
HRESULT _stdcall ReadTagHisSampling( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD dwInterval, 
									   DWORD wFlag, DWORD dwTimeDiff, DWORD &dwCount, TAGVALSTATE** ppValue );
//��ȡ����λ����ĳ��ʱ����ڵĴ�����ʷֵ�����ֵ
HRESULT _stdcall ReadMaxDiskHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pMaxValue);

//��ȡ����λ����ĳ��ʱ����ڵĴ�����ʷֵ����Сֵ
HRESULT _stdcall ReadMinDiskHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pMinValue);

//��ȡ����λ����ĳ��ʱ����ڵĴ�����ʷֵ�ĺ�ֵ
HRESULT _stdcall ReadSumDiskHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pSumValue);

//��ȡ����λ����ĳ��ʱ����ڵĴ�����ʷֵ��ƽ��ֵ
HRESULT _stdcall ReadAvgDiskHisInTime( HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pAvgValue);

//����һ���û�
HRESULT _stdcall AddUser(HANDLE handle, USER_DEF* pUser);
//ɾ��һ���û�		
HRESULT _stdcall DelUser(HANDLE handle, LPCWSTR lpUserName);
//��ȡһ���û���Ϣ
HRESULT _stdcall GetUserInfo(HANDLE handle, LPCWSTR lpUserName, USER_DEF** ppUser);
//����һ���û���Ϣ		
HRESULT _stdcall SetUserDef(HANDLE handle, USER_DEF* pUser);
//��ȡ�û��������û���
HRESULT _stdcall GetGroupsInUser(HANDLE handle, LPCWSTR lpUserName, DWORD& dwCount, USERGROUP_DEF** lpUserGroups );
//ö���û�	
HRESULT _stdcall EnumUser(HANDLE handle, DWORD& dwCount, USER_DEF** ppUser);
//����һ���û���
HRESULT _stdcall AddUserGroup(HANDLE handle, USERGROUP_DEF* pUserGroup);
//ɾ��һ���û���		
HRESULT _stdcall DelUserGroup(HANDLE handle, LPCWSTR lpUserGroup);
//��ȡһ���û�����Ϣ		
HRESULT _stdcall GetUserGroupDef(HANDLE handle, LPCWSTR lpUserGroup, USERGROUP_DEF** ppUserGroup);
//����һ���û�����Ϣ		
HRESULT _stdcall SetUserGroupDef(HANDLE handle, USERGROUP_DEF* pUserGroup);
//��һ���û�������һ���û�	
HRESULT _stdcall AddUserToUserGroup(HANDLE handle, LPCWSTR lpUserGroup, LPCWSTR lpUserName);
//��һ���û���ɾ��һ���û�
HRESULT _stdcall RemoveUserFromUserGroup(HANDLE handle, LPCWSTR lpUserGroup, LPCWSTR lpUserName);
//ö���û���		
HRESULT _stdcall EnumUserGroup(HANDLE handle, DWORD& dwCount, USERGROUP_DEF** ppUserGroup);
//ö��һ��ָ���û���Ȩ���б�
HRESULT _stdcall EnumUserACL(HANDLE handle, LPCWSTR lpUserName, DWORD& dwCount, ACEHEADER** ppACL);
//ö��һ��ָ���û����Ȩ���б�
HRESULT _stdcall EnumUserGroupACL(HANDLE handle, LPCWSTR lpUserGroup, DWORD& dwCount, ACEHEADER** ppACL);
//��֤һ���û�/�û���Ĳ���Ȩ��
HRESULT _stdcall Authenticate(HANDLE handle, LPCWSTR lpUserName, LPCWSTR wszPath,  DWORD dwCount, LPWSTR* pszRight, HRESULT** ppRes);
//����һ���û�/�û���Ĳ���Ȩ��	
HRESULT _stdcall AdjustPrivilege(HANDLE handle, LPCWSTR lpUserName, LPCWSTR wszPath,
								 BOOL bAdd,
								 BOOL bGrand,
								 DWORD dwCount, 
								 LPWSTR* pszRight, 
								 HRESULT** ppRes);
//��ȡ���������Լ����������͵�Ȩ��
HRESULT _stdcall GetAllTypeRight(HANDLE handle, DWORD &dwCount, TYPE_INFO** ppTypeInfo);
//���߱���������̬��Ϣ		
HRESULT _stdcall OnlineSave(HANDLE handle);

//����һ���ӿ�	
HRESULT _stdcall AddTransfer(HANDLE handle, TRANSFER_DEF* pTransDef, DWORD& hTransferID);
//ɾ��һ���ӿ�
HRESULT _stdcall DelTransfer(HANDLE handle, LPCWSTR lpTransName);
//ö�ٽӿ�
HRESULT _stdcall EnumTransfer(HANDLE handle, DWORD& dwCount, TRANSFER_DEF** ppTransDef);
//��ȡһ��ָ���ӿڵ���̬��Ϣ	
HRESULT _stdcall GetTransferDef(HANDLE handle, LPCWSTR lpTransName, TRANSFER_DEF** ppTransDef);
//����һ��ָ���ӿڵ���̬��Ϣ	
HRESULT _stdcall SetTransferDef (HANDLE handle, TRANSFER_DEF* pTransDef);
//��ȡ�����Ѿ�ע��Ľӿ�������б�	
HRESULT _stdcall GetAllRegisteredTransers(HANDLE handle, DWORD& dwCount, TRANSFER_INFO** ppTransRegInfo);
//��ȡһ��ָ���ӿ���֧�ֵ������б�
HRESULT _stdcall QueryTransferAvailableProperties(HANDLE handle, LPCWSTR lpTransName, DWORD& dwCount, TRANSFER_PROPERTYDEF** ppProperDef);
//��ȡָ���ӿڵ�һ������ֵ	
HRESULT _stdcall GetPropertiesVal(HANDLE handle, LPCWSTR lpTransName, DWORD& dwCount, TRANSFER_PROPERTYVALUE** ppProperty);
//����ָ���ӿڵ�һ������ֵ	
HRESULT _stdcall SetPropertiesVal(HANDLE handle, LPCWSTR lpTransName, DWORD dwCount,TRANSFER_PROPERTYVALUE* pProperty, HRESULT** ppRes);


///**********ʵʱ���ݿ�5.1�����ӿ� add at 2015/9/9 ***************///
//���ö��Ļص�����
HRESULT _stdcall SetCallbackInfo(HANDLE handle, CALLBACKINFO & callbackInfo);
//��Ӷ���λ�ţ�����TagRealUpdata��TagAlarm��TagStopUpdata��TagRecoveryDone��һ����϶��ķ�ʽ��
HRESULT _stdcall AddNotifyTags(HANDLE handle,				//���Ӿ��
							DWORD dwCount,			//��������ȥ��λ�Ÿ���
							HTAG *hTags,			//λ�ž������
							WORD* wSubscriptionType,//λ�Ŷ��������������飬����ΪdwCount��ΪSubscriptionEnumö��ֵ֮һ������ϣ�����Ӧ����TagDataChange
							HRESULT** ppRes);		//���ؽ�����飬��СΪdwCount��ֵΪS_OK����λ����ӳɹ�

//ȡ������λ��
HRESULT _stdcall CancelNotifyTags(HANDLE handle,				//���Ӿ��
							DWORD dwCount,			//��������ȥ��λ�Ÿ���
							HTAG *hTags,			//λ�ž������
							WORD* wSubscriptionType = NULL);	//Ҫȡ���Ķ��������������飬��ΪNULL�����ȡ������hTags�е�λ�ŵ����ж�������

//������ݱ仯֪ͨ����λ��
HRESULT _stdcall AddDataChangeNotifyTags(HANDLE handle,		//���Ӿ��
							DWORD dwCount,			//��������ȥ��λ�Ÿ���
							HTAG *hTags,			//λ�ž������
							double* dblChange,		//���ݱ仯����ֵ���飬����ΪdwCount������λ��ֵ�仯�����˶��ٸ���֪ͨ�����������ַ�����λ�ź��Ըò���
							HRESULT** ppRes);		//���ؽ�����飬��СΪdwCount��ֵΪS_OK����λ����ӳɹ�
//��ȡָ�������ѽ�������֪ͨ��λ����Ϣ
HRESULT _stdcall GetNotifyTagInfos(HANDLE handle,			//���Ӿ��
							DWORD & dwCount,		//��������ȥ��λ�Ÿ���
							HTAG ** pphTags,		//����λ�ž������
							WORD** ppSubscripType,	//λ�Ŷ��������������飬����ΪdwCount��ΪSubscriptionEnumö��ֵ֮һ�������
							double** ppdblChange);	////���ݱ仯����ֵ���飬����ΪdwCount������λ��ֵ�仯�����˶��ٸ���֪ͨ�����������ַ�����λ�ź��Ըò���
//��������ѽ����Ķ���λ��
HRESULT _stdcall ClearNotifyTags(HANDLE handle);			//���Ӿ��

//��ȡ�Ƿ������Զ����ӱ�ʶ
HRESULT _stdcall GetAutoRelink(HANDLE handle, bool & bIsAutoRelink);
//�����Ƿ������Զ����ӱ�ʶ
HRESULT _stdcall SetAutoRelink(HANDLE handle, bool bIsAutoRelink);
//�ж�ָ�����Ӿ���Ƿ��Ѿ����ӵ����ݿ�
HRESULT _stdcall IsConnected(HANDLE handle, bool & bIsConnected);

//��������λ��ID��ȡ������
HRESULT _stdcall GetTagsNameByIDs( HANDLE handle,DWORD dwCount, HTAG* hTag, LPWSTR** ppszName, HRESULT** ppRes );
//��������λ�����ƻ�ȡ��ID
HRESULT _stdcall GetTagsIDByNames( HANDLE handle, DWORD dwCount,LPWSTR* lpNames, HTAG** pphTags, HRESULT** ppRes);
//��������λ��·����ȡ��ID
HRESULT _stdcall GetTagsIDByPaths( HANDLE handle, DWORD dwCount,LPWSTR* lpPath, HTAG** pphTags, HRESULT** ppRes );
//��������λ��ID��ȡ��·��
HRESULT _stdcall GetTagsPathByIDs( HANDLE handle, DWORD dwCount, HTAG* hTag, LPWSTR** ppPaths, HRESULT** ppRes );
//��ȡ����λ����ĳ��ʱ����ڵĴ�����ʷֵ�����ֵ(�������ֵ����ʱ���)
HRESULT _stdcall ReadMaxDiskHisInTimeEx(HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pMaxValue, FILETIME* pftTime, WORD* pwQuliaty);
//��ȡ����λ����ĳ��ʱ����ڵĴ�����ʷֵ����Сֵ(������Сֵ����ʱ���)
HRESULT _stdcall ReadMinDiskHisInTimeEx(HANDLE handle, HTAG hTag, FILETIME ftBegin, FILETIME ftEnd, DWORD wFlag, VARIANT* pMinValue, FILETIME* pftTime, WORD* pwQuliaty);

//������λ�ŷ�Χ��ģ������ʵλ�ţ�֧�֡�*��?���Ȼ���������ʽ��ģ�����ҡ�
HRESULT _stdcall FindRealTags(HANDLE handle,LPCWSTR wszFilter, LPCWSTR wszRegionPath, LPCWSTR wszTransferName,  DWORD dwVtCount, VARTYPE* pVtDataTypes, DWORD* pdwCount, HTAG** pphTags);
//������λ�ŷ�Χ��ģ��������λ�ţ�֧�֡�*��?���Ȼ���������ʽ��ģ�����ҡ�
HRESULT _stdcall FindVirtualTags(HANDLE handle,LPCWSTR wszFilter, LPCWSTR wszRegionPath, DWORD dwVtCount, VARTYPE* pVtDataTypes, DWORD* pdwCount, HTAG** pphTags);
