all: bin rtgen rtdump rtsort rcrack rtstats hello pi
all_clean: clean bin rtgen rtdump rtsort rcrack rtstats hello pi
bin:
	mkdir ../bin	
clean:
	rm -rf ../bin
rtgen:
	mpic++ Public.cpp ChainWalkContext.cpp HashAlgorithm.cpp HashRoutine.cpp RainbowTableGenerate.cpp -lcrypto -lpthread -lssl -O3 -o ../bin/rtgen
	cp charset.txt ../bin/charset.txt
rtdump:
	g++ Public.cpp ChainWalkContext.cpp HashAlgorithm.cpp HashRoutine.cpp RainbowTableDump.cpp -lcrypto -lssl -o ../bin/rtdump
rtsort:
	g++ Public.cpp RainbowTableSort.cpp -lcrypto -o ../bin/rtsort
rcrack:
	g++ Public.cpp ChainWalkContext.cpp HashAlgorithm.cpp -lcrypto HashRoutine.cpp HashSet.cpp MemoryPool.cpp ChainWalkSet.cpp CrackEngine.cpp RainbowCrack.cpp -lssl -O3 -o ../bin/rcrack
rtstats:
	g++ rtstats.cpp Public.cpp -o ../bin/rtstats
hello:
	mpic++ Public.cpp MPI_Hello.cpp -o ../bin/mpi_hello
pi:
	mpic++ dboard.c pi.cpp -lm -o ../bin/pi

# TODO: fix this section so we can use PATH variables (not eveery one has this setup)
install:
	yes | cp -fR ../bin /export/home/admiral/
	yes | cp -fR ../tests /export/home/admiral/
	yes | cp BenchMark.sh /export/home/admiral/bin/benchmark
	chown admiral:admiral -R /export/home/admiral