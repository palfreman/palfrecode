# Illustrate how functions work
# calcultating 2^ + 5^2
# 64bit version

# No data, all in registers 
 .section .data

 .section .text

 .globl _start
_start:
push $3
push $2
call power
add $16, %rsp       # Move stack pointer back
push %rax          # Save answer

push $2
push $5
call power
add $16, %rsp       # Stack back

pop %rbx           # Second answer is already in %eax. 
                    # We just need to pop back the first answer
add %rax, %rbx     # Add %eax to $ebx, into %ebx

mov $1, %rax
int $0x80           # quit

# power func: purpose, raise number by power
# Input:    first arg, base number
#           second arg, what to raise it by
#
# Output: return value
#
# Note: only powers >=1
#
# Var:  %rbx    - base number
#       %rcx    - power
#       -8(%rbp) - current result
#       %rax    - temp storage
 .type power, @function
power:
push %rbp      # Save old base pointer
mov %rsp, %rbp # Copy stack pointer to base pointer
sub $8, %rsp   # Make us some space

mov 16(%rbp), %rbx      # First arg into %ebx [ says %eax in book]
mov 24(%rbp), %rcx     # Second arg into %ecx
mov %rbx, -8(%rbp)     # save current result

power_loop_start:
cmp $1, %rcx           # We've finished when the power to multiply is 1
je end_power
mov -8(%rbp), %rax     # Move current result into %eax 
imul %rbx, %rax         # Multiply current result by base number, into %eax
mov %rax, -8(%rbp)     # Save result
dec %rcx               # Count down one, decrease power
jmp power_loop_start

end_power:
mov -8(%rbp), %rax     # Return value
mov %rbp, %rsp         # Restore stack pointer
pop %rbp               # Restore base pointer
ret

