set pagination off
target remote localhost:1234

add-inferior
inferior 2
attach 2
info thread

file ./r5_test.elf