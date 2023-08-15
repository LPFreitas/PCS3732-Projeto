.text
.global main
main:
	mov r0, #2
	mov r1, #3
	add r0, r0, r1
	mov r1, #4
	mul r0, r0, r1
	mov r1, #5
	sub r0, r0, r1
end:
	b end
