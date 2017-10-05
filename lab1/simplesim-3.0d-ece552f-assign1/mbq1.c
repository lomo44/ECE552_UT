//
// Created by lizhuan1 on 10/1/17.
//

int main (int argc, char** argv) {

    register unsigned int a = 0; 	// Need to use register keyword so that compiler does not put this on stack
    register unsigned int b = 0;
    register unsigned int c = 0;

    int j;
    for ( j = 0; j < 10000000; ++j ){ //Here existing some RAW for the compare and addtion

        a = 1;				// Write to the register 

        b = 1;				// Write to another register

        c = a + b;			// Add two output register from last two instructions and write it to another register

    }

    return 0;
}

/*004001f0 <main>:
main():
mbq1.c:5
  4001f0:	43 00 00 00 	addiu $29,$29,-32 >1
  4001f4:	e0 ff 1d 1d
  4001f8:	34 00 00 00 	sw $31,28($29)
  4001fc:	1c 00 1f 1d
  400200:	34 00 00 00 	sw $30,24($29)
  400204:	18 00 1e 1d
  400208:	42 00 00 00 	addu $30,$0,$29
  40020c:	00 1e 1d 00
  400210:	34 00 00 00 	sw $4,32($30) >2
  400214:	20 00 04 1e
  400218:	34 00 00 00 	sw $5,36($30)
  40021c:	24 00 05 1e
  400220:	02 00 00 00 	jal 400498 <__main>
  400224:	26 01 10 00
mbq1.c:7
  400228:	42 00 00 00 	addu $3,$0,$0
  40022c:	00 03 00 00
mbq1.c:8
  400230:	42 00 00 00 	addu $4,$0,$0
  400234:	00 04 00 00
mbq1.c:9
  400238:	42 00 00 00 	addu $5,$0,$0
  40023c:	00 05 00 00
mbq1.c:12
  400240:	34 00 00 00 	sw $0,16($30)
  400244:	10 00 00 1e
  400248:	28 00 00 00 	lw $2,16($30)
  40024c:	10 00 02 1e
  400250:	a2 00 00 00 	lui $6,152
  400254:	98 00 06 00
  400258:	51 00 00 00 	ori $6,$6,38527 				-> RAW (2 Cycle stall for q1,1 Cycle stall for q2)
  40025c:	7f 96 06 06
  400260:	5b 00 00 00 	slt $2,$6,$2 					-> RAW (2 Cycle stall for q1,1 Cycle stall for q2)
  400264:	00 02 02 06
  400268:	05 00 00 00 	beq $2,$0,400278 <main+0x88>    -> RAW (2 Cycle stall for q1,1 Cycle stall for q2)
  40026c:	02 00 00 02
  400270:	01 00 00 00 	j 4002b8 <main+0xc8>
  400274:	ae 00 10 00
mbq1.c:14
  400278:	43 00 00 00 	addiu $3,$0,1
  40027c:	01 00 03 00
mbq1.c:16
  400280:	43 00 00 00 	addiu $4,$0,1
  400284:	01 00 04 00
mbq1.c:18
  400288:	42 00 00 00 	addu $5,$3,$4 					 ->RAW (2 Cycle stall for q1,1 Cycle stall for q2)
  40028c:	00 05 04 03
mbq1.c:12
  400290:	28 00 00 00 	lw $6,16($30)
  400294:	10 00 06 1e
  400298:	43 00 00 00 	addiu $2,$6,1 					 ->RAW (2 Cycle stall for q1,2 Cycle stall for q2)
  40029c:	01 00 02 06
  4002a0:	42 00 00 00 	addu $6,$0,$2                    ->RAW (2 Cycle stall for q1,1 Cycle stall for q2)
  4002a4:	00 06 02 00
  4002a8:	34 00 00 00 	sw $6,16($30)                    ->RAW (2 Cycle stall for q1,0 Cycle stall for q2)
  4002ac:	10 00 06 1e
  4002b0:	01 00 00 00 	j 400248 <main+0x58>
  4002b4:	92 00 10 00
mbq1.c:22
  4002b8:	42 00 00 00 	addu $2,$0,$0
  4002bc:	00 02 00 00
  4002c0:	01 00 00 00 	j 4002c8 <main+0xd8>
  4002c4:	b2 00 10 00
mbq1.c:23
  4002c8:	42 00 00 00 	addu $29,$0,$30
  4002cc:	00 1d 1e 00
  4002d0:	28 00 00 00 	lw $31,28($29)
  4002d4:	1c 00 1f 1d
  4002d8:	28 00 00 00 	lw $30,24($29)
  4002dc:	18 00 1e 1d
  4002e0:	43 00 00 00 	addiu $29,$29,32
  4002e4:	20 00 1d 1d
  4002e8:	03 00 00 00 	jr $31
  4002ec:	00 00 00 1f
 * */
