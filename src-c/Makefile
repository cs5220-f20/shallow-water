PLATFORM=gcc
include ../config/Makefile.in.$(PLATFORM)

# ===
# Main driver and sample run

lshallow: ldriver.o shallow2d.o stepper.o
	$(CC) $(CFLAGS) $(LUA_CFLAGS) -o $@ $^ $(LUA_LIBS) $(LIBS)

ldriver.o: ldriver.c shallow2d.h
	$(CC) $(CFLAGS) $(LUA_CFLAGS) -c $<

shallow2d.o: shallow2d.c
	$(CC) $(CFLAGS) -c $<

stepper.o: stepper.c
	$(CC) $(CFLAGS) -c $<

# ===
# Documentation

shallow.md: shallow2d.h shallow2d.c stepper.h stepper.c ldriver.c
	ldoc $^ -o $@

# ===
# Clean up

.PHONY: clean
clean:
	rm -f lshallow *.o
	rm -f shallow.md
