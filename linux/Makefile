# ---------------------------------------------------
# Linux/GCC Makefile for building LCA tools
# QRegZ QDecZ and DRegZ are build by default with make all
# SDDecoder needs files crated by DRegZ before it can be built
# so it's not built by default.
# Read ../src/DRegZ/SDDecoder/readme.txt to 
#

OUTPUTDIR      = ./builds
QREGMAIN = ../src/QRegZ
DREGMAIN = ../src/DRegZ
JREGMAIN = ../src/JRegZ
DCOMMON  = ../src/common
ROOTPATH = ../../..

vpath %.cpp $(ROOTPATH)/src/DRegZ:$(ROOTPATH)/src/JRegZ
vpath %.c   $(ROOTPATH)/src/QRegZ:$(ROOTPATH)/src/common:$(ROOTPATH)/src/DRegZ:$(ROOTPATH)/src/JRegZ:$(ROOTPATH)/src/DRegZ/SDDecoder:$(ROOTPATH)/src/JRegZ/jdecoder

CC  = gcc
CXX = g++
CFLAGS = -O3 -Wall -I$(ROOTPATH)/src/QRegZ -I$(ROOTPATH)/src/DRegZ -I$(ROOTPATH)/src/JRegZ -I$(ROOTPATH)/src/common
CXXFLAGS = $(CFLAGS)
LIBS = 

export CFLAGS

QREGZ = qzmain expoly expolymod ffpoly fileio fred103 irredpoly \
	karatsuba keygen lintra osdepn polyprint polytest pool \
	qmtx quartzsig quartztest arc4 rootfind userio 

QDECZ = maindec 

QTESTZ  = qtmain expoly expolymod ffpoly fred103 irredpoly \
		  karatsuba lintra osdepn polyprint pool qmtx quartzsig\
		  quartztest arc4 rootfind 

DREGZ	= dzmain KeyRing FastKey KeyPair LNMatrix LNPoly StringGen Remapper \
		  SDCluster Globals RandPool mstub SDMatrix SDVect SHA Blowfish \
		  crc32 osdepn userio ffpoly fred103 karatsuba lintra arc4

JREGZ	= jmain arc4 expoly ExPolyHold expolymod ffpoly fred103 Globals JRegZ \
		  karatsuba lintra LNMatrix LNPoly MVTrap osdepn polyprint pool qmtx \
		  quartzsig quartztest quartzver RandPool rootfind SDVect shs \
		  LicenseCode StringGen KeyPair Remapper SDCluster SDMatrix \
		  SHA FastKey Blowfish crc32 DRegZ

QZCOMMON = decoder quartzver shs verystub

SDDECODER = decmain decode lcvec
JDECODER  = jdecmain jdecode lcvec

QREGZOBJS=$(addsuffix .o, $(QREGZ) $(QZCOMMON))
QDECZOBJS=$(addsuffix .o, $(QDECZ) $(QZCOMMON))
QTESTZOBJS=$(addsuffix .o, $(QTESTZ) $(QZCOMMON))
DREGZOBJS=$(addsuffix .o, $(DREGZ))
JREGZOBJS=$(addsuffix .o, $(JREGZ))
SDDECODEROBJS=$(addsuffix .o, $(SDDECODER))
JDECODEROBJS=$(addsuffix .o, $(JDECODER))
	
.PHONY: clean

all: qregz qtestz qdecz dregz jregz
	@echo "Build completed"

# ---------------------------------------------------
# do not invoke these targets directly

make_folders:
	-mkdir $(OUTPUTDIR)

qregz_src: $(QREGZOBJS)
	$(CC) $(CFLAGS) $(QREGZOBJS) $(LIBS) -o ../QRegZ

qdecz_src: $(QDECZOBJS)
	$(CC) $(CFLAGS) $(QDECZOBJS) $(LIBS) -o ../QDecZ

qtestz_src: $(QTESTZOBJS)
	$(CC) $(CFLAGS) $(QTESTZOBJS) $(LIBS) -o ../QTestZ
	
dregz_src: $(DREGZOBJS)
	$(CXX) $(CXXFLAGS) $(DREGZOBJS) $(LIBS) -o ../DRegZ

jregz_src: $(JREGZOBJS)
	$(CXX) $(CXXFLAGS) $(JREGZOBJS) $(LIBS) -o ../JRegZ

sddec_src: $(SDDECODEROBJS)
	$(CC) $(CFLAGS) $(SDDECODEROBJS) $(LIBS) -o ../SDDecoder

jdec_src: $(JDECODEROBJS)
	$(CC) $(CFLAGS) $(JDECODEROBJS) $(LIBS) -o ../jdecoder


sddec_build:
	-mkdir $(OUTPUTDIR)/sddec_objs; \
	cd $(OUTPUTDIR)/sddec_objs; \
	$(MAKE) -f ../../Makefile CFLAGS+="-I$(ROOTPATH)/src/DRegZ/SDDecoder" sddec_src;

jdec_build:
	-mkdir $(OUTPUTDIR)/jdec_objs; \
	cd $(OUTPUTDIR)/jdec_objs; \
	$(MAKE) -f ../../Makefile CFLAGS+="-I$(ROOTPATH)/src/JRegZ/jdecoder" jdec_src;


# ---------------------------------------------------
# you can invoke this targets directly if you wish
# in particular sddecoder is not build by make all

qregz: make_folders
	@echo "Now building QRegZ"
	-mkdir $(OUTPUTDIR)/qregz_objs; \
	cd $(OUTPUTDIR)/qregz_objs; \
	$(MAKE) -f ../../Makefile qregz_src

qtestz: make_folders
	@echo "Now building QTestZ"
	-mkdir $(OUTPUTDIR)/qtestz_objs; \
	cd $(OUTPUTDIR)/qtestz_objs; \
	$(MAKE) -f ../../Makefile qtestz_src

qdecz: make_folders
	@echo "Now building QDecZ"
	-mkdir $(OUTPUTDIR)/qdecz_objs; \
	cd $(OUTPUTDIR)/qdecz_objs; \
	$(MAKE) -f ../../Makefile qdecz_src 
	
dregz: make_folders
	@echo "Now building DRegZ"
	-mkdir $(OUTPUTDIR)/dregz_objs; \
	cd $(OUTPUTDIR)/dregz_objs; \
	$(MAKE) -f ../../Makefile dregz_src 

jregz: make_folders
	@echo "Now building JRegZ"
	-mkdir $(OUTPUTDIR)/jregz_objs; \
	cd $(OUTPUTDIR)/jregz_objs; \
	$(MAKE) -f ../../Makefile jregz_src 

sddecoder: make_folders
	@echo "Now building SDDecoder"
	if test -e $(DREGMAIN)/SDDecoder/lcvec.c; \
		then $(MAKE) -f Makefile sddec_build; \
		else echo "Missing lcvec.c file, read ../src/DRegZ/SDDecoder/readme.txt for instructions"; \
		fi

jdecoder: make_folders
	@echo "Now building jdecoder"
	if test -e $(JREGMAIN)/jdecoder/lcvec.c; \
		then $(MAKE) -f Makefile jdec_build; \
		else echo "Missing lcvec.c file, read ../src/JRegZ/jdecoder/readme.txt for instructions"; \
		fi


clean:
	-rm -f $(DREGMAIN)/SDDecoder/lcvec.c
	-rm -f $(DREGMAIN)/SDDecoder/lcvec.h
	-rm -f $(JREGMAIN)/jecoder/lcvec.c
	-rm -f $(JREGMAIN)/jecoder/lcvec.h
	-rm -rf $(OUTPUTDIR)
	@echo "Done"

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
	
