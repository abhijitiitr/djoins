
%%%-------------------------------------------------------------------
%%% @author Abhijit Pratap Singh <singh.abhijitpratap@gmail.com>
%%% @copyright (C) 2014, Abhijit Pratap Singh
%%% @doc
%%%
%%% @end
%%% Created : 01 Nov 2014 by Abhijit Pratap Singh <singh.abhijitpratap@gmail.com>
%%%-------------------------------------------------------------------
-module(djoins_db_query).

-export([open/1
        ,close/1
        ,get_connection/1
        ,return_connection/2
        ,query/1]).

-spec get_connection(atom()) -> {ok, pid()} | {error, timeout}.
get_connection(Pool) ->    
    case episcina:get_connection(Pool, 100) of
        {ok, C} ->
            {ok, C};
        {error, timeout} ->
            {error, timeout}
    end.

-spec return_connection(atom(), pid()) -> ok.
return_connection(Pool, Pid) ->
    episcina:return_connection(Pool, Pid).

-spec open(list()) -> {ok, pid()} | {error, undefined}.
open(DBArgs) ->  
    case DBArgs of
        [{host, Host}, {database, DBName}, {port, Port}, {user, Username},{password, Password}]->
            pgsql:connect(Host, [Username], [], [{database, DBName}, {port, Port}]);
        _ -> 
            {error, undefined}
    end.  

-spec close(pid()) -> ok.
close(Pid) ->
    epgsql:close(Pid).

-spec query(string()) -> tuple().
query(Query) ->
        % {ok, C} = get_connection(primary),
    case get_connection(primary) of
        {ok, C} ->
            try
                {ok, Columns, Rows} = pgsql:squery(C, Query)
            after
                return_connection(primary, C)
            end;
        _ ->
            {error, undefined}
    end.
