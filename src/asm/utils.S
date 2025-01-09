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

/* void strcpy_s(const char* dest, uint64_t destsz, const char* src); */
.globl strcpy_s
strcpy_s:
	cmp x1, xzr
	b.eq ret
	sub x1, x1, #1

	ldr w3, [x2], #1
	str w3, [x0], #1

	cmp w3, wzr
	b.gt strcpy_s

ret:
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

/* sint8_t strcmp(char* a, uint32_t size, char* b); */
.globl strcmp
strcmp:
	ldrb w3, [x0], #1
	ldrb w4, [x1], #1
	
	cmp w3, wzr
	b.eq zero

	cmp w4, wzr
	b.eq zero
	
	subs w2, w3, w4
	b.eq strcmp
	mov w0, w2
	ret
zero:
	sub w0, w3, w4
	ret  