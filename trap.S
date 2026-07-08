.section .text
.global trap_entry
.align 4

trap_entry:
    addi sp, sp, -256

    sd ra, 8(sp)
    sd t0, 16(sp)
    sd t1, 24(sp)
    sd t2, 32(sp)
    sd s0, 40(sp)
    sd s1, 48(sp)
    sd a0, 56(sp)
    sd a1, 64(sp)
    sd a2, 72(sp)
    sd a3, 80(sp)
    sd a4, 88(sp)
    sd a5, 96(sp)
    sd a6, 104(sp)
    sd a7, 112(sp)
    sd s2, 120(sp)
    sd s3, 128(sp)
    sd s4, 136(sp)
    sd s5, 144(sp)
    sd s6, 152(sp)
    sd s7, 160(sp)
    sd s8, 168(sp)
    sd s9, 176(sp)
    sd s10, 184(sp)
    sd s11, 192(sp)
    sd t3, 200(sp)
    sd t4, 208(sp)
    sd t5, 216(sp)
    sd t6, 224(sp)
    sd gp, 232(sp)
    sd tp, 240(sp)

    csrr t0, mepc
    sd t0, 0(sp)
    csrr t0, mstatus
    sd t0, 248(sp)

    mv a0, sp
    call trap_dispatch
    mv sp, a0

    ld t0, 0(sp)
    csrw mepc, t0
    ld t0, 248(sp)
    csrw mstatus, t0

    ld ra, 8(sp)
    ld t0, 16(sp)
    ld t1, 24(sp)
    ld t2, 32(sp)
    ld s0, 40(sp)
    ld s1, 48(sp)
    ld a0, 56(sp)
    ld a1, 64(sp)
    ld a2, 72(sp)
    ld a3, 80(sp)
    ld a4, 88(sp)
    ld a5, 96(sp)
    ld a6, 104(sp)
    ld a7, 112(sp)
    ld s2, 120(sp)
    ld s3, 128(sp)
    ld s4, 136(sp)
    ld s5, 144(sp)
    ld s6, 152(sp)
    ld s7, 160(sp)
    ld s8, 168(sp)
    ld s9, 176(sp)
    ld s10, 184(sp)
    ld s11, 192(sp)
    ld t3, 200(sp)
    ld t4, 208(sp)
    ld t5, 216(sp)
    ld t6, 224(sp)
    ld gp, 232(sp)
    ld tp, 240(sp)

    addi sp, sp, 256
    mret



.global scheduler_start
scheduler_start:
    mv sp, a0 
   
    ld t0, 0(sp)
    csrw mepc, t0
    ld t0, 248(sp)
    csrw mstatus, t0

    ld ra, 8(sp)
    ld t0, 16(sp)
    ld t1, 24(sp)
    ld t2, 32(sp)
    ld s0, 40(sp)
    ld s1, 48(sp)
    ld a0, 56(sp)
    ld a1, 64(sp)
    ld a2, 72(sp)
    ld a3, 80(sp)
    ld a4, 88(sp)
    ld a5, 96(sp)
    ld a6, 104(sp)
    ld a7, 112(sp)
    ld s2, 120(sp)
    ld s3, 128(sp)
    ld s4, 136(sp)
    ld s5, 144(sp)
    ld s6, 152(sp)
    ld s7, 160(sp)
    ld s8, 168(sp)
    ld s9, 176(sp)
    ld s10, 184(sp)
    ld s11, 192(sp)
    ld t3, 200(sp)
    ld t4, 208(sp)
    ld t5, 216(sp)
    ld t6, 224(sp)
    ld gp, 232(sp)
    ld tp, 240(sp)

    addi sp, sp, 256
    mret

