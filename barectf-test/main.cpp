#include <stdio.h>

/* Include the Linux FS platform header */
#include "barectf-platform-linux-fs.h"

/* Include the barectf public header */
#include "barectf.h"

int main(const int argc, const char *const argv[]) {
    /* Platform context */
    struct barectf_platform_linux_fs_ctx *platform_ctx;

    /* barectf context */
    struct barectf_default_ctx *ctx;

    int i;

    /*
     * Obtain a platform context.
     *
     * The platform is configured to write 512-byte packets to a data
     * stream file within the `trace` directory.
     */
    platform_ctx = barectf_platform_linux_fs_init(512, "trace/stream", 0, 0, 0);
    if (NULL == platform_ctx) {
        printf("Failed to initialize platform ctx\n");
        return -1;
    }

    /* Obtain the barectf context from the platform context */
    ctx = barectf_platform_linux_fs_get_barectf_ctx(platform_ctx);

    /*
     * Write a `one_integer` event record which contains the number of
     * command arguments.
     */
    barectf_trace_sched_switch(ctx, "task_prev_name", 5, 2, 0, "task_next_name", 10, 3);

    /* Finalize (free) the platform context */
    barectf_platform_linux_fs_fini(platform_ctx);

    return 0;
}
