# Assignment Overview

## The Fifth Dwarf

In an 2006 report on ["The Landscape of Parallel Computing"][view],
a group of parallel computing researchers at Berkeley suggested
that high-performance computing platforms be evaluated with respect
to "13 dwarfs" -- frequently recurring computational patterns in
high-performance scientific code.  This assignment represents the
fifth dwarf on the list: structured grid computations.  Structured
grid computations feature high spatial locality and allow regular
access patterns. Consequently, they are ,in principle, one of the easier types of
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
with a little animation similar to the one you will be producing.  Prof. 
Bindel was inspired to use this system for our assignment by reading the
chapter on [shallow water simulation in MATLAB][exm] from Cleve
Moler's books on "Experiments in MATLAB" and then getting annoyed that
Cleve chose a method with a stability problem.

[view]: http://www.eecs.berkeley.edu/Pubs/TechRpts/2006/EECS-2006-183.pdf
[exm]: https://www.mathworks.com/moler/exm/chapters/water.pdf
[wiki]: https://en.wikipedia.org/wiki/Shallow_water_equations

## Cloud Computing

Infrastructure as a Service (IaaS) is the main purpose of cloud computing, 
offering on-demand computing resources. It has become a cornerstone of distributed
computing and to a certain extent, at-scale high performance computing as well. 

In this light, we decided to cover a bit of cloud computing in this assigment. More
practically speaking, Totient does not have an GPUs on it as well. We expect you to
create, manage, and configure a virtual machine on the cloud; we will be using 
Google Compute Platform (GCP), but the same principles hold for most large cloud vendors. 

We will be giving an end-to-end demo of setting up a GCP account, 
spinning up a VM, and configuring it with the minimum amount of
software to successfully complete this assigment. For instructions to
refer to, please see [this instruction link][cloudinstructions]

[cloudinstructions]: https://github.com/ericlee0803/water-cuda/blob/master/instructions.md

## Shallow Waters

You are provided with the following performance-critical C++ files:

- `shallow2d.h` -- an implementation of shallow water physics
- `minmod.h` -- a (possibly efficient)  MinMod limiter
- `central2d.h` -- a finite volume solver for 2D hyperbolic PDE

In addition, you are given the following codes for running the
simulation and getting pretty pictures out:

- `meshio.h` -- I/O routines
- `driver.cc` -- a driver file that runs the simuation
- `driver.cu` -- a CUDA driver file that runs the simuation
- `visualizer.py` -- Python visualization script

Finally, we have makefiles with recipes for the serial version (make shallow) and
the CUDA version (make shallow-cuda)

- `Makefile` -- Main makefile
- `Makefile.in.gcc` -- Makefile options for gcc
- `Makefile.in.nvcc` -- Makefile options for nvcc (the CUDA compiler)
- `Makefile.in.clang` -- Makefile options for clang


For this assignment, you should attempt three tasks:

1.  *Parallelization*: You should offload the code to a GPU via CUDA. 
    Set up both strong and weak scaling studies, varying the number of threads you
    employ.  You may start with a naive parallelization (e.g. parallelizing the for 
    loops in the various subroutines), but this is not likely to give you the best 
    possible performance.

1.  *Profiling*:  We've been using Intel's Vtune in class so far, but for 
    CUDA we will use nvprof, which is Nvidia's profiler for CUDA. You should
    use nvprof to identify bottlenecks in your code. 

3.  *Tuning*:  You should tune your code in order to get it to run as
    fast as possible.  This may involve a domain decomposition
    with per-processor ghost cells and batching of time steps between
    synchronization; it may involve vectorization of the computational
    kernels; or it may involve eliminating redundant computations in
    the current implementation. 

Note that you will have to change more than just `driver.cu`. At the very least, 
you will have to modify `central2d.h` and `shallow2d.h` to offload the physics
computation to the GPU. 

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

## The Math Behind Shallow Waters

You are welcome to read about the shallow water equations and about
finite volume methods if you would like, and this may help you in
understanding what you're seeing. 

But don't be intimidated by the physics or mathematics if you don't understand it! 
You should be able to tune the code without understanding all the physics. For 
a good reference, we recommend taking a look at: 
[optimizing a shallow water simulator][brodtkorb]
on Nvidia GPUs. Note that this paper covers more than we need for this class
(in particular, the multi-GPU and sparse cases, which we are not requiring
you to do). 

[brodtkorb]: http://cmwr2012.cee.illinois.edu/Papers/Special%20Sessions/Advances%20in%20Heterogeneous%20Computing%20for%20Water%20Resources/Brodtkorb.Andre_R.pdf

## CUDA References

CUDA is a very mature language for programming Nvidia GPUs. It's
primary open source competitor is OpenCL, which is a bit more complicated
to use, but ports to AMD GPUs as well. 

We did not cover CUDA in-depth during class. For implementation specifics, 
you will have to look up documentation yourself. Some good starting resources are

* A basic CUDA tutorial (highly recommended for anyone without experience) 
[here](https://devblogs.nvidia.com/parallelforall/even-easier-introduction-cuda/)
* If you prefer slides, a comporable tutorial is 
[here](http://www.nvidia.com/content/GTC-2010/pdfs/2131_GTC2010.pdf)
* The nvprof [documentation](http://docs.nvidia.com/cuda/profiler-users-guide/index.html)
can be a bit tricky to explore and parse, but is the best resource for learning how 
to use nvprof from the command line
* You will need to change all basic math functions (min, abs, etc) to their
CUDA equivalents. See the CUDA [Math API](http://docs.nvidia.com/cuda/cuda-math-api/index.html)
for precise details. 

Also, don't be too worried about achieving far-superior performance numbers with
perfect scaling! Most people in this class have no experience with CUDA, and we are
not expecting everyone be CUDA experts by the time this project is over. 
We only want people to learn the fundamentals, which includes

* The CUDA offloading model
* CUDA thread indexing
* CUDA loop parallelization
* CUDA memory managment
* Tuning with CUDA block sizes and nvprof

You should be able to write a fairly optimized code with just these. Finally, 
note that traditional I/O is not supported in the default CUDA libraries; 
to write to files, you will have to copy data on the GPU back to the GPU. 
Repeated communication between the CPU and GPU across the PCI-Express bus 
will become the bottleneck if you are  doing it at every step. For the 
purposes of this assignment, you do not to perform I/O other than to verify 
that your implementation is still correct. 

## CUDA in C or C++
Basic CUDA does not natively support offloading to C++ vectors, which is what
the base code uses. You have a few choices for writing your parallel implementation

* Pure C++: keep the C++ implementation, using the CUDA C++ Library 
[Thrust](https://developer.nvidia.com/thrust)
 to allocate and manipulate C++ vectors. 
 Thrust is quite easy to use and supports most C++ vector operations. 
 See the Thrust [tutorial](http://www.mariomulansky.de/data/uploads/cuda_thrust.pdf) and 
 [documentation](http://docs.nvidia.com/cuda/thrust/index.html) for details. 
 
* Pure C: Change the C++ implementation to C, and standard CUDA operations. 
In the base code, each vector entry is a 3-tuple of floating point numbers. 
Therefore, the most straightforward (but messy!) method to convert everything to
C is to expand each vector into three different floating point arrays. 

* Something in between: use the base C++ implementation but use standard 
(non-C++, non-Thrust) CUDA operations. One idea is to maintain a set of 
floating point buffers, and using these buffers to copy data between your C++ vectors and 
GPU-allocated CUDA floating point arrays. 

There is no correct way of doing things. Make sure to fully understand the base code 
and read CUDA documentation before deciding. 
## Notes on the documentation

The documentation for this project is generated automatically from
structured comments in the source files using a simple tool called
[ldoc][ldoc] that Prof. Bindel wrote some years ago.  You may or may not choose
to use [ldoc][ldoc] for your version.

[ldoc]: https://github.com/dbindel/ldoc

## Notes on C++ usage

The reference code given to you is in C++. Prof. Bindel wanted to use C, but
was persuaded he could write a clearer, cleaner implementation in
C++ -- and an implementation that will ultimately be easier for you to
tune.

While we have tried not to do anything too obscure, this code does use
some C++ 11 features (e.g. the `constexpr` notation used to tell the
compiler something is a compile-time constant).  If you want to build
on your own machine, you may need to figure out the flag needed to
tell your compiler that you are using this C++ dialect.
