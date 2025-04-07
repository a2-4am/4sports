;license:BSD-3-Clause
;extended open/read/write binary file in ProDOS filesystem, with random access
;copyright (c) Peter Ferrie 2013-2020

                ;zpage usage, arbitrary selection except for the "ProDOS constant" ones
                ;feel free to move them around

!if read_only=0 {
                sparseblk  = $50        ;(internal) last-read block was sparse
}
                sizelo    = $52         ;set if enable_write=1 and writing, or reading, or if enable_seek=1 and seeking
                sizehi    = $53         ;set if enable_write=1 and writing, or reading, or if enable_seek=1 and seeking
                reqcmd    = $54         ;set (read/write/seek) if enable_write=1 or enable_seek=1
                                        ;if allow_multi=1, bit 7 selects floppy drive in current slot (clear=drive 1, set=drive 2) during open call
                                        ;bit 7 must be clear for read/write/seek on opened file
                ldrlo     = $55         ;set to load address if override_adr=1
                ldrhi     = $56         ;set to load address if override_adr=1
                namlo     = $57         ;name of file to access
                namhi     = $58         ;name of file to access


                command   = $42         ;ProDOS constant
                unit      = $43         ;ProDOS constant
                adrlo     = $44         ;ProDOS constant
                adrhi     = $45         ;ProDOS constant
                bloklo    = $46         ;ProDOS constant
                blokhi    = $47         ;ProDOS constant

                scratchlo = $48         ;(internal)
                scratchhi = $49         ;(internal)

                entries   = $3f         ;(internal) total number of entries in directory

                treeidx   = $59         ;(internal) index into tree block
                treeblklo = $5a
                treeblkhi = $5b
                blkidx    = $5c         ;(internal) index into sapling block list
                !set last_zp = $5c      ;highest address to save if swap_zp enabled (max 127 entries later)

                ;constants
                cmdseek   = 0           ;requires enable_seek=1
                cmdread   = 1           ;requires enable_write=1
                cmdwrite  = 2           ;requires enable_write=1
                SETKBD    = $fe89
                SETVID    = $fe93
                DEVNUM    = $bf30
                MLI       = $bf00
                NAME_LENGTH = $4        ;ProDOS constant
                MASK_SUBDIR = $d0       ;ProDOS constant
                MASK_ALL    = $f0       ;ProDOS constant
                KEY_POINTER = $11       ;ProDOS constant
                EOF_LO    = $15         ;ProDOS constant
                EOF_HI    = $16         ;ProDOS constant
                AUX_TYPE  = $1f         ;ProDOS constant
                ENTRY_SIZE = $27        ;ProDOS constant
                NEXT_BLOCK_LO = $2      ;ProDOS constant
                NEXT_BLOCK_HI = $3      ;ProDOS constant
                SAPLING   = $20         ;ProDOS constant
                FILE_COUNT = $25        ;ProDOS constant
                DEVADR01HI = $bf11      ;ProDOS constant
                LCBANK1RO = $c080
                ROMIN     = $c081
                LCBANK1   = $c083
                LCBANK2   = $c089

                first_zp  = $40         ;lowest address to save if swap_zp enabled
                                        ;last_zp is calculated automatically

.init           lda     DEVNUM
                sta     .x80_parms + 1
                sta     .unrunit1 + 1
                and     #$70
                pha
                lda     #1
                sta     namlo
                lda     #$80
                sta     namhi

                ;fetch path, if any

                jsr     MLI
                !byte   $c7
                !word   .c7_parms
                ldx     $8000
                dex
                stx     sizelo
                sec
                bmi     +++

                ;find current directory name in directory

                php

.readblock       jsr     MLI
                !byte   $80
                !word   .x80_parms

                lda     #<(.readbuff + NAME_LENGTH)
                sta     scratchlo
                lda     #>(.readbuff + NAME_LENGTH)
                sta     scratchhi
.inextent        ldy     #0
                lda     (scratchlo), y
                pha
                and     #$0f
                tax
--              iny
                lda     (scratchlo), y
                cmp     (namlo), y
                beq     .ifoundname

                ;match failed, move to next directory in this block, if possible

-               pla

.skiphdr         clc
                lda     scratchlo
                adc     #ENTRY_SIZE
                sta     scratchlo
                bcc     +

                ;there can be only one page crossed, so we can increment instead of adc

                inc     scratchhi
+               cmp     #<(.readbuff + $1ff) ;4 + ($27 * $0d)
                lda     scratchhi
                sbc     #>(.readbuff + $1ff)
                bcc     .inextent

                ;read next directory block when we reach the end of this block

                lda     .readbuff + NEXT_BLOCK_LO
                ldx     .readbuff + NEXT_BLOCK_HI
                bcs     +

.ifoundname      dex
                bne     --

                ;parse path until last directory is seen

                iny
                lda     (namlo), y
                cmp     #$2f ;'/'
                bne     -
                pla
                and     #$20 ;Volume Directory Header XOR subdirectory
                beq     .adjpath
                pla
                clc
                php
                lsr
                bcc     .skiphdr
                inx

.adjpath         tya
                eor     #$ff
                adc     sizelo
                sta     sizelo
                clc
                tya
                adc     namlo
                sta     namlo
                dex
                beq     ++

                ;cache block number of current directory
                ;as starting position for subsequent searches

                ldy     #(KEY_POINTER + 1)
                lda     (scratchlo), y
                tax
                dey
                lda     (scratchlo), y
                sta     .unrhddblocklo + 1
                stx     .unrhddblockhi + 1
+               sta     .x80_parms + 4
                stx     .x80_parms + 5
++              lda     sizelo
                bne     .readblock
                pla

                ;unit to slot for ProDOS interface

+++             pla
                lsr
                lsr
                lsr
                tay
                ldx     DEVADR01HI, y
                cpx     #$c8
                bcc     .set_slot1
                php

                ;find SmartPort device for basic MicroDrive support

                ldx     #$c8
-               dex
                stx     scratchhi
                ldy     #0
                sty     scratchlo
                iny
                lda     (scratchlo), y
                cmp     #$20
                bne     -
                iny
                iny
                lda     (scratchlo), y
                bne     -
                iny
                iny
                lda     (scratchlo), y
                cmp     #3
                bne     -
                ldy     #$ff
                lda     (scratchlo), y
                beq     -

.set_slot        plp
.set_slot1       stx     .slot + 2
                stx     .unrentry + 2
.slot            ldx     $cfff
                stx     .unrentry + 1
                bcs     ++
+               jmp     .bankram

++              ldy     #$8c ;STY
!if read_only=0 {
                sty     .unrcommand1
}
                sty     .unrcommand3
                lda     #<.pcommand
!if read_only=0 {
                sta     .unrcommand1 + 1
}
                sta     .unrcommand3 + 1
                lda     #>.pcommand
!if read_only=0 {
                sta     .unrcommand1 + 2
}
                sta     .unrcommand3 + 2
                iny      ;STA
                sty     .unrblokhi1
                sty     .unrunit1 + 2
                iny     ;STX
                sty     .unrbloklo1
                ;;lda     #>.pblock
                sta     .unrbloklo1 + 2
!if read_only=0 {
                sta     .unrbloklo2 + 2
}
                ;;lda     #>(.pblock + 1)
                sta     .unrblokhi1 + 2
!if read_only=0 {
                sta     .unrblokhi2 + 2
                sta     .unrblokhi3 + 2
}
                ;;lda     #>.paddr
                sta     .unrunit1 + 4
                ldy     #<.pblock
                sty     .unrbloklo1 + 1
!if read_only=0 {
                sty     .unrbloklo2 + 1
}
                iny
                sty     .unrblokhi1 + 1
!if read_only=0 {
                sty     .unrblokhi2 + 1
                sty     .unrblokhi3 + 1
}
                lda     #$a5 ;LDA
                sta     .unrunit1
!if read_only=0 {
                lda     #$ee ;INC
                sta     .unrblokhi2
                ldy     #$ad ;LDA
                sty     .unrblokhi3
                iny ;LDX
                sty     .unrbloklo2
}
                lda     #adrlo
                sta     .unrunit1 + 1
                lda     #<.paddr
                sta     .unrunit1 + 3

                ;use SmartPort entrypoint instead

                inx
                inx
                inx
                stx     .unrentry + 1

                ldx     #2
                stx     .x80_parms + 4
                lda     #0
                sta     .x80_parms + 5
                jsr     MLI
                !byte   $80
                !word   .x80_parms
                sta     .readbuff + $205
                lda     #cmdread
                sta     .unrpcommand
                lda     #$ea
                sta     .hackstar

.iterunit        inc     .unrunit2
                jsr     .unrentry
                bcs     .iterunit

+               ldy     #$10
-               lda     .readbuff + 3, y
                cmp     .readbuff + $203, y
                bne     .iterunit
                dey
                bne     -
                lda     #$68
                sta     .hackstar
                lda     #<.packet
                sta     .unrppacket
                lda     #>.packet
                sta     .unrppacket + 1

.bankram
!if is_sundog=1 {
                ldx     #>(.hddcodeend + $ff - .patch_jsr_swap_zpg)
                ldy     #0
.multicopy
-               lda     .unrelochdd, y
                sta     .reloc, y
                iny
                bne     -
                inc     .multicopy + 2
                inc     .multicopy + 5
                dex
                bne     .multicopy
}
!if disks>2 {
                lda     #disks
.find_vols
                pha
}
                jsr     .hddopendir
!if disks>1 {
                inc     filename_e - 1
  !if disks=2 {
                jsr     .hddopendir
  } else {
                pla
                sec
                sbc     #1
                bne     .find_vols
  }
}
