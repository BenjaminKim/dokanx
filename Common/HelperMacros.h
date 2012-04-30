#pragma once


//
// 코멘트 대신 사용할 수 있는 매크로. 프로그래머의 실수를 줄여준다.
// 빌드할 때 출력창에 내용이 나타나게 된다.
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
			

//
// 가변 매개변수가 릴리즈 빌드 등에서 참조되지 않을 때 경고를 없애려는 용도로 사용한다.
// 
#define UNREFERENCED_PARAMETERS(...)    __VA_ARGS__

//
// 함수 이름를 Widechar로 얻어낸다.
// 
#define __STR2WSTR(str)    L##str
#define _STR2WSTR(str)     __STR2WSTR(str)
#define __FUNCTIONW__      _STR2WSTR(__FUNCTION__)

//
// 특정 함수를 더 이상 사용하지 못하도록 막기 위해서 사용한다.
// 대안 함수를 만들고 나면 Deprecate 처리할 함수의 선언 윗줄에 써준다.
//
#define _NDRIVE_DEPRECATE(_AlternativeFunc) __declspec(deprecated("This function or variable has been deprecated."\
										" Consider using " #_AlternativeFunc " instead."))

//
// switch문에서 case문의 정의값을 그대로 문자열로 리턴해야 하는 경우
#define CASE_RETURN_CODE_TO_STRING(str) case str: return L#str