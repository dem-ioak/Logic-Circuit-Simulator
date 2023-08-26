
# Digital Logic Circuit Simulator
A program that reads the description of a digital logic circuit and prints out the corresponding truth table.

Using multiple Linked Lists and a Queue, this program parses the unsorted input logic circuit description, sorts it according to when each variable is made available, and generates every possible combination of variables to output the corresponding truth table.

Supports AND, NAND, OR, NOR, XOR, MULTIPLEXER, and DECODER Gates

Makes use of a simple string hashing algorithm to allow for constant time comparison of strings, optimizing code to support upwards of 2^20 combinations for the truth table.

Example Input / Output:
```python
INPUT 6 IN1 IN2 IN3 IN4 IN5 IN6
OUTPUT 1 OUT1
DECODER 2 IN1 IN2 temp1 temp2 temp3 temp4
DECODER 2 IN2 IN3 temp5 temp6 temp7 temp8
XOR temp23 temp20 OUT1
DECODER 2 IN3 IN4 temp9 temp10 temp11 temp12
DECODER 2 IN4 IN5 temp13 temp14 temp15 temp16
OR temp4 temp8 temp21
OR temp21 temp12 temp22
DECODER 2 IN5 IN6 temp17 temp18 temp19 temp20
OR temp22 temp16 temp23
```
```
0 0 0 0 0 0 | 0
0 0 0 0 0 1 | 0
0 0 0 0 1 0 | 0
0 0 0 0 1 1 | 1
0 0 0 1 0 0 | 0
0 0 0 1 0 1 | 0
0 0 0 1 1 0 | 1
0 0 0 1 1 1 | 0
0 0 1 0 0 0 | 0
0 0 1 0 0 1 | 0
0 0 1 0 1 0 | 0
0 0 1 0 1 1 | 1
0 0 1 1 0 0 | 1
0 0 1 1 0 1 | 1
0 0 1 1 1 0 | 1
0 0 1 1 1 1 | 0
0 1 0 0 0 0 | 0
0 1 0 0 0 1 | 0
0 1 0 0 1 0 | 0
0 1 0 0 1 1 | 1
0 1 0 1 0 0 | 0
0 1 0 1 0 1 | 0
0 1 0 1 1 0 | 1
0 1 0 1 1 1 | 0
0 1 1 0 0 0 | 1
0 1 1 0 0 1 | 1
0 1 1 0 1 0 | 1
0 1 1 0 1 1 | 0
0 1 1 1 0 0 | 1
0 1 1 1 0 1 | 1
0 1 1 1 1 0 | 1
0 1 1 1 1 1 | 0
1 0 0 0 0 0 | 0
1 0 0 0 0 1 | 0
1 0 0 0 1 0 | 0
1 0 0 0 1 1 | 1
1 0 0 1 0 0 | 0
1 0 0 1 0 1 | 0
1 0 0 1 1 0 | 1
1 0 0 1 1 1 | 0
1 0 1 0 0 0 | 0
1 0 1 0 0 1 | 0
1 0 1 0 1 0 | 0
1 0 1 0 1 1 | 1
1 0 1 1 0 0 | 1
1 0 1 1 0 1 | 1
1 0 1 1 1 0 | 1
1 0 1 1 1 1 | 0
1 1 0 0 0 0 | 1
1 1 0 0 0 1 | 1
1 1 0 0 1 0 | 1
1 1 0 0 1 1 | 0
1 1 0 1 0 0 | 1
1 1 0 1 0 1 | 1
1 1 0 1 1 0 | 1
1 1 0 1 1 1 | 0
1 1 1 0 0 0 | 1
1 1 1 0 0 1 | 1
1 1 1 0 1 0 | 1
1 1 1 0 1 1 | 0
1 1 1 1 0 0 | 1
1 1 1 1 0 1 | 1
1 1 1 1 1 0 | 1
1 1 1 1 1 1 | 0
```
