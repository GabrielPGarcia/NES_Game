.export _Title_Screen_pal
.export _Title_Screen_rle


;;{pal:"nes",layout:"nes"};;
_Title_Screen_pal:
.byte $0F,$31,$30,$06,$1C,$0A,$31,$1C
.byte $1C,$0C,$38,$21,$1C,$09,$19,$07

;;
;;{w:32,h:30,bpp:8,comp:"rletag",map:"nesnt"};;
_Title_Screen_rle:
.byte $01,$10,$10,$11,$01,$05,$10,$01,$15,$14,$15,$10,$11,$01,$06,$10
.byte $01,$0a,$14,$15,$11,$01,$03,$14,$15,$10,$11,$10,$24,$25,$11,$01
.byte $05,$10,$11,$11,$10,$01,$09,$24,$25,$11,$01,$03,$24,$25,$10,$11
.byte $10,$01,$02,$11,$01,$03,$10,$11,$01,$03,$10,$01,$0c,$11,$01,$02
.byte $10,$01,$02,$11,$11,$10,$01,$02,$11,$11,$10,$01,$03,$11,$11,$10
.byte $01,$0c,$14,$15,$11,$11,$10,$01,$02,$11,$11,$10,$10,$11,$11,$10
.byte $01,$04,$11,$10,$01,$02,$14,$15,$10,$01,$07,$24,$25,$10,$11,$11
.byte $10,$01,$02,$11,$11,$10,$11,$11,$10,$10,$14,$15,$10,$01,$04,$24
.byte $25,$10,$01,$05,$11,$10,$01,$03,$11,$11,$10,$01,$03,$11,$10,$11
.byte $01,$02,$10,$24,$25,$10,$01,$0c,$11,$10,$01,$06,$14,$15,$10,$11
.byte $10,$11,$01,$02,$10,$01,$0e,$11,$11,$10,$01,$06,$24,$25,$10,$11
.byte $10,$10,$11,$11,$10,$01,$19,$11,$11,$10,$11,$01,$02,$10,$01,$13
.byte $14,$15,$10,$01,$03,$11,$10,$10,$11,$01,$03,$10,$01,$12,$24,$25
.byte $10,$01,$06,$11,$01,$02,$10,$01,$05,$00,$01,$0b,$10,$01,$09,$11
.byte $01,$03,$10,$01,$05,$00,$01,$0b,$10,$01,$09,$11,$10,$01,$08,$00
.byte $01,$0b,$10,$01,$13,$00,$01,$0b,$10,$01,$13,$00,$01,$0b,$10,$01
.byte $13,$00,$01,$0b,$10,$01,$1e,$14,$15,$10,$01,$02,$11,$10,$01,$07
.byte $11,$01,$03,$10,$01,$0d,$24,$25,$10,$10,$11,$11,$10,$01,$06,$11
.byte $01,$02,$10,$11,$11,$10,$01,$10,$11,$10,$01,$07,$11,$01,$02,$10
.byte $10,$11,$10,$01,$0d,$11,$01,$04,$10,$01,$0b,$11,$10,$01,$0d,$11
.byte $10,$11,$11,$10,$01,$09,$14,$15,$10,$11,$10,$01,$0b,$11,$01,$08
.byte $10,$01,$03,$11,$10,$10,$24,$25,$10,$10,$11,$10,$01,$0a,$11,$01
.byte $03,$10,$10,$11,$01,$03,$10,$01,$02,$11,$11,$10,$01,$04,$11,$10
.byte $01,$0a,$11,$01,$03,$10,$01,$04,$11,$11,$10,$01,$02,$11,$01,$02
.byte $10,$01,$02,$11,$10,$01,$0c,$11,$10,$01,$02,$14,$15,$10,$10,$11
.byte $10,$01,$03,$11,$01,$06,$10,$01,$0f,$24,$25,$10,$10,$11,$10,$01
.byte $09,$11,$11,$10,$11,$11,$10,$01,$1b,$11,$01,$05,$10,$01,$0f,$ff
.byte $01,$02,$f3,$00,$01,$03,$ff,$01,$03,$00,$01,$03,$ff,$01,$03,$33
.byte $00,$01,$02,$ff,$ff,$bb,$aa,$aa,$e2,$f0,$f0,$ff,$ff,$fb,$fa,$fa
.byte $fe,$ff,$01,$11,$0f,$01,$06,$0f,$01,$00