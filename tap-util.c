/* Copyright (c) 2009, Jim Belton. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "tap.h"

/**
 * Duplicate memory in a malloced buffer
 */
void *
tap_dup(const void * mem, size_t size)
{
    void * ret;

    if ((ret = malloc(size + 1)) != NULL) {
        memcpy(ret, mem, size);
        ((char *)ret)[size] = '\0';
    }

    return ret;
}

/**
 * Run a command and return its output in a malloced buffer
 */
char *
tap_shell(const char * command, int * status)
{
    char *         output;
    size_t         size;
    size_t         len;
    int            ret;
    int            ends[2];
    int            pid;
    unsigned       i;
    const unsigned TEST    = 0;
    const unsigned COMMAND = 1;

    assert(pipe(ends) == 0);

    switch (pid = fork()) {
    case -1:    // Fork failed
        assert(pid >= 0);
        break;

    case 0:    // Child process
        assert(close(ends[TEST])      == 0);
        assert(dup2(ends[COMMAND], 1) == 1);    // Redirect stdout to be the command's end of the pipe
        assert(dup2(ends[COMMAND], 2) == 2);    // Redirect stderr to be the command's end of the pipe
        assert(close(ends[COMMAND])   == 0);
        assert(execl("/bin/sh", "sh", "-c", command, NULL) == 0);
        exit(1);
    }

    assert(close(ends[COMMAND]) == 0);
    len  = 0;
    size = 256;
    assert((output = malloc(size)));

    while ((ret = read(ends[TEST], &output[len], size - len)) > 0) {
        len += ret;

        if (len == size) {
            size *= 2;
            assert((output = realloc(output, size)));
        }
    }

    assert(ret == 0);
    output[len] = '\0';
    close(ends[TEST]);

    for (i = 0; i < 2000 && (ret = waitpid(pid, status, WNOHANG)) == 0; i++)    // Wait for up to 2 second for the command to die
        usleep(1000);

    assert(ret == pid);
    return output;
}
