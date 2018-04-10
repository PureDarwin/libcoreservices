#include "CrashReporterClient.h"

char *__crashreporter_info__;
asm(".desc __crashreporter_info__, 0x10");
