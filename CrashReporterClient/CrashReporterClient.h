#ifndef __CrashReporterClient__
#define __CrashReporterClient__

extern char *__crashreporter_info__;

#define CRGetCrashLogMessage() (__crashreporter_info__)
#define CRSetCrashLogMessage(msg) (__crashreporter_info__ = msg);

#endif /* __CrashReporterClient__ */
