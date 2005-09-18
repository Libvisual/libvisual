type 'a point = { x: 'a; y: 'a }

let put_pixel_color surface x y color =
  (* comment this out to see how slow Sdlvideo.put_pixel_color is *)
  Sdlvideo.put_pixel_color surface x y color
;;
