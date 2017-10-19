//
// Created by lizhuan1 on 13/10/17.
//

#include <stdlib.h>
#include <stdio.h>
int main(){
    register int i;
    register int j = 0;
    for (i = 0 ; i < 10000; i++){
        if( j % 2 != 0){ //will create a pattern of TN*
            j+=2 // This line does not change the even/oddness of j, just add to make sure this code is not optimized away
        }
        j++; // This line change the even/oddness of J
    }
}

/**
 * // Created by lizhuan1 on 13/10/17.
//

#include <stdlib.h>
#include <stdio.h>
int main(){
 690:	55                   	push   %rbp
 691:	48 89 e5             	mov    %rsp,%rbp
 694:	41 54                	push   %r12
 696:	53                   	push   %rbx
    register int i;
    register int j = 0;
 697:	bb 00 00 00 00       	mov    $0x0,%ebx
    for (i = 0 ; i < 10000; i++){
 69c:	41 bc 00 00 00 00    	mov    $0x0,%r12d
 6a2:	41 81 fc 0f 27 00 00 	cmp    $0x270f,%r12d
 6a9:	7f 15                	jg     6c0 <main+0x30>   // We ignore this jump for saturate counter because it is a loop-related jump
        if( j % 2 != 0){
 6ab:	89 d8                	mov    %ebx,%eax
 6ad:	83 e0 01             	and    $0x1,%eax
 6b0:	85 c0                	test   %eax,%eax
 6b2:	74 03                	je     6b7 <main+0x27>   // This is the conditional jump that we are focusing on. If the jump is taken, then
            j+=2;                                        // it skip the j+=2 part, thus the if-statement is not taken. We can then conclude the
 6b4:	83 c3 02             	add    $0x2,%ebx         // branch-pattern of this jump to be (TN)*, which should be miss predicted by 2 bit saturate
        }                                                // totally. For 2 level predictor, this problem is solved due to the private BHT.
        j++;
 6b7:	83 c3 01             	add    $0x1,%ebx
    for (i = 0 ; i < 10000; i++){
 6ba:	41 83 c4 01          	add    $0x1,%r12d
 6be:	eb e2                	jmp    6a2 <main+0x12> // We ignore this jump for saturate counter because it is a loop-related jump
    }
}
 6c0:	b8 00 00 00 00       	mov    $0x0,%eax
 6c5:	5b                   	pop    %rbx
 6c6:	41 5c                	pop    %r12
 6c8:	5d                   	pop    %rbp
 6c9:	c3                   	retq
 6ca:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)
 */
