
@ void VSync()
.text
.global VSync
.thumb
.align 2
.thumb_func
VSync:
swi 0x5
bx lr



@ void UnCompLZ77VRam(void *src, void *dest)
.text
.global UnCompLZ77VRam
.thumb
.align 2
.thumb_func
UnCompLZ77VRam:
swi 0x12
bx lr



@ s32 sqrt(s32 num)
.text
.global sqrt
.thumb
.align 2
.thumb_func
sqrt:
swi		0x8
bx		lr



@ s32 div(s32 numer, s32 denom)
@ Stub for divI
.text
.global div
.thumb
.align 2
.thumb_func
div:
ldr		r2, =divI	@ here, we'll leave lr like it is, and
bx		r2			@ let divI return to this function's caller
.pool



@ s32 mod(s32 numer, s32 denom)
.text
.global mod
.thumb
.align 2
.thumb_func
mod:
swi		0x6
mov		r0, r1
bx		lr



@ s32 fpMul(f16_16 m1, f16_16 m2)
.text
.global fpMul
.arm
.align 4
fpMul:
smull	r2, r0, r1, r0
mov		r2, r2, lsr #16
orr		r0, r2, r0, lsl #16
bx		lr



@ s32 fpDiv(f16_16 numer, f16_16 denom)
.text
.global fpDiv
.thumb
.align 2
.thumb_func
fpDiv:
push	{lr}
lsr		r1, r1, #8	@ lose some accuracy, but none of the integer portion
ldr		r2, =divI
mov		lr, pc		@ a blx instruction would be nice here
bx		r2
lsl		r0, r0, #8	@ shift back up to 16.16 fixed-point
pop		{r1}		@ get old lr
bx		r1			@ and return to it
.pool



@ s32 divI(s32 numer, s32 denom)
@ Signed integer divide, code in IWRAM
.section .iwram, "ax", %progbits
.global divI
.arm
.align 2
divI:
mov	r3, #0
eor	r2, r0, r1
cmp	r0, #0
rsblt	r0, r0, #0
cmp	r1, #0
rsblt	r1, r1, #0

cmp	r0, r1, lsl #8
blt	div_bit7

cmp	r0, r1, lsl #16
blt	div_bit15

cmp	r0, r1, lsl #24
blt	div_bit23

cmp	r0, r1, lsl #31
subge	r0, r0, r1, lsl #31
addge	r3, r3, #1<<31

cmp	r0, r1, lsl #30
subge	r0, r0, r1, lsl #30
addge	r3, r3, #1<<30

cmp	r0, r1, lsl #29
subge	r0, r0, r1, lsl #29
addge	r3, r3, #1<<29

cmp	r0, r1, lsl #28
subge	r0, r0, r1, lsl #28
addge	r3, r3, #1<<28

cmp	r0, r1, lsl #27
subge	r0, r0, r1, lsl #27
addge	r3, r3, #1<<27

cmp	r0, r1, lsl #26
subge	r0, r0, r1, lsl #26
addge	r3, r3, #1<<26

cmp	r0, r1, lsl #25
subge	r0, r0, r1, lsl #25
addge	r3, r3, #1<<25

cmp	r0, r1, lsl #24
subge	r0, r0, r1, lsl #24
addge	r3, r3, #1<<24

div_bit23:
cmp	r0, r1, lsl #23
subge	r0, r0, r1, lsl #23
addge	r3, r3, #1<<23

cmp	r0, r1, lsl #22
subge	r0, r0, r1, lsl #22
addge	r3, r3, #1<<22

cmp	r0, r1, lsl #21
subge	r0, r0, r1, lsl #21
addge	r3, r3, #1<<21

cmp	r0, r1, lsl #20
subge	r0, r0, r1, lsl #20
addge	r3, r3, #1<<20

cmp	r0, r1, lsl #19
subge	r0, r0, r1, lsl #19
addge	r3, r3, #1<<19

cmp	r0, r1, lsl #18
subge	r0, r0, r1, lsl #18
addge	r3, r3, #1<<18

cmp	r0, r1, lsl #17
subge	r0, r0, r1, lsl #17
addge	r3, r3, #1<<17

cmp	r0, r1, lsl #16
subge	r0, r0, r1, lsl #16
addge	r3, r3, #1<<16

div_bit15:
cmp	r0, r1, lsl #15
subge	r0, r0, r1, lsl #15
addge	r3, r3, #1<<15

cmp	r0, r1, lsl #14
subge	r0, r0, r1, lsl #14
addge	r3, r3, #1<<14

cmp	r0, r1, lsl #13
subge	r0, r0, r1, lsl #13
addge	r3, r3, #1<<13

cmp	r0, r1, lsl #12
subge	r0, r0, r1, lsl #12
addge	r3, r3, #1<<12

cmp	r0, r1, lsl #11
subge	r0, r0, r1, lsl #11
addge	r3, r3, #1<<11

cmp	r0, r1, lsl #10
subge	r0, r0, r1, lsl #10
addge	r3, r3, #1<<10

cmp	r0, r1, lsl #9
subge	r0, r0, r1, lsl #9
addge	r3, r3, #1<<9

cmp	r0, r1, lsl #8
subge	r0, r0, r1, lsl #8
addge	r3, r3, #1<<8

div_bit7:
cmp	r0, r1, lsl #7
subge	r0, r0, r1, lsl #7
addge	r3, r3, #1<<7

cmp	r0, r1, lsl #6
subge	r0, r0, r1, lsl #6
addge	r3, r3, #1<<6

cmp	r0, r1, lsl #5
subge	r0, r0, r1, lsl #5
addge	r3, r3, #1<<5

cmp	r0, r1, lsl #4
subge	r0, r0, r1, lsl #4
addge	r3, r3, #1<<4

cmp	r0, r1, lsl #3
subge	r0, r0, r1, lsl #3
addge	r3, r3, #1<<3

cmp	r0, r1, lsl #2
subge	r0, r0, r1, lsl #2
addge	r3, r3, #1<<2

cmp	r0, r1, lsl #1
subge	r0, r0, r1, lsl #1
addge	r3, r3, #1<<1

cmp	r0, r1
addge	r3, r3, #1

mov	r0, r3
tst	r2, #1<<31
rsbne	r0, r0, #0

bx		lr
