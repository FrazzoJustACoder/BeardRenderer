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
	echo TODO learn to use makefiles and make this error disappear
	rm $(OBJECTS) $(BINDIR)\$(TEST_EXECUTABLE) $(BINDIR)\TestWinNormal.o $(BINDIR)\TestWinSplitscreen.o $(BINDIR)\WinDebug.o

$(BINDIR)\BeardRenderer.o: $(SRCDIR)\BeardRenderer.cpp $(INCDIR)\BeardRenderer.h
	$(CC) $(CFLAGS) -c -o $@ $< -std=c++11 -Wno-pointer-arith

$(BINDIR)\BeardMatrix.o: $(SRCDIR)\BeardMatrix.cpp $(INCDIR)\BeardMatrix.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\lib.o: $(SRCDIR)\lib.S $(INCDIR)\lib.h
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean_lib
clean_lib:
	echo TODO learn to use makefiles and make this error disappear
	rm $(OBJECTS)

#test area
.PHONY: test_win_normal
test_win_normal: $(OBJECTS) $(BINDIR)\TestWinNormal.o $(BINDIR)\WinDebug.o
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $^ -lgdi32

.PHONY: test_win_splitscreen
test_win_splitscreen:  $(OBJECTS) $(BINDIR)\TestWinSplitscreen.o $(BINDIR)\WinDebug.o
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $^ -lgdi32

.PHONY: test_win_console
test_win_console: $(OBJECTS) $(BINDIR)\TestWinConsole.o
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $^

$(BINDIR)\WinDebug.o: $(TSTDIR)\WinDebug.cpp $(TSTDIR)\WinDebug.h
	$(CC) -c -o $@ $<

$(BINDIR)\TestWinNormal.o: $(TSTDIR)\TestWinNormal.cpp $(LIBDEPS) $(TSTDIR)\WinDebug.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\TestWinSplitscreen.o: $(TSTDIR)\TestWinSplitscreen.cpp $(LIBDEPS) $(TSTDIR)\WinDebug.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\TestWinConsole.o: $(TSTDIR)\TestWinConsole.cpp $(LIBDEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean_test
clean_test:
	echo TODO learn to use makefiles and make this error disappear
	rm $(BINDIR)\$(TEST_EXECUTABLE) $(BINDIR)\WinDebug.o $(BINDIR)\TestWinNormal.o $(BINDIR)\TestWinSplitscreen.o $(BINDIR)\TestWinConsole.o

#.PHONY: debug_normal
#debug_normal:
#	$(CC) $(CFLAGS) -c -g -o $(BINDIR)\BeardRenderer.o $(SRCDIR)\BeardRenderer.cpp -std=c++11 -Wno-pointer-arith
#	$(CC) $(CFLAGS) -g -o $(BINDIR)\$(TEST_EXECUTABLE) $(SRCDIR)\BeardMatrix.cpp $(SRCDIR)\lib.S $(TSTDIR)\TestWinNormal.cpp $(TSTDIR)\WinDebug.cpp $(BINDIR)\BeardRenderer.o -lgdi32
