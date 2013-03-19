;// Some macros for blur operations.
;// Each of these macros assumes the following:
;// %rdx contains the number of bytes per raster line (the "img_bpl" variable)
;// %rsi points to current pixel's source pointer for blurring motion
;// %rdi points to current pixel

    .MACRO  BLUR
    lodsq   (%rsi),%rax;     ;// %rax = source pixel pointer (from table)
    movzbl  (%rdx,%rax),%ebx ;// Use %ebx and %ecx to compute pixel average,
    movzbl  (%rax),%ecx      ;//   leaving the result in %rax
    subq    %rdx,%rax
    addl    %ecx,%ebx
    movzbl  -1(%rax),%ecx
    addl    %ecx,%ebx
    movzbl  1(%rax),%eax
    addl    %ebx,%eax
    shrl    $2,%eax
    stosb   %al,(%rdi)       ;// Store the average as the new pixel value
    .ENDM

    .MACRO  SHARP
    lodsq   (%rsi),%rax      ;// Fetch the next source pointer from table
    movb    (%rax),%al       ;// Fetch the pixel via that pointer
    stosb   %al,(%rdi)       ;// Store the pixel at the current location
    .ENDM

    .MACRO  SMEAR
    movzbl  (%rdx,%rbp),%ebx   ;// %rax = source pixel pointer (from table)
    negq    %rdx               ;// Use %ebx and %ecx to compute pixel average,
    lea     -1(%rdx,%rbp),%rax ;// leaving the result in %bl
    movzbl  (%rax),%ecx
    addl    %ecx,%ebx
    movzbl  2(%rax),%ecx
    lodsl   (%rsi),%eax
    addl    %ecx,%ebx
    movzbl  (%rax),%eax
    addl    %eax,%ebx
    shrl    $2,%ebx
    subl    %eax,%ebx          ;// if average is greater, use it
    setc    %ah
    decb    %ah
    andb    %ah,%bl
    addb    %bl,%al
    stosb   %al,(%rdi)         ;// Store the average as the new pixel value
    inc     %rbp
    .ENDM

    .MACRO  MELT
    movb    (%rbp),%al       ;// if current pixel is >= 160 (unsigned),
    cmpb    $160,%al         ;//   then keep it.
    jae .Ma\@
    lodsq   (%rsi),%rax;     ;// %rax = source pixel pointer (from table)
    movzbl  (%rdx,%rax),%ebx ;// Use %ebx and %ecx to compute pixel average,
    movzbl  (%rax),%ecx      ;//   leaving the result in %eax
    subq    %rdx,%rax
    addl    %ecx,%ebx
    movzbl  -1(%rax),%ecx
    addl    %ecx,%ebx
    movzbl  1(%rax),%eax
    addl    %ebx,%eax
    shrl    $2,%eax
    jmp .Mb\@
.Ma\@:
    addq    $4,%rsi     ;// Inc %rsi explicitly since we didn't lodsl
.Mb\@:
    stosb   %al,(%rdi)
    inc     %rbp
    .ENDM

;//////////////////////////////////////////////////////////////////////////////

    .text
    .align  8
    .globl  loopblur
    .type   loopblur,@function
loopblur:
    pushq   %rdi
    pushq   %rsi
    pushq   %rbx
    pushq   %rcx
    pushq   %rdx
    movq    img_chunks@GOTPCREL(%rip),%rax
    movl    (%rax), %eax
    pushq   %rax                           ;// (%rsp) = chunk counter
    movq    img_tmp@GOTPCREL(%rip),%rdi    ;// %rdi = destination pixel pointer
    movq    (%rdi), %rdi
    movq    img_source@GOTPCREL(%rip),%rsi ;// %rsi = source table scanner
    movq    (%rsi), %rsi
    movq    img_bpl@GOTPCREL(%rip),%rdx    ;// %rdx = bytes per line
    movsxd  (%rdx), %rdx
    .align  8
.L1:
    BLUR
    negq    %rdx
    BLUR
    negq    %rdx
    BLUR
    negq    %rdx
    BLUR
    negq    %rdx
    BLUR
    negq    %rdx
    BLUR
    negq    %rdx
    BLUR
    negq    %rdx
    BLUR
    negq    %rdx

    decl    (%rsp)      ;// Decrement the chunk counter
    jne .L1     ;// Loop if more chunks remain

    addq    $8,%rsp
    popq    %rdx
    popq    %rcx
    popq    %rbx
    popq    %rsi
    popq    %rdi
    ret
.Lfe1:
    .size    loopblur,.Lfe1-loopblur

;//////////////////////////////////////////////////////////////////////////////

    .align  8
    .globl  loopsmear
    .type   loopsmear,@function
loopsmear:
    pushq   %rdi
    pushq   %rsi
    pushq   %rbx
    pushq   %rcx
    pushq   %rdx
    pushq   %rbp
    movq    img_chunks@GOTPCREL(%rip),%rax
    movl    (%rax), %eax
    pushq   %rax                           ;// (%rsp) = chunk counter
    movq    img_tmp@GOTPCREL(%rip),%rdi    ;// %rdi = destination pixel pointer
    movq    (%rdi), %rdi
    movq    img_source@GOTPCREL(%rip),%rsi ;// %rsi = source table scanner
    movq    (%rsi), %rsi
    movq    img_bpl@GOTPCREL(%rip),%rdx    ;// %rdx = bytes per line
    movsxd  (%rdx), %rdx
    movq    img_buf@GOTPCREL(%rip),%rbp    ;// %rbp = source pixel pointer w/o movement
    movq    (%rbp), %rbp
    .align  8
.L2:
    SMEAR
    SMEAR
    SMEAR
    SMEAR
    SMEAR
    SMEAR
    SMEAR
    SMEAR

    decl    (%rsp)      ;// Decrement the chunk counter
    jne .L2             ;// Loop if more chunks remain

    addq    $8,%rsp
    popq    %rbp
    popq    %rdx
    popq    %rcx
    popq    %rbx
    popq    %rsi
    popq    %rdi
    ret
.Lfe2:
    .size    loopsmear,.Lfe2-loopsmear

;//////////////////////////////////////////////////////////////////////////////

    .align  8
    .globl  loopmelt
    .type   loopmelt,@function
loopmelt:
    pushq   %rdi
    pushq   %rsi
    pushq   %rbx
    pushq   %rcx
    pushq   %rdx
    pushq   %rbp
    movq    img_chunks@GOTPCREL(%rip),%rax
    movl    (%rax), %eax
    pushq   %rax            ;// (%rsp) = chunk counter
    movq    img_tmp@GOTPCREL(%rip),%rdi    ;// %rdi = destination pixel pointer
    movq    (%rdi), %rdi
    movq    img_source@GOTPCREL(%rip),%rsi ;// %rsi = source table scanner
    movq    (%rsi), %rsi
    movq    img_bpl@GOTPCREL(%rip),%rdx    ;// %rdx = bytes per line
    movsxd  (%rdx), %rdx
    movq    img_buf@GOTPCREL(%rip),%rbp    ;// %rbp = source pixel pointer w/o movement
    movq    (%rbp), %rbp
    .align  8
.L3:
    MELT
    negq    %rdx
    MELT
    negq    %rdx
    MELT
    negq    %rdx
    MELT
    negq    %rdx
    MELT
    negq    %rdx
    MELT
    negq    %rdx
    MELT
    negq    %rdx
    MELT
    negq    %rdx

    decl    (%rsp)      ;// Decrement the chunk counter
    jne     .L3         ;// Loop if more chunks remain

    addq    $8,%rsp
    popq    %rbp
    popq    %rdx
    popq    %rcx
    popq    %rbx
    popq    %rsi
    popq    %rdi
    ret
.Lfe3:
    .size    loopmelt,.Lfe3-loopmelt

;//////////////////////////////////////////////////////////////////////////////

    .align  8
    .globl  loopsharp
    .type   loopsharp,@function
loopsharp:
    pushq   %rsi
    pushq   %rdi
    pushq   %rcx
    movq    img_tmp@GOTPCREL(%rip),%rdi    ;// %rdi = destination pixel pointer
    movq    (%rdi), %rdi
    movq    img_source@GOTPCREL(%rip),%rsi ;// %rsi = source table scanner
    movq    (%rsi), %rsi
    movq    img_chunks@GOTPCREL(%rip),%rcx ;// %ecx = chunk counter
    movl    (%rcx), %ecx
    .align  8
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

    popq    %rcx
    popq    %rdi
    popq    %rsi
    ret
.Lfe4:
    .size    loopsharp,.Lfe4-loopsharp

;//////////////////////////////////////////////////////////////////////////////

    .align  8
    .globl  loopreduced1
    .type   loopreduced1,@function
loopreduced1:
    pushq   %rdi
    pushq   %rsi
    pushq   %rbx
    pushq   %rcx
    pushq   %rdx
    movq    img_chunks@GOTPCREL(%rip),%rax
    movl    (%rax), %eax
    pushq   %rax                           ;// (%rsp) = chunk counter
    movq    img_tmp@GOTPCREL(%rip),%rdi    ;// %rdi = destination pixel pointer
    movq    (%rdi), %rdi
    movq    img_source@GOTPCREL(%rip),%rsi ;// %rsi = source table scanner
    movq    (%rsi), %rsi
    movq    img_bpl@GOTPCREL(%rip),%rdx    ;// %rdx = bytes per line
    movsxd  (%rdx), %rdx
    .align  8
.L5:
    BLUR
    SHARP
    SHARP
    SHARP
    BLUR
    SHARP
    SHARP
    SHARP

    decl    (%rsp)      ;// Decrement the chunk counter
    jne     .L5         ;// Loop if more chunks remain

    addq    $8,%rsp
    popq    %rdx
    popq    %rcx
    popq    %rbx
    popq    %rsi
    popq    %rdi
    ret
.Lfe5:
    .size    loopreduced1,.Lfe5-loopreduced1

;//////////////////////////////////////////////////////////////////////////////

    .align  8
    .globl  loopreduced2
    .type   loopreduced2,@function
loopreduced2:
    pushq   %rdi
    pushq   %rsi
    pushq   %rbx
    pushq   %rcx
    pushq   %rdx
    movq    img_chunks@GOTPCREL(%rip),%rax
    movl    (%rax), %eax
    pushq   %rax                           ;// (%rsp) = chunk counter
    movq    img_tmp@GOTPCREL(%rip),%rdi    ;// %rdi = destination pixel pointer
    movq    (%rdi), %rdi
    movq    img_source@GOTPCREL(%rip),%rsi ;// %rsi = source table scanner
    movq    (%rsi), %rsi
    movq    img_bpl@GOTPCREL(%rip),%rdx    ;// %rdx = bytes per line
    movsxd  (%rdx), %rdx
    negq    %rdx                           ;// negate %rdx, as it would be for full blur
    .align  8
.L6:
    SHARP
    BLUR
    SHARP
    SHARP
    SHARP
    BLUR
    SHARP
    SHARP

    decl    (%rsp)      ;// Decrement the chunk counter
    jne     .L6         ;// Loop if more chunks remain

    addq    $8,%rsp
    popq    %rdx
    popq    %rcx
    popq    %rbx
    popq    %rsi
    popq    %rdi
    ret
.Lfe6:
    .size    loopreduced2,.Lfe6-loopreduced2

;//////////////////////////////////////////////////////////////////////////////

    .align  8
    .globl  loopreduced3
    .type   loopreduced3,@function
loopreduced3:
    pushq   %rdi
    pushq   %rsi
    pushq   %rbx
    pushq   %rcx
    pushq   %rdx
    movq    img_chunks@GOTPCREL(%rip),%rax
    movl    (%rax), %eax
    pushq   %rax                           ;// (%rsp) = chunk counter
    movq    img_tmp@GOTPCREL(%rip),%rdi    ;// %rdi = destination pixel pointer
    movq    (%rdi), %rdi
    movq    img_source@GOTPCREL(%rip),%rsi ;// %rsi = source table scanner
    movq    (%rsi), %rsi
    movq    img_bpl@GOTPCREL(%rip),%rdx    ;// %rdx = bytes per line
    movsxd  (%rdx), %rdx
    .align  8
.L7:
    SHARP
    SHARP
    BLUR
    SHARP
    SHARP
    SHARP
    BLUR
    SHARP

    decl    (%rsp)      ;// Decrement the chunk counter
    jne     .L7         ;// Loop if more chunks remain

    addq    $8,%rsp
    popq    %rdx
    popq    %rcx
    popq    %rbx
    popq    %rsi
    popq    %rdi
    ret
.Lfe7:
    .size    loopreduced3,.Lfe7-loopreduced3

;//////////////////////////////////////////////////////////////////////////////

    .align  8
    .globl  loopreduced4
    .type   loopreduced4,@function
loopreduced4:
    pushq   %rdi
    pushq   %rsi
    pushq   %rbx
    pushq   %rcx
    pushq   %rdx
    movq    img_chunks@GOTPCREL(%rip),%rax
    movl    (%rax), %eax
    pushq   %rax                           ;// (%rsp) = chunk counter
    movq    img_tmp@GOTPCREL(%rip),%rdi    ;// %rdi = destination pixel pointer
    movq    (%rdi), %rdi
    movq    img_source@GOTPCREL(%rip),%rsi ;// %rsi = source table scanner
    movq    (%rsi), %rsi
    movq    img_bpl@GOTPCREL(%rip),%rdx    ;// %rdx = bytes per line
    movl    (%rdx), %edx
    negq    %rdx                           ;// negate %rdx, as it would be for full blur
    .align  8
.L8:
    SHARP
    SHARP
    SHARP
    BLUR
    SHARP
    SHARP
    SHARP
    BLUR

    decl    (%rsp)      ;// Decrement the chunk counter
    jne     .L8         ;// Loop if more chunks remain

    addq    $8,%rsp
    popq    %rdx
    popq    %rcx
    popq    %rbx
    popq    %rsi
    popq    %rdi
    ret
.Lfe8:
    .size    loopreduced4,.Lfe8-loopreduced4

;//////////////////////////////////////////////////////////////////////////////

;// The following macros are used in the loopfade() function.  %bl is a
;// positive value to be subtracted (in the FADE macro) or added (in the
;// FLASH macro) to each pixel.

    .MACRO  FADE
    movb    (%rdi),%al  ;// fetch the pixel
    subb    %bl,%al     ;// fade the pixel, possibly causing underflow
    setc    %ah         ;// make a mask byte: 0 if underflow, else 255
    decb    %ah
    andb    %ah,%al     ;// mask out underflows
    stosb   %al,(%rdi)  ;// store pixel, increment pointer
    .ENDM

    .MACRO FLASH
    movb    (%rdi),%al  ;// fetch the pixel
    addb    %bl,%al     ;// fade the pixel, possibly causing underflow
    setnc   %ah         ;// make a mask byte: 255 if overflow, else 0
    decb    %ah
    orb     %ah,%al     ;// mask out overflows
    stosb   %al,(%rdi)  ;// store pixel, increment pointer
    .ENDM

;//////////////////////////////////////////////////////////////////////////////

    .align  8
    .globl  loopfade
    .type   loopfade,@function
loopfade:
    pushq   %rbp
    movq    %rsp,%rbp
    pushq   %rcx
    pushq   %rbx
    pushq   %rdi

    movl    %edi,%ebx                         ;// %ebx = change (%edi holds first argument)
    movq    img_chunks@GOTPCREL(%rip),%rcx    ;// %ecx = chunk counter
    movl    (%rcx), %ecx
    movq    img_buf@GOTPCREL(%rip),%rdi       ;// %edx = pixel pointer
    movq    (%rdi), %rdi
    testl   %ebx,%ebx                         ;// if change < 0
    jge     .L10flash

    negl    %ebx               ;// find the absolute value of the change
    .align  8
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

    jmp     .L10endif          ;// else (change not negative)

    .align  8

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
    popq    %rdi
    popq    %rbx
    popq    %rsi
    movq    %rbp,%rsp
    popq    %rbp
    ret
.Lfe10:
    .size   loopfade,.Lfe10-loopfade

;//////////////////////////////////////////////////////////////////////////////

;// The INTERP macro and loopinter function are used to perform interpolation
;// while doubling the width of a raster line.

    .MACRO  INTERP
    lodsb   (%rsi),%al  ;// %al=this source pixel, %ah=previous
    xchg    %al,%ah     ;// swap, so %ah=this pixel
    addb    %ah,%al     ;// set %al to interpolated pixel
    rcrb    $1,%al
    stosw   %ax,(%rdi)  ;// store both pixels, with interp in low byte
    .ENDM

    .align  8
    .globl  loopinterp
    .type   loopinterp,@function
loopinterp:
    pushq   %rdi
    pushq   %rsi
    pushq   %rcx
    movq    img_chunks@GOTPCREL(%rip),%rcx ;// %ebx = chunk counter
    movl    (%rcx), %ecx
    movq    img_buf@GOTPCREL(%rip),%rsi    ;// %esi = source pixel pointer
    movq    (%rsi), %rsi
    movq    img_tmp@GOTPCREL(%rip),%rdi    ;// %edi = destination pixel pointer
    movq    (%rdi), %rdi
    sub     %ah,%ah         ;// %ah = prev byte, used for interpolation
    .align  8
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

    popq    %rcx
    popq    %rsi
    popq    %rdi
    ret
.Lfe11:
    .size   loopinterp,.Lfe11-loopinterp
