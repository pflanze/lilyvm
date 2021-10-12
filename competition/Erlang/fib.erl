-module(fib).
-export([fib/1]).

fib(N) ->
   if
        N < 2 -> 1;
        true  -> fib(N - 1) + fib(N - 2)
   end.

