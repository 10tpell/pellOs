.globl memcpy
memcpy:
	mov x5, #64
	udiv x3, x2, x5
	msub x4, x2, x5, x2

	subs x4, x4, #0
	b.eq u64
	b memcpy_u8
u64:
	b memcpy_u64

/* void memcpy_u64(uint64_t* dest, uint64_t* src, uint64_t size) */
.globl memcpy_u64
memcpy_u64:
	ldr x3, [x1], #8
	str x3, [x0], #8
	subs x2, x2, #8
	b.gt memcpy_u64
	ret

.globl memcpy_u8
memcpy_u8:
	ldrb w3, [x1], #1
	strb w3, [x0], #1
	subs x2, x2, #1
	b.gt memcpy_u8
	ret

.globl strcpy
strcpy:
	ldr w2, [x1], #1
	str w2, [x0], #1

	cmp w2, wzr
	b.gt strcpy
	ret

.globl memzero
memzero:
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero
	ret

.globl memzero_u8
memzero_u8:
	str wzr, [x0], #1
	subs x1, x1, #1
	b.gt memzero_u8
	ret

.globl memset
memset:
    str x1, [x0], #8
    subs x2, x2, #8
    b.gt memset

/* sint8_t strcmp(char* a, char* b); */
.globl strcmp
strcmp:
	ldr x3, [x0], #8
	ldr x4, [x1], #8
	
	cmp x3, xzr
	b.eq zero

	cmp x4, xzr
	b.eq zero
	
	subs x2, x3, x4
	b.eq strcmp
	mov x0, x2
	ret
zero:
	sub x0, x3, x4
	ret  