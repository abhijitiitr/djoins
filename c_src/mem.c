 
/* gcc -fPIC -shared -o mem.so mem.c -I /usr/local/lib/erlang/usr/include */
 
#include <stdio.h>
#include <string.h>
#include "erl_nif.h"
 
 
static ERL_NIF_TERM atom_ok;
static ERL_NIF_TERM atom_error;
 
static ErlNifResourceType *MEM_RESOURCE;
 
void cleanup(ErlNifEnv *env, void *p);
 
 
    static int
load(ErlNifEnv *env, void **priv_data, ERL_NIF_TERM load_info)
{
    atom_ok = enif_make_atom(env, "ok");
    atom_error = enif_make_atom(env, "error");
 
    if ( (MEM_RESOURCE = enif_open_resource_type(env, NULL,
            "mem_resource", cleanup,
            ERL_NIF_RT_CREATE, NULL)) == NULL)
        return -1;
 
    return 0;
}
 
/* 0: size_t */
    static ERL_NIF_TERM
nif_malloc(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    size_t size = 0;
    void **p = NULL;
    ERL_NIF_TERM res = {0};
 
    if (!enif_get_ulong(env, argv[0], (ulong *)&size))
        return enif_make_badarg(env);
 
    p = enif_alloc_resource(MEM_RESOURCE, sizeof(char *));
 
    if (p == NULL)
        return atom_error;
 
    *p = (char *)malloc(size);
 
    if (*p == NULL) {
        enif_release_resource(p);
        return atom_error;
    }
 
    (void)fprintf(stderr, "alloc: p=%p/%p\n", *p, p);
 
    res = enif_make_resource(env, p);
    enif_release_resource(p);
 
    return enif_make_tuple(env, 2,
        atom_ok,
        res);
}
 
/* 0: void* */
    static ERL_NIF_TERM
nif_free(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    void **p = NULL;
 
    if (!enif_get_resource(env, argv[0], MEM_RESOURCE, (void **)&p))
        return enif_make_badarg(env);
 
    (void)fprintf(stderr, "free: p=%p/%p\n", *p, p);
 
    free(*p);
    *p = NULL;
 
    return atom_ok;
}
 
    void
cleanup(ErlNifEnv *env, void *obj)
{
    void **p = obj;
    (void)fprintf(stderr, "cleanup: *p=%p/%p\n", *p, p);
    if (*p)
        free(*p);
}
 
 
static ErlNifFunc nif_funcs[] = {
    {"alloc", 1, nif_malloc},
    {"free", 1, nif_free}
};
 
ERL_NIF_INIT(mem, nif_funcs, load, NULL, NULL, NULL)