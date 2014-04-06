#pragma once


//
// You can use this macro instead of using a comment.
// It prints its contents to output window when you build the project.
//
#define chSTR2(x) #x
#define chSTR(x)  chSTR2(x)

#define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "): --------" #desc "--------")
#define chFixLater message(__FILE__ "(" chSTR(__LINE__) "): --------Fix this later--------")

#define FixLater \
	do { \
	__pragma(chFixLater) \
	__pragma (warning(push)) \
	__pragma (warning(disable:4127)) \
	} while(0) \
	__pragma (warning(pop))

#define MSG(desc) \
	do { \
	__pragma(chMSG(desc)) \
	__pragma (warning(push)) \
	__pragma (warning(disable:4127)) \
	} while(0) \
	__pragma (warning(pop))



// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)
			

#define UNREFERENCED_PARAMETERS(...)    __VA_ARGS__

//
// Extracting function name as widechar.
// 
#define __STR2WSTR(str)    L##str
#define _STR2WSTR(str)     __STR2WSTR(str)
#define __FUNCTIONW__      _STR2WSTR(__FUNCTION__)



#define _DOKAN_DEPRECATE(_AlternativeFunc) __declspec(deprecated("This function or variable has been deprecated."\
										" Consider using " #_AlternativeFunc " instead."))


#define CASE_RETURN_CODE_TO_STRING(str) case str: return L#str
