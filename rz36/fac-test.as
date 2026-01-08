







(fun fac (else (== #1 0 )(return 1 )(return (* #1 (call fac (- #1 1 ))))))
(fun main (print (call fac 7 )))
.symbol
 fp 30
 sp 29
 retval 28
 rads 27
.code 0
 mov fp #3500
 mov sp #3000
 jal rads main
 trap r0 #0
; fun fac pv 1
:fac
st r1 @1 fp
st r2 @2 fp
st r3 @3 fp
add fp fp #4
st rads @0 fp
pop sp r1
eq r2 r1 #0
jf r2 L102
mov retval #1
jmp L101
jmp L103
:L102
sub r3 r1 #1
push sp r3
jal rads fac
mul r2 r1 r28
mov retval r2
jmp L101
:L103
:L101
ld rads @0 fp
sub fp fp #4
ld r3 @3 fp
ld r2 @2 fp
ld r1 @1 fp
ret rads
; fun main pv 0
:main
st r1 @1 fp
add fp fp #2
st rads @0 fp
mov r1 #7
push sp r1
jal rads fac
trap r28 #1
:L104
ld rads @0 fp
sub fp fp #2
ld r1 @1 fp
ret rads
.data 200
.end
