/*************************************************
 * @Description: file content
 * @Author: yuanquan
 * @Email: yuanquan2011@qq.com
 * @Date: 2021-02-18 20:41:31
 * @LastEditors: yuanquan
 * @LastEditTime: 2021-02-25 16:08:09
 * @copyright: Copyright (c) yuanquan
 *************************************************/
#pragma  once 

#define ISYS_SQL_NAMESPACE_BEGIN namespace ISYS{namespace SQL{
#define ISYS_SQL_NAMESPACE_END }}

#include "ILogger.h"

#define ISYS_LOG_TRACE(loger, msg)			TRACE_LOG(loger, 	__FILE__, __FUNCTION__, NumberConverter::ConvertInt32ToWString(__LINE__).GetAsPlatformString().c_str(), (msg).GetAsPlatformString().c_str())
#define ISYS_LOG_DEBUG(loger, msg)			DEBUG_LOG(loger, 	__FILE__, __FUNCTION__, NumberConverter::ConvertInt32ToWString(__LINE__).GetAsPlatformString().c_str(), (msg).GetAsPlatformString().c_str())
#define ISYS_LOG_INFO(loger, msg)			INFO_LOG(loger,		__FILE__, __FUNCTION__, NumberConverter::ConvertInt32ToWString(__LINE__).GetAsPlatformString().c_str(), (msg).GetAsPlatformString().c_str())
#define ISYS_LOG_WARNING(loger, msg)		WARNING_LOG(loger, 	__FILE__, __FUNCTION__, NumberConverter::ConvertInt32ToWString(__LINE__).GetAsPlatformString().c_str(), (msg).GetAsPlatformString().c_str())
#define ISYS_LOG_ERROR(loger, msg)			ERROR_LOG(loger, 	__FILE__, __FUNCTION__, NumberConverter::ConvertInt32ToWString(__LINE__).GetAsPlatformString().c_str(), (msg).GetAsPlatformString().c_str())
#define ISYS_LOG_FATAL(loger, msg)			FATAL_LOG(loger, 	__FILE__, __FUNCTION__, NumberConverter::ConvertInt32ToWString(__LINE__).GetAsPlatformString().c_str(), (msg).GetAsPlatformString().c_str())


