#!/usr/bin/python
import os
import re

file_out= open("predictor_performance.csv","w");
file_out.write("Simulator, 2 bit counter, 2 level, open-ended\n");
os.system("make");
list_in= ["predictor /cad2/ece552f/cbp4_benchmarks/astar.cbp4.gz",\
		  "predictor /cad2/ece552f/cbp4_benchmarks/bwaves.cbp4.gz",\
		  "predictor /cad2/ece552f/cbp4_benchmarks/bzip2.cbp4.gz",\
		  "predictor /cad2/ece552f/cbp4_benchmarks/gcc.cbp4.gz",\
		  "predictor /cad2/ece552f/cbp4_benchmarks/gromacs.cbp4.gz",\
		  "predictor /cad2/ece552f/cbp4_benchmarks/hmmer.cbp4.gz",\
		  "predictor /cad2/ece552f/cbp4_benchmarks/mcf.cbp4.gz",\
		  "predictor /cad2/ece552f/cbp4_benchmarks/soplex.cbp4.gz"];
for simulator in list_in:
	system_call= simulator + "|tee predict.txt"
	os.system(system_call);
	using_simulator=re.match("predictor \/cad2\/ece552f\/cbp4_benchmarks\/(.*)\.cbp4",simulator);
	print_result = using_simulator.group(1);
	with open("predict.txt","r") as file_in:
		for line in file_in:
			b2_miss=re.match("2bitsat: MISPRED_PER_1K_INST  	 :(\s*)(.*)",line)
			if b2_miss:
				print_result = print_result + "," + b2_miss.group(2);
			l2_miss=re.match("2level:  MISPRED_PER_1K_INST  	 :(\s*)(.*)",line)
			if l2_miss:
				print_result = print_result + "," + l2_miss.group(2);
			op_miss=re.match("openend: MISPRED_PER_1K_INST  	 :(\s*)(.*)",line)
			if op_miss:
				print_result = print_result + "," + op_miss.group(2);

		print_result += "\n";
		file_out.write(print_result); 
file_out.write("Average MPKI,=AVERAGE(B2:B9),=AVERAGE(C2:C9),=AVERAGE(D2:D9)\n"); 
os.system("rm predict.txt");
