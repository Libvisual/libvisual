.function simd_mul_floats_float
.dest 4 dst float
.source 4 src float
.floatparam 4 k
mulf dst, src, k

.function simd_mul_floats_floats
.dest 4 dst float
.source 4 src1 float
.source 4 src2 float
mulf dst, src1, src2

.function simd_add_floats_float
.dest 4 dst float
.source 4 src float
.floatparam 4 k
addf dst, src, k

.function simd_floats_to_int32s
.dest 4 dst
.source 4 src float
convfl dst, src

.function simd_int32s_to_floats
.dest 4 dst float
.source 4 src
convlf dst, src

.function simd_sqrt_floats
.dest 4 dst float
.source 4 src float
sqrtf dst, src

.function simd_floats_to_int32s_mul_float
.dest 4 dst
.source 4 src float
.floatparam 4 k
.temp 4 t1
convfl t1, src
mulf dst, k, t1

.function simd_int32s_to_floats_mul_float
.dest 4 dst float
.source 4 src
.floatparam 4 k
.temp 4 t1
convlf t1, src
mulf dst, k, t1

.function simd_complex_norm
.dest 4 dst float
.source 4 real float
.source 4 imag float
.temp 4 t1
.temp 4 t2
mulf t1, real, real
mulf t2, imag, imag
addf t1, t1, t2
sqrtf dst, t1

.function simd_complex_scaled_norm
.dest 4 dst float
.source 4 real float
.source 4 imag float
.floatparam 4 k
.temp 4 t1
.temp 4 t2
mulf t1, real, real
mulf t2, imag, imag
addf t1, t1, t2
sqrtf t2, t1
mulf dst, t2, k

.function simd_denorm_floats_to_int32s
.dest 4 dst
.source 4 src float
.floatparam 4 k
.const 4 lower 0.0
.const 4 upper 1.0
.temp 4 t1
.temp 4 t2
maxf t1, src, lower
minf t2, t1, upper
mulf t1, t2, k
convfl dst, t1

.function simd_denorm_neg_floats_to_int32s
.dest 4 dst
.source 4 src float
.floatparam 4 k
.const 4 lower -1.0
.const 4 upper 1.0
.const 4 half 0.5
.temp 4 t1
.temp 4 t2
maxf t1, src, lower
minf t2, t1, upper
addf t1, t2, upper
mulf t2, t1, half
mulf t1, t2, k
convfl dst, t1
