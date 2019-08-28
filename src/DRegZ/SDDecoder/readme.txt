
Files lcvec.c/.h are created by DRegZ with option -P (see help).

They contain the public key to be embedded and compiled within
the decoder.

=======================================================================

example:


; create a keypair named testkey

>dregz -c testkey

DRegZ V1.01, written by Giuliano Bertoletti
Copyright (C) 2005 GBE 32241 Software PR. All rights reserved
Last revision Jun 14 2008 - 09:15:26

Creating key...
Jolting public key from private...
You may enter a password now in order to protect your private key file,
or hit ESC to leave it in plain text

Password:
Confirm :


Saving private key: testkey.prv
Saving public key testkey.pub


; generate source C code for that key

>dregz -P testkey.prv c .

DRegZ V1.01, written by Giuliano Bertoletti
Copyright (C) 2005 GBE 32241 Software PR. All rights reserved
Last revision Jun 14 2008 - 09:15:26

Loading public key testkey.prv
Creating some samples codes...
Writing file .\lcvec.h
Writing file .\lcvec.c

; now you can compile the decoder and test

