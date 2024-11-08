org 0x100   ; Starting at address 0x100

start main

main:
; Main program starts
    load r1, #5          ; num = 5
    call factorial       ; fact = factorial(num)
    store r1, fact_result ; Store result of factorial

    load r1, #5          ; num = 5
    call fibonacci       ; fib = fibonacci(num)
    store r1, fib_result ; Store result of fibonacci

    load r1, array_addr  ; r1 points to the start of the array
    load r2, #5          ; r2 = array size
    call sumArray        ; arrSum = sumArray(arr, 5)
    store r1, arrSum_result ; Store result of array sum

    ; End of main program
    ret

; Factorial function (recursive)
factorial:
    load r2, #1         ; Base case: if n <= 1
    jlez r1, factorial_end ; Jump if n <= 1
    load r3, r1         ; r3 = n
    dec r1              ; r1 = n - 1
    call factorial      ; recursive call factorial(n-1)
    mul r1, r1, r3      ; r1 = n * factorial(n-1)

factorial_end:
    ret

; Fibonacci function (recursive)
fibonacci:
    load r2, #0         ; if n == 0, return 0
    jz r1, fib_base_case0
    load r2, #1         ; if n == 1, return 1
    jz r1, fib_base_case1
    load r3, r1         ; Save n in r3
    dec r1              ; n - 1
    call fibonacci      ; fibonacci(n-1)
    store r1, fib_temp1 ; Store temp result
    load r1, r3         ; Restore n
    load r4, #2
    sub r1, r1, r4      ; n - 2
    call fibonacci      ; fibonacci(n-2)
    load r2, fib_temp1  ; Load previous result
    add r1, r1, r2      ; r1 = fibonacci(n-1) + fibonacci(n-2)

fib_base_case0:
    load r1, #0
    ret
fib_base_case1:
    load r1, #1
    ret

; Sum array function
sumArray:
    load r3, #0         ; sum = 0
sum_loop:
    jz r2, sum_end      ; If size == 0, jump to end
    load r4, [r1]       ; Load arr[i] into r4
    add r3, r3, r4      ; sum += arr[i]
    inc r1              ; Increment array pointer
    dec r2              ; Decrease size
    jmp sum_loop        ; Repeat
sum_end:
    load r1, r3         ; Store sum in r1 (return value)
    ret

; Data Section
fact_result: dd 0x0           ; Placeholder for factorial result
fib_result: dd 0x0            ; Placeholder for fibonacci result
arrSum_result: dd 0x0         ; Placeholder for array sum result
fib_temp1: dd 0x0             ; Temporary variable for Fibonacci

array_addr : dd 1, 2, 3, 4, 5  ; Array with 5 elements

lookup: dd (0x3 up 0x6)
