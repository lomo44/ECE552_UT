CC=/cad2/ece552f/compiler/bin/ssbig-na-sstrix-gcc

SIM_CACHE=../sim-cache

NL_CONFIG=../cache-config/cache-lru-nextline.cfg
SD_CONFIG=../cache-config/cache-lru-stride.cfg
OE_CONFIG=../cache-config/cache-lru-open.cfg
BS_CONFIG=../cache-config/cache-lru.cfg
COMPRESS_BM=/cad2/ece552f/benchmarks/compress.eio
GO_BM=/cad2/ece552f/benchmarks/go.eio
GCC_BM=/cad2/ece552f/benchmarks/gcc.eio

BENCHMARKS=mbq1 mbq2 mbq6
TRACES=go gcc compress
FLAG=-O0
all: mbq2 mbq1 mbq6

clean:
	rm -f mbq2 
	rm -f mbq1
	rm -f mbq6
	rm -f ../data/*.sim_out
	rm -f ../data/*.prg_out
mbq2: mbq2.c
	$(CC) mbq2.c $(FLAG) -o mbq2
mbq1: mbq1.c
	$(CC) mbq1.c $(FLAG) -o mbq1
mbq6: mbq6.c
	$(CC) mbq6.c $(FLAG) -o mbq6

benchmark: mbq1 mbq2 mbq6
	for benchmark in $(BENCHMARKS) ; do \
		$(SIM_CACHE) -redir:sim ../data/$${benchmark}_bs.sim_out -config $(BS_CONFIG) $${benchmark};\
		$(SIM_CACHE) -redir:sim ../data/$${benchmark}_nl.sim_out -config $(NL_CONFIG) $${benchmark};\
		$(SIM_CACHE) -redir:sim ../data/$${benchmark}_sd.sim_out -config $(SD_CONFIG) $${benchmark};\
		$(SIM_CACHE) -redir:sim ../data/$${benchmark}_oe.sim_out -config $(OE_CONFIG) $${benchmark};\
    done

traces:
	for trace in $(TRACES) ; do \
		$(SIM_CACHE) -redir:sim ../data/$${trace}_bs.sim_out -config $(BS_CONFIG) /cad2/ece552f/benchmarks/$${trace}.eio;\
		$(SIM_CACHE) -redir:sim ../data/$${trace}_nl.sim_out -config $(NL_CONFIG) /cad2/ece552f/benchmarks/$${trace}.eio;\
		$(SIM_CACHE) -redir:sim ../data/$${trace}_sd.sim_out -config $(SD_CONFIG) /cad2/ece552f/benchmarks/$${trace}.eio;\
		$(SIM_CACHE) -redir:sim ../data/$${trace}_oe.sim_out -config $(OE_CONFIG) /cad2/ece552f/benchmarks/$${trace}.eio;\
	done
