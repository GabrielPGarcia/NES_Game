.export _Level_3E_pal
.export _Level_3E_rle


;;{pal:"nes",layout:"nes"};;
_Level_3E_pal:
 .byte $0F,$1D,$30,$30,$1C,$01,$21,$31
 .byte $1C,$16,$30,$06,$1C,$17,$19,$29
 
;;
;;{w:32,h:30,bpp:8,comp:"rletag",map:"nesnt"};;
_Level_3E_rle:	
	.byte $04,$02,$04,$3f,$01,$02,$04,$02,$01,$01,$02,$04,$02,$01,$04,$03
	.byte $02,$04,$02,$01,$04,$02,$02,$02,$01,$04,$02,$02,$02,$01,$01,$02
	.byte $02,$01,$01,$02,$04,$1f,$00,$04,$19,$03,$00,$04,$2c,$03,$00,$04
	.byte $38,$03,$00,$04,$0c,$03,$00,$04,$06,$03,$00,$04,$05,$03,$00,$04
	.byte $23,$03,$00,$04,$23,$03,$00,$04,$08,$03,$00,$04,$19,$03,$00,$04
	.byte $46,$03,$00,$04,$03,$03,$00,$04,$29,$03,$00,$00,$03,$00,$04,$07
	.byte $03,$00,$04,$0d,$03,$00,$04,$56,$03,$00,$04,$03,$03,$00,$04,$06
	.byte $03,$00,$04,$05,$03,$00,$04,$02,$03,$00,$04,$74,$03,$00,$04,$37
	.byte $02,$04,$1f,$01,$02,$04,$03,$01,$04,$02,$02,$02,$01,$04,$02,$02
	.byte $04,$03,$01,$01,$02,$04,$02,$01,$04,$02,$02,$04,$02,$01,$01,$02
	.byte $01,$02,$04,$3f,$00,$04,$3e,$00,$04,$00

