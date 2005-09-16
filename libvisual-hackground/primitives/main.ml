open Printf

type 'a point = { x: 'a; y: 'a };;
type ipoint = int point;;

let screen_width = 1024;;
let screen_height = 768;;

let put_pixel_color surface x y color =
  (* comment this out to see how slow Sdlvideo.put_pixel_color is *)
  Sdlvideo.put_pixel_color surface x y color
;;

(* computes the outcode of a point *)
let outcode bound p =
  let above_below = 
    if p.y < bound.Sdlvideo.r_y then 8 
    else if p.y > (bound.Sdlvideo.r_y + bound.Sdlvideo.r_h - 1) then 4
    else 0
  in

  let right_left = 
    if p.x < bound.Sdlvideo.r_x then 1
    else if p.x > (bound.Sdlvideo.r_x + bound.Sdlvideo.r_w - 1) then 2
    else 0
  in
  
  above_below lor right_left
;;

(* clips a pair of endpoints representing a line segment against
   a horizontal line *)
let intersect_horizontal p0 p1 y =
  { x = p0.x + ((p1.x - p0.x) * (y - p0.y)) / (p1.y - p0.y); y = y }
;;
  
(* clips a pair of endpoints representing a line segment against
   a vertical line *)
let intersect_vertical p0 p1 x = 
  { x = x; y = p0.y + ((p1.y - p0.y) * (x - p0.x)) / (p1.x - p0.x) }
;;

(* clips a pair of endpoints representing a line segment against a
   given rectangle *)
let clip_line bound p0 p1 =
  let outcode = outcode bound in
  let contains p = (outcode p) == 0 in

  let r_x0 = bound.Sdlvideo.r_x in
  let r_y0 = bound.Sdlvideo.r_y in
  let r_x1 = r_x0 + bound.Sdlvideo.r_w - 1 in
  let r_y1 = r_y0 + bound.Sdlvideo.r_h - 1 in

  let clip_above (p0, p1) =
    if (p0.y < r_y0 or p1.y < r_y0) then
      let p = intersect_horizontal p0 p1 r_y0 in
      if p0.y > r_y0 then p0, p else p, p1
    else
      p0, p1
  in

  let clip_below (p0, p1) =
    if (p0.y > r_y1 or p1.y > r_y1) then
      let p = intersect_horizontal p0 p1 r_y1 in
      if p0.y < r_y1 then p0, p else p, p1
    else
      p0, p1
  in
  
  let clip_right (p0, p1) =
    if (p0.x > r_x1 or p1.x > r_x1) then
      let p = intersect_vertical p0 p1 r_x1 in
      if p0.x < r_x1 then p0, p else p, p1
    else
      p0, p1
  in

  let clip_left (p0, p1) =
    if (p0.x < r_x0 or p1.x < r_x0) then
      let p = intersect_vertical p0 p1 r_x0 in
      if p0.x > r_x0 then p0, p else p, p1
    else
      p0, p1
  in

  let clip_left_right (p0, p1) =
    if (p0.x <> p1.x) then clip_right (clip_left (p0, p1)) else p0, p1
  in

  let clip_above_below (p0, p1) =
    if (p0.y <> p1.y) then clip_below (clip_above (p0, p1)) else p0, p1
  in

  if (outcode p0) land (outcode p1) = 0 then
    let p0, p1 = clip_left_right (clip_above_below (p0, p1)) in
    p0, p1, (contains p0) && (contains p1)
  else
    p0, p1, false
;;

(* draws a horizontal line (no clipping) *)
let draw_horizontal_line surface p0 p1 color =
  if p0.y = p1.y then
    let p0, p1 = if p0.x > p1.x then p1, p0 else p0, p1 in
    
    let rec draw_loop x =
      if x <= p1.x then
        begin
          put_pixel_color surface x p0.y color;
          draw_loop (x + 1)
        end
    in
    
    draw_loop p0.x
;;

(* draws a vertical line (no clipping) *)
let draw_vertical_line surface p0 p1 color =
  if p0.x = p1.x then
    let p0, p1 = if p0.y > p1.y then p1, p0 else p0, p1 in
    
    let rec draw_loop y =
      if y <= p1.y then
        begin
          put_pixel_color surface p0.x y color;
          draw_loop (y + 1)
        end
    in
    
    draw_loop p0.y
;;
      
(* draws a x axis major line (no clipping) *)
let draw_x_major_line surface p0 p1 color =
  let dx, dy = (p1.x - p0.x), (p1.y - p0.y) in
      
  let p0, p1, dx, dy =
    if dx < 0 then
      p1, p0, -dx, -dy
    else
      p0, p1, dx, dy
  in
    
  let y_step = if dy < 0 then -1 else 1 in
  let error_step = 2 * (abs dy) in
  let threshold = 2 * dx in
  
  let rec draw_loop x y error =
    if x <= p1.x then
      if error < threshold then
        begin
          put_pixel_color surface x y color;
          draw_loop (x + 1) y (error + error_step)
        end
      else
        draw_loop x (y + y_step) (error - threshold)
  in
    
  draw_loop p0.x p0.y 0
;;

(* draws a y axis major line (no clipping) *)
let draw_y_major_line surface p0 p1 color =
  let dx, dy = (p1.x - p0.x), (p1.y - p0.y) in

  let p0, p1, dx, dy =
    if dy < 0 then
      p1, p0, -dx, -dy
    else
      p0, p1, dx, dy
  in
  
  let x_step = if dx < 0 then -1 else 1 in
  let error_step = 2 * (abs dx) in
  let threshold = 2 * dy in
  
  let rec draw_loop x y error =
    if y <= p1.y then
      if error < threshold then
        begin
          put_pixel_color surface x y color;
          draw_loop x (y + 1) (error + error_step)
        end
      else
        draw_loop (x + x_step) y (error - threshold);
  in

  draw_loop p0.x p0.y 0;
;;

(* draws a staright line (no clipping) *)
let draw_line_no_clip surface p0 p1 color =
  let dx, dy = (p1.x - p0.x), (p1.y - p0.y) in

  if dx = 0 then
    draw_vertical_line surface p0 p1 color
  else if dy = 0 then
    draw_horizontal_line surface p0 p1 color
  else if (abs dx) >= (abs dy) then
    draw_x_major_line surface p0 p1 color
  else
    draw_y_major_line surface p0 p1 color
;;

(* draw a straight line, clipped to a given rectangle *)
let draw_line_clip surface bound p0 p1 color =
  let p0, p1, accept = clip_line bound p0 p1 in

  if accept then
    draw_line_no_clip surface p0 p1 color
;;

(* draws a straight line (clipped) *)
let draw_line surface p0 p1 color =
  draw_line_clip surface (Sdlvideo.get_clip_rect surface) p0 p1 color
;;

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

(* sorts 3 vertices by their y coordinates *)
let sort_vertices_by_y p0 p1 p2 =
  if p0.y <= p1.y then
    if p1.y <= p2.y then
      p0, p1, p2
    else if p2.y <= p0.y then
      p2, p0, p1
    else
      p0, p2, p1
  else
    if p0.y <= p2.y then
      p1, p0, p2
    else if p2.y <= p1.y then
      p2, p1, p0
    else
      p1, p2, p0
;;

let draw_solid_segment surface p0 p1 p2 p3 color =
  let p0, p1 = if p0.x < p1.x then p0, p1 else p1, p0 in
  let p2, p3 = if p2.x < p3.x then p2, p3 else p3, p2 in

  let dy = float_of_int (p2.y - p0.y + 1) in
  let dx0 = (float_of_int (p2.x - p0.x)) /. dy in
  let dx1 = (float_of_int (p3.x - p1.x)) /. dy in

  let rec rasterise x0 x1 y0 y1 =
    let rec draw_span x0 x1 y =
      if (x0 <= x1) then
        begin
          put_pixel_color surface x0 y color;
          draw_span (x0 + 1) x1 y
        end
    in

    if y0 <= y1 then
      begin
        draw_span (int_of_float x0) (int_of_float x1) y0;
        rasterise (x0 +. dx0) (x1 +. dx1) (y0 + 1) y1
      end
  in

  rasterise (float_of_int p0.x) (float_of_int p1.x) p0.y p2.y
;;

(* draws a flat-shaded triangle (no clipping) *)
let draw_triangle surface p0 p1 p2 color = 
  let p0, p1, p2 = sort_vertices_by_y p0 p1 p2 in

  let draw_flat_base p0 p1 p2 = 
    draw_solid_segment surface p0 p0 p1 p2 color
  in

  let draw_flat_top p0 p1 p2 = 
    draw_solid_segment surface p0 p1 p2 p2 color
  in

  if p0.y = p1.y then
    draw_flat_top p0 p1 p2
  else if p1.y = p2.y then
    draw_flat_base p0 p1 p2
  else
    begin
      let p = intersect_horizontal p0 p2 p1.y in
      draw_flat_base p0 p p1;
      draw_flat_top  p1 p p2
    end
;;

let draw_texture_triangle surface (p0, t0) (p1, t1) (p2, t2) texture =
  ()
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
      eprintf "Error loading texture: %s" error
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
