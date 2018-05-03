; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
;; --UUU-- Complete this (copy from Lab7-8)
cnt EQU 0
fp  RN  11
    PUSH {R4-R8, LR}
    SUB SP, #4
    MOV fp, SP
    MOV R4, #10
    STRB R4, [SP, #cnt]
    LDRB R5, [SP, #cnt]
rloopDec
    SUB R4, #1
    UDIV R6, R0, R5
    MUL R7, R6, R5
    SUB R7, R0, R7
    STRB R7, [SP, #-4]
    SUB SP, #4
    MOV R0, R6
    CMP R4, #0
    BNE rloopDec
fdigitDec
    LDRB R0, [SP]
    LDRB R6,[SP, #4]
    CMP R6, #10
    BEQ printDec
    ADD SP, #4
    CMP R0, #0
    BEQ fdigitDec
    SUB SP, #4
ploopDec
    LDRB R0, [SP]
    CMP R0, #10
    BEQ retDec
printDec
    ADD R0, #0x30
    BL  ST7735_OutChar
    ADD SP, #4
    B   ploopDec
retDec
    ADD SP, #4
    POP {R4-R8, LR}
    BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
;; --UUU-- Complete this (copy from Lab7-8)
    PUSH {R4-R8, LR}
    SUB SP, #4
    MOV R4, #9999
    CMP R0, R4
    BHI overflowFix
    MOV fp, SP
    MOV R4, #10
    STRB R4, [SP, #cnt]
    LDRB R5, [SP, #cnt]
rloopFix
    SUB R4, #1
    UDIV R6, R0, R5
    MUL R7, R6, R5
    SUB R7, R0, R7
    STRB R7, [SP, #-4]
    SUB SP, #4
    MOV R0, R6
    CMP R4, #0
    BNE rloopFix
fdigitFix
    LDRB R0, [SP]
    LDRB R6,[SP, #4*4]
    CMP R6, #10
    BEQ printpoint
    ADD SP, #4
    CMP R0, #0
    BEQ fdigitFix
    SUB SP, #4
ploopFix
    LDRB R0, [SP]
    CMP R0, #10
    BEQ retFix
printFix
    ADD R0, #0x30
    BL  ST7735_OutChar
    ADD SP, #4
    B   ploopFix
printpoint
    ADD R0, #0x30
    BL  ST7735_OutChar
    MOV R0, #0x2E
    BL  ST7735_OutChar
    ADD SP, #4
    B   ploopFix
overflowFix
    MOV R0, #0x2A
    BL  ST7735_OutChar
    MOV R0, #0x2E
    BL  ST7735_OutChar
    MOV R0, #0x2A
    BL  ST7735_OutChar
    MOV R0, #0x2A
    BL  ST7735_OutChar
    MOV R0, #0x2A
    BL  ST7735_OutChar
retFix
    ADD SP, #4
    POP {R4-R8, LR}
    BX   LR
;* * * * * * * * End of LCD_OutFix * * * * * * * *

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
