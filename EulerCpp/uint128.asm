.code

Multiply128  PROC
; RCX , RDX, R8, R9

		push	rbx
		push	rsi
		push	rdi

		mov	r10, rcx

		mov	rsi, QWORD PTR [RDX + 08h] ; l2
		mov	rdi, QWORD PTR [RDX] ; h2

		mov	rbx, QWORD PTR [R10 + 08h] ; l1
		mov rcx, QWORD PTR [R10] ; h1

		; l1 * l2
		mov rax, rbx
		mul rsi ; result in %rdx:%rax
		mov r8, rax
		mov r9, rdx

		; l1 * h2
		mov rax, rbx
		mul rdi ; result in %rdx:%rax
		add r9, rax ; adc?

		; h1 * l2
		mov rax, rcx
		mul rsi ; result in %rdx:%rax
		add r9, rax

		mov QWORD PTR [R10 + 08h], r8
		mov QWORD PTR [R10], r9

		pop		rdi
		pop		rsi
		pop		rbx

    ret

Multiply128  ENDP


Adc128  PROC
; RCX , RDX, R8, R9

		mov r8, QWORD PTR [RCX + 08h]
		mov rax, QWORD PTR [RCX]
		add r8, QWORD PTR [RDX + 08h]
		mov QWORD PTR [RCX + 08h], r8
		adc rax, QWORD PTR [RDX]
		mov QWORD PTR [RCX], rax

    ret

Adc128  ENDP


END

