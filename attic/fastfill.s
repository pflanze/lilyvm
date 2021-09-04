        .setcpu		"6502"
        .smart		on
        .autoimport	on
        .case		on
        .debuginfo	off
        .macpack	longbranch
        .export         _fastfill_base
        .export		_fastfill4pages

.segment	"DATA"

_fastfill_base:
        .byte $00

.segment	"CODE"

;; base page in fastfill_base, value in A; fills 4 pages (1024 bytes)
_fastfill4pages:
        ldx _fastfill_base
        stx insta0+2
        stx instb0+2
        inx
        stx insta1+2
        stx instb1+2
        inx
        stx insta2+2
        stx instb2+2
        inx
        stx insta3+2
        stx instb3+2
        ldx #0
loop:
insta0: sta $ff00,x
insta1: sta $ff00,x
insta2: sta $ff00,x
insta3: sta $ff00,x
        inx
instb0: sta $ff00,x
instb1: sta $ff00,x
instb2: sta $ff00,x
instb3: sta $ff00,x
        inx
        bne loop
        rts
