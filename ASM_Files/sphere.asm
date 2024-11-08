
; add instruction jgz!!!

; FILE CHECKED - OK


; =========================================================
; sphere_intersect
; Arguments:
;   a0: const sphere *s (struct pointer)
;   a1: const ray *r (struct pointer)
;   does not use a2 as argument -> instead, it returns value of t trough a1!!!
; Returns:
;   a0: 1 if the ray intersects the sphere, 0 otherwise
;   a1: distance from the origin to the sphere
; =========================================================
sphere_intersect:
    load t0, a0 ; sphere
    load t1, a1 ; ray
    ; load ray_origin
    load a0, [t1]
    load a1, [t1 + 0x01]
    load a2, [t1 + 0x02]
    ; load sphere_center
    load a3, [t0]
    load a4, [t0 + 0x01]
    load a5, [t0 + 0x02]
    call vec3_subtract ; calculate oc
    ; save oc
    load t3, a0
    load t4, a1
    load t5, a2
    ; load ray_direction
    load a3, [t1 + 0x03]
    load a4, [t1 + 0x04]
    load a5, [t1 + 0x05]
    call vec3_dot ; calculate b
    load t1, a0 ; save b, ray is not needed anymore
    ; load oc
    load a0, t3
    load a1, t4
    load a2, t5
    ; load oc
    load a3, t3
    load a4, t4
    load a5, t5
    call vec3_dot
    load a1, [t0 + 0x04]
    sub a1, a0, a1 ; calculate c in a1
    real_mul a0, t1, t1 ; calculate b * b
    sub a0, a0, a1 ; calculate discriminant
    jgz a0, sphere_intersect_else
sphere_intersect_fail:
    load a0, zero
    ret
sphere_intersect_else:
    real_sqrt a0, a0
    neg t1, t1           ; t1 = -b
    sub t2, t1, a0  ; temp = -b - sqrt_discriminant
    jgz t2, sphere_intersect_success ; If temp > 0, record t
    add t2, t1, a0  ; temp = -b + sqrt_discriminant
    jgz t2, sphere_intersect_success ; If temp > 0, record t
    jmp sphere_intersect_fail
sphere_intersect_success:
    load a0, 0x01
    load a1, t2
    ret