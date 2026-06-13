# ============================================================
# Startup.S - RV64 bare-metal boot entry for QEMU virt
# ============================================================
.section .text.init
.global _start

_start:
    # ----- 1) hart 가드: hart 0만 살리고 나머지는 재움 -----
    csrr  t0, mhartid          # 현재 hart(코어) ID 읽기
    bnez  t0, park             # 0번이 아니면 park로

    # ----- 2) 스택 포인터 세팅 -----
    la    sp, _stack_top       # 링커가 정한 스택 꼭대기

    # ----- 3) .bss 0으로 클리어 -----
    la    t0, _bss_start
    la    t1, _bss_end
1:
    bgeu  t0, t1, 2f
    sd    zero, (t0)           # RV64 = 8바이트 단위 store
    addi  t0, t0, 8
    j     1b
2:
    # ----- 4) C 세계로 진입 -----
    call  main

    # main이 리턴하면 여기로 떨어짐 → 무한 대기
park:
    wfi                        # wait-for-interrupt, 저전력 대기
    j     park
