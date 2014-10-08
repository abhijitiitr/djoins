-module(append_tuple).
-export([initialize_result_set/1,return_result_set/1,append_result_set/2]).
-on_load(init/0).

init() ->
    ok = erlang:load_nif("./append_tuple", 0).

initialize_result_set(_T) ->
    exit(nif_library_not_loaded).

return_result_set(_T) ->
    exit(nif_library_not_loaded).

append_result_set(_R,_E) ->
	exit(nif_library_not_loaded).