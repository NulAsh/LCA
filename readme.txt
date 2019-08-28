
License Codes Algorithms 
V0.02, release 07 November 2009
==================================================================

Copyright (C) 2003-2009 Giuliano Bertoletti & GBE32241 Software PR
All rights reserved.

See License.txt for information about licensing.

You can find the latest version of this package at:
http://www.webalice.it/giuliano.bertoletti/lca.html


Overview
--------

This package is a collection of algorithms (C/C++ source code included) 
which implement the generation and verification of license keys in the 
form of short strings of characters which might be easily printed on a 
CD label or spoken over a phone line.

The main objective is to creare strong codes difficult to forge without
the knowledge of a secret (private key) which is kept by the trusted 
entity emitting licenses.

Although well known cryptographic public key schemes are proven to be 
strong and secure, they are not particulary fit for this purpose because 
they generate strings of characters which are usually too long to be 
practical.

This suite is based on less known algorithms like HFE, Quartz and other 
variants which are better suited to this task.




Content
-------

Executables are built from source files and have the following purpose:

- QRegZ
  Main engine using 32 characters codes. The underlaying algorithm is
  based on the Quartz Signature Scheme. Good security (at least until 
  Quartz will be broken). It can create and verfify codes.

- QTestZ
  simple program which shows how to use the underlaying Quartz Signature
  Engine to apply 128-bit signatures to arbitrary messages (not related
  to the specific function of generating license codes).

- QDecZ
  Main decoder. Given a code and a public key it can verify if the code
  is valid. Logically portions of its code are included into a software
  application that needs to verify if a given code can be accepted.

- DRegZ
  Simpler and maybe weaker than QRegZ, it generates 25 characters codes
  using a scheme derived from Patarin's "Asymetric Crypto with S-Boxes" 
  scheme. 
  It's main advantage is that it requires a much simpler decoder to be 
  embedded in a software application. In particular, the public key is 
  outputted as C source code and can be directly compiled into the decoder.
  Supporting files for a C/C++ decoder are located in src\DRegZ\SDDecoder.
  Other languages might be supported in the future.

- SDDecoder
  this is a sample program which decodes and validates DRegZ generated codes
  (found in: src\DRegZ\SDDecoder, see readme.txt in that folder for 
  instructions on how to build it). Not available in the windows binaries only
  package.

- JRegZ
  this is an HFE based library which can be used to generate license code with
  features embedded like expiration date and selective features activation.



Codes generation (QRegZ and DRegZ)
----------------------------------

You need to create a keypair before you can generate a family of codes.
For every keypair there exists one and only one family of codes.

Each code in a family has an ID. You can generate up to 2^31 codes (or 
more depending on the algorithm) per family.
To different IDs correspond different codes and ideally every final user
has her own code (and ID).

You emit licenses by ID and it's up to you not to deliver the same code to
two different persons.




Algorithms
==========

The core algorithm is a trapdoor function similar to RSA but which operates on
shorter bit vectors (120-132 bits); its difficulty to invert is based on
the hardness of solving systems of multivariate quadratic equations over GF(2).

QRegZ implements a slightly different variant of the Quartz Signature Scheme
and basically signs a 32-bit counter producing a 32+128=160 bit license code
(Quartz signature is 128 bit long).

DRegZ implements a variant of Patarin's Asymmetric Cryptography with S-boxes
scheme and has a much simpler decoder.

JRegZ tries to combine both advantages i.e. strong security and simple decoding
plus flexibility, but being a library needs to be integrated into an user 
licensing application.

These algorithms may be subject to patent fees and it's up to the developer to
acquire proper permissions to use them in his projects (especially for commercial 
purposes).

See my website for further information:
http://www.webalice.it/giuliano.bertoletti/lca.html


References
==========

Hidden Field Equations Homepage (not up to date anymore)
http://www.cryptosystem.net/hfe/

Quartz Signature Scheme Homepage
http://www.minrank.org/quartz/



The author
===================

Giuliano Bertoletti
gbe32241@libero.it



















