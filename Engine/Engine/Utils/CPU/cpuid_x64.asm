; Calls CPUID with arguments in EAX and ECX registers
; Output (EAX ~ EDX) is written to the regs

PUBLIC cpuid_x64
.CODE
       
        ALIGN       8
cpuid_x64           PROC FRAME
        sub         rsp, 32
        .allocstack 32
        push        rbx
        .pushreg    rbx
        .endprolog

        mov         r8, rcx
        mov         eax, DWORD PTR [r8 + 0]
        mov         ecx, DWORD PTR [r8 + 8]
        cpuid
        mov         DWORD PTR [r8 + 0], eax
        mov         DWORD PTR [r8 + 4], ebx
        mov         DWORD PTR [r8 + 8], ecx
        mov         DWORD PTR [r8 + 12], edx

        pop         rbx
        add         rsp, 32     

        ret
        ALIGN       8
cpuid_x64 ENDP
_TEXT ENDS

END