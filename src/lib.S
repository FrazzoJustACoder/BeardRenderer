.global _strncpy
.global _memset4

.intel_syntax noprefix

_strncpy:
	pushd	esi
	pushd	edi
	mov	edi, [esp+8+4]
	mov	esi, [esp+8+8]
	mov	eax, [esp+8+12]
	mov	ecx, eax
	shr	ecx, 2
	jz	L_snc_no4
	rep	movsd
L_snc_no4:
	mov	ecx, eax
	and	ecx, 3
	jz	L_snc_no1
	rep	movsb
L_snc_no1:
	pop	edi
	pop	esi
	ret

_memset4:
	pushd	edi
	mov	edi, [esp+4+4]
	mov	eax, [esp+4+8]
	mov	ecx, [esp+4+12]
	rep	stosd
	pop	edi
	ret
