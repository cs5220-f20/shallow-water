PLATFORM=gcc
include ../config/Makefile.in.$(PLATFORM)

# ===
# Main driver and sample run

lshallow: ldriver.cc stepper.h shallow2d.h minmod.h meshio.h
	$(CXX) $(CXXFLAGS) $(LUA_CFLAGS) -o $@ $< $(LUA_LIBS) $(LIBS)

# ===
# Generate documentation

shallow.md: shallow2d.h minmod.h stepper.h meshio.h ldriver.cc
	ldoc $^ -o $@

# ===
# Clean up

.PHONY: clean
clean:
	rm -f lshallow
	rm -f shallow.md
