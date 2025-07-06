#include <unistd.h>

#include "tap.h"

int
main(void)
{
    size_t size;

    plan_tests(3);
    unlink("test-file");
    ok(tap_create_file("test-file", "hello, world\n"),         "Created a test file");
    is_eq(tap_read_file("test-file", &size), "hello, world\n", "Read the expected contents");
    is(size, 13,                                               "Got the expected size");
    return exit_status();
}
