# Illustrate how functions work
# calcultating 2^3+ 5^2

# No data, all in registers 
 .section .data

 .section .text

 .globl _start
_start:
pushl $3
pushl $2
call power
addl $8, %esp       # Move stack pointer back
pushl %eax          # Save answer

pushl $2
pushl $5
call power
addl $8, %esp       # Stack back

popl %ebx           # Second answer is already in %eax. 
                    # We just need to pop back the first answer
addl %eax, %ebx     # Add %eax to $ebx, into %ebx

movl $1, %eax
int $0x80           # quit

# power func: purpose, raise number by power
# Input:    first arg, base number
#           second arg, what to raise it by
#
# Output: return value
#
# Note: only powers >=1
#
# Var:  %ebx    - base number
#       %ecx    - power
#       -4(%ebp) - current result
#       %eax    - temp storage
 .type power, @function
power:
pushl %ebp      # Save old base pointer
movl %esp, %ebp # Copy stack pointer to base pointer
subl $4, %esp   # Make us some space

movl 8(%ebp), %ebx      # First arg into %ebx [ says %eax in book]
movl 12(%ebp), %ecx     # Second arg into %ecx
movl %ebx, -4(%ebp)     # save current result

power_loop_start:
cmpl $1, %ecx           # We've finished when the power to multiply is 1
je end_power
movl -4(%ebp), %eax     # Move current result into %eax 
imul %ebx, %eax         # Multiply current result by base number, into %eax
movl %eax, -4(%ebp)     # Save result
decl %ecx               # Count down one, decrease power
jmp power_loop_start

end_power:
movl -4(%ebp), %eax     # Return value
movl %ebp, %esp         # Restore stack pointer
popl %ebp               # Restore base pointer
ret

