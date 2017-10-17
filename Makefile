LANG=c
PYTHON=python

.PHONY: src-c/lshallow
.PHONY: src-c++/lshallow

src-$(LANG)/lshallow:
	( cd src-$(LANG) ; make )

# ===
# Run small or big version

.PHONY: run big
run: dam_break.gif

big: src-$(LANG)/lshallow
	src-$(LANG)/lshallow tests.lua dam 1000

# ===
# Generate visualizations (animated GIF or MP4)

dam_break.gif: dam_break.out
	$(PYTHON) util/visualizer.py dam_break.out dam_break.gif dam_break.png

wave.gif: wave.out
	$(PYTHON) util/visualizer.py wave.out wave.gif wave.png

dam_break.mp4: dam_break.out
	$(PYTHON) util/visualizer.py dam_break.out dam_break.mp4 dam_break.png

wave.mp4: wave.out
	$(PYTHON) util/visualizer.py wave.out wave.mp4 wave.png

# ===
# Generate output files

dam_break.out: src-$(LANG)/lshallow
	src-$(LANG)/lshallow tests.lua dam

wave.out: src-$(LANG)/lshallow
	src-$(LANG)/lshallow tests.lua wave

# ===
# Generate documentation

shallow.pdf: doc/jt-scheme.md src-$(LANG)/shallow.md
	pandoc --toc $^ -o $@

.PHONY: src-$(LANG)/shallow.md
src-$(LANG)/shallow.md:
	( cd src-$(LANG) ; make shallow.md )

# ===
# Clean up

.PHONY: clean
clean:
	rm -f lshallow
	rm -f dam_break.* wave.*
	rm -f src-c/shallow.md src-c++/shallow.md
	rm -f shallow.pdf
	( cd src-c   ; make clean )
	( cd src-c++ ; make clean )
