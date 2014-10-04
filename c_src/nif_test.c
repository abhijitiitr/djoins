#include <stdio.h>
#include <errno.h>

#include "erl_nif.h"

#define UNUSED(p) (void)(p)

// -- --

typedef struct {
  int id;
} nif_t;

// -- --

static ERL_NIF_TERM open(ErlNifEnv *env, int argc, const ERL_NIF_TERM *argv) {

  ErlNifResourceType *resource_type = (ErlNifResourceType *)enif_priv_data(env);

  if (1 == argc &&
      enif_is_number(env, argv[0])) {

    int id = 0;
    enif_get_int(env, argv[0], &id);

    nif_t *resource = (nif_t *)enif_alloc_resource(resource_type, sizeof(nif_t));

    if (NULL != resource) {

      resource->id = id;

      printf("open, resource=%ld, id=%d\r\n", (long)resource, resource->id);

      return enif_make_tuple2(env,
                              enif_make_atom(env, "ok"),
                              enif_make_resource(env, resource));
    } else {

      return enif_make_tuple2(env,
                              enif_make_atom(env, "error"),
                              enif_make_string(env, "enomem", ERL_NIF_LATIN1));
    }
  }

  return enif_make_badarg(env);
}

static ERL_NIF_TERM close(ErlNifEnv *env, int argc, const ERL_NIF_TERM *argv) {

  ErlNifResourceType *resource_type = (ErlNifResourceType *)enif_priv_data(env);

  if (1 == argc) {

    nif_t *resource = NULL;

    if (enif_get_resource(env, argv[0], resource_type, (void **)&resource)) {

      printf("close, resource=%ld, id=%d\r\n", (long)resource, resource->id);

      enif_release_resource(resource);
      resource = NULL;

      return enif_make_atom(env, "ok");
    }
  }

  return enif_make_badarg(env);
}

// -- --

void dtor(ErlNifEnv* env, void* obj) {

  UNUSED(env);

  nif_t *resource = (nif_t *)obj;

  printf("dtor, resource=%ld, id=%d\r\n", (long)resource, resource->id);
}

// -- --

static ErlNifFunc funcs[] = {
  {"open", 1, open},
  {"close", 1, close},
};

static int load(ErlNifEnv *env, void **priv_data, ERL_NIF_TERM load_info) {

  UNUSED(env), UNUSED(load_info);

  ErlNifResourceType *resource_type =
    enif_open_resource_type(env, NULL, "nif_t", dtor, ERL_NIF_RT_CREATE, NULL);

  if (NULL == resource_type) {
    return ENOMEM;
  }

  *priv_data = resource_type;

  return 0;
}

/*
 */
ERL_NIF_INIT(nif_test, funcs, load, NULL, NULL, NULL);