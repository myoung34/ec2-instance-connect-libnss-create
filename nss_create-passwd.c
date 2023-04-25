#include <nss.h>
#include <pwd.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#ifndef NSS_CREATE_SCRIPT
#define NSS_CREATE_SCRIPT "/sbin/nss_create"
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

#define debug_printf(fmt, ARGS...) \
            do { if (DEBUG) fprintf(stderr, "NSS DEBUG: " fmt, ## ARGS); } while (0)

enum nss_status run_nss_create(char *command_code, char *data) {
    debug_printf("Called %s with args (command_code: %s, data: %s)\n", __FUNCTION__, command_code, data);
    pid_t pid = fork();

    if (pid == -1) {
        fprintf(stderr, "NSS ERROR: fork failed\n");
        return NSS_STATUS_UNAVAIL;
    } else if (pid == 0) {
        debug_printf("fork succeeded, executing\n");
        int res = execl(NSS_CREATE_SCRIPT, NSS_CREATE_SCRIPT, command_code, data, NULL);
        fprintf(stderr, "NSS ERROR: exec failed with status %d, errno=%d\n", res, errno);
        return NSS_STATUS_UNAVAIL;
    }

    debug_printf("waiting for pid %d\n", pid);
    int status;
    waitpid(pid, &status, 0);
    debug_printf("finished waiting for pid %d, status = %d\n", pid, status);
    if (status == 0) {
        return NSS_STATUS_TRYAGAIN;
    }
    if (status == 1) {
        return NSS_STATUS_NOTFOUND;
    }
    if (status == 2) {
        return NSS_STATUS_TRYAGAIN;
    }
    return NSS_STATUS_UNAVAIL;
}


// Called to open the passwd file
enum nss_status
_nss_create_setpwent(int stayopen)
{
    debug_printf("Called %s with args (stayopen: %d)\n", __FUNCTION__, stayopen);
    // Must be marked as success otherwise getpwent_r won't be called.
    return NSS_STATUS_SUCCESS;
}

// Called to close the passwd file
enum nss_status
_nss_create_endpwent(void)
{
    debug_printf("Called %s\n", __FUNCTION__);
    return NSS_STATUS_NOTFOUND;
}

// Called to look up next entry in passwd file
enum nss_status
_nss_create_getpwent_r(struct passwd *result, char *buffer, size_t buflen, int *errnop)
{
    debug_printf("Called %s\n", __FUNCTION__);
    return NSS_STATUS_NOTFOUND;
}

// Find a user account by uid
enum nss_status
_nss_create_getpwuid_r(uid_t uid, struct passwd *result, char *buffer, size_t buflen, int *errnop)
{
    debug_printf("Called %s with args (uid: %d)\n", __FUNCTION__, uid);
    return NSS_STATUS_NOTFOUND;
}

// Find a user account by name
enum nss_status
_nss_create_getpwnam_r(const char *name, struct passwd *result, char *buffer, size_t buflen, int *errnop)
{
    debug_printf("Called %s with args (name: %s)\n", __FUNCTION__, name);
    return run_nss_create("getpwnam_r", (char *)name);
}
