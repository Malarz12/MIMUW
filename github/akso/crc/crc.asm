section .data
    segment_switch      db 1, 1, 2, 3       ; Four bytes for switching segments.
    segment_length      db 1, 1             ; Two bytes reserved for the length of each segment.

section .bss
    result              resb 65             ; 65 because it is the maximum length of the CRC polynomial, so the remainder cannot be longer.
    array_data          resq 256            ; Memory allocated for lookup table, which will be used to efficiently compute the checksum.
    segment_data        resb 65541          ; Memory allocated for the longest possible segment.
                                            ; The maximum length of a segment is 65535. 
                                            ; The beginning of the segment has a length of 2 bytes, and the segment ends with 4 bytes.
                                            ; So 65535 + 2 + 4 = 65541.
section .text
    global _start

_start:
    xor rbx, rbx                      ; This will hold length of polynomial.
    xor r8 , r8                       ; r8 will hold the polynomial.
    xor r9 , r9                       ; r9 will hold the values for each entry in the array.
    xor r10, r11                      ; Initialize CRC to 0.
    xor r11, r11                      ; r11 will hold the numbers in array.

    mov rcx, 64                     ; rcx will hold the shift count: 64 - length of polynomial.
    mov rdi, [rsp + 8 * 3]          ; rdi points to the start of the polynomial string.

get_polynomial:                     ; Loop to load the CRC polynomial.
    movzx r10, byte[rdi]
   
    cmp r10b, 0
    je end_of_polynomial            ; If end of string, break the loop.

    sub rcx, 1                      ; Decrement shift count.
    sal r8, 1                       ; Shift left r8 by 1.
    cmp r10b, '0'
    je if_zero
    cmp r10b, '1'
    je if_one

wrong_polynomial:
    pop rbx
    mov rdi, 1                      ; This will indicate wrong data.
    mov rax, 60                     ; This rax will cause exiting, program if no polynomial was read.
    syscall
    
if_one:
    add r8, 1                       ; Set the least significant bit to 1.

if_zero:
    add rdi, 1                      ; Move to the next character.
    jmp get_polynomial              ; Get back to "get_polynomial" and continue the loop.

end_of_polynomial:
    sal r8, cl                      ; Shift the polynomial to the most significant positions.
    mov rbx, rcx                    ; Save the length of the polynomial.

handling_no_polynomial:
    cmp rcx, 64
    jne array_main                  ; If no polynomial was read, exit with error.
    pop rbx
    mov rdi, 1                      ; This will indicate wrong data.
    mov rax, 60                     ; This rax will cause exiting, program if no polynomial was read.
    syscall

array_main:                         ; The next segment concentrates on creating an lookup table,
                                    ; which will be used for computing the result.
    mov edi, array_data             ; rdi points to the start of the array.
    mov r9, r11
    sal r9, 8 * 7                   ; Shift left r9 by 56 bits.
    mov rax, 0
    clc                             ; Setting carry flag to 0.

array_loop_1:               
    sal r9, 1                       ; Shift left r9 by 1.
    jnc array_loop_2                ; If no carry, go to array_loop_end.
    xor r9, r8                      ; XOR r9 with the polynomial.

array_loop_2:
    add rax, 1                      ; Increment loop counter.
    cmp rax, 8                      ; Check if  iterations have been completed.
    jl array_loop_1                 ; Jump back to array_loop if less than 8 iterations.

    mov [rdi + 8 * r11], r9         ; Store the computed value in the array.
    add r11, 1                      ; Move to the next array element.
    cmp r11, 256                    ; Check if all array elements have been processed.
    jle array_main                  ; If not, jump back to array_main.

get_file:                           ; Load the file for CRC calculation.
    mov rdi, [rsp + 8 * 2]          ; Filename is the second argument.
    mov rax, 2                      ; Set rax to 2 to open the file.
    syscall                         ; Perform syscall to open the file.
    mov r8, rax                     ; Store the file descriptor.

read_beggining_of_segment:          ; Read the initial file pointer position.
    mov rdi, r8                     ; Move the file descriptor to rdi.
    mov rdx, 1                      ; Number of bytes to move.
    xor rsi, rsi                      ; Offset from current position.
    mov rax, 8                      ; Set rax to 8 to change the current file pointer.
    syscall                         ; Perform syscall to change the file pointer.

    mov r9, rax                     ; Save the current file pointer position.

    mov rax, 0                      ; Set rax to 0 for syscall: sys_read.
    mov rsi, segment_length         ; Address of the buffer for the segment length.
    mov rdx, 2                      ; Number of bytes to read.
    syscall                         ; Perform syscall to read the segment length.

compute_crc:                        ; Read the current segment into segment_data.
                                    ; Prepare data for computing CRC.
    mov rdi, r8                     ; Move the file descriptor to rdi.
    xor rax, rax                      ; Set rax to 0 for syscall: sys_read.
    xor rdx, rdx
    mov dx, word[segment_length]    ; Load the segment length into rdx.
    mov rsi, segment_data           ; Address of the buffer for the segment data.
    syscall                         ; Perform syscall to read the segment data.

    cmp rdx, 0                      ; In case the segment length is equal to 0.
    je find_next_segment        ; Jump to find_next_segment.
                                    ; If not move data to registers to calculate CRC.
    xor rdx, rdx
    mov dx, [segment_length]        ; Move the segment length into dx.

    xor r11, r11                      ; Initialize index for accessing segment data.
    mov rax, segment_data           ; Load the address of the segment data into rax.
    mov rdi, array_data             ; Load the address of the array into rdi.

compute_crc_loop:                   ; Loop to calculate CRC for each byte in the segment.
                                    ; Based on the one on Wikipedia.
    xor rsi, rsi
    movzx rsi, word[r11 + rax]      ; Load the current byte from segment data into rsi.
    add r11, 1                      ; Move to the next byte in the segment.

    sal rsi, 8 * 7                  ; Shift left rsi by 56 bits.
    xor rsi, r10                    ; XOR rsi with the current CRC value in r10.
    sal r10, 8                      ; Shift left CRC value in r10 by 8 bits.
    shr rsi, 8 * 7                  ; Shift right rsi by 56 bits to clear upper bits.
    xor r10, [rdi + 8 * rsi]        ; XOR CRC value in r10 with corresponding value from the array.

    cmp r11, rdx                    ; Compare current index with segment length.
    jge find_next_segment           ; If end of segment, jump to find_next_segment.
    loop compute_crc_loop           ; Loop until all bytes in the segment are processed.

find_next_segment:                  ; Look for the next segment.
    mov rsi, segment_switch         ; Address of the buffer for the segment switch.
    mov rdx, 4                      ; Number of bytes to read for the segment switch.
    xor rax, rax                      ; Syscall: sys_read to read the segment switch.
    mov rdi, r8                     ; Move the file descriptor to rdi.
    syscall                         ; Perform syscall to read the segment switch.

    movsx rsi, dword[segment_switch]    ; Sign extend the segment jump value to rsi.
    mov rdi, r8                         ; Move the file descriptor to rdi.
    mov rax, 8                          ; Syscall: sys_lseek to move the file pointer.
    mov rdx, 1                          ; Set offset to 1.
    syscall                             ; Perform syscall to move the file pointer.

    cmp rax, 0                      ; Check if syscall failed.
    js handling_wrong_file          ; If syscall failed, handle wrong file.
    
    cmp rax, r9                     ; Compare current file pointer position with the saved position.
    jne read_beggining_of_segment   ; If not at the saved position, go back to read_beggining_of_segment.

system_translation:                 ; Finalize and return CRC.
    mov rcx, rbx                    ; Move the polynomial length into rcx.
    sar r10, cl                     ; Shift right CRC by the offset.
    mov rsi, result + 8 * 8         ; Address of the result buffer.
    mov r9, 8 * 8                   ; Set r9 to the buffer size.
    sub r9, rcx                     ; Subtract the polynomial length from the buffer size.
    mov rcx, r9                     ; Move the adjusted buffer size into rcx.
    sub rcx, 1                      ; Adjust rcx for loop iteration.

system_translation_loop:            ; Loop to convert CRC to ASCII characters.
    sub rsi, 1                      ; Move to the previous byte in the result buffer.
    mov rdx, r10                    ; Move CRC value into rdx.
    sar r10, 1                      ; Shift right CRC by 1.
    and rdx, 1                      ; Mask the least significant bit of CRC.
    or dl, '0'                      ; Convert the bit to ASCII.
    mov [rsi], dl                   ; Store the ASCII character in the result buffer.
    sub rcx, 1                      ; Decrement loop counter.
    cmp rcx, 0                      ; Check if all bytes are processed.
    jge system_translation_loop     ; If not, repeat the loop.

write_result:                       ; Write the result to stdout.
    mov qword[result + 8 * 8], 10   ; Add newline character to the result.
    mov rax, 3                      ; Syscall: sys_write to print the result.
    syscall                         ; Perform syscall to write the result.

close_file:                         
    mov rdi, 1                      ; Set rdi to 1 for syscall: sys_exit.
    mov rdx, r9                     ; Move the buffer size into rdx.
    add rdx, 1                      ; Adjust rdx for syscall.
    mov rax, 1                      ; Set rax to 1 for syscall: sys_exit.
    syscall                         ; Perform syscall to close the file and exit.
    mov rdi, 0                      ; Setting exit code to 0.
    jmp exit                        ; Jumping to exit.

handling_wrong_file:                
    mov rdi, 1                      ; This will indicate wrong data.
    mov rax, 3                      ; Set rax to 3 for syscall: sys_exit.
    syscall                         ; Perform syscall to handle the wrong file.

exit:                               ; Exit the program.
    pop rbx
    mov rax, 60                     ; Set rax to 60 for syscall: sys_exit.
    syscall                         ; Perform syscall to exit the program.