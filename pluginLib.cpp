#include "pluginunjail.h"

extern "C" {
#include "unjail.h"
}

int FreeUnjail(void)
{
	struct thread td;
	return Sys::kexec((void *)&unjail455, &td);

}
