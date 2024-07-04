global mdiv
mdiv:
    ; Description of register purposes:
    ; rdi - pointer to the dividend array
    ; rsi - number of elements in the dividend array
    ; rdx - divisor
    ; r8  - flag indicating the sign of the divisor (0 - positive, 1 - negative)
    ; r9  - flag indicating the sign of the dividend (0 - positive, 1 - negative)
    ; r10 - loop iterator

start:
    xor r8, r8             ; Set the flag to 0, assuming by default that the divisor is positive.
    xor r9, r9             ; Set the flag to 0, assuming by default that the dividend is positive.

    cmp rdx, 0             ; Check if the divisor is greater than 0.
    jge positive_divider   ; If so, jump to positive_divider.

negative_divider:
    inc r8                 ; Indicate that the divisor is negative.
    neg rdx                ; Negate the divisor.

positive_divider:
    cmp qword [rdi - 8 + 8 * rsi], 0    ; Check if the most significant digit of the dividend is positive.
    jge division_1                      ; If so, jump to division_1.

normalising_dividend:
    inc r9                 ; Indicate that the dividend is negative.
    mov rcx, rsi           ; Set the loop iterator to the number of elements in the dividend array.
    xor r10, r10           ; Clear the iterator.
    stc                    ; Set the carry flag to 1.

normalising_dividend_loop:
    not qword [rdi + r10 * 8]                        ; Flip bits 0->1 and 1->0.
    adc qword [rdi + r10 * 8], 0                     ; Add 1 to each number in the dividend array.
    inc r10                                          ; Increment the iterator.
    loop normalising_dividend_loop                  

division_1:
    mov r10, rdx           ; Copy the divisor to the auxiliary register r10.
    xor rdx, rdx           ; Clear the divisor, to leave space in register rdx for the remainder from the div instruction.
    mov rcx, rsi           ; Reset the loop iterator to the number of elements in the dividend array.

division_loop:
    mov rax, qword [rdi - 8 + 8 * rcx]   ; Load a number from the dividend array into register rax.
    div r10                              ; Divide the number in rax by the divisor in r11. Remainder is stored in rdx.
    mov qword [rdi - 8 + 8 * rcx], rax   ; Store the quotient from rax back into the dividend array.
    loop division_loop                   
        
division_2:
    mov rax, rdx           ; Copy the remainder from the division to register rax.
    cmp r9, 0              ; Check if the dividend was positive.
    je positive_sign       ; If so, proceed to positive_sign.

negating_remainder:        ; Negate the result.
    neg rax                

positive_sign:
    cmp r8, r9             ; Check if the divisor and dividend had the same sign.
    je overflow            ; If so, skip negating the result and proceed to check for overflow.

negating_result:           ; Negate the quotient.
    mov rcx, rsi           ; Reset the loop iterator to the number of elements in the dividend array.
    xor r10, r10           ; Clear the iterator.
    stc                    ; Set the carry flag to 1.

negating_result_loop: 
    not qword [rdi + 8 * r10]      ; Flip bits 0->1 and 1->0.
    adc qword [rdi + 8 * r10], 0   ; Add 1 to each number in the dividend array when the carry flag is set to 1.
    inc r10                        
    loop negating_result_loop      
    jmp return_result                         

overflow:                           ; Check if overflow occurred.
    cmp qword [rdi - 8 + 8 * rsi] , 0                      
    jns return_result                                                

return_overflow:
    div rcx                       ; Divide by 0, which will result in returning an overflow.

return_result:
    ret                           ; Return result of the function.