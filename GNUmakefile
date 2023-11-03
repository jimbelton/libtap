#Copyright (c) 2009, Sophos Group. All rights reserved.
#
#Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# - Neither the name of Sophos  nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

OS          ?= $(shell uname -s)
CC_OUT	    ?= -o
CFLAGS	    ?= -I. -Wall -c
EXT.obj     ?= .o
INSTALL     := install -o 0 -g 0
LINK	    ?= cc
LINK_FLAGS  ?= $(LFLAGS)
LINK_OUT    ?= -o
DST.obj     ?= tap$(EXT.obj) tap-ev$(EXT.obj) tap-tmpnam$(EXT.obj) tap-util$(EXT.obj)

.PHONY: all clean install package test

all:		target target/libtap.a

test:		target test/test-tap.t test/test-tap-ev.t
	test/test-tap.t
	test/test-tap-ev.t

%$(EXT.obj):	%.c tap.h
	$(CC) -c $(CFLAGS) -o $@ -fPIC -DPIC $*.c

target:
	mkdir target

target/libtap.a:	$(DST.obj)
	ar cru $@ $^

package:	target target/libtap.a
	mkdir -p output
ifeq ($(OS), FreeBSD)
	cd freebsd; /usr/bin/make all package
else
	cp -rp debian target
	gzip -fck tap.3 > target/tap.3.gz
	cd target && dpkg-buildpackage -b -rfakeroot -uc -us
	mkdir -p output
	mv libtap_*_amd64.deb output
endif

test/%$(EXT.obj):       test/%.c
	$(CC) $(CFLAGS) $< $(CC_OUT)$@

test/%.t:	test/%$(EXT.obj) target/libtap.a $(DST.deps)
	$(LINK) $(LINK_FLAGS) $^ $(COVERAGE_LIBS) $(LINK_OUT)$@

clean:
ifeq ($(OS), FreeBSD)
	cd freebsd; /usr/bin/make clean
endif
	rm -fr *$(EXT.obj) output target test/*$(EXT.obj) test/*.t libtap_*.changes libtap_*.buildinfo
