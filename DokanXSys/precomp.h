#include <ntifs.h>

#define _DOKAN_SYS
#include "dokan.h"

/* 
When the compiler sees a line like this:
chMSG(Fix this later);

it outputs a line like this:

c:\CD\CmnHdr.h(82):Fix this later

You can easily jump directly to this line and examine the surrounding code.
*/

#define chSTR2(x) #x
#define chSTR(x)  chSTR2(x)

#define chMSG(desc) message(__FILE__ "("chSTR(__LINE__)"): --------" #desc "--------")
#define chFixLater message(__FILE__ "("chSTR(__LINE__)"): Fix this later")

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

//#define _LOG_ENABLED