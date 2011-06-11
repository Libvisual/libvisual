;// Some macros for blur operations.
;// Each of these macros assumes the following:
;// %edx contains the number of bytes per raster line (the "img_bpl" variable)
;// %esi points to current pixel's source pointer for blurring motion
;// %edi points to current pixel

    .MACRO  BLUR
    lodsl   (%esi),%eax;    ;// %eax = source pixel pointer (from table)
    movzbl  (%edx,%eax),%ebx;// Use %ebx and %ecx to compute pixel average,
    movzbl  (%eax),%ecx ;//   leaving the result in %eax
    subl    %edx,%eax
    addl    %ecx,%ebx
    movzbl  -1(%eax),%ecx
    addl    %ecx,%ebx
    movzbl  1(%eax),%eax
    addl    %ebx,%eax
    shrl    $2,%eax
    stosb   %al,(%edi)  ;// Store the average as the new pixel value
    .ENDM

    .MACRO  SHARP
    lodsl   (%esi),%eax ;// Fetch the next source pointer from table
    movb    (%eax),%al  ;// Feth the pixel via that pointer
    stosb   %al,(%edi)  ;// Store the pixel at the current location
    .ENDM

    .MACRO  SMEAR
    movzbl  (%edx,%ebp),%ebx;// %eax = source pixel pointer (from table)
    negl    %edx        ;// Use %ebx and %ecx to compute pixel average,
    lea -1(%edx,%ebp),%eax;// leaving the result in %bl
    movzbl  (%eax),%ecx
    addl    %ecx,%ebx
    movzbl  2(%eax),%ecx
    lodsl   (%esi),%eax
    addl    %ecx,%ebx
    movzbl  (%eax),%eax
    addl    %eax,%ebx
    shrl    $2,%ebx
    subl    %eax,%ebx   ;// if average is greater, use it
    setc    %ah
    decb    %ah
    andb    %ah,%bl
    addb    %bl,%al
    stosb   %al,(%edi)  ;// Store the average as the new pixel value
    inc %ebp
    .ENDM

    .MACRO  MELT
    movb    (%ebp),%al  ;// if current pixel is >= 160 (unsigned),
    cmpb    $160,%al    ;//   then keep it.
    jae .Ma\@
    lodsl   (%esi),%eax;    ;// %eax = source pixel pointer (from table)
    movzbl  (%edx,%eax),%ebx;// Use %ebx and %ecx to compute pixel average,
    movzbl  (%eax),%ecx ;//   leaving the result in %eax
    subl    %edx,%eax
    addl    %ecx,%ebx
    movzbl  -1(%eax),%ecx
    addl    %ecx,%ebx
    movzbl  1(%eax),%eax
    addl    %ebx,%eax
    shrl    $2,%eax
    jmp .Mb\@
.Ma\@:
    addl    $4,%esi     ;// Inc %esi explicitly since we didn't lodsl
.Mb\@:
    stosb   %al,(%edi)
    inc %ebp
    .ENDM

;//////////////////////////////////////////////////////////////////////////////

.text
    .align 4
.globl loopblur
    .type    loopblur,@function
loopblur:
    pushl   %edi
    pushl   %esi
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    movl    img_chunks,%eax
    pushl   %eax        ;// (%esp) = chunk counter
    movl    img_tmp,%edi    ;// %edi = destination pixel pointer
    movl    img_source,%esi ;// %esi = source table scanner
    movl    img_bpl,%edx    ;// %edx = bytes per line
    .align 4
.L1:
    BLUR
    negl    %edx
    BLUR
    negl    %edx
    BLUR
    negl    %edx
    BLUR
    negl    %edx
    BLUR
    negl    %edx
    BLUR
    negl    %edx
    BLUR
    negl    %edx
    BLUR
    negl    %edx

    decl    (%esp)      ;// Decrement the chunk counter
    jne .L1     ;// Loop if more chunks remain

    addl    $4,%esp
    popl    %edx
    popl    %ecx
    popl    %ebx
    popl    %esi
    popl    %edi
    ret
.Lfe1:
    .size    loopblur,.Lfe1-loopblur

;//////////////////////////////////////////////////////////////////////////////

    .align 4
.globl loopsmear
    .type    loopsmear,@function
loopsmear:
    pushl   %edi
    pushl   %esi
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    pushl   %ebp
    movl    img_chunks,%eax
    pushl   %eax        ;// (%esp) = chunk counter
    movl    img_tmp,%edi    ;// %edi = destination pixel pointer
    movl    img_source,%esi ;// %esi = source table scanner
    movl    img_bpl,%edx    ;// %edx = bytes per line
    movl    img_buf,%ebp    ;// %ebp = source pixel pointer w/o movement
    .align 4
.L2:
    SMEAR
    SMEAR
    SMEAR
    SMEAR
    SMEAR
    SMEAR
    SMEAR
    SMEAR

    decl    (%esp)      ;// Decrement the chunk counter
    jne .L2     ;// Loop if more chunks remain

    addl    $4,%esp
    popl    %ebp
    popl    %edx
    popl    %ecx
    popl    %ebx
    popl    %esi
    popl    %edi
    ret
.Lfe2:
    .size    loopsmear,.Lfe2-loopsmear

;//////////////////////////////////////////////////////////////////////////////

    .align 4
.globl loopmelt
    .type    loopmelt,@function
loopmelt:
    pushl   %edi
    pushl   %esi
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    pushl   %ebp
    movl    img_chunks,%eax
    pushl   %eax        ;// (%esp) = chunk counter
    movl    img_tmp,%edi    ;// %edi = destination pixel pointer
    movl    img_source,%esi ;// %esi = source table scanner
    movl    img_bpl,%edx    ;// %edx = bytes per line
    movl    img_buf,%ebp    ;// %ebp = source pixel pointer w/o movement
    .align 4
.L3:
    MELT
    negl    %edx
    MELT
    negl    %edx
    MELT
    negl    %edx
    MELT
    negl    %edx
    MELT
    negl    %edx
    MELT
    negl    %edx
    MELT
    negl    %edx
    MELT
    negl    %edx

    decl    (%esp)      ;// Decrement the chunk counter
    jne .L3     ;// Loop if more chunks remain

    addl    $4,%esp
    popl    %ebp
    popl    %edx
    popl    %ecx
    popl    %ebx
    popl    %esi
    popl    %edi
    ret
.Lfe3:
    .size    loopmelt,.Lfe3-loopmelt

;//////////////////////////////////////////////////////////////////////////////

    .align 4
.globl loopsharp
    .type    loopsharp,@function
loopsharp:
    pushl   %esi
    pushl   %edi
    pushl   %ecx
    movl    img_tmp,%edi    ;// %edi = destination pixel pointer
    movl    img_source,%esi ;// %esi = source table scanner
    movl    img_chunks,%ecx ;// %ecx = chunk counter
    .align 4
.L4:
    SHARP
    SHARP
    SHARP
    SHARP
    SHARP
    SHARP
    SHARP
    SHARP

    loop    .L4     ;// Loop if more chunks remain

    popl    %ecx
    popl    %edi
    popl    %esi
    ret
.Lfe4:
    .size    loopsharp,.Lfe4-loopsharp

;//////////////////////////////////////////////////////////////////////////////

    .align 4
.globl loopreduced1
    .type    loopreduced1,@function
loopreduced1:
    pushl   %edi
    pushl   %esi
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    movl    img_chunks,%eax
    pushl   %eax        ;// (%esp) = chunk counter
    movl    img_tmp,%edi    ;// %edi = destination pixel pointer
    movl    img_source,%esi ;// %esi = source table scanner
    movl    img_bpl,%edx    ;// %edx = bytes per line
    .align 4
.L5:
    BLUR
    SHARP
    SHARP
    SHARP
    BLUR
    SHARP
    SHARP
    SHARP

    decl    (%esp)      ;// Decrement the chunk counter
    jne .L5     ;// Loop if more chunks remain

    addl    $4,%esp
    popl    %edx
    popl    %ecx
    popl    %ebx
    popl    %esi
    popl    %edi
    ret
.Lfe5:
    .size    loopreduced1,.Lfe5-loopreduced1

;//////////////////////////////////////////////////////////////////////////////

    .align 4
.globl loopreduced2
    .type    loopreduced2,@function
loopreduced2:
    pushl   %edi
    pushl   %esi
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    movl    img_chunks,%eax
    pushl   %eax        ;// (%esp) = chunk counter
    movl    img_tmp,%edi    ;// %edi = destination pixel pointer
    movl    img_source,%esi ;// %esi = source table scanner
    movl    img_bpl,%edx    ;// %edx = bytes per line
    negl    %edx        ;// negate %edx, as it would be for full blur
    .align 4
.L6:
    SHARP
    BLUR
    SHARP
    SHARP
    SHARP
    BLUR
    SHARP
    SHARP

    decl    (%esp)      ;// Decrement the chunk counter
    jne .L6     ;// Loop if more chunks remain

    addl    $4,%esp
    popl    %edx
    popl    %ecx
    popl    %ebx
    popl    %esi
    popl    %edi
    ret
.Lfe6:
    .size    loopreduced2,.Lfe6-loopreduced2

;//////////////////////////////////////////////////////////////////////////////

    .align 4
.globl loopreduced3
    .type    loopreduced3,@function
loopreduced3:
    pushl   %edi
    pushl   %esi
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    movl    img_chunks,%eax
    pushl   %eax        ;// (%esp) = chunk counter
    movl    img_tmp,%edi    ;// %edi = destination pixel pointer
    movl    img_source,%esi ;// %esi = source table scanner
    movl    img_bpl,%edx    ;// %edx = bytes per line
    .align 4
.L7:
    SHARP
    SHARP
    BLUR
    SHARP
    SHARP
    SHARP
    BLUR
    SHARP

    decl    (%esp)      ;// Decrement the chunk counter
    jne .L7     ;// Loop if more chunks remain

    addl    $4,%esp
    popl    %edx
    popl    %ecx
    popl    %ebx
    popl    %esi
    popl    %edi
    ret
.Lfe7:
    .size    loopreduced3,.Lfe7-loopreduced3

;//////////////////////////////////////////////////////////////////////////////

    .align 4
.globl loopreduced4
    .type    loopreduced4,@function
loopreduced4:
    pushl   %edi
    pushl   %esi
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    movl    img_chunks,%eax
    pushl   %eax        ;// (%esp) = chunk counter
    movl    img_tmp,%edi    ;// %edi = destination pixel pointer
    movl    img_source,%esi ;// %esi = source table scanner
    movl    img_bpl,%edx    ;// %edx = bytes per line
    negl    %edx        ;// negate %edx, as it would be for full blur
    .align 4
.L8:
    SHARP
    SHARP
    SHARP
    BLUR
    SHARP
    SHARP
    SHARP
    BLUR

    decl    (%esp)      ;// Decrement the chunk counter
    jne .L8     ;// Loop if more chunks remain

    addl    $4,%esp
    popl    %edx
    popl    %ecx
    popl    %ebx
    popl    %esi
    popl    %edi
    ret
.Lfe8:
    .size    loopreduced4,.Lfe8-loopreduced4

;//////////////////////////////////////////////////////////////////////////////

;// The following macros are used in the loopfade() function.  %bl is a
;// positive value to be subtracted (in the FADE macro) or added (in the
;// FLASH macro) to each pixel.

    .MACRO  FADE
    movb    (%edi),%al  ;// fetch the pixel
    subb    %bl,%al     ;// fade the pixel, possibly causing underflow
    setc    %ah     ;// make a mask byte: 0 if underflow, else 255
    decb    %ah
    andb    %ah,%al     ;// mask out underflows
    stosb   %al,(%edi)  ;// store pixel, increment pointer
    .ENDM

    .MACRO FLASH
    movb    (%edi),%al  ;// fetch the pixel
    addb    %bl,%al     ;// fade the pixel, possibly causing underflow
    setnc   %ah     ;// make a mask byte: 255 if overflow, else 0
    decb    %ah
    orb %ah,%al     ;// mask out overflows
    stosb   %al,(%edi)  ;// store pixel, increment pointer
    .ENDM

;//////////////////////////////////////////////////////////////////////////////

    .align 4
.globl loopfade
    .type    loopfade,@function
loopfade:
    pushl %ebp
    movl %esp,%ebp
    pushl %ecx
    pushl %ebx
    pushl %edi

    movl img_buf,%edi   ;// %edx = pixel pointer
    movl img_chunks,%ecx    ;// %ecx = chunk counter
    movl 8(%ebp),%ebx   ;// %ebx = change
    testl %ebx,%ebx     ;// if change < 0
    jge .L10flash

    negl %ebx       ;// find the absolute value of the change
    .align 4
.L10fade:
    FADE
    FADE
    FADE
    FADE
    FADE
    FADE
    FADE
    FADE

    loop    .L10fade

    jmp .L10endif       ;// else (change not negative)

    .align 4

.L10flash:
    FLASH
    FLASH
    FLASH
    FLASH
    FLASH
    FLASH
    FLASH
    FLASH

    loop .L10flash

.L10endif:
    popl %edi
    popl %ebx
    popl %esi
    movl %ebp,%esp
    popl %ebp
    ret
.Lfe10:
    .size    loopfade,.Lfe10-loopfade

;//////////////////////////////////////////////////////////////////////////////

;// The INTERP macro and loopinter function are used to perform interpolation
;// while doubling the width of a raster line.

    .MACRO  INTERP
    lodsb   (%esi),%al  ;// %al=this source pixel, %ah=previous
    xchg    %al,%ah     ;// swap, so %ah=this pixel
    addb    %ah,%al     ;// set %al to interpolated pixel
    rcrb    $1,%al
    stosw   %ax,(%edi)  ;// store both pixels, with interp in low byte
    .ENDM

    .align 4
.globl loopinterp
    .type    loopinterp,@function
loopinterp:
    pushl   %edi
    pushl   %esi
    pushl   %ecx
    movl    img_chunks,%ecx ;// %ebx = chunk counter
    movl    img_buf,%esi    ;// %esi = source pixel pointer
    movl    img_tmp,%edi    ;// %edi = destination pixel pointer
    sub %ah,%ah     ;// %ah = prev byte, used for interpolation
    .align 4
.L11:
    INTERP
    INTERP
    INTERP
    INTERP
    INTERP
    INTERP
    INTERP
    INTERP

    loop    .L11        ;// Decrement the chunk counter, loop if more

    popl    %ecx
    popl    %esi
    popl    %edi
    ret
.Lfe11:
    .size    loopinterp,.Lfe11-loopinterp
