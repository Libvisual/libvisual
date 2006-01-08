open Complex

let pi = 3.1415926536
let complex_re x = { re = x; im = 0.0 }
let complex_im x = { re = 0.0; im = x }
let float = float_of_int
        
(* Brute force DFT implementation *)

let dft input =
  let n = Array.length input in
  let output = Array.make n zero in

  let rec sum j k =
    if (k < n) then 
      add (mul (complex_re input.(k)) 
               (exp (complex_im (2.0 *. pi *. (float j) *. (float k) /. (float n)))))
          (sum j (k + 1))
    else
      zero
  in

  for j = 0 to n - 1 do
    output.(j) <- sum j 0
  done;

  output
;;

(* Implementation of Cooley Tukey FFT (radix-2 DIT) *)

let rec permutate j m =
  if m >= 1 && j >= m then
    permutate (j - m) (m / 2)
  else
    j + m
;;

let bit_reverse_copy input output =
  let size = Array.length input in

  let rec copy i j =
    if i < size then
      begin
        output.(i) <- complex_re input.(j);
        copy (i + 1) (permutate j (size / 2))
      end
  in

  copy 0 0
;;

let fft input =
  let size = Array.length input in
  let output = Array.make size zero in

  let rec transform dft_size =
    let wp = exp (complex_im (2.0 *. pi /. (float_of_int dft_size))) in

    let rec outer m w =
      let rec inner i =
        if i < size then
          begin
            let j = i + dft_size / 2 in
            let term = mul w output.(j) in
            
            output.(j) <- sub output.(i) term;
            output.(i) <- add output.(i) term;

            inner (i + dft_size)
          end
      in

      if m < dft_size / 2 then
        begin
          inner m;
          outer (m + 1) (mul w wp)
        end
    in

    if dft_size <= size then
      begin
        outer 0 one;
        transform (dft_size * 2)
      end
  in

  bit_reverse_copy input output;
  transform 2;

  output
;;
