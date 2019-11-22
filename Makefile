DEBUG=1
DEBUGMODE=-DDEBUG
ifeq ($(DEBUG), 1)
    DEBUGMODE=-DDEBUG
else
    DEBUGMODE=
endif

OUTDIR=./bin/
LIBDIR=./lib/
OBJDIR=./obj/

OSX_VERSION=10.13
CC=cc
CFLAGS=-Wall -g
COPTS=-fsanitize=address
SOURCES=stack.c queue.c cstring.c ring.c
OBJECTS=$(SOURCES:.c=.o)
DLIB=$(OBJECTS:.o=.o.dylib)
EXECUTABLES=$(filter-out ./tests/test.h, $(wildcard ./tests/*))
TESTS=$(foreach var,$(SOURCES:.c=),$(OUTDIR)$(var))
LIBS=$(addprefix $(LIBDIR),$(OBJECTS:.o=.o.dylib))

all: $(SOURCES) $(EXECUTABLES)

.c.o:
	$(SILENT) mkdir -p bin lib obj
	$(CC) -c $< -o $(addprefix $(OBJDIR),$@)
	libtool -macosx_version_min $(OSX_VERSION) -dynamic $(addprefix $(OBJDIR),$@) -o $(addprefix $(LIBDIR),$@.dylib) -lpthread -lSystem


$(EXECUTABLES): $(OBJECTS)
	echo $(OBJECTS)
	$(CC) -o $(addprefix $(OUTDIR),$(notdir $(@:.c=))) $(CFLAGS) $@ $(COPTS) $(LIBS) -L./lib/ -I. $(DEBUGMODE)

clean:
	rm -f $(OBJECTS)
	rm -f $(LIBS)
	find ./tests -name "*~" -delete
	rm -rf $(OUTDIR)
	rm -rf $(LIBDIR)
	rm -rf $(OBJDIR)

normal:
	$(CC) -g -Wall ./tests/stack_test.c stack.c  -I. -lSystem -lpthread -fsanitize=address -o ./bin/stack_test
	$(CC) -g -Wall ./tests/queue_test.c queue.c -I. -lpthread -fsanitize=address -o ./bin/queue_test

stack:
	clang -c stack.c
	libtool -macosx_version_min $(OSX_VERSION) -dynamic stack.o -o libstack.dylib -lpthread -lSystem
	clang -Wall -g ./tests/stack_test.c -fsanitize=address -I. -lpthread ./libstack.dylib -o stack_test $(DEBUGMODE)

queue:
	clang -c queue.c
	libtool -macosx_version_min $(OSX_VERSION) -dynamic queue.o -o libqueue.dylib -lpthread -lSystem
	clang -Wall -g ./tests/queue_test.c -fsanitize=address -I. -lpthread ./libqueue.dylib -o queue_test $(DEBUGMODE)

string:
	clang -c cstring.c
	libtool -macosx_version_min $(OSX_VERSION) -dynamic cstring.o -o libcstring.dylib -lSystem
	clang -Wall -g ./tests/cstring_test.c -fsanitize=address -I. ./libcstring.dylib -o cstring_test $(DEBUGMODE)

ring:
	clang -c ring.c
	libtool -macosx_version_min $(OSX_VERSION) -dynamic ring.o -o libring.dylib -lSystem
	clang -Wall -g ./tests/ring_test.c -fsanitize=address -I. ./libring.dylib -o ring_test $(DEBUGMODE)

stack_clean:
	rm -f {stack_test,stack.o,libstack.dylib}
	rm -rf stack_test.dSYM

queue_clean:
	rm -f {queue_test,queue.o,libqueue.dylib}
	rm -rf queue_test.dSYM

string_clean:
	rm -f {cstring.o,cstring,libcstring.dylib}
	rm -rf cstring_clean.dSYM

ring_clean:
	rm -f {ring.o,ring,libring.dylib}
	rm -rf ring_test.dSYM
