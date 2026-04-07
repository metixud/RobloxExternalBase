.code

; quicker than NtReadVirtualMemory & NtWriteVirtualMemory marginally.

Luck_ReadVirtualMemory PROC
	mov r10, rcx
	mov eax, 63
	syscall
	ret
Luck_ReadVirtualMemory ENDP

Luck_WriteVirtualMemory PROC
	mov r10, rcx
	mov eax, 58
	syscall
	ret
Luck_WriteVirtualMemory ENDP

END