.function simd_interpolate_8
.dest 1 dst
.source 1 src1
.source 1 src2
.param 1 alpha
.temp 1 t1
.temp 2 t2
.temp 2 t3
subb t1, 255, alpha
mulubw t3, t1, src1
mulubw t2, alpha, src2
addw t3, t3, t2
div255w t3, t3
convwb dst, t3

.function simd_interpolate_8_2d
.flags 2d
.dest 1 dst
.source 1 src
.param 1 alpha
.temp 1 t1
.temp 2 t2
.temp 2 t3
subb t1, 255, alpha
mulubw t3, t1, dst
mulubw t2, alpha, src
addw t3, t3, t2
div255w t3, t3
convwb dst, t3
