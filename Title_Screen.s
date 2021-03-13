.export _Title_Screen_pal
.export _Title_Screen_rle


;;{pal:"nes",layout:"nes"};;
_Title_Screen_pal:
 .byte $0F,$10,$00,$30,$1C,$01,$21,$31
 .byte $1C,$16,$30,$06,$1C,$09,$19,$29
 
;;
;;{w:32,h:30,bpp:8,comp:"rletag",map:"nesnt"};;
_Title_Screen_rle:
	.byte $00,$10,$00,$fe,$10,$00,$fe,$10,$00,$fe,$10,$00,$c2,$ff,$00,$37
	.byte $0f,$00,$06,$0f,$00,$00




