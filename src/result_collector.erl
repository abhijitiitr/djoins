-module(result_collector).
-export([spawn_concurrent_append/1]).

spawn_concurrent_append(Sql) ->
	receive
		{From, {data, Ref, Rows, Uid}} ->
			io:format("~p\n",[From]),

			Result = par_append_tuple:append_result_set(Ref, Rows, Uid),
			case Result of
				{result, ResultBody}  ->
					From ! {result, ResultBody};
				_ -> 
				    ok
			end,
			spawn_concurrent_append(Sql);
		{ok, finished} -> 
			ok
	end.



