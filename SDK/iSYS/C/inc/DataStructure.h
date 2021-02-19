#pragma  once 
#include <Windows.h>
#include <winsock2.h>
#include "oaidl.h"

//λ�Ų������ݽṹ

typedef DWORD HTAG;
typedef DWORD HELEMENT;
typedef WORD  HELEMENTTYPE;
typedef DWORD HRIGHTTYPE;

//����λ��ֵ�Ķ��壬�������ݡ�ʱ��ȡ�
typedef struct 
{
	HTAG		hTag;			//λ�ŵ�ȫ�־��
	FILETIME    ftTimeStamp;	//ʱ���
	WORD        wQuality;		//������
	WORD		wAlarmState;	//����״̬
	VARIANT     vEng_value;		//����ֵ
}TAGVALSTATE;

//λ�ŵĹ�ͬ����,��������ʵλ�Ż�����λ�Ŷ��������Ϣ
typedef struct 
{
	//������Ϣ
	LPWSTR  szTagName;				//λ������
	LPWSTR  szTagAlias;				//λ�ű���
	LPWSTR  szTagDescr;				//λ������
	LPWSTR	 szPath;				//λ������·������������Ȩ�޹���ϵͳ��������Ҫ
	LPWSTR	 szUnit;				//λ�ŵĵ�λ
	HTAG        hTag;               //λ�ŵĿͻ��˾��
	VARTYPE     vtDataType;         //λ�ŵ�������������
	WORD        bReadOnly;          //λ��ֻ��
	//����������
	float  fEngHigh;
	float  fEngLow;
	//�ڴ���ʷ��Ϣ
	WORD        bStore;             //�Ƿ������ʷ���ݿ��־
	DWORD       dwPoolLenth;        //λ�ŵ��ڴ���ʷ����
	//������Ϣ
	double		dbHihi_limit;		//�߸���
	double		dbHi_limit;			//����
	double		dbLolo_limit;		//�͵���
	double		dbLo_limit;			//����
	WORD		bAlarmFlag;		    //����ʹ��
	//������ʷ���ݿ�����Ҫ������
	WORD		bArchive;			//�Ƿ�������ĵ�
	WORD        bCompress;          //�Ƿ�ѹ��
	WORD        wCompMax;           //���ѹ�����
	WORD        wCompMin;           //��Сѹ�����
	double       dbCompDev;          //ѹ��ƫ����
	DWORD     dwBufferCount;      //������ʷλ�Ż������ݸ���
	WORD        wFloatPrecision;     //���㾫��
} BASETAGDEF;

typedef struct//ʵλ�ŵ����ݽṹ
{
	//��������
	BASETAGDEF baseDef;
	//�ӿڲ���Ϣ
	LPWSTR szTransferName;			//λ�����ڽӿ����������
	LPWSTR szItemID;				//λ���ڵײ������
	WORD        bScan;              //�ӿڲ��Ƿ�ɨ��
	DWORD		dwScanSec;			//�ӿڲ�ɨ������
	DWORD       dwInterfaceFlag;    //�ӿڲ��־λ
} REALTAGDEF;

typedef struct//����λ�Ŷ���
{
	//��������
	BASETAGDEF baseDef;
	//�ű�ִ����Ϣ
	DWORD       dwTriggerFlag;       //����λ�ŵĴ�������
	DWORD       dwInterval;          //����Ƕ�ʱ����,��ò�������������
	LPWSTR  szTimerScript;			 //����λ�Ŷ�Ӧ�Ķ�ʱִ�нű�����
	//LPWSTR  szTimerScriptText;  
	LPWSTR  szReadScript;			 //����λ�Ŷ�Ӧ�Ķ�ȡִ�нű�����
	//LPWSTR  szReadScriptText;
	LPWSTR  szWriteScript;			 //����λ�Ŷ�Ӧ��д��ִ�нű�����
	//LPWSTR  szWriteScriptText;  
	LPWSTR  szScriptText; 
} VIRTUALTAGDEF;


//�й���̬�������ݽṹ
typedef struct tagREGION_DEF//�����壬���������Ҷ�ڵ�
{
	LPWSTR wszParentPath;	        //���ڵ�·��
	LPWSTR wszName;					//������
	LPWSTR wszDesc;					//��������
	VARIANT  vCustomData;			//������Զ�����Ϣ
} REGION_DEF;

typedef struct tagREGION_ATTRIBUTE//��������
{
	LPWSTR wszPath;					//����ȫ·��
	LPWSTR wszDesc;					//��������
	VARIANT  vCustomData;			//������Զ�����Ϣ
}REGION_ATTRIBUTE;

typedef struct tagUSERGROUP_DEF//�û��鶨��
{
	LPWSTR wszUserGroupName;			//�û�������
	LPWSTR wszUserGroupDesc;			//�û�������
	VARIANT  vCustomData;			   //�û�����Զ�������
} USERGROUP_DEF;

typedef struct tagUSER_DEF//�û�����
{
	LPWSTR wszUserName;        //�û�����
	LPWSTR wszUserPassword;    //�û�����
	LPWSTR wszUserDesc;        //�û�����
	VARIANT  vCustomData;				//�û����Զ�������
} USER_DEF;

typedef struct tagACEHEADER//Ȩ�޷�����Ŀ
{
	LPWSTR wszElement;			//����·��
	LPWSTR wszRightName;		//Ȩ������
	USHORT bGrant;				//����Ȩ��/�ܾ�Ȩ��
}ACEHEADER;

typedef struct tagTYPEDEF//��������
{
	LPWSTR wszTypeName;   //�������͵�����
	LPWSTR wszTypeAlias; //�������ͱ���
	LPWSTR wszTypeDescr; //������������
	HELEMENTTYPE hElementType;    //�������;��
}TYPE_DEF;

typedef struct tagRIGHTDEF//Ȩ�޶���
{
	LPWSTR wszRightName;  //Ȩ�޵�����	
	LPWSTR wszRightAlias;//Ȩ�����ͱ���
	LPWSTR wszRightDescr;//Ȩ����������
	HRIGHTTYPE hRight;			  //Ȩ�޵ľ��
}RIGHT_DEF;

struct REGION_TREE //��������Ϣ
{   
	REGION_ATTRIBUTE RegionAttr;		//��������
	DWORD dwRealTagCount;				//�������һ����ʵλ�ŵĸ���
	REALTAGDEF* pRealTags;				//�������һ����ʵλ����������
	DWORD dwVirtualTagCount;			//�������һ������λ�ŵĸ���
	VIRTUALTAGDEF* pVirtualTags;		//�������һ������λ����������
	DWORD dwChildRegionCount;			//�������һ���µ����������
	REGION_TREE* pChildRegions;			//�������һ���µ���������Ϣ
};

typedef struct tagTYPEINFO  //����������Ϣ
{
	LPWSTR wszTypeName;				//�������͵����ƣ�������Global��Region��Tag
	LPWSTR wszTypeAlias;			//�������͵ı���
	LPWSTR wszTypeDescr;			//�������͵������������Կ���Ϊ��
	HELEMENTTYPE hElementType;    	//�������͵ľ��
	DWORD dwRightCount;				//�����ڸö����Ȩ�޸���
	RIGHT_DEF* pRightDef;			//�����ڸö����Ȩ������
}TYPE_INFO;

//�йؽӿڲ������ݽṹ
typedef struct//�ӿڻ�����Ϣ����
{
	//������Ϣ
	DWORD hTransfer; //�ӿ����ID
	LPWSTR szTransferName; //�ӿ��������
	LPWSTR szTransferProgID; //�ӿ������ProgID
	LPWSTR szTransferDescription; //�ӿ��������
}TRANSFER_DEF;

typedef struct tagTransferInfo  //��ע��ӿڻ�����Ϣ
{
	LPWSTR wszTransferProgID;
	LPWSTR wszTransferDescription;
} TRANSFER_INFO;



typedef struct  
{
	//��������
	LPWSTR wszName;			//���Ե����ƣ����ַ�������Ϊ�գ����Ե����ֿ�������������һ��Ԫ���Զ��仯
	LPWSTR wszDescription;  //���Ե������� ע�⣺���ַ��������ǿ� NULL
	BOOL	bReadOnly; //�Ƿ���ֻ�����ԣ������ֻ�����ԣ���ô��̬��ʱ����Ҫ���û�ȥ���������ǿ���ͨ�������Է�ӳ�������ڲ�״̬
	VARTYPE   vt;   //���Ե���������
	VARIANT   varDefault; //ȱʡֵ�������VARIANT���Ͳ���VT_EMPY˵����λ����ȱʡֵ
	//��־λ������ʲô��ʽ�����Ե�ֵ����У�飬
	//������0(��У��)�� PROP_HI(У������)�� PROP_LO(У������)�� PROP_HILO(У��������)�� PROP_REG(ͨ��������ʽУ��)
	WORD      wLimitType;
	//���Ե�������
	double    dbUpLimit; //���Ե����ޣ�������ֵ������Ч
	double    dbLowLimit; //���Ե����ޣ�������ֵ������Ч
	//����һЩ��������룬����ö�����͵ȣ����Բ���������ʽ��У�飬��������������㷺��
	//ע�⣺������ǲ���������ʽУ�飬���ַ��������ǿ� NULL
	LPWSTR wszRegularExpression; //������ʽ
} TRANSFER_PROPERTYDEF;

typedef struct  //����ֵ
{
	LPWSTR wszName; //���Ե�����
	VARIANT varPropty; //���Ե�ֵ
} TRANSFER_PROPERTYVALUE;

#define INVALID_ID  0xFFFFFFFF