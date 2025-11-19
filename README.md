# SRAM Dumper

utility program that extracts the data from your System2x6 SRAM memory into a file, wich can be located on either USB, MMCE SD Card or Dongle. the file is created on the same directory where you put this program.

dumps will be called `sramX.bin`, where `X` can be a number from 0 to 16, once 16 dumps exist on the same location, the program will just overwrite `sram16.bin`