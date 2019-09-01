#ifndef _EXPORT_DEF_H_
#define _EXPORT_DEF_H_
	
	
#ifdef XCONFIG_EXPORTS
#define XCONFIG_API __declspec(dllexport)
#else
#define XCONFIG_API __declspec(dllimport)
#endif


#endif //_EXPORT_DEF_H_

