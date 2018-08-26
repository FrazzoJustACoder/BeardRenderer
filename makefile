#NOTE to build, first create a bin directory in the project directory

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
.PHONY: all
all: lib

.PHONY: lib
lib: $(OBJECTS)

.PHONY: clean
clean:
	rm $(OBJECTS) $(BINDIR)\$(TEST_EXECUTABLE) $(BINDIR)\TestWinNormal.o $(BINDIR)\TestWinSplitscreen.o $(BINDIR)\WinDebug.o

$(BINDIR)\LongBeard.o: $(OBJECTS)
	$(CC) -r -o $@ $^

$(BINDIR)\BeardRenderer.o: $(SRCDIR)\BeardRenderer.cpp $(INCDIR)\BeardRenderer.h
	$(CC) $(CFLAGS) -c -o $@ $< -std=c++11 -Wno-pointer-arith

$(BINDIR)\BeardMatrix.o: $(SRCDIR)\BeardMatrix.cpp $(INCDIR)\BeardMatrix.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\lib.o: $(SRCDIR)\lib.S $(INCDIR)\lib.h
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean_lib
clean_lib:
	rm $(OBJECTS)

#test area
.PHONY: test_win_normal
test_win_normal: $(OBJECTS) $(BINDIR)\TestWinNormal.o $(BINDIR)\WinDebug.o
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $^ -lgdi32

.PHONY: test_win_splitscreen
test_win_splitscreen:  $(OBJECTS) $(BINDIR)\TestWinSplitscreen.o $(BINDIR)\WinDebug.o
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $^ -lgdi32

$(BINDIR)\WinDebug.o: $(TSTDIR)\WinDebug.cpp $(TSTDIR)\WinDebug.h
	$(CC) -c -o $@ $<

$(BINDIR)\TestWinNormal.o: $(TSTDIR)\TestWinNormal.cpp $(LIBDEPS) $(TSTDIR)\WinDebug.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\TestWinSplitscreen.o: $(TSTDIR)\TestWinSplitscreen.cpp $(LIBDEPS) $(TSTDIR)\WinDebug.h
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean_test
clean_test:
	rm $(BINDIR)\$(TEST_EXECUTABLE) $(BINDIR)\TestWinNormal.o $(BINDIR)\TestWinSplitscreen.o $(BINDIR)\WinDebug.o
#NOTE debug may or may not be compiled
