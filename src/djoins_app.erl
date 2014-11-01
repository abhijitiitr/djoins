%%%-------------------------------------------------------------------
%%% @author Abhijit Pratap Singh <singh.abhijitpratap@gmail.com>
%%% @copyright (C) 2014, Abhijit Pratap Singh
%%% @doc
%%%
%%% @end
%%% Created : 31 October 2014 by Abhijit Pratap Singh <singh.abhijitpratap@gmail.com>
%%%-------------------------------------------------------------------
-module(djoins_app).

-behaviour(application).

%% Application callbacks
-export([start/2, stop/1]).

%% ===================================================================
%% Application callbacks
%% ===================================================================

start(_StartType, _StartArgs) ->
    djoins_sup:start_link().

stop(_State) ->
    ok.
