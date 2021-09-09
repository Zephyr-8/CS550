
THIS IS A SIMPLE SHELL. 

FEATURE: pipe, redirection, illegal command detection.

the detailed description of this task is in the HW.pdf.



two bugs for this shell:

1.for redirection, if the input/output file doesn't exist. the program will report an error instead of creating a file and execute the command.（but if the file already exists,it works successfully)


2.when typing any command involving pipeline, the program will jump out of the while(1) loop.(no matter whether the command line is executed successfully)

