; Complete MASM 6.14 Driver Code
.386
.model flat, stdcall
option casemap:none

; Constants
IMAGE_DOS_SIGNATURE    equ 5A4Dh    ; MZ
IMAGE_NT_SIGNATURE    equ 00004550h ; PE00

; External function declarations
extrn ZwOpenKey@12:PROC
extrn ZwQueryValueKey@24:PROC 
extrn ZwClose@4:PROC

; Data segments
.data
ALIGN 4

; Registry key and value names
reg_key db 'vkfrva',0
ALIGN 4
query_value dd 0Ch, 0Eh

; Code segment
.code

; Main entry point
start:
    push ebp
    mov ebp, esp
    sub esp, 30h        ; Local variables space
    xor ebx, ebx

    ; Initialize OBJECT_ATTRIBUTES structure
    lea ecx, [ebp-1Ch]
    mov dword ptr [ecx], 18h    ; Size of OBJECT_ATTRIBUTES
    mov [ecx+4], ebx            ; RootDirectory = NULL
    mov [ecx+0Ch], ebx          ; ObjectName.Buffer
    mov [ecx+10h], ebx          ; ObjectName.Length
    push dword ptr [ebp+0Ch]    ; param_2
    pop dword ptr [ecx+8]       ; Store in ObjectName
    and dword ptr [ecx+14h], 0  ; Attributes = 0

    ; Open registry key
    push ecx                    ; ObjectAttributes
    push 20019h                 ; DesiredAccess
    lea eax, [ebp-20h]         ; KeyHandle
    push eax
    call ZwOpenKey@12
    test eax, eax
    jnz exit_handler

    ; Query registry value
    push 0                      ; ReturnLength
    push esp                    ; ResultLength
    push 10h                    ; Length
    lea eax, [ebp-30h]         ; Buffer
    push eax
    push 2                      ; KeyValueInformationClass
    push offset query_value     ; ValueName
    push dword ptr [ebp-20h]    ; KeyHandle
    call ZwQueryValueKey@24
    pop ecx
    test eax, eax
    jnz exit_handler

    ; Close registry key
    push dword ptr [ebp-20h]
    call ZwClose@4

    ; Get value and process
    lea ecx, [ebp-30h]
    mov eax, [ecx+0Ch]
    mov [ebp-4], eax           ; local_8 = value

    ; Value comparison and processing
    cmp dword ptr [ebp-4], 1
    jnz check_value_2
    
    call send_cmd_type1
    call delay_routine
    call send_cmd_type3
    jmp exit_handler

check_value_2:
    cmp dword ptr [ebp-4], 2
    jnz check_value_3
    
    call send_cmd_type2
    call delay_routine
    call send_cmd_type2
    call delay_routine
    call send_cmd_type4
    call delay_routine
    call send_cmd_type5
    jmp exit_handler

check_value_3:
    cmp dword ptr [ebp-4], 3
    jnz check_value_4
    
    call send_cmd_type3
    call delay_routine
    call send_cmd_type3
    call delay_routine
    call send_cmd_type4
    call delay_routine
    call send_cmd_type5
    jmp exit_handler

check_value_4:
    cmp dword ptr [ebp-4], 4
    jnz check_value_5
    
    call send_cmd_type4
    call delay_routine
    call send_cmd_type4
    call delay_routine
    call send_cmd_type4
    call delay_routine
    call send_cmd_type5
    jmp exit_handler

check_value_5:
    cmp dword ptr [ebp-4], 5
    jnz exit_handler
    
    call send_cmd_type5
    jmp exit_handler

exit_handler:
    mov eax, 0C0000182h        ; STATUS_DEVICE_CONFIGURATION_ERROR
    leave
    ret 8

; Command sending functions
send_cmd_type1 proc
    call wait_port
    mov al, 0D2h
    out 64h, al
    call wait_port
    mov al, 13h
    out 60h, al
    call wait_port
    mov al, 0D2h
    out 64h, al
    call wait_port
    mov al, 93h
    out 60h, al
    ret
send_cmd_type1 endp

send_cmd_type2 proc
    call wait_port
    mov al, 0D2h
    out 64h, al
    call wait_port
    mov al, 19h
    out 60h, al
    call wait_port
    mov al, 0D2h
    out 64h, al
    call wait_port
    mov al, 99h
    out 60h, al
    ret
send_cmd_type2 endp

send_cmd_type3 proc
    call wait_port
    mov al, 0D2h
    out 64h, al
    call wait_port
    mov al, 1Eh
    out 60h, al
    call wait_port
    mov al, 0D2h
    out 64h, al
    call wait_port
    mov al, 9Eh
    out 60h, al
    ret
send_cmd_type3 endp

send_cmd_type4 proc
    call wait_port
    mov al, 0D2h
    out 64h, al
    call wait_port
    mov al, 0Fh
    out 60h, al
    call wait_port
    mov al, 0D2h
    out 64h, al
    call wait_port
    mov al, 8Fh
    out 60h, al
    ret
send_cmd_type4 endp

send_cmd_type5 proc
    call wait_port
    mov al, 0D2h
    out 64h, al
    call wait_port
    mov al, 1Ch
    out 60h, al
    call wait_port
    mov al, 0D2h
    out 64h, al
    call wait_port
    mov al, 9Ch
    out 60h, al
    ret
send_cmd_type5 endp

; Utility functions
wait_port proc
    in al, 64h
    test al, 2
    jnz wait_port
    ret
wait_port endp

delay_routine proc
    mov eax, 5000h
delay_loop:
    dec eax
    or eax, eax
    jnz delay_loop
    ret
delay_routine endp

; Data structures for linking
.data
ALIGN 4

ntoskrnl_name db 'ntoskrnl.exe',0
ZwOpenKey_name db 'ZwOpenKey',0
ZwQueryValueKey_name db 'ZwQueryValueKey',0
ZwClose_name db 'ZwClose',0

end start