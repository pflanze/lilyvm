	.setcpu		"6502"
	.smart		on
	.autoimport	on
	.case		on
	.debuginfo	off
	.importzp	sp, sreg, regsave, regbank
	.importzp	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
	.macpack	longbranch
	.import		_alloc_ptr
        .import         _vm_errno
        .export         _fastcons
        .export         new_zp

aptr = $FB                      ; kept equal to _alloc_ptr

;;; cons(val a, val b)
;;; a in , b in ax
;;;     -- ehr, direct sometimes i say man. no need to copy.wheninlin
_fastcons:
        sta ptr2+0
        stx ptr2+1
        ldy #0
        lda (sp),y
        sta ptr1+0
        iny
        lda (sp),y
        sta ptr1+1
        jsr incsp2
        ldy #1                  ; TYPE_PAIR
        ldx #2                  ; numfields

new_zp: 
        ;; new_zp(y=type, x=numfields[1..4], ptr1...[up to ptr4]) -> ax
        inx
        txa
        asl
        ;; https://codebase64.org/doku.php?id=base:inverse_subtraction
        eor #$ff
        sec
        adc _alloc_ptr+0
        sta aptr+0
        sta _alloc_ptr+0        ; could avoid copying if always using aptr
        bcc __slow              ; bcc bcs XX?
        lda _alloc_ptr+1
__cont: 
        sta aptr+1

        ;; set head
        tya                     ; type
        ldy #0
        sta (aptr),y
        dex                     ; numwords
        txa
        iny
        sta (aptr),y
        
        ;; copy ptr1...ptr4, stopping when done
        lda ptr1+0
        iny
        sta (aptr),y
        lda ptr1+1
        iny
        sta (aptr),y
        dex
        beq __end

        lda ptr2+0
        iny
        sta (aptr),y
        lda ptr2+1
        iny
        sta (aptr),y
        dex
        beq __end

        lda ptr3+0
        iny
        sta (aptr),y
        lda ptr3+1
        iny
        sta (aptr),y
        dex
        beq __end

        lda ptr4+0
        iny
        sta (aptr),y
        lda ptr4+1
        iny
        sta (aptr),y
__end:  
        ;; return aptr
        lda aptr+0
        ldx aptr+1
        rts
__slow:
        ;; decrement allocptr hi byte, and if too low, do GC, and if
	;; that fails, return an error
        dec _alloc_ptr+1
        lda _alloc_ptr+1
        cmp _alloc_area+1
        bpl __cont              ; alloc_ptr+1 >= alloc_area+1
        ;; ^ XX inexact! HAVE TO PAGE-ALIGN ALLOCATIONS *!*

        ;; GC would be called here.

        ;;  error: set vm_errno and return 0
        lda #1
        sta _vm_errno+0
        lda #0
        sta _vm_errno+1
        lda #0
        tax
        rts

