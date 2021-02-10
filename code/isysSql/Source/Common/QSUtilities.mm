// =================================================================================================
///  @file DirectoryUtil_Bundles_Frameworks.mm
///
///  Implementation of the Class DirectoryUtil for iOS nested bundles and OSX Frameworks
///
///  Copyright (C) 2014-2015 Simba Technologies Incorporated.
// =================================================================================================


/*
 Maintenance note:
 In order to avoid using a prefix header (.pch), the Foundation import (that enables the use of ObjC)
 was moved here, to the only file that needs it. There seems to be a mismatch between the typedef si-
 gned char BOOL in something that Foundation.h includes itself (objc.h), and the define BOOL int from
 sqltypes.h which is in the include hierarchy of DirectoryUtil.h. In order to compile despite the af-
 orementioned, it is necessary to include DirectoryUtil.h after importing Foundation.h.
 */

#import <Foundation/Foundation.h>

#include "QSUtilities.h"
#include "SimbaSettingReader.h"
using namespace Simba::Quickstart;

#ifndef USE_RESOURCE_BUNDLE
@interface Placeholder:NSObject
{
    
}
@end

@implementation Placeholder
@end
#endif

// Public Static ===================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSUtilities::GetTableDataFullPathNameIOS(const simba_wstring& in_fileName) const
{
#ifdef USE_RESOURCE_BUNDLE
    // Get the path to the nested bundle's resources.
    simba_string bundleName = SimbaSettingReader::GetResourceBundleName();
    NSString * bundleNameNS = [NSString stringWithUTF8String:bundleName.c_str()];
    NSURL * tempURL = [[NSBundle mainBundle]URLForResource:bundleNameNS withExtension:@"bundle"];
    NSBundle * tempBundle = [NSBundle bundleWithURL:tempURL];
    NSString * resourcePath = [tempBundle resourcePath];
    return simba_wstring::CreateFromUTF8([resourcePath UTF8String], SIMBA_NTS);
#else
    // Get the path to the framework's resource path by deriving it from the placeholder class.
    NSString * frameworkResourcePath = [[NSBundle bundleForClass:[Placeholder class]] resourcePath];
    return simba_wstring::CreateFromUTF8([frameworkResourcePath UTF8String], SIMBA_NTS);
#endif
}

