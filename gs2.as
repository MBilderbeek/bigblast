; Functions to load GS2 stuff - in assembly for speed

OPEN	equ	00Fh
CLOSE	equ	010h
RBLKRD	equ	027h
SETDTA	equ	01Ah
BDOS	equ	00005h

LINWRK	equ	0FC18h
CPYDST	equ	LINWRK
CPYPG	equ	LINWRK+2
SCRMOD	equ	0FCAFh
DTASIZE	equ	512

	psect	text
	global	_gs2loadgrp,_gs2loadpal
	global	csv,cret

; int gs2loadgrp(char *filename, uchar page)
;
; loads a GraphSaurus ("bload,s") image into the page 'page'
;

; return/error codes
E_PAGE	equ	-1
E_OPEN	equ	-2
E_READ	equ	-3

; parameter indices
FILENM	equ	6
PAGE    equ	8

_gs2loadgrp:
	call	csv
	
	; prepare graphic stuff

	ld	hl,0
	ld	a,(SCRMOD)
	cp	7
	ld	a,(ix+PAGE)
	jr	nc,sc78
sc56:
	cp	4
	jr	nc,err
	ccf
	rra
	rr	h
	jr	goon
sc78:
	cp	2
	jr	c,goon
err:
	ld	hl,E_PAGE
	jp	cret
goon:
	ld	(CPYPG),a	; physical VRAM page
	ld	(CPYDST),hl	; VRAM offset (0000h or 80000h)

	; set Disk buffer
	push	ix
	push	iy
	ld	de,_dta
	ld	c,SETDTA
	call	BDOS
	pop	iy
	pop	ix
	
	; open file
	ld	l,(ix+FILENM)
	ld	h,(ix+FILENM+1)
	call	openf
	and	a
	ld	hl,E_OPEN
	jp	nz,cret

	; skip BLOAD header...
	push	ix
	push	iy
	ld	hl,7
	ld	de,_fcb
	ld	c,RBLKRD
	call	BDOS
	pop	iy
	pop	ix
	and	a
	ld	hl,E_READ
	jp	nz,clos2

	; determine filesize
	ld	de,(_dta+1)
	ld	hl,(_dta+3)
	sbc	hl,de
	ld	b,h
	ld	c,l
	
	; now start the real loading...
load:
	ld	hl,DTASIZE
	ld	a,h		; more than DTASIZE bytes to read?
	cp	b
	jr	c,cpy
small:
	ld	h,b
	ld	l,c
cpy:
	push	bc
	push	ix
	push	iy
	ld	de,_fcb
	ld	c,RBLKRD
	call	BDOS
	pop	iy
	pop	ix
	push	hl		; the real number of bytes read
	and	a
	ld	hl,E_READ
	jp	nz,close

	; copy read data to VRAM
	ld	a,(CPYPG)
	ld	b,a
	ld	hl,(CPYDST)	; hl gets VRAM address
	ld	c,a		; VDP #14 get 3 top bits of dest
	ld	a,h		; addr in bits 0-2. 
	and	0C0h
	or	b
	rlca
	rlca
	di
	out	(099h),a
	ld	a,08Eh		; register 14
	out	(099h),a
	ld	a,l		; rest of address
	out	(099h),a
	ld	a,h		; rest of address
	and	03Fh
	or	040h
	out	(099h),a
	; now the VDP is ready to receive VRAM data
	pop	de		; push-ed as hl!! - nr of bytes read
	ld	hl,_dta		; from file
	ld	b,e
	ld	c,098h
	xor	a
	or	b		; if e != 0, OUT e bytes
	jr	z,2f
	otir
2:	ld	a,d		; now OUT d blocks of 256 bytes
	ld	b,0
3:	otir
	dec	a
	jr	nz,3b
	ei			; presto
	ld	hl,(CPYDST)	; save new VRAM address
	add	hl,de
	ld	(CPYDST),hl

	; determine next block of data to read
	pop	hl		; pushed as bc!! number of bytes to go
	xor	a
	sbc	hl,de		; subtract nr of bytes read
	ld	b,h		; store in bc
	ld	c,l
	ld	a,b
	or	c
	jp	nz,load
	ld	hl,0
	jr	clos2
	
close:	pop	hl
	pop	bc
clos2:	push	hl
	call	closef
	pop	hl
	
quit:	jp	cret

		
;-------------------------------------------------------------------
; int gs2loadpal(char *filename, int *palette)

PALET	equ	8
FNAME	equ	6
_gs2loadpal:
	call	csv

	push	iy
	push	ix
	ld	e,(ix+PALET)
	ld	d,(ix+PALET+1)
	ld	c,SETDTA
	call	BDOS
	pop	ix
	
	; open file
	ld	l,(ix+FNAME)
	ld	h,(ix+FNAME+1)
	push	ix
	call	openf
	pop	ix
	pop	iy
	and	a
	ld	hl,E_OPEN
	jp	nz,cret

	; read palette data
	push	iy
	push	ix
	ld	de,_fcb
	ld	hl,32	; read the 1st palette
	ld	c,RBLKRD
	call	BDOS
	push	af
	call	closef
	pop	af
	pop	ix
	pop	iy
	and	a
	ld	hl,E_READ
	jp	nz,cret

	; now set palette
;	ld	hl,_dta
	ld	l,(ix+PALET)
	ld	h,(ix+PALET+1)

	push	ix
	push	iy
	di
	ld	c,099h
	xor	a
	out	(c),a
	ld	a,090h
	out	(c),a
	inc	c
	ld	b,32
	otir
	ei
	pop	iy
	pop	ix
	ld	hl,0
	jp	cret

;-------------------------------------------------------------------
openf:	; open file with filename.ext in (hl)
	; return with file  opened, error code of BDOS in a

	; clear FCB
	xor	a
	ld	de,_fcb
	ld	(de),a
	ld	a,32 ; spaces
	inc	de
	ld	(de),a
	inc	de
	push	hl
	ld	hl,_fcb+1
	ld	bc,11
	ldir
	xor	a
	ld	(hl),a
	ld	bc,23
	ldir
	pop	hl

	; copy file name into FCB
	ld	b,8
	ld	de,_fcb+1
_fname:
	ld	a,(hl)
	cp	'.'
	jr	z,_fext
	cp	0
	jr	z,_fext1
	ld	(de),a
	inc	hl
	inc	de
	djnz	_fname
	
_fext:	inc	hl
_fext1:
	ld	de,_fcb+9
	ld	b,3
_fext2:	ld	a,(hl)
	cp	0
	jr	z,_open
	ld	(de),a
	inc	de
	inc	hl
	djnz	_fext2

_open:	ld	de,_fcb
	ld	c,OPEN
  	push	ix
	push	iy
	call	BDOS
	pop	iy
	pop	ix
	ld	c,a
	ld	hl,0
	ld	(_fcb+12),hl	; just to be sure
	ld	(_fcb+33),hl
	ld	(_fcb+35),hl
	inc	hl	
	ld	(_fcb+14),hl
	ld	a,c
	ret


;-------------------------------------------------------------------
closef:
	xor	a
	ld	de,_fcb
	ld	c,CLOSE
	call	BDOS
	ret

	psect   data
_fcb:	defs	40
_dta:	defs	DTASIZE

