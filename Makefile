PLATFORM=gcc
SRC=src
PYTHON=python

.PHONY: src/lshallow

$(SRC)/lshallow:
	( cd $(SRC) ; make PLATFORM=$(PLATFORM) )

# ===
# Run small or big version

.PHONY: run big
run: dam_break.gif

big: $(SRC)/lshallow
	$(SRC)/lshallow tests.lua dam 1000

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

dam_break.out: $(SRC)/lshallow
	$(SRC)/lshallow tests.lua dam

wave.out: $(SRC)/lshallow
	$(SRC)/lshallow tests.lua wave

# ===
# Generate documentation

shallow.pdf: doc/jt-scheme.md $(SRC)/shallow.md
	pandoc --toc $^ -o $@

.PHONY: $(SRC)/shallow.md
$(SRC)/shallow.md:
	( cd $(SRC) ; make PLATFORM=$(PLATFORM) shallow.md )

# ===
# Clean up

.PHONY: clean
clean:
	rm -f lshallow
	rm -f dam_break.* wave.*
	rm -f src/shallow.md
	rm -f shallow.pdf
	( cd src; make PLATFORM=$(PLATFORM) clean )
