#include "stepper.h"
#include "shallow2d.h"
#include "minmod.h"
#include "meshio.h"

#include <string>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <unistd.h>

#ifdef _OPENMP
#include <omp.h>
#elif defined SYSTIME
#include <sys/time.h>
#endif

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

//ldoc on
/**
 * # Driver code
 *
 * The driver code is where we put together the time stepper and
 * the physics routines to actually solve the equations and make
 * pretty pictures of the solutions.
 *
 * For the driver, we need to put everything together: we're running
 * a `Central2D` solver for the `Shallow2D` physics with a `MinMod`
 * limiter:
 */

typedef Central2D< Shallow2D, MinMod<Shallow2D::real> > Sim;

/**
 * ## Lua driver routines
 *
 * A better way to manage simulation parameters is by a scripting
 * language.  Python is a popular choice, but I prefer Lua for many
 * things (not least because it is an easy build).  It's also quite
 * cheap to call a Lua function for every point in a mesh
 * (less so for Python, though it probably won't make much difference).
 *
 * ### Lua helpers
 *
 * We want to be able to get numbers and strings with a default value
 * when nothing is specified.  Lua 5.3 has this as a built-in, I think,
 * but the following codes are taken from earlier versions of Lua.
 */

double lget_number(lua_State* L, const char* name, double x)
{
    lua_getfield(L, 1, name);
    if (lua_type(L, -1) != LUA_TNIL) {
        if (lua_type(L, -1) != LUA_TNUMBER)
            luaL_error(L, "Expected %s to be a number", name);
        x = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return x;
}


int lget_int(lua_State* L, const char* name, int x)
{
    lua_getfield(L, 1, name);
    if (lua_type(L, -1) != LUA_TNIL) {
        if (lua_type(L, -1) != LUA_TNUMBER)
            luaL_error(L, "Expected %s to be a number", name);
        x = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);
    return x;
}


const char* lget_string(lua_State* L, const char* name, const char* x)
{
    lua_getfield(L, 1, name);
    if (lua_type(L, -1) != LUA_TNIL) {
        if (lua_type(L, -1) != LUA_TSTRING)
            luaL_error(L, "Expected %s to be a string", name);
        x = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    return x;
}


/**
 * ### Lua callback functions
 *
 * We specify the initial conditions by providing the simulator
 * with a callback function to be called at each cell center.
 * The callback function is assumed to be the `init` field of
 * a table at index 1.
 */

class LuaInit {
public:
    LuaInit(lua_State* L) : L(L) {}

    void push_init() {
        lua_getfield(L, 1, "init");
        if (lua_type(L, -1) != LUA_TFUNCTION)
            luaL_error(L, "Expected init to be a string");
    }
    
    void operator()(Sim::vec& u, double x, double y) {
        lua_pushvalue(L, -1);
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        int nfield = u.size();
        lua_call(L, 2, nfield);
        for (int k = 0; k < nfield; ++k)
            u[k] = lua_tonumber(L, k-nfield);
        lua_pop(L, nfield);
    }
    
private:
    lua_State* L;
};


/**
 * ### Running the simulation
 *
 * The `run_sim` function looks a lot like the main routine of the
 * "ordinary" command line driver.  We specify the initial conditions
 * by providing the simulator with a callback function to be called at
 * each cell center.  Note that we have two different options for
 * timing the steps -- we can use the OpenMP timing routines
 * (preferable if OpenMP is available) or the POSIX `gettimeofday`
 * if the `SYSTIME` macro is defined.  If there's no OpenMP and
 * `SYSTIME` is undefined, we fall back to just printing the number
 * of steps without timing information.
 */

int run_sim(lua_State* L)
{
    int n = lua_gettop(L);
    if (n != 1 || !lua_istable(L, 1))
        luaL_error(L, "Argument must be a table");

    double w = lget_number(L, "w", 2.0);
    double h = lget_number(L, "h", w);
    double cfl = lget_number(L, "cfl", 0.45);
    double ftime = lget_number(L, "ftime", 0.01);
    int nx = lget_int(L, "nx", 200);
    int ny = lget_int(L, "ny", nx);
    int frames = lget_int(L, "frames", 50);
    const char* fname = lget_string(L, "out", "sim.out");

    Sim sim(w, h, nx, ny);
    
    LuaInit initf(L);
    initf.push_init();
    sim.init(initf);
    lua_pop(L,1);
    
    printf("%g %g %d %d %g %d %g\n", w, h, nx, ny, cfl, frames, ftime);
    SimViz<Sim> viz(fname, sim);
    sim.solution_check();
    viz.write_frame();

    double tcompute = 0;
    for (int i = 0; i < frames; ++i) {
#ifdef _OPENMP
        double t0 = omp_get_wtime();
        int nstep = sim.run(ftime);
        double t1 = omp_get_wtime();
        double elapsed = t1-t0;
#elif defined SYSTIME
        struct timeval t0, t1;
        gettimeofday(&t0, NULL);
        int nstep = sim.run(ftime);
        gettimeofday(&t1, NULL);
        double elapsed = (t1.tv_sec-t0.tv_sec) + (t1.tv_usec-t0.tv_usec)*1e-6;
#else
        int nstep = sim.run(ftime);
        double elapsed = 0;
#endif
        sim.solution_check();
        tcompute += elapsed;
        printf("  Time: %e (%e for %d steps)\n", elapsed, elapsed/nstep, nstep);
        viz.write_frame();
    }
    printf("Total compute time: %e\n", tcompute);

    return 0;
}


/**
 * ### Main
 *
 * The main routine has the usage pattern
 *
 *     lshallow tests.lua args
 *
 * where `tests.lua` has a call to the `simulate` function to run
 * the simulation.  The arguments after the Lua file name are passed
 * into the Lua script via a global array called `args`.
 */

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s fname args\n", argv[0]);
        return -1;
    }

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lua_register(L, "simulate", run_sim);

    lua_newtable(L);
    for (int i = 2; i < argc; ++i) {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, 1, i-1);
    }
    lua_setglobal(L, "args");

    if (luaL_dofile(L, argv[1]))
        printf("%s\n", lua_tostring(L,-1));
    lua_close(L);
    return 0;
}
