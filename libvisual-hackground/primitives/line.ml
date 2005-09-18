open Point

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
