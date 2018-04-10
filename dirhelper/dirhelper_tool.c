#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <bootstrap.h>
#include <bsm/libbsm.h>
#include "../libcoreservices/dirhelper_priv.h"
#include "dirhelperServer.h"

// We cannot use xpc_transaction_begin/end here, because libxpc is not yet implemented
// on PureDarwin. At the same time, the definitions in Apple's <vproc.h> are deprecated.
// So, we redeclare them here.
#pragma mark Transaction Support
typedef struct vproc_s *vproc_t;
typedef struct vproc_transaction_s *vproc_transaction_t;
extern vproc_transaction_t vproc_transaction_begin(vproc_t proc);
extern void vproc_transaction_end(vproc_t proc, vproc_transaction_t transaction);

#pragma mark libSystem Imports

extern char * __user_local_dirname(uid_t uid, dirhelper_which_t which, char *path, size_t pathlen);
extern char * __user_local_mkdir_p(char *path);

#pragma mark Server Routines

kern_return_t do___dirhelper_create_user_local(mach_port_t server_port, audit_token_t remote_creds)
{
	vproc_transaction_t transaction = vproc_transaction_begin(NULL);
	kern_return_t kr = KERN_SUCCESS;

	uid_t uid = audit_token_to_ruid(remote_creds);
	gid_t gid = audit_token_to_rgid(remote_creds);

	char *path = calloc(PATH_MAX, sizeof(char));
	char *real_path = __user_local_dirname(uid, DIRHELPER_USER_LOCAL, path, PATH_MAX);
	if (real_path != NULL) {
		__user_local_mkdir_p(real_path);
		chown(real_path, uid, gid);
		chmod(real_path, 0755);
	} else {
		fprintf(stderr, "*** __user_local_dirname() failed: %s\n", strerror(errno));
		kr = KERN_FAILURE;
	}

	free(path);
	vproc_transaction_end(NULL, transaction);
	return kr;
}

kern_return_t do___dirhelper_idle_exit(mach_port_t server_port, audit_token_t remote_creds)
{
	// I don't know what this is supposed to do. We use vproc transactions and EnablePressuredExit instead anyway.
	return KERN_SUCCESS;
}

#pragma mark Main

#define __max(a, b) (((a) > (b)) ? (a) : (b))
#define DIRHELPER_MAX_MSG_SIZE __max(sizeof(union __RequestUnion__do_dirhelper_subsystem), sizeof(union __ReplyUnion__do_dirhelper_subsystem))

int main(int argc, const char * argv[]) {
	if (geteuid() != 0) {
		fprintf(stderr, "%s: must be run as root\n", argv[0]);
		return 1;
	}

	mach_port_t service_port;
	kern_return_t kr = bootstrap_check_in(bootstrap_port, DIRHELPER_BOOTSTRAP_NAME, &service_port);
	if (kr != KERN_SUCCESS) {
		fprintf(stderr, "%s: could not check in with launchd: kern_return_t 0x%08X\n", argv[0], kr);
		return 1;
	}

	kr = mach_msg_server(dirhelper_server, DIRHELPER_MAX_MSG_SIZE, service_port, MACH_RCV_TRAILER_ELEMENTS(MACH_RCV_TRAILER_AUDIT) | MACH_RCV_TRAILER_TYPE(MACH_MSG_TRAILER_FORMAT_0));
	if (kr != KERN_SUCCESS) {
		fprintf(stderr, "%s: mach_msg_server() failed with kern_return_t 0x%08X", argv[0], kr);
		return 1;
	}

	return 0;
}
