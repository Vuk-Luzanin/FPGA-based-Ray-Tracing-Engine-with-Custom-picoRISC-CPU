include scene


; =========================================================
; scene_interpolate
; Arguments:
;   a0..2 - vec3 color
;   a3 - const material* mat
; Returns:
;   a0 - a2: vec3 result color
; =========================================================
scene_interpolate:
    load t0, [a3]                  ; t1 = mat->color.x
    load t1, [a3 + 0x01]           ; t1 = mat->color.y
    load t2, [a3 + 0x02]           ; t2 = mat->color.z
    load t3, [a3 + 0x03]           ; t3 = mat->reflectivity
    load t4, #0x00010000
    sub t4, t4, t3                 ; t4 = 0x00010000 - mat->reflectivity
    ; move color xyz
    load t6, a0
    load a4, a1
    load a5, a2
    load a0, t0
    load a1, t1
    load a2, t2
    load a3, t4
    call vec3_multiply_scalar
    load t0, a0
    load t1, a1
    load t2, a2
    ; prepare for temp1
    load a0, t6
    load a1, a4
    load a2, a5
    load a3, t3
    call vec3_multiply_scalar
    load a3, t0
    load a4, t1
    load a5, t2
    call vec3_add
    call vec3_clamp 
    ret



; =========================================================
; scene_trace_ray
; Arguments:
; sp pokazuje na prvu slobodnu lokaciju, a stek raste ka nizim adresama
; argumenti se stavljaju na stek s desna na levo
;   on stack ()
; Returns:
;   vec3 - x, y, z (skidati z, pa y, pa x)
; =========================================================
scene_trace_ray:
    ; remove args from stack
    pop t0                            ; t0 = ray *r
    pop t1                            ; t1 = depth
    ; push s registers
    push s0
    push s1
    push s2
    load t6, #MAX_DEPTH
    sub t6, t1, t6
    jlz t6, scene_trace_ray_depth_ok
    load a0, [t0 + 0x04]                ; t1 = r->direction.y
    call scene_get_sky_color
scene_trace_ray_return:
    ; pop s registers
    pop s2
    pop s1
    pop s0
    ; push return value on stack
    push a0
    push a1
    push a2
    ret

scene_trace_ray_depth_ok:
    load t2, #SCENE_FLOAT_INFINITY
    load t3, zero
    load t4, zero
    load t5, t2

    ; for loop
    load a4, zero                           ; i = 0
    load a5, scene_num_of_spheres
scene_trace_ray_loop:
    sub t6, a4, a5
    jgez t6, scene_intersect_plane_check

    load s0, #scene_spheres
    load t6, a4
    asl t6
    asl t6
    asl t6
    add t6, t6, a4
    add s0, s0, t6
    load a0, s0
    load a1, t0
    call sphere_intersect
    jz a0, scene_trace_ray_loop         ; izlaz iz prvog if-a0

    sub t6, a1, t2
    jgez t6, scene_trace_ray_loop       ; izlaz iz drugog if-a
    load t2, a1
    load t3, s0
    jmp scene_trace_ray_loop

scene_intersect_plane_check:
    jnzs t3, scene_intersect_plane_check_end
    load a0, [t0 + 0x01]
    load a1, [t0 + 0x04]
    call scene_intersect_plane
    load t5, a1                                 ; write t_plane
    jz a0, scene_intersect_plane_check_end
    load t2, a1
    load t3, zero
    load t4, 0x01

scene_intersect_plane_check_end:
    jnz t3, scene_trace_ray_continue
    jnz t4, scene_trace_ray_continue
    load a0, [t0 + 0x04]
    call scene_get_sky_color
    jmp scene_trace_ray_return

scene_trace_ray_continue:
    load t6, #SCENE_EPSILON
    sub t2, t2, t6
    ; temp0
    load a0, [t0 + 0x03]
    load a1, [t0 + 0x04]
    load a2, [t0 + 0x05]
    load a3, t2
    call vec3_multiply_scalar
    load s0, a0
    load s1, a1
    load s2, a2
    ; load r->origin
    load a3, [t0]
    load a4, [t0]
    load a5, [t0]
    call vec3_add
    ; za dalje dodati jos s registara



; pomocni kod
; =========================================================
scene_trace_ray:
    ; for (int i = 0; i < *scene_num_of_spheres; ++i)
    load t2, scene_num_of_spheres
    load t3, zero                   ; i = 0

sphere_check_plane:
    ; Check for intersection with the plane
    load t4, r->origin.y
    load t5, r->direction.y
    call scene_intersect_plane
    jz finalize                      ; no plane intersection
    ; if there is a plane hit
    load hit_plane, #1              ; hit_plane = 1
    load t_closest, t_plane         ; t_closest = t_plane

finalize:
    ; Process the hit
    jz sky_color_return              ; if no hit, return sky color

    ; Decrease t_closest by EPSILON
    load t4, t_closest
    sub t4, t4, #EPSILON
    load t0, r->direction            ; load ray direction
    call vec3_multiply_scalar        ; multiply direction by t_closest

    ; hit_point = r->origin + temp0
    call vec3_add                   ; calculate hit point

    ; Check if we hit a plane or sphere
    load t1, hit_plane
    jz sphere_hit                    ; if hit_plane == 0, it's a sphere
    call scene_get_floor_color       ; if plane hit, get floor color
    load mat.color, a0              ; set material color
    load mat.reflectivity, #0x00003333
    ; Reflect direction
    load reflected_direction, r->direction
    not reflected_direction.y
    jmp trace_ray

sphere_hit:
    call vec3_subtract               ; get normal and material
    load mat, closest_sphere->mat
    ; Compute reflection direction
    call vec3_dot                    ; calculate dot product
    ; double it
    shl t0, t0, 1
    call vec3_multiply_scalar        ; multiply normal by scalar
    call vec3_subtract               ; reflected direction

trace_ray:
    load hit_point, reflected_direction
    inc a1                           ; depth + 1
    call scene_trace_ray             ; recursively trace the ray
    call scene_interpolate           ; interpolate colors
    ret


------------------------------------------------------------------