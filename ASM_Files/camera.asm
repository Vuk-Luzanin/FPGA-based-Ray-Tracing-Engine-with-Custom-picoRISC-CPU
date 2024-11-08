include vec

; FILE CHECKED -OK

; Define constants for camera dimensions
CAMERA_WIDTH  def 0x320  ; 800
CAMERA_HEIGHT def 0x258  ; 600

; Global variables for the camera system
camera_position     def 0x0
camera_pitch        def 0x0
camera_yaw          def 0x0
camera_cos_pitch    def 0x0
camera_sin_pitch    def 0x0
camera_cos_yaw      def 0x0
camera_sin_yaw      def 0x0
camera_right        def 0x0
camera_up           def 0x0
camera_forward      def 0x0

; =========================================================
; camera_get_ray_direction
; Computes the ray direction given u and v coordinates.
; Arguments:
;   a0: real u
;   a1: real v
; Returns:
;   a0 - a2: vec3 ray direction
; 
; =========================================================
camera_get_ray_direction:
    load a3, a0 ; move u
    load t1, a1 ; save v
    ; load camera_right
    load t0, #camera_right
    load a0, [t0]
    load a1, [t0 + 0x01]
    load a2, [t0 + 0x02]
    ; scale camera_right by u
    call vec3_multiply_scalar
    load t2, a0
    load a4, a1
    load a5, a2
    load a3, t1 ; move v
    ; load camera_up
    load t0, #camera_up
    load a0, [t0]
    load a1, [t0 + 0x01]
    load a2, [t0 + 0x02]
    ; scale camera_up by v
    call vec3_multiply_scalar
    load a3, t2 ; restore scaled camera_right to a3 - a5
    call vec3_add
    load t0, #camera_forward
    ; load camera_forward
    load a3, [t0]
    load a4, [t0 + 0x01]
    load a5, [t0 + 0x02]
    call vec3_add
    call vec3_normalized
    ret

; =========================================================
; camera_init
; Initializes the camera position and angles.
; No arguments.
; Returns:
;   None
; =========================================================
camera_init:
    load a0, zero
    store a0, camera_pitch
    store a0, camera_yaw
    load a1, camera_position
    store a0, [a1] ; camera_position->x
    store a0, [a1 + 0x02] ; camera_position->z
    load a0, #0x00030000
    store a0, [a1 + 0x01]; camera_position->y
    ret
