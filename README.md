LL1Parser
=========

LL1 parser implementation.  
A program to generate an LL(1) parser for a given grammar and to parse input strings. 

=>The input will give the number of productions in the grammar and each line will list one production.  
=>Capital letters signify non-terminals and all other characters signify terminals.  
=>The only exception is ‘e’ which signifies the empty string.  
=>All terminals are one-character long.  
=>Colon (:) demarcates the boundary between the left and right hand sides of a production.  

example input: 

8  
E:TX  
X:+TX  
X:e  
T:FY  
Y:*FY  
Y:e  
F:a  
F:(E)  
<path to file containing string to be parsed> 


example input string:  
a+a*a 


The output prints the sentential form in each step.


example output:  
E  
TX  
FYX  
aYX  
aX  
a+TX  
a+FYX  
a+aYX  
a+a*FYX  
a+a*aYX  
a+a*aX  
a+a*a  
