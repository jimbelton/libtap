/* Copyright (c) 2009, Jim Belton. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __GNUC__
#   define _GNU_SOURCE
#endif

#include <assert.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h> 

#ifdef _WIN32
#   include <io.h>
#   define open(file, oflag) _open((file), (oflag))
#endif

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

#define DOT ".tap-tmpfile"

/**
 * Create a file atomically
 *
 * @param fn   The file path
 * @param data A printf-like format, followed by arguments
 */
bool
tap_create_file(const char * fn, const char * data, ...)
{
    char *  block;
    ssize_t len;
    va_list ap;
    ssize_t wr;
    int     fd;

    if ((fd = open(DOT, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
        return false;

    va_start(ap, data);
    assert((len = vasprintf(&block, data, ap)) != -1);
    va_end(ap);

    wr = write(fd, block, len);
    close(fd);
    free(block);
    return wr == len && rename(DOT, fn) == 0;
}

/**
 * Read a file's contents
 *
 * @param fn       The file path
 * @param size_out A ponter to a variable to store the file's size (unchanged on error) or NULL
 *
 * @return NULL on error or a pointer to a malloced buffer containing the file's contents
 */
char *
tap_read_file(const char * fn, size_t * size_out)
{
    struct stat st;
    char *      buf;
    int         fd;

    if (fn == NULL || (fd = open(fn, O_RDONLY)) == -1)
        return NULL;

    assert(fstat(fd, &st) == 0);
    assert((buf = malloc(st.st_size + 1)));
    assert(read(fd, buf, st.st_size) == st.st_size);
    buf[st.st_size] = '\0';
    close(fd);

    if (size_out)
        *size_out = st.st_size;

    return buf;
}

/**
 * Create a temporary filename
 *
 * @param file_stem e.g. test-sxe-mmap-pool
 * @param buffer         Buffer in which to store temporary file name (/tmp/<file_stem>-pid-<pid>.bin)
 * @param size           Buffer size
 *
 * @return length of the result
 *
 * @note For win32, the temporary file name will be: C:\DOCUME~1\SIMONH~1.GRE\LOCALS~1\Temp\<file_stem>-pid-<pid>.bin
 */
unsigned
tap_tmpnam(
    const char *file_stem,
    char       *buffer,
    unsigned    size)
{
    int   used     = 0;
    int   done;
#ifdef _WIN32
    DWORD UniqueId = GetCurrentThreadId();
#else
    pid_t UniqueId = getpid();
#endif

#ifdef _WIN32
    assert(done = GetTempPath(size, buffer));    // e.g. 'C:\TEMP\'
#else
    assert((done = snprintf(buffer, size, "/tmp/")) <= size && done > 0);
#endif
    size -= done;
    used += done;

    assert((done = snprintf(&buffer[used], size, "%s-pid-%d.bin", file_stem, UniqueId)) <= size && done > 0);
    return used + done;
}
