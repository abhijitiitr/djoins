% c(query_distributor).
% c(result_collector).
% c(par_append_tuple).
% Pid=spawn(query_distributor,spawn_concurrent_append,[{ok,[]}]).
% Pid ! {self(), {data,<<"1">>}}.
-module(query_distributor).
-export([spawn_concurrent_append/1]).

spawn_concurrent_append({Pid, List}) ->
	Sql = <<"abc">>,
	receive
		{From, {data, Rows}} ->
			Ref = par_append_tuple:initialize_result_set(3),
			R1 = [{<<"23">>,<<"twentythree">>,<<"dcjcjd">>},{<<"12">>,<<"twelve">>,<<"dcjcjd">>},{<<"14">>,<<"fourteen">>,<<"dcjcjd">>}],
 			R2 = [{<<"45">>,<<"fortyfive">>,<<"dcjcjd">>,<<"dchdfhcd">>},{<<"12">>,<<"twelvesecond">>,<<"dcdbjbvjcjd">>,<<"dchdfhcd">>},{<<"23">>,<<"twentythreesecond">>,<<"dcjcjddbcjbd">>,<<"dchdfhcd">>}],
 			R3 = [{<<"12">>,<<"twelvethird">>,<<"zbcsbcds">>,<<"yuwyewjeh">>,<<"cbdjbcdjd">>},{<<"14">>,<<"fourteen">>,<<"notfourteen">>,<<"yesfourteen">>,<<"trollfourteen">>}],
			Pid1 = spawn(result_collector, spawn_concurrent_append, [Sql]),
			Pid2 = spawn(result_collector, spawn_concurrent_append, [Sql]),
			Pid3 = spawn(result_collector, spawn_concurrent_append, [Sql]),
			Cols1 = [<<"id">>,<<"comment1">>,<<"created_at1">>],
			Cols2 = [<<"id">>,<<"comment2">>,<<"created_at2">>,<<"updated_at2">>],
			Cols3 = [<<"id">>,<<"comment3">>,<<"created_at3">>,<<"updated_at3">>,<<"extra_column">>],
			NewList = [Pid1 | [Pid2 | [Pid3]]],
			Pid1 ! {self(), {data, Ref, R1, Cols1, 1}},
			Pid2 ! {self(), {data, Ref, R2, Cols2, 2}},
			Pid3 ! {self(), {data, Ref, R3, Cols3, 3}},
			spawn_concurrent_append({From, NewList});
		{result, ResultBody} -> 
			Pid ! {self(), {result, ResultBody}},
			lists:foreach(fun(Int_Pid) ->
				Int_Pid ! {ok, finished}
              end, List)
	end.



