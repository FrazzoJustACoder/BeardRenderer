CC=g++
BINDIR=bin
SRCDIR=src
INCDIR=include
TSTDIR=test
OBJECTS=$(BINDIR)\BeardRenderer.o $(BINDIR)\BeardMatrix.o $(BINDIR)\lib.o
LIBDEPS=$(INCDIR)\BeardMatrix.h $(INCDIR)\BeardRenderer.h
TEST_EXECUTABLE=test.exe
CFLAGS=-I$(INCDIR)

#lib != lib.S
all: lib

lib: $(OBJECTS)

$(BINDIR)\LongBeard.o: $(OBJECTS)
	$(CC) -r -o $@ $^

$(BINDIR)\BeardRenderer.o: $(SRCDIR)\BeardRenderer.cpp $(INCDIR)\BeardRenderer.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\BeardMatrix.o: $(SRCDIR)\BeardMatrix.cpp $(INCDIR)\BeardMatrix.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\lib.o: $(SRCDIR)\lib.S $(INCDIR)\lib.h
	$(CC) $(CFLAGS) -c -o $@ $<

#test area
test_win_normal: $(OBJECTS) $(BINDIR)\TestWinNormal.o $(BINDIR)\WinDebug.o
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $^ -lgdi32

test_win_splitscreen:  $(OBJECTS) $(BINDIR)\TestWinSplitscreen.o $(BINDIR)\WinDebug.o
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $^ -lgdi32

$(BINDIR)\WinDebug.o: $(TSTDIR)\WinDebug.cpp $(TSTDIR)\WinDebug.h
	$(CC) -c -o $@ $<

$(BINDIR)\TestWinNormal.o: $(TSTDIR)\TestWinNormal.cpp $(LIBDEPS) $(TSTDIR)\WinDebug.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\TestWinSplitscreen.o: $(TSTDIR)\TestWinSplitscreen.cpp $(LIBDEPS) $(TSTDIR)\WinDebug.h
	$(CC) $(CFLAGS) -c -o $@ $<
