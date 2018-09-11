    add $t0 $s0 $0
    addi $t0 $t0 20
outterLoop:
    add $a0 $s0 $0
    add $t1 $0 $0
innerLoop:
    lw $t2 0($a0)
    lw $t3 4($a0)
    slt $t5 $t2 $t3
    beq $t5 $0 continue
    addi $t1 $0 1
    sw $t2 4($a0)
    sw $t3 0($a0)
continue:
    addi $a0 $a0 4
    bne $a0 $t0 innerLoop
    bne $t1 $0 outterLoop

