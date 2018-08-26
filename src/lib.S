.global _strncpy
.global _memset
.global _memset2
.global _memset3
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

_memset:
	pushd	edi
	mov	edi, [esp+4+4]
	mov	eax, [esp+4+8]
	mov	ecx, [esp+4+12]
	mov	ah, al
	mov	edx, eax
	shl	eax, 16
	mov	ax, dx
	mov	edx, ecx
	shr	ecx, 2
	jz	L_ms_no4
	rep	stosd
L_ms_no4:
	mov	ecx, edx
	and	ecx, 3
	jz	L_ms_no1
	rep	stosb
L_ms_no1:
	pop	edi
	ret

_memset2:
	pushd	edi
	mov	edi, [esp+4+4]
	mov	eax, [esp+4+8]
	mov	ecx, [esp+4+12]
	mov	edx, eax
	shl	eax, 16
	mov	ax, dx
	mov	edx, ecx
	shr	ecx, 1
	jz	L_ms_no4
	rep	stosd
L_ms2_no4:
	test	edx, 1
	jz	L_ms2_no2
	stosw
L_ms2_no2:
	pop	edi
	ret

_memset3:
	push	edi
	mov	edi, [esp+4+4]
	mov	eax, [esp+4+8]
	mov	ecx, [esp+4+12]
	mov	edx, eax
	shr	edx, 16
	push	ecx
	shr	ecx, 4
	jz	L_ms3_no16
L_ms3_loop16:
	mov	[edi], ax #0
	mov	[edi+2], dl
	mov	[edi+3], ax #1
	mov	[edi+5], dl
	mov	[edi+6], ax
	mov	[edi+8], dl
	mov	[edi+9], ax
	mov	[edi+11], dl
	mov	[edi+12], ax #4
	mov	[edi+14], dl
	mov	[edi+15], ax
	mov	[edi+17], dl
	mov	[edi+18], ax
	mov	[edi+20], dl
	mov	[edi+21], ax
	mov	[edi+23], dl
	mov	[edi+24], ax #8
	mov	[edi+26], dl
	mov	[edi+27], ax
	mov	[edi+28], dl
	mov	[edi+30], ax
	mov	[edi+31], dl
	mov	[edi+33], ax
	mov	[edi+34], dl
	mov	[edi+36], ax #12
	mov	[edi+37], dl
	mov	[edi+39], ax
	mov	[edi+40], dl
	mov	[edi+42], ax
	mov	[edi+43], dl
	mov	[edi+45], ax
	mov	[edi+46], dl
	add	edi, 3*16
	dec	ecx
	jnz	L_ms3_loop16
L_ms3_no16:
	mov	ecx, [esp]
	and	ecx, 15
	shr	ecx, 2
	jz	L_ms3_no4
L_ms3_loop4:
	mov	[edi], ax
	mov	[edi+2], dl
	mov	[edi+3], ax
	mov	[edi+5], dl
	mov	[edi+6], ax
	mov	[edi+8], dl
	mov	[edi+9], ax
	mov	[edi+11], dl
	add	edi, 3*4
	dec	ecx
	jnz	L_ms3_loop4
L_ms3_no4:
	pop	ecx
	and	ecx, 3
	jz	L_ms3_no1
L_ms3_loop1:
	mov	[edi], ax
	mov	[edi+2], dl
	add	edi, 3
	dec	ecx
	jnz	L_ms3_loop1
L_ms3_no1:
	pop	edi
	ret

_memset4:
	pushd	edi
	mov	edi, [esp+4+4]
	mov	eax, [esp+4+8]
	mov	ecx, [esp+4+12]
	rep	stosd
	pop	edi
	ret
