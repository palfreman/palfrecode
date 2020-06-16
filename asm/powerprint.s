.section .data
.section .text

# Stack
# -> arg2       16
# -> arg1       12
# -> prog path  8
# argc          4
# old ebp       0

.globl _start
_start:
    pushl %ebp              
    movl %esp, %ebp

    # number of command line arguments
    # So "./myprog hello Lucy" gives 3, because
    # myprog=1, hello=2 and Lucy=3
    # Not used in rest of program, only an example
    movl 4(%ebp), %ecx

    # This is a pointer (a memory address)
    # where the first arg is kept. 
    # The number is stored there
    push 12(%ebp)
    call num2_string
    call print_string
    # rest of prog

    # Convert the string pointer on the stack to a number
    num_2_string:
        pushl %ebp
        movl $esp, %ebp
        push 8(%ebp)        # Pointer to string
        call count_string
        push %eax           # String length
        call calc_string

count_string:
    pushl %ebp
    movl %esp, %ebp
    movl $0, %eax       # the default to return, string is zero length
    start_count:
        movl 8(%ebp,%eax,4), %ebx  # Current letter of string
        cmpl $0, %ebx       # Is it the end of the string?
        je end_count

        incl %eax           # We know the string is at least this long
        cmpl $0xff, %eax    # Stop string at 255 bytes
        jge end_count

        jmp start_count

end_count:
    pop %ebp
    ret             # Answer is in is %eax already

# 8(ebp) is string count, 12(%ebp) is string pointer
calc_string:
    push %ebp
    movl %esp, %ebp
    movl 8(%ebp), %ecx
    cmpl $0, %ecx
    je end_count
    
    decl %ecx
    movl 12(%ebp,%ecx,4), %ebx      # Next letter start from end
    
    cmpl $48, %ebx                  # Is it below zero?
    jl  bad_number
    
    subl $48, %ebx                  # Convert ASCII to number
    cmpl $9, %ebx                   # Is it above 9? We only want 0-9
    jg bad_number
    
    movl 8(%ebp), %edx              # How far along the string are we?
    subl %ecx, %edx                 # This far
    push %ecx                       # Function might overwrite it
    push %edx                       # The factor to convert to multipler
    call get_factor

# One arg, the factor to convert into a multiplier
get_factor:
    push %ebp
    movl %esp, %ebp
    mov 8(%ebp), %ecx   # How far from end of string
    movl $1, %eax       # Base case when one byte from end of string
    


    bad_number:
        movl $0, %eax
        end_count







    # Get the number out of that memory location
    # using (brackets)
    movl (%eax), %esi

    # it is an ASCII number, so 0 = 48, 1=49, 2=50 etc.
    # So subtracting $48 gives the number
    subl $48, %ebx

    # Tidy up stack
    popl %ebp

    # Exit. $? is the smallest byte of $ebx and
    # answer is in %ebx already
    movl $1, %eax
    int $0x80
