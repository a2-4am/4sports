;license:BSD-3-Clause
;extended open/read/write binary file in ProDOS filesystem, with random access
;copyright (c) Peter Ferrie 2013-2020

                ;read volume directory key block
                ;self-modified by init code

.hddopendir
.unrhddblocklo = *
                ldx     #2
.unrhddblockhi = *
                lda     #0
.hddreaddir1     jsr     .hddreaddirsel

.hddfirstent     lda     #NAME_LENGTH
                sta     scratchlo
                lda     #>(pas_hdddirbuf - 1)
                sta     scratchhi

                ;there can be only one page crossed, so we can increment here

.hddnextent1     inc     scratchhi
.hddnextent      ldy     #0

                ;match name lengths before attempting to match names

                lda     (scratchlo), y
                and     #$0f
                tax
                inx
-               cmp     filename, y
                beq     .hddfoundname

                ;match failed, move to next entry in this block, if possible

+               clc
                lda     scratchlo
                adc     #ENTRY_SIZE
                sta     scratchlo
                bcs     .hddnextent1
                cmp     #$ff ;4 + ($27 * $0d)
                bne     .hddnextent

                ;read next directory block when we reach the end of this block

                ldx     pas_hdddirbuf + NEXT_BLOCK_LO
                lda     pas_hdddirbuf + NEXT_BLOCK_HI
                bcs     .hddreaddir1

.hddfoundname    iny
                lda     (scratchlo), y
                dex
                bne -

                stx     .zp_array + treeidx - first_zp
                stx     .zp_array + blkidx - first_zp

                ;fetch KEY_POINTER

!if disks>1 {
                ldx     .vol_idx
                inc     .vol_idx
}
                ldy     #KEY_POINTER
                lda     (scratchlo), y
!if disks=1 {
                sta     .zp_array + treeblklo - first_zp
} else {
                sta     .blokloarray, x
}
                iny
                lda     (scratchlo), y
!if disks=1 {
                sta     .zp_array + treeblkhi - first_zp
} else {
                sta     .blokhiarray, x
}
                ldy     #>pas_hddtreebuf
                jmp     .hddreaddirsect

!if disks>1 {
.vol_idx         !byte   0
}

.c7_parms        !byte   1
                !word   $8000

.x80_parms       !byte   3, $d1
                !word   .readbuff, 2
