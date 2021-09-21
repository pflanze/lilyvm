

let rec fib n =
  if n < 2 then 1 else fib (n - 1) + fib (n - 2);;

let n_str = ref "<missing argument>"

let anon_fun str =
  n_str := str

let () =
  Arg.parse [] anon_fun "fib <n>";
  let n = (int_of_string !n_str) in
  print_int (fib n);
  print_string "\n"
  ;;