#VAR:
# %edi - index, item being examined
# %ebx - largest item found so far
# %eax - current item
# 
# Memory:
#
# data_items - the items to compare. 0 terminates

.section .data

data_items:
.long 3,67,34,222,45,75,54,34,44,33,22,11,66,0

.section .text

.globl _start
_start:
    movl $0, %edi                       # zero the index
    movl data_items(,%edi,4), %eax      # load first byte
    movl %eax, %ebx                     # first one is biggest so far

start_loop:
    cmpl $0, %eax                       # zero terminates
    je loop_exit                        # Jump to exit if above was equal
    incl %edi                           # Increment %edi for next item
    movl data_items(,%edi,4), %eax      # lod next item            
    cmpl %ebx, %eax                     # compare
    jle start_loop                      # If less than, go to start of loop
    movl %eax, %ebx                     # Move new biggest value
    jmp start_loop

loop_exit:
    movl $1, %eax                       # exit syscall
    int $0x80
