open Printf
open Complex

let main () =
  let input = [| 0.0; 2.0; 1.0; 1.0; 1.0; 1.0; 0.0; 0.0 |] in
  let output = Fourier.dft input in

  Array.iter (fun x -> printf "%.3f " (norm x)) output;
  printf "\n";

  let output = Fourier.fft input in

  Array.iter (fun x -> printf "%.3f " (norm x)) output;
  printf "\n"
;;

let _ =
  main ()
