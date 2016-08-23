// 110128

#include "viddebug.h"

#ifdef _DEBUG
void __vid_error(int n)
{
	if(n == 0) printf("checked cg error\n");
	else if(n == 1) printf("checked opengl error\n");
	else if(n == 2) printf("*");//! printf("returns error\n");
}
#endif