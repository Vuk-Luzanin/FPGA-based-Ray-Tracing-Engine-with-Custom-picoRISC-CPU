include real

; FILE CHECKED - OK

; -------------------------------
; vec3 struct layout in memory:
; vec3.x  -> 0x00 (real)
; vec3.y  -> 0x01 (real)
; vec3.z  -> 0x02 (real)
; 
; ray struct layout in memory:
; ray.origin.x  -> 0x00 (real)
; ray.origin.y  -> 0x01 (real)
; ray.origin.z  -> 0x02 (real)
; ray.direction.x  -> 0x03 (real)
; ray.direction.y  -> 0x04 (real)
; ray.direction.z  -> 0x05 (real)
; -------------------------------

; All functions use arguments as values, not pointers
; vec3_normalized changes t0..t2 registers and a3!!!!

; =========================================================
; vec3_add
; Arguments:
;   a0 - a2: vec3 v1
;   a3 - a5: vec3 v2
; Returns:
;   a0 - a2: vec3 result
; =========================================================
vec3_add:
    add a0, a0, a3 ; x
    add a1, a1, a4 ; y
    add a2, a2, a5 ; z
    ret


; =========================================================
; vec3_subtract
; Arguments:
;   a0 - a2: vec3 v1
;   a3 - a5: vec3 v2
; Returns:
;   a0 - a2: vec3 result
; =========================================================
vec3_subtract:
    sub a0, a0, a3 ; x
    sub a1, a1, a4 ; y
    sub a2, a2, a5 ; z
    ret

; =========================================================
; vec3_multiply_scalar
; Arguments:
;   a0 - a2: vec3 v
;   a3: real scalar
; Returns:
;   a0 - a2: vec3 result
; =========================================================
vec3_multiply_scalar:
    real_mul a0, a0, a3 ; x
    real_mul a1, a1, a3 ; y
    real_mul a2, a2, a3 ; z
    ret

; =========================================================
; vec3_multiply_vec3
; Arguments:
;   a0 - a2: vec3 v1
;   a3 - a5: vec3 v2
; Returns:
;   a0 - a2: vec3 result
; =========================================================
vec3_multiply_vec3:
    real_mul a0, a0, a3 ; x
    real_mul a1, a1, a4 ; y
    real_mul a2, a2, a5 ; z
    ret

; =========================================================
; vec3_length2
; Returns the squared length of a vec3.
; Arguments:
;   a0 - a2: vec3 v1
; Returns:
;   a0: real length2
; =========================================================
vec3_length2:
    real_mul a0, a0, a0 ; a0 = x * x
    real_mul a1, a1, a1 ; a1 = y * y
    add a0, a0, a1 ; a0 = x * x + y * y
    real_mul a1, a2, a2 ; a1 = z * z
    add a0, a0, a1 ; a1 = length2
    ret

; =========================================================
; vec3_length
; Returns the length of a vec3.
; Arguments:
;   a0 - a2: vec3 v1
; Returns:
;   a0: real length
; =========================================================
vec3_length:
    call vec3_length2      ; Compute the squared length
    real_sqrt a0, a0       ; Compute the square root of the squared length
    ret

; =========================================================
; vec3_normalized
; Returns the normalized (unit vector) of a vec3.
; Arguments:
;   a0 - a2: vec3 v1
; Returns:
;   a0: real length
; =========================================================
vec3_normalized:
    load t0, a0
    load t1, a1
    load t2, a2
    call vec3_length2      ; Compute the squared length
    real_sqrt_inv a3, a0   ; Compute the inverse square root
    load a0, t0
    load a1, t1
    load a2, t2

    ; Multiply the vec3 by the inverse length
    call vec3_multiply_scalar
    ret

; =========================================================
; vec3_dot
; Returns the dot product of two vec3s.
; Arguments:
;   a0 - a2: vec3 v1
;   a3 - a5: vec3 v2
; Returns:
;   real result in r0
; =========================================================
vec3_dot:
    real_mul a0, a0, a3
    real_mul a1, a1, a4
    real_mul a2, a2, a5
    add a0, a0, a1
    add a0, a0, a2
    ret


; =========================================================
; vec3_clamp
; Clamp a vec3 color
; Arguments:
;   a0 - a2: vec3 color -> not pointer
; Returns:
;   a0 - a2: clamped vec3
; =========================================================
vec3_clamp:
    real_clamp a0, a0
    real_clamp a1, a1
    real_clamp a2, a2
    ret