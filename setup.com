$ define z80$pun	z80.punch
$ define z80$rdr	z80.rdr
$ define z80$lpt	z80.lpt
$! define z80$boot	TRS80.HEX
$ define z80$boot	Z80TST.HEX
$ define z80$drive_a    DRIVE.A
$ define Z80$CONSOLE	'f$trnlnm("TT")'
$ define z80$term       _tta0:
$ !define dbg$input      _tta0:
$ !define dbg$output      _tta0:
$ set proc/priv=all
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! debug
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ define/group z80$dbg   "3"
$ define Z80$TRACEORDER  "DI"
$ define Z80$TRCLVL      0
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$ set terminal _TTA0:/PERMANENT -
	/noECHO -
	/noLINE_EDITING -
	/PASTHRU	-
	/NOTTSYNC	-
	/noTYPE_AHEAD   -
	/NOTAB		-

$!
	
