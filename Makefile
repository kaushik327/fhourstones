# GNU Makefile

CFLAGS = -O3 -march=native -flto -funroll-loops -fno-stack-protector -ffast-math -Wextra -Wall
CFLAGS_MT = $(CFLAGS) -pthread
CFILES = SearchGame.c TransGame.c Game.c
CFILES_MT = SearchGameMT.c TransGame.c Game.c
JAVAFILES = SearchGame.java Connect4.java Game.java TransGame.java
JAVAFILES_MT = SearchGameMT.java Connect4.java Game.java TransGame.java
HASKELLFILES = Main.hs Connect4.hs GameTreeSearch.hs
HASKELLFILES_MT = MainMT.hs Connect4.hs GameTreeSearchMT.hs
FILES = Makefile inputs $(CFILES) $(CFILES_MT) $(JAVAFILES) $(JAVAFILES_MT) $(HASKELLFILES) $(HASKELLFILES_MT)

.PHONY : all exe run java haskell clean mt mt-c mt-java mt-haskell run-mt

all : SearchGame SearchGameMT

clean : ; rm -f SearchGame SearchGameMT SearchGame.ghc SearchGameMT.ghc *.class *.o *.hi *.tar.gz *.zip

run : SearchGame inputs
	./SearchGame < inputs

run-mt : SearchGameMT inputs
	./SearchGameMT 4 < inputs

SearchGame : $(CFILES)
	$(CC) $(CFLAGS) -o $@ $<

SearchGameMT : $(CFILES_MT)
	$(CC) $(CFLAGS_MT) -o $@ $<

java : SearchGame.class
SearchGame.class : $(JAVAFILES)
	javac -O $<
	# to run, 'java SearchGame < inputs'

java-mt : SearchGameMT.class
SearchGameMT.class : $(JAVAFILES_MT)
	javac -O $<
	# to run, 'java SearchGameMT 4 < inputs'

haskell : SearchGame.ghc
SearchGame.ghc : $(HASKELLFILES)
	ghc -o SearchGame.ghc -O --make $<
	# to run, './SearchGame.ghc < inputs'

haskell-mt : SearchGameMT.ghc
SearchGameMT.ghc : $(HASKELLFILES_MT)
	ghc -o SearchGameMT.ghc -O --make $< -threaded
	# to run, './SearchGameMT.ghc 4 < inputs'

# Multithreaded targets
mt : SearchGameMT SearchGameMT.class SearchGameMT.ghc

mt-c : SearchGameMT

mt-java : SearchGameMT.class

mt-haskell : SearchGameMT.ghc

tar:    $(FILES)
	tar -cf Fhourstones.tar $(FILES)
	gzip Fhourstones.tar
	zip -r Fhourstones.zip $(FILES)
