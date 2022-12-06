
.include "p30f6012a.inc"

.global		_read_EE1
.global		_write_EE1
.global		_write_EE_row1

.section	.text
_read_EE1:
	tblrdl	[W0],W1			;read
	mov		W1,W0			;set return value
	return

_write_EE1:
	mov		W0,W2			;erase
	mov		W0,NVMADR
	mov		#0x4044,W0
	mov		W0,NVMCON
	push	SR				;disable interrupt
	mov		#0xE0,W0
	ior		SR
	mov		#0x55,W0
	mov		W0,NVMKEY
	mov		#0xAA,W0
	mov		W0,NVMKEY
	bset	NVMCON,#WR
	pop		SR				;enable interrupt
	nop
	nop
L1:	btsc	NVMCON,#WR		;wait
	bra		L1

	mov		W2,W0			;write
	tblwtl	W1,[W0]
	mov		#0x4004,W0
	mov		W0,NVMCON
	push	SR				;disable interrupt
    mov		#0x00E0,W0
	ior		SR
	mov		#0x55,W0
	mov		W0,NVMKEY
	mov		#0xAA,W0
	mov		W0,NVMKEY
	bset	NVMCON,#WR
    pop		SR				;enable interrupt
	nop
	nop
L2:	btsc	NVMCON,#WR		;wait
	bra		L2
	return
	

_write_EE_row1:
	mov		W0,W2			;erase
	mov		W0,NVMADR
	mov		#0x4045,W0
	mov		W0,NVMCON
	push	SR				;disable interrupt
	mov		#0xE0,W0
	ior		SR
	mov		#0x55,W0
	mov		W0,NVMKEY
	mov		#0xAA,W0
	mov		W0,NVMKEY
	bset	NVMCON,#WR
	pop		SR				;enable interrupt
	nop
	nop
L3:	btsc	NVMCON,#WR		;wait
	bra		L3

	mov		W2,W0			;write
	mov		#16, W3	
L4:	tblwtl	[W1++],[W0++]
    dec     W3, W3
    bra     NZ, L4
	mov		#0x4005,W0
	mov		W0,NVMCON
	push	SR				;disable interrupt
    mov		#0x00E0,W0
	ior		SR
	mov		#0x55,W0
	mov		W0,NVMKEY
	mov		#0xAA,W0
	mov		W0,NVMKEY
	bset	NVMCON,#WR
    pop		SR				;enable interrupt
	nop
	nop
L5:	btsc	NVMCON,#WR		;wait
	bra		L5
	return
end:
