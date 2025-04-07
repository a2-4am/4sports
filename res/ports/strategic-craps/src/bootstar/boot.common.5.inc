;license:BSD-3-Clause
;extended open/read/write binary file in ProDOS filesystem, with random access
;copyright (c) Peter Ferrie 2013-2020

.hddrdfile
.hddrdwrfile

.hddrdwrfilei
.hddrdwrloop
!if read_only=0 {
                ldy     #0
                sty     sparseblk
}
                ;fetch tree data block and read it

                ldx     treeblklo
                lda     treeblkhi
.patch_jsr_hddreaddirsel
                jsr     .hddreaddirsel
                ldy     treeidx
.patch_ldx_hdddirbuf1
                ldx     pas_hdddirbuf, y
.patch_lda_hdddirbuf1
                lda     pas_hdddirbuf + 256, y
.patch_jsr_hddseekrd2
                jsr     .hddseekrd

                ;fetch data block and read/write it

.hddskiptree     ldy     blkidx
.patch_ldx_hdddirbuf2
                ldx     pas_hdddirbuf, y
.patch_lda_hdddirbuf2
                lda     pas_hdddirbuf + 256, y
                pha
.patch_ora_hdddirbuf
                ora     pas_hdddirbuf, y
                cmp     #1
                pla
!if read_only=0 {
                ldy     reqcmd
                bcs     .hddseekrdwr
} else {
                bcs     .hddseekrd
}
.savebyte
                tay

.hddissparse
-               sta     (adrlo), y
                inc     adrhi
                sta     (adrlo), y
                dec     adrhi
                iny
                bne     -
!if read_only=0 {
                inc     sparseblk
}
.skipblk         rts

.hddreaddirsel
                ldy     #0
                sty     adrlo

.hddreaddirsec
.patch_ldy_hdddirbuf
                ldy     #>pas_hdddirbuf
.hddreaddirsect
                sty     adrhi
.hddseekrd       ldy     #cmdread
.hddseekrdwr
.unrcommand3 = .unrelochdd + (* - .patch_jsr_swap_zpg)
                sty     command
                nop ;allow replacing "sty command" with "sty pcommand" in extended SmartPort mode

.unrbloklo1 = .unrelochdd + (* - .patch_jsr_swap_zpg)
                stx     bloklo
                nop ;allow replacing "stx bloklo" with "stx pblock" in extended SmartPort mode
.unrblokhi1 = .unrelochdd + (* - .patch_jsr_swap_zpg)
                sta     blokhi
                nop ;allow replacing "sta blokhi" with "sta pblock + 1" in extended SmartPort mode
.unrunit1 = .unrelochdd + (* - .patch_jsr_swap_zpg)
                lda     #$d1
                sta     unit
                nop ;allow replacing "lda #$d1/sta unit" with "lda adrlo/sta paddr" in extended SmartPort mode

.hddwriteimm     lda     adrhi ;for Trackstar support
                pha
.patch_sta_paddr
                sta     .paddr + 1

.unrentry = .unrelochdd + (* - .patch_jsr_swap_zpg)
                jsr     $d1d1

.unrpcommand = .unrelochdd + (* - .patch_jsr_swap_zpg)
.pcommand        !byte   $2c ;hide packet in non-SmartPort mode
.unrppacket = .unrelochdd + (* - .patch_jsr_swap_zpg)
                !word   .unrelochdd + (.packet - .patch_jsr_swap_zpg)

.hackstar = .unrelochdd + (* - .patch_jsr_swap_zpg)
                pla
                sta     adrhi ;Trackstar does not preserve adrhi
                rts

.unrpacket = .unrelochdd + (* - .patch_jsr_swap_zpg)
.packet          !byte   3
.unrunit2 = .unrelochdd + (* - .patch_jsr_swap_zpg)
                !byte   0
.paddr           !word   .readbuff + $200
.pblock          !byte   2, 0, 0

.hddcodeend
!if disks>1 {
.blokloarray
.blokhiarray = .blokloarray + disks
.zp_array        = .blokhiarray + disks
} else {
.zp_array
}
zp_array        = .zp_array
.hdddataend      = .zp_array + 1 + last_zp - first_zp
