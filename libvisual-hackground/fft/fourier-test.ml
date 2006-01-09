open Printf
open Complex

let rec count_bits n =
  if n > 0 then
    count_bits (n lsr 1) + (n land 1)
  else
    0
;;

let is_power2 n = count_bits n == 1;;

let parse_args () =
  let append input list =
    float_of_string input :: list
  in

  let args = Array.sub Sys.argv 1 ((Array.length Sys.argv) - 1) in

  Array.of_list (Array.fold_right append args [])
;;

let main () =
  try
    let input = parse_args () in
    let input_size = Array.length input in

    if input_size > 0 then
      begin
        printf "Input: ";
        Array.iter (fun x -> printf "%.3f " x) input;
        printf "\n";
        
        let output = Fourier.dft input in
        printf "Output: ";
        Array.iter (fun x -> printf "%.3f " (norm x)) output;
        printf "\n";

        if is_power2 input_size then
          begin
            let output = Fourier.fft input in
            printf "Output (FFT): ";
            Array.iter (fun x -> printf "%.3f " (norm x)) output;
            printf "\n";
          end
      end
    else
      printf "Usage: %s input1 input2 .. inputN\n" Sys.argv.(0)

  with
    error -> printf "Exception caught: %s\n" (Printexc.to_string error)
;;

let _ =
  main ()
