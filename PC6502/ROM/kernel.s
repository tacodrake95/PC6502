; PC6502 Kernel
; Written by Jake Drake
; Copyright 2019 Finger Blaster Studios

; Special Register Pointers

;ZERO .equ	$00

;Ptrs .equ	$20

;CallbackPtrs .equ	$40

GfxMode .equ	$60

CacheBank .equ	$70 ;Cache Bank
CachePage .equ	$71 ;Cache Page

ROMAddrL .equ	$72
ROMAddrH .equ	$73


PileALL .equ	$74 ;System State Header
PileALH	.equ	$75 ;
PileAHL .equ	$76 ;
PileAHH .equ	$77 ;

PileBLL .equ	$78 ;
PileBLH	.equ	$79 ;
PileBHL .equ	$7A ;
PileBHH .equ	$7B ;

PileCLL .equ	$7C ;
PileCLH	.equ	$7D ;
PileCHL .equ	$7E ;
PileCHH .equ	$7F ;

;RetPtrL .equ	$8C ;
;RetPtrH .equ	$8D ;

;StartPtrL .equ	$8E ;
;StartPtrH .equ	$8F ;System State Header

;Cache .equ	$90 ;Cache Window Entry Point

Gfx .equ $0F

kernel .org		$F000 ; Kernel entry point

	lda #$00		; Load init values
	
	sta PileALH		;
	sta PileAHL		;
	sta PileAHH		; 
	
	sta PileBLH		; 
	sta PileBHL		;
	sta PileBHH		;
	
	sta PileCLH		;
	sta PileCHL		;
	sta PileCHH		;
	
	sta CachePage	; Set cache page to 0
	
	sta ROMAddrH	; Set ROM page to 0
	sta ROMAddrL	; Set ROM bank to 0

	
	sta PileALL		; set A to 0th bank 
	
	lda #$01		;
	sta PileBLL		; Set B to 1st bank
	
	lda #$02		;
	sta PileCLL		; Set C to 2nd bank
	
	lda #$00
	sta $20			; Set LB of address offset
	
	lda #$20		;
	sta $21			; Set HB of address offset
	
	lda #Gfx
	sta GfxMode
	
	
	lda #$7F
	ldx #$FF
	ldy #$00

loop sta ($20),Y
	 iny
	 dex
	 bne loop
	
end jmp end ; End

		