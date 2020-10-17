% Shallow water simulation
% David Bindel
% 2017-10-16

# Introduction

## The Fifth Dwarf

In an 2006 report on ["The Landscape of Parallel Computing"][view],
a group of parallel computing researchers at Berkeley suggested
that high-performance computing platforms be evaluated with respect
to "13 dwarfs" -- frequently recurring computational patterns in
high-performance scientific code.  This assignment represents the
fifth dwarf on the list: structured grid computations.  Structured
grid computations feature high spatial locality and allow regular
access patterns.  They are, in principle, one of the easier types of
computations to parallelize.

Structured grid computations are particularly common in fluid dynamics
simulations, and the code that you will tune in this assignment is an
example of such a simulation.  You will be optimizing and
parallelizing a finite volume solver for the shallow water equations,
a two-dimensional PDE system that describes waves that are very long
compared to the water depth.  This is an important system of equations
that applies even in situations that you might not initially think of
as "shallow"; for example, tsunami waves are long enough that they can
be modeled using the shallow water equations even when traveling over
mile-deep parts of oceans.  There is also a very readable
[Wikipedia article][wiki] on the shallow water equations, complete
with a little animation similar to the one you will be producing.  I
was inspired to use this system for our assignment by reading the
chapter on [shallow water simulation in MATLAB][exm] from Cleve
Moler's books on "Experiments in MATLAB" and then getting annoyed that
he chose a method with a stability problem.

[view]: http://www.eecs.berkeley.edu/Pubs/TechRpts/2006/EECS-2006-183.pdf
[exm]: https://www.mathworks.com/moler/exm/chapters/water.pdf
[wiki]: https://en.wikipedia.org/wiki/Shallow_water_equations

## Your mission

You are provided with a (partially tuned) reference implementations of
a finite volume solver for 2D hyperbolic PDEs.  The most performance
critical components are in modules called `stepper` (the generic
central finite volume scheme) and `shallow2d` (which defines flux
functions that govern the physics of the shallow water equations).  In
addition, there is a Lua-based driver that runs the code on various
test problems (in `tests.lua`) and a visualization script (under
`util/visualization.py` that produces movies and pretty pictures from
the simulation outputs).

For this assignment, you should attempt three tasks:

1.  *Parallelization*: You should parallelize your code using either
    MPI or OpenMP.  You may try both if you have time.

2.  *Scaling study*: You should run strong and weak scaling studies
    analyses on Graphite and/or Comet.

3.  *Profiling and tuning*: Using either profiling tools or manual
    instrumentation, look for bottlenecks in the code.  Your goal is
    to get the implementation to run as quickly as possible.  This may
    involve a domain decomposition (useful even in the serial case, as
    we have seen); it may involve vectorization of the computational
    kernels; or it may involve eliminating redundant computations.
    Note that I have already done some serial tuning, so higher-level
    optimizations (time step batching, blocking) are likely to be more
    effective than low-level tuning for vectorization.

The primary deliverable for your project is a report that describes
your performance experiments and attempts at tuning, along with what
you learned about things that did or did not work.  Good things for
the report include:

- Profiling results
- Speedup plots and scaled speedup plots
- Performance models that predict speedup

In addition, you should also provide the code, and ideally scripts
that make it simple to reproduce any performance experiments you've
run.
