% c(par_append_tuple).
%%Pid=spawn(query_distributor,spawn_concurrent_append,[{ok,[]}]).
%% Pid ! {self(), {data,<<"1">>}}.
-module(query_distributor).
-export([spawn_concurrent_append/1]).

spawn_concurrent_append({Pid, List}) ->
	Sql = <<"abc">>,
	receive
		{From, {data, Rows}} ->
			Ref = par_append_tuple:initialize_result_set(2),
			R1 = [{<<"23">>,<<"twentythree">>,<<"dcjcjd">>},{<<"12">>,<<"twelve">>,<<"dcjcjd">>},{<<"14">>,<<"fourteen">>,<<"dcjcjd">>}],
 			R2 = [{<<"45">>,<<"fortyfive">>,<<"dcjcjd">>,<<"dchdfhcd">>},{<<"12">>,<<"twelvesecond">>,<<"dcdbjbvjcjd">>,<<"dchdfhcd">>},{<<"23">>,<<"twentythreesecond">>,<<"dcjcjddbcjbd">>,<<"dchdfhcd">>}],
			Pid1 = spawn(result_collector, spawn_concurrent_append, [Sql]),
			Pid2 = spawn(result_collector, spawn_concurrent_append, [Sql]),
			NewList = [Pid1 | [Pid2]],
			Pid1 ! {self(), {data, Ref, R1, 1}},
			Pid2 ! {self(), {data, Ref, R2, 2}},
			spawn_concurrent_append({From, NewList});
		{result, ResultBody} -> 
			Pid ! {self(), {result, ResultBody}},
			lists:foreach(fun(Int_Pid) ->
				Int_Pid ! {ok, finished}
              end, List)

	end.



