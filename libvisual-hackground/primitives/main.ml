open Printf
open Point
open Line
open Triangle

let screen_width = 1024;;
let screen_height = 768;;

(* draws a rectangle (clipped) *)
let draw_rect surface rect color =
  let r_x0 = rect.Sdlvideo.r_x in
  let r_y0 = rect.Sdlvideo.r_y in
  let r_x1 = r_x0 + rect.Sdlvideo.r_w - 1 in
  let r_y1 = r_y0 + rect.Sdlvideo.r_h - 1 in

  draw_line surface {x = r_x0; y = r_y0} {x = r_x1; y = r_y0} color;
  draw_line surface {x = r_x0; y = r_y1} {x = r_x1; y = r_y1} color;
  draw_line surface {x = r_x0; y = r_y0} {x = r_x0; y = r_y1} color;
  draw_line surface {x = r_x1; y = r_y0} {x = r_x1; y = r_y1} color;
;;

(* returns a random point in a given surface *)
let random_pos_out surface = 
  let width, height, _ = Sdlvideo.surface_dims surface in
  { x = (Random.int width) * 2 - width/2; 
    y = (Random.int height) * 2 - height/2 }
;;

(* returns a random point in a given surface *)
let random_pos surface = 
  let width, height, _ = Sdlvideo.surface_dims surface in
  { x = (Random.int width); y = (Random.int height) }
;;
    
(* returns a random colour *)
let random_color () = 
  (Random.int 256), (Random.int 256), (Random.int 256)
;;

(* runs a function for a number of times and computes its average running time *)
let benchmark count func =
  let rec loop count  =
    if count > 0 then
      begin
        func ();
        loop (count - 1)
      end
  in

  let start_time = Sdltimer.get_ticks () in
  loop count;

  let time_taken = Sdltimer.get_ticks () - start_time in
  let rate = (float_of_int (count * 1000)) /. (float_of_int time_taken) in
  
  rate
;;

(* line demo *)
let line_demo () =
  let count = 10000 in

  let screen = Sdlvideo.get_video_surface () in
  let random_pos () = random_pos_out screen in
  let draw_line () = draw_line screen (random_pos ()) (random_pos ()) (random_color ()) in

  let rate = benchmark count draw_line in
  Sdlvideo.flip screen;

  printf "Lines per second: %f\n" rate
;;

(* solid triangle demo *)
let solid_triangle_demo () =
  let count = 1000 in

  let screen = Sdlvideo.get_video_surface () in
  let random_pos () = random_pos screen in
  let draw_triangle () = draw_triangle screen (random_pos ()) (random_pos ()) (random_pos ()) (random_color ()) in

  let rate = benchmark count draw_triangle in
  Sdlvideo.flip screen;

  printf "Triangles per second: %f\n" rate
;;

(* texture mapped triangle demo *)
let texture_triangle_demo () =
  try
    let texture = Sdlloader.load_image "picture.jpg" in
    let count = 1000 in

    let screen = Sdlvideo.get_video_surface () in
    let random_pos () = random_pos screen in

    let draw_triangle () = 
      draw_texture_triangle screen 
        (random_pos (), { x = 0.; y = 0. })
        (random_pos (), { x = 1.; y = 0. })
        (random_pos (), { x = 0.; y = 1. })
        (random_color ()) in
    
    let rate = benchmark count draw_triangle in
    Sdlvideo.flip screen;
    
    printf "Triangles per second: %f\n" rate

  with
    Sdlloader.SDLloader_exception error ->
      eprintf "Error loading texture: %s\n" error
;;


(* waits for a keypress *)
let rec wait_for_keypress () =
  let event = Sdlevent.poll () in
  let finish = match event with
  | None -> false
  | Some event -> 
      match event with
      | Sdlevent.KEYDOWN _ -> true
      | _ -> false
  in

  if not finish then
    wait_for_keypress ()
;;

(* demo *)
let demo () =
  Random.self_init ();

  line_demo ();
  wait_for_keypress ();

  solid_triangle_demo ();
  wait_for_keypress ();

  texture_triangle_demo ();
  wait_for_keypress ()
;;

(* main *)
let main () =
  try
    Sdl.init [`VIDEO; `EVENTTHREAD; `TIMER];

    ignore (Sdlvideo.set_video_mode ~w:screen_width ~h:screen_height []);
    Sdlwm.set_caption ~title:"Graphics demo" ~icon:"";

    demo ();

    Sdl.quit ();

  with
  | Sdl.SDL_init_exception error
      -> eprintf "Fail to initialize SDL: %s\n" error
  | Sdlvideo.Video_exn error
      -> eprintf "Video error: %s\n" error
  | Sdlevent.Event_exn error
      -> eprintf "Event error: %s\n" error
;;

let _ = main ();;
