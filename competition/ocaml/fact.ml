

let rec fact n =
  if n < 2 then 1 else n * fact (n - 1);;

let n_str = ref "<missing argument>"

let anon_fun str =
  n_str := str

let () =
  Arg.parse [] anon_fun "fact <n>";
  let n = (int_of_string !n_str) in
  print_int (fact n);
  print_string "\n"
  ;;
