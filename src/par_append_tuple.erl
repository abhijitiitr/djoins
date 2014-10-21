%%%%%%

%%%%% TEST SUITE IN ERLANG REPL
% c(par_append_tuple).
% Ref=par_append_tuple:initialize_result_set(2).
% R1=[{<<"23">>,<<"twentythree">>,<<"dcjcjd">>},{<<"12">>,<<"twelve">>,<<"dcjcjd">>},{<<"14">>,<<"fourteen">>,<<"dcjcjd">>}].
% R2=[{<<"45">>,<<"fortyfive">>,<<"dcjcjd">>,<<"dchdfhcd">>},{<<"12">>,<<"twelvesecond">>,<<"dcdbjbvjcjd">>,<<"dchdfhcd">>},{<<"23">>,<<"twentythreesecond">>,<<"dcjcjddbcjbd">>,<<"dchdfhcd">>}].
% par_append_tuple:append_result_set(Ref,R1,1).
% par_append_tuple:append_result_set(Ref,R2,2).
% par_append_tuple:return_result_set(Ref).
% par_append_tuple:return_result_set(Ref).

-module(par_append_tuple).
-export([initialize_result_set/1,return_result_set/1,append_result_set/3]).
-on_load(init/0).

init() ->
    ok = erlang:load_nif("./par_append_tuple", 0).

initialize_result_set(_T) ->
    exit(nif_library_not_loaded).

return_result_set(_T) ->
    exit(nif_library_not_loaded).

append_result_set(_R,_E,_U) ->
	exit(nif_library_not_loaded).

