	.file	1 "mbq1.c"

 # GNU C 2.7.2.3 [AL 1.1, MM 40, tma 0.1] SimpleScalar running sstrix compiled by GNU C

 # Cc1 defaults:
 # -mgas -mgpOPT

 # Cc1 arguments (-G value = 8, Cpu = default, ISA = 1):
 # -quiet -dumpbase -g -o

gcc2_compiled.:
__gnu_compiled_c:
	.text
	.align	2
	.globl	main

	.text
	.def	main;	.val	main;	.scl	2;	.type	0x24;	.endef
	.def	argc;	.val	0;	.scl	9;	.type	0x4;	.endef
	.def	argv;	.val	4;	.scl	9;	.type	0x52;	.endef

	.loc	1 5
	.ent	main
main:
	.frame	$fp,32,$31		# vars= 8, regs= 2/0, args= 16, extra= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,32
	sw	$31,28($sp)
	sw	$fp,24($sp)
	move	$fp,$sp
	sw	$4,32($fp)
	sw	$5,36($fp)
	jal	__main

	.loc	1 7
	.def	a;	.val	3;	.scl	4;	.type	0xe;	.endef
	.def	b;	.val	4;	.scl	4;	.type	0xe;	.endef
	.def	c;	.val	5;	.scl	4;	.type	0xe;	.endef
	.def	j;	.val	-16;	.scl	1;	.type	0x4;	.endef
	move	$3,$0

	.loc	1 8
	move	$4,$0

	.loc	1 9
	move	$5,$0

	.loc	1 12
	sw	$0,16($fp)
$L2:
	lw	$2,16($fp)
	li	$6,0x0098967f		# 9999999
	slt	$2,$6,$2
	beq	$2,$0,$L5
	j	$L3
$L5:

	.loc	1 14
	li	$3,0x00000001		# 1

	.loc	1 16
	li	$4,0x00000001		# 1

	.loc	1 18
	addu	$5,$3,$4

	.loc	1 12
$L4:
	lw	$6,16($fp)
	addu	$2,$6,1
	move	$6,$2
	sw	$6,16($fp)
	j	$L2
$L3:

	.loc	1 22
	move	$2,$0
	j	$L1

	.loc	1 23
$L1:
	move	$sp,$fp			# sp not trusted here
	lw	$31,28($sp)
	lw	$fp,24($sp)
	addu	$sp,$sp,32
	j	$31
	.end	main
