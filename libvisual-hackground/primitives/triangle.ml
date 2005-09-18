open Point
open Line

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
