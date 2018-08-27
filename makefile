#NOTE to build, first create a "bin" directory in the project directory

CC=g++
BINDIR=bin
SRCDIR=src
INCDIR=include
TSTDIR=test
OBJECTS=$(BINDIR)\BeardRenderer.o $(BINDIR)\BeardMatrix.o $(BINDIR)\lib.o
LIBOUT=$(BINDIR)\libBeard.a
LIBDEPS=$(INCDIR)\BeardMatrix.h $(INCDIR)\BeardRenderer.h
TEST_EXECUTABLE=test.exe
CFLAGS=-I$(INCDIR)
LDFLAGS_TST=-L$(BINDIR) -lBeard

#lib != lib.S
.PHONY: all
all: lib

.PHONY: lib
lib: $(LIBOUT)

.PHONY: clean
clean: clean_lib clean_test


$(LIBOUT): $(OBJECTS)
	ar rcs $@ $^
#TODO hide unneeded symbols

$(BINDIR)\BeardRenderer.o: $(SRCDIR)\BeardRenderer.cpp $(INCDIR)\BeardRenderer.h
	$(CC) $(CFLAGS) -c -o $@ $< -std=c++11 -Wno-pointer-arith

$(BINDIR)\BeardMatrix.o: $(SRCDIR)\BeardMatrix.cpp $(INCDIR)\BeardMatrix.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\lib.o: $(SRCDIR)\lib.S $(INCDIR)\lib.h
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean_lib
clean_lib:
	rm -f $(OBJECTS) $(LIBOUT)

#test area
.PHONY: test_win_normal
test_win_normal: $(BINDIR)\TestWinNormal.o $(BINDIR)\WinDebug.o $(LIBOUT)
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $< $(BINDIR)\WinDebug.o -lgdi32 $(LDFLAGS_TST)

.PHONY: test_win_splitscreen
test_win_splitscreen: $(BINDIR)\TestWinSplitscreen.o $(BINDIR)\WinDebug.o $(LIBOUT)
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $< $(BINDIR)\WinDebug.o -lgdi32 $(LDFLAGS_TST)

.PHONY: test_win_console
test_win_console: $(BINDIR)\TestWinConsole.o $(LIBOUT)
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $< $(LDFLAGS_TST)

.PHONY: test_win_shader
test_win_shader: $(BINDIR)\TestWinShader.o $(BINDIR)\WinDebug.o $(LIBOUT)
	$(CC) -o $(BINDIR)\$(TEST_EXECUTABLE) $< $(BINDIR)\WinDebug.o -lgdi32 $(LDFLAGS_TST)

$(BINDIR)\WinDebug.o: $(TSTDIR)\WinDebug.cpp $(TSTDIR)\WinDebug.h
	$(CC) -c -o $@ $<

$(BINDIR)\TestWinNormal.o: $(TSTDIR)\TestWinNormal.cpp $(LIBDEPS) $(TSTDIR)\WinDebug.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\TestWinSplitscreen.o: $(TSTDIR)\TestWinSplitscreen.cpp $(LIBDEPS) $(TSTDIR)\WinDebug.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\TestWinConsole.o: $(TSTDIR)\TestWinConsole.cpp $(LIBDEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)\TestWinShader.o: $(TSTDIR)\TestWinShader.cpp $(LIBDEPS) $(TSTDIR)\WinDebug.h
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean_test
clean_test:
	rm -f $(BINDIR)\$(TEST_EXECUTABLE) $(BINDIR)\WinDebug.o $(BINDIR)\TestWinNormal.o $(BINDIR)\TestWinSplitscreen.o $(BINDIR)\TestWinConsole.o $(BINDIR)\TestWinShader.o
