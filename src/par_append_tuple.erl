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
-export([initialize_result_set/1,return_result_set/1,append_result_set/3,spawn_concurrent_append/1,spawn_core/0]).
-on_load(init/0).

init() ->
    ok = erlang:load_nif("./par_append_tuple", 0).

initialize_result_set(_T) ->
    exit(nif_library_not_loaded).

return_result_set(_T) ->
    exit(nif_library_not_loaded).

append_result_set(_R,_E,_U) ->
	exit(nif_library_not_loaded).

spawn_concurrent_append(List) ->
	receive
		{From, {data, Ref, Rows, Uid}} ->
			Pid = spawn(?MODULE, spawn_core, []),
			Pid ! {self(), {data_core, Ref, Rows, Uid, From}},
			NewList = [Pid | [List]],
			spawn_concurrent_append(NewList);
		{result, ok} -> 
			lists:foreach(fun(Pid) ->
				Pid ! {self(), {completed, ok}}
              end, List)

	end.

spawn_core() ->
	receive
		{From, {data_core, Ref, Rows, Uid, OrigPid}} -> 
			Result = append_result_set(Ref, Rows, Uid),
			case Result of
				{result, ResultBody}  ->
					OrigPid ! {self(), {result, ResultBody}},
					From ! {result, ok};
				_ -> 
				    ok
			end,
			spawn_core();
		{completed, ok} ->
			ok
	end.



