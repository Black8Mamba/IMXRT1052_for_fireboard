    .section .text
    .global jump_to_app
    .global jump_to_app_2

jump_to_app:
    CPSID I                  // 关闭全局中断 (Disable IRQ interrupts)
    LDR R0, =0x60002000      // 应用程序的起始地址
    LDR R2, [R0]             // 从应用程序起始地址加载初始堆栈指针到 R2
    MOV SP, R2               // 将 R2 的值加载到 SP（设置堆栈指针）
    LDR R1, [R0, #4]         // 获取应用程序复位向量地址
    BX R1                    // 跳转到应用程序复位向量

jump_to_app_2:
    CPSID I                  // 关闭全局中断 (Disable IRQ interrupts)
    LDR R2, [R0]             // 从应用程序起始地址加载初始堆栈指针到 R2
    MOV SP, R2               // 将 R2 的值加载到 SP（设置堆栈指针）
    LDR R1, [R0, #4]         // 获取应用程序复位向量地址
    BX R1                    // 跳转到应用程序复位向量
