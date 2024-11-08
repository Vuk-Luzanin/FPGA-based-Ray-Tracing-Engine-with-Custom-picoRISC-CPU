include material

; Define constants
MAX_DEPTH                               def 0x5
MAX_SPHERES                             def 0x20
SCENE_SPAWN_DISTANCE                    def 0x00010000
SCENE_GHOST_RADIUS_INIT                 def 0x00008000
SCENE_GHOST_RADIUS_INIT                 def 0x00000400
SCENE_GHOST_RADIUS_MIN                  def 0x00002000
SCENE_GHOST_RADIUS_MIN2                 def 0x00000200
scene_ghost_radius                      def 0x00010000
scene_ghost_radius2                     def 0x00008000
scene_ghost_mode                        def 0x00000400
scene_num_of_spheres                    def 0x00002000
scene_marked_sphere_address             def 0x00000200
SCENE_FLOAT_INFINITY                    def 0x7FFFFFFF
SCENE_EPSILON                           def 0x00000147
OFFSET                                  def 0x00030000
scene_spheres                           def 0x00000000

; =========================================================
; scene_get_sky_color
; Arguments:
;   a0: real direction_y
; Returns:
;   a0 - a2: vec3 color
; =========================================================
scene_get_sky_color:
    ; a3 = k
    asl a0
    real_clamp a0, a0
    load a1, #OFFSET
    add a3, a0, a1
    asr a3
    asr a3
    ; load sky_color
    load t0, #material_color_sky
    load t1, material_light_mode
    load t2, t1
    asl t1          ;   material_light_mode * 2
    add t1, t1, t2  ;   material_light_mode * 3
    add t0, t0, t1
    load a0, [t0]
    load a1, [t0 + 0x01]
    load a2, [t0 + 0x02]
    call vec3_multiply_scalar
    ; move sky_color for now
    load t2, a0
    load a4, a1
    load a5, a2
    ; k = 0x0000ffff - k;
    load t0, #0x0000ffff
    sub a3, t0, a3
    ; load horizon_color
    load t0, #material_color_horizon
    ; in t1 is already material_light_mode * 3
    add t0, t0, t1
    load a0, [t0]
    load a1, [t0 + 0x01]
    load a2, [t0 + 0x02]
    call vec3_multiply_scalar
    ; compute result
    load a3, t2
    call vec3_add
    ret


; need to implement real_div
; =========================================================
; scene_intersect_plane
; Arguments:
;   a0: real origin_y
;   a1: real direction_y
; Returns:
;   a0: 1 if the ray intersects the plane, 0 otherwise
;   a1: distance from the origin to the plane = t (caller allocates)
; =========================================================
scene_intersect_plane:
    jnz a1, scene_intersect_plane_continue
scene_intersect_plane_fail: 
    load a0, zero
    ret
scene_intersect_plane_continue:
    xor a2, a0, a1
    load a3, 0x80000000
    and a2, a2, a3
    jz a2, scene_intersect_plane_fail
    ; real_div
    call real_div
    load a1, a0
    sub a1, zero, a1            ; a1 = -a1
    load a0, 0x01
    ret


; =========================================================
; scene_transform
; Arguments:
;   a0: real t
; Returns:
;   a0: real f(t)
; =========================================================
scene_transform:
    jgez a0, scene_transform_continue
scene_transform_bound:
    load a0, zero
    ret
scene_transform_continue:
    asr a0
    asr a0
    asr a0
    asr a0
    asr a0
    asr a0
    asr a0
    real_mul a0, a0, a0
    load t0, #0x0000ffff
    sub a0, t0, a0
    jlz a0, scene_transform_bound
    ret


; =========================================================
; scene_get_floor_color
; Arguments:
;   a0: real x
;   a1: real z
;   a2: real t
; Returns:
;   a0 - a2: vec3 color
; =========================================================
scene_get_floor_color:
    int_real a0, a0
    int_real a1, a1
    add a0, a0, a1
    load t0, 0x01
    and a0, a0, t0
    load t1, material_light_mode
    load t2, t1
    asl t1
    add t1, t1, t2
    jz a0, base2
    load t0, #material_color_floor1
    add t0, t0, t1
    jmp end_if
base2:
    load t0, #material_color_floor2
    add t0, t0, t1
end_if:
    load t2, [t0]
    load t3, [t0 + 0x01]
    load t4, [t0 + 0x02]
    load a0, a2
    call transform
    load a3, a0
    load t0, a0
    load a0, t2
    load a1, t3
    load a2, t4
    call vec3_multiply_scalar
    load t2, a0
    load t3, a1
    load t4, a2
    load a3, 0x0000ffff
    sub t0, a3, t0
    load a3, #material_color_floor_average
    add a3, a3, t1
    load a0, [a3]
    load a1, [a3 + 0x01]
    load a2, [a3 + 0x02]
    load a3, t0
    call vec3_multiply_scalar
    load a3, t2
    load a4, t3
    load a5, t4
    call vec3_add
    ret




; =========================================================
; scene_reset
; Arguments: none
; Returns: none
; =========================================================
scene_reset:
    call camera_init
    store zero, scene_num_of_spheres            ; memdir address mode
    store zero, scene_marked_sphere_address
    store zero, scene_ghost_mode
    ret


; static int index
index: dd 0x0
reflexivities: dd 0x00008000, 0x00008000, 0x00008000, 0x00008000

; =========================================================
; scene_add_sphere
; Arguments: 
;   a0..2: vec3 position
;   a3: radius
;
; Returns: none
; t4 register is for trash
; =========================================================
scene_add_sphere:
    load t0, scene_num_of_spheres
    load t4, #MAX_SPHERES
    sub t4, t0, t4
    jgez t4, end_add_sphere            ; *scene_num_of_spheres >= MAX_SPHERES
    sub t4, a1, a3
    jlz t4, end_add_sphere             ; position->y < radius

    ; check if new sphere overlaps another sphere
    load t1, #scene_spheres                     ; address of first sphere
    ; find end of list of spheres
    load t2, t0 
    asl t2
    asl t2
    asl t2
    add t2, t2, t0                  ;  9 * *scene_num_of_spheres
    add t2, t1, t2                  ; end - address

scene_add_sphere_loop:
    sub t4, t1, t2
    jz t4, scene_add_sphere_break       ;sp == end?
    ; pointer 
    ; move position and radius
    load t4, a0
    load t5, a1
    load t6, a2
    load t3, a3     
    load a3, [t1]
    load a4, [t1 + 0x01]
    load a5, [t1 + 0x01]
    call vec3_subtract
    ; move pointer
    load a3, a0
    load a4, a1
    load a5, a2

    ; min_distance
    load a0, [t1 + 0x03]        ; * (sp + 3)
    add a0, t3, a0 
    real_mul t0, a0, a0

    ; length2 from pointer
    load a0, a3
    load a1, a4
    load a2, a5
    call vec3_length2
    sub a0, a0, t0
    jlz a0, end_add_sphere

    load a0, 0x09
    add t1, t1, a0
    ; prepare values for next iteration (initial arguments)
    load a0, t4
    load a1, t5
    load a2, t6
    load a3, t3
    jmp scene_add_sphere_loop

scene_add_sphere_break:
    real_mul a4, a3, a3         ; radius2
    load a5, #material_color_sphere
    load t0, index
    load t1, t0
    asl t0
    add t0, t0, t1
    add a5, a5, t0
    load t4, [a5]
    load t5, [a5 + 0x01]
    load t6, [a5 + 0x02]
    load t0, #reflexivities
    add t0, t0, t1 
    load t3, [t0]
    inc t1                          ; index++
    load t2, 0x03
    and t1, t1, t2
    store t1, index

    ; adding new sphere
    load a5, scene_num_of_spheres
    load t2, a5
    asl t2
    asl t2
    asl t2
    add t2, t2, a5
    load t1, #scene_spheres
    add t1, t1, t2
    store a0, [t1]
    store a1, [t1 + 0x01]
    store a2, [t1 + 0x02]
    store a3, [t1 + 0x03]
    store a4, [t1 + 0x04]
    ; color
    store t4, [t1 + 0x05]
    store t5, [t1 + 0x06]
    store t6, [t1 + 0x07]

    store t3, [t1 + 0x08]

    ; post-decrement scene_num_of_spheres
    inc a5
    store a5, scene_num_of_spheres

end_add_sphere:
    ret




; =========================================================
; scene_remove_sphere
; Arguments: 
;   a0: sphere *sp (address)
;
; Returns: none
; =========================================================
scene_remove_sphere:
    jz a0, end_remove_sphere          
    load t0, scene_num_of_spheres
    load t1, zero               ; i = 0

loop_remove_spheres:
    sub t4, t1, t0
    jgez t4, end_remove_sphere      ; check i < *scene_num_of_spheres

    ; load current sphere address  
    load a1, #scene_spheres
    load a2, t1
    asl a2
    asl a2
    asl a2
    add a2, a2, t1              ; a2 = i*9
    add t2, a1, a2              ; t2 = &scene_spheres[i]
    sub t4, a0, t2              ; t2 == sp?
    jz t4, remove_sphere
    inc t1
    jmp loop_remove_spheres     ; loop

remove_sphere:
    dec t0
    store t0, scene_num_of_spheres
    load a4, t0
    asl t0
    asl t0
    asl t0
    add t0, t0, a4              ; --(*scene_num_of_spheres) * 9
    add t0, a1, t0              ; &scene_spheres[--(*scene_num_of_spheres)]
    load t0, [t0]               ; scene_spheres[--(*scene_num_of_spheres)]
    store t0, [t2]              ;  scene_spheres[i] = scene_spheres[--(*scene_num_of_spheres)]
end_remove_sphere:
    ret
