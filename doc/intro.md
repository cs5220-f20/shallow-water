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

## Cloud Computing

Infrastructure as a Service (IaaS) is the main purpose of cloud
computing, offering on-demand computing resources. It has become a
cornerstone of distributed computing and is increasingly useful in
high performance computing as well.  Because our class cluster does
not have GPUs, we will use the GPU-enabled virtual machines (VMs)
under the Google Compute Engine (GCE) to give you a taste of both GPU
programming and working in the cloud.

We will give an end-to-end demo of setting up a Google Cloud Platform
(GCP) account, spinning up a VM, and configuring it with the minimum
amount of software to successfully complete this assigment. For
instructions to refer to, please see [this instruction
link][cloudinstructions]

[cloudinstructions]: https://github.com/cornell-cs5220-f17/water-cuda/blob/master/doc/cloud.md

## Your mission

You are provided with two possible reference implementations of a
finite volume solver for 2D hyperbolic PDEs.  In each case, the most
performance critical components are in modules called `stepper` (the
generic central finite volume scheme) and `shallow2d` (which defines
flux functions that govern the physics of the shallow water
equations).  In addition, there is a Lua-based driver that runs the
code on various test problems (in `tests.lua`) and a visualization
script (under `util/visualization.py` that produces movies and pretty
pictures from the simulation outputs).

For this assignment, you should attempt three tasks:

2.  *Parallelization*: You should parallelize your code by offloading
    to a GPU via CUDA.  You may start from either the C or the C++
    reference implementation (though the C implementation is rather
    faster on a CPU, and should be used as the basis of performance
    comparisons when judging speedup).

1.  *Profiling*:  Use the NVidia profiler `nvprof` to identify
    bottlenecks in your code.

3.  *Tuning*:  You should tune your code in order to get it to run as
    fast as possible.  This may involve a domain decomposition
    with per-processor ghost cells and batching of time steps between
    synchronization; it may involve vectorization of the computational
    kernels; or it may involve eliminating redundant computations in
    the current implementation.

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

## Tuning readings

You are welcome to read about the shallow water equations and about
finite volume methods if you would like, and this may help you in
understanding what you're seeing.  But it is possible to tune your
codes without understanding all the physics behind what is going on!
I recommend two papers in particular that talk about tuning of finite
difference and finite volume codes on accelerators: one on
[optimizing a 3D finite difference code][3dfd] on the Intel cores and
the Xeon Phi, and one on
[optimizing a shallow water simulator][brodtkorb]
on NVidia GPUs.  The Phi architecture is different, but some of the
concepts should transfer to thinking about improving performance on
the GPUs.

[3dfd]: https://software.intel.com/en-us/articles/eight-optimizations-for-3-dimensional-finite-difference-3dfd-code-with-an-isotropic-iso
[brodtkorb]: http://cmwr2012.cee.illinois.edu/Papers/Special%20Sessions/Advances%20in%20Heterogeneous%20Computing%20for%20Water%20Resources/Brodtkorb.Andre_R.pdf

## CUDA References

CUDA is a very mature language for programming Nvidia GPUs. Its
primary open source competitor is OpenCL, which is a bit more complicated
to use, but ports to AMD GPUs as well. 

We covered CUDA during class, but only superficially. For implementation
specifics, you will have to look up documentation yourself. Some good
starting resources are

* A basic CUDA tutorial (highly recommended for anyone without experience) 
[here](https://devblogs.nvidia.com/parallelforall/even-easier-introduction-cuda/)
* If you prefer slides, a comparable tutorial is 
[here](http://www.nvidia.com/content/GTC-2010/pdfs/2131_GTC2010.pdf)
* The nvprof
[documentation](http://docs.nvidia.com/cuda/profiler-users-guide/index.html)
can be a bit tricky to explore and parse, but is the best resource for
learning how to use nvprof from the command line
* You will need to change all basic math functions (min, abs, etc) to
their CUDA equivalents. See the CUDA 
[Math API](http://docs.nvidia.com/cuda/cuda-math-api/index.html) for precise
details.

Also, don't be too worried about achieving far-superior performance
numbers with perfect scaling! Most people in this class have no
experience with CUDA, and we are not expecting everyone be CUDA
experts by the time this project is over.  We only want people to
learn the fundamentals, which includes

* The CUDA offloading model
* CUDA thread indexing
* CUDA loop parallelization
* CUDA memory managment
* Tuning with CUDA block sizes and nvprof

You should be able to write a fairly optimized code with just
these. Finally, note that traditional I/O is not supported in the
default CUDA libraries; to write to files, you will have to copy data
on the GPU back to the GPU.  Repeated communication between the CPU
and GPU across the PCI-Express bus will become the bottleneck if you
are doing it at every step. For the purposes of this assignment, you
do not need to perform I/O other than to verify that your
implementation is still correct.

## Notes on C vs C++

I originally wrote this code in C++; for the tuned version, I
eventually switched to C.  You are free to use either one as the basis
of your CUDA code.  The C++ code arguably provides a nicer
abstraction; the C code definitely runs faster.

While I have tried not to do anything too obscure, the C++ code does
use some C++ 11 features (e.g. the `constexpr` notation used to tell
the compiler something is a compile-time constant).  If you want to
build on your own machine, you may need to figure out the flag needed
to tell your compiler that you are using this C++ dialect.

## CUDA in C or C++

Basic CUDA does not natively support offloading to C++ vectors, which
is what the base C++ code uses. You have a few choices for writing your
parallel implementation: you can start from the C implementation; you
can start from the C++ implementation and use standard CUDA
operations; or you can start from the C++ implementation and use the
[Thrust](https://developer.nvidia.com/thrust) library to allocate
and manipulate C++ vectors on the device (see the Thrust [tutorial](http://www.mariomulansky.de/data/uploads/cuda_thrust.pdf) and 
 [documentation](http://docs.nvidia.com/cuda/thrust/index.html) for details)

There is are many correct ways of doing things. Make sure to fully
understand the base code and read the CUDA documentation before deciding.

### Notes on the documentation

The documentation for this project is generated automatically from
structured comments in the source files using a simple tool called
[ldoc][ldoc] that I wrote some years ago.  You may or may not choose
to use [ldoc][ldoc] for your version.

[ldoc]: https://github.com/dbindel/ldoc

