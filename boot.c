// 101226
// 101228

#include "ut.h"
#include "boot.h"

int wmain(int argc, wchar_t **argv)
{
#if defined(_DEBUG) && defined (_MSC_VER)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	return app_entry(argc, argv);
}
