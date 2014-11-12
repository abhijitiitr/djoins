% For testing purposes please compile the required modules and run the following instructions
% c(par_append_tuple).
% Ref=par_append_tuple:initialize_result_set(2).
% R1=[{<<"23">>,<<"twentythree">>,<<"dcjcjd">>},{<<"12">>,<<"twelve">>,<<"dcjcjd">>},{<<"14">>,<<"fourteen">>,<<"dcjcjd">>}].
% R2=[{<<"45">>,<<"fortyfive">>,<<"dcjcjd">>,<<"dchdfhcd">>},{<<"12">>,<<"twelvesecond">>,<<"dcdbjbvjcjd">>,<<"dchdfhcd">>},{<<"23">>,<<"twentythreesecond">>,<<"dcjcjddbcjbd">>,<<"dchdfhcd">>}].
% par_append_tuple:append_result_set(Ref,R1,1).
% par_append_tuple:append_result_set(Ref,R2,2).
% par_append_tuple:return_result_set(Ref).
% par_append_tuple:return_result_set(Ref).



% c(par_append_tuple).
% Ref=par_append_tuple:initialize_result_set(2).
% R1=[{<<"23">>,<<"twentythree">>,<<"dcjcjd">>},{<<"12">>,<<"twelve">>,<<"dcjcjd">>},{<<"14">>,<<"fourteen">>,<<"dcjcjd">>}].
% R2=[{<<"45">>,<<"fortyfive">>,<<"dcjcjd">>,<<"dchdfhcd">>},{<<"12">>,<<"twelvesecond">>,<<"dcdbjbvjcjd">>,<<"dchdfhcd">>},{<<"23">>,<<"twentythreesecond">>,<<"dcjcjddbcjbd">>,<<"dchdfhcd">>}].
% Pid=spawn(par_append_tuple,spawn_concurrent_append,[]).
% Pid ! {self(), {data, Ref, R1, 1}}.  
% Pid ! {self(), {data, Ref, R2, 2}}.  

-module(par_append_tuple).
-export([initialize_result_set/1,return_result_set/1,append_result_set/4]).
-on_load(init/0).

init() ->
    ok = erlang:load_nif("./par_append_tuple", 0).

initialize_result_set(_Count) ->
    exit(nif_library_not_loaded).

return_result_set(_Ref) ->
    exit(nif_library_not_loaded).

append_result_set(_Ref, _Rows, _Cols, _Uid) ->
	exit(nif_library_not_loaded).



