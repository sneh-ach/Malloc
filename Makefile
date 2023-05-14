CC=       	gcc
CFLAGS= 	-g -gdwarf-2 -std=gnu99 -Wall
LDFLAGS=
LIBRARIES=      lib/libmalloc-ff.so \
		lib/libmalloc-nf.so \
		lib/libmalloc-bf.so \
		lib/libmalloc-wf.so

TESTS=		tests/test1 \
                tests/test2 \
                tests/test3 \
                tests/test4 \
                tests/bfwf \
                tests/ffnf \
                tests/realloc \
                tests/calloc \
				tests/test_malloc

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all:    $(LIBRARIES) $(TESTS)

lib/libmalloc-ff.so:     src/malloc.c
	$(CC) -shared -fPIC $(CFLAGS) -DFIT=0 -o $@ $< $(LDFLAGS)

lib/libmalloc-nf.so:     src/malloc.c
	$(CC) -shared -fPIC $(CFLAGS) -DNEXT=0 -o $@ $< $(LDFLAGS)

lib/libmalloc-bf.so:     src/malloc.c
	$(CC) -shared -fPIC $(CFLAGS) -DBEST=0 -o $@ $< $(LDFLAGS)

lib/libmalloc-wf.so:     src/malloc.c
	$(CC) -shared -fPIC $(CFLAGS) -DWORST=0 -o $@ $< $(LDFLAGS)

test:
	env LD_PRELOAD=lib/libmalloc-bf.so tests/bfwf
	env LD_PRELOAD=lib/libmalloc-bf.so tests/ffnf
	env LD_PRELOAD=lib/libmalloc-bf.so tests/calloc
	env LD_PRELOAD=lib/libmalloc-bf.so tests/realloc
	env LD_PRELOAD=lib/libmalloc-bf.so tests/test1
	env LD_PRELOAD=lib/libmalloc-bf.so tests/test2
	env LD_PRELOAD=lib/libmalloc-bf.so tests/test3
	env LD_PRELOAD=lib/libmalloc-bf.so tests/test4

	env LD_PRELOAD=lib/libmalloc-ff.so tests/bfwf
	env LD_PRELOAD=lib/libmalloc-ff.so tests/ffnf
	env LD_PRELOAD=lib/libmalloc-ff.so tests/calloc
	env LD_PRELOAD=lib/libmalloc-ff.so tests/realloc
	env LD_PRELOAD=lib/libmalloc-ff.so tests/test1
	env LD_PRELOAD=lib/libmalloc-ff.so tests/test2
	env LD_PRELOAD=lib/libmalloc-ff.so tests/test3
	env LD_PRELOAD=lib/libmalloc-ff.so tests/test4

	env LD_PRELOAD=lib/libmalloc-nf.so tests/bfwf
	env LD_PRELOAD=lib/libmalloc-nf.so tests/ffnf
	env LD_PRELOAD=lib/libmalloc-nf.so tests/calloc
	env LD_PRELOAD=lib/libmalloc-nf.so tests/realloc
	env LD_PRELOAD=lib/libmalloc-nf.so tests/test1
	env LD_PRELOAD=lib/libmalloc-nf.so tests/test2
	env LD_PRELOAD=lib/libmalloc-nf.so tests/test3
	env LD_PRELOAD=lib/libmalloc-nf.so tests/test4

	env LD_PRELOAD=lib/libmalloc-wf.so tests/bfwf
	env LD_PRELOAD=lib/libmalloc-wf.so tests/ffnf
	env LD_PRELOAD=lib/libmalloc-wf.so tests/calloc
	env LD_PRELOAD=lib/libmalloc-wf.so tests/realloc
	env LD_PRELOAD=lib/libmalloc-wf.so tests/test1
	env LD_PRELOAD=lib/libmalloc-wf.so tests/test2
	env LD_PRELOAD=lib/libmalloc-wf.so tests/test3
	env LD_PRELOAD=lib/libmalloc-wf.so tests/test4

malloc_test:
	./test_malloc
	env LD_PRELOAD=lib/libmalloc-bf.so tests/test_malloc
	env LD_PRELOAD=lib/libmalloc-ff.so tests/test_malloc
	env LD_PRELOAD=lib/libmalloc-bf.nf tests/test_malloc
	env LD_PRELOAD=lib/libmalloc-wf.so tests/test_malloc

	

test2:
	env LD_PRELOAD=lib/libmalloc-ff.so tests/ffnf
	env LD_PRELOAD=lib/libmalloc-nf.so tests/ffnf


clean:
	rm -f $(LIBRARIES) $(TESTS) test_malloc

.PHONY: all clean
