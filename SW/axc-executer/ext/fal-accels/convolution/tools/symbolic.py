#!/usr/bin/env python3
from sympy import *

# Declare symbols
var('w00 w01 w02 w03 w10 w11 w12 w13 w20 w21 w22 w23 w30 w31 w32 w33')
var('h00 h01 h02 h03 h10 h11 h12 h13 h20 h21 h22 h23 h30 h31 h32 h33')
var('k00 k01 k02 k10 k11 k12 k20 k21 k22')

# Declare matrices
A = Matrix([[1, 0],[1, 1],[1, -1],[0, -1]])
B = Matrix([[1, 0, 0, 0],[0, 1, -1, 1],[-1, 1, 1, 0],[0, 0, 0, -1]])
G = Matrix([[1, 0, 0],[0.5, 0.5, 0.5],[0.5, -0.5, 0.5], [0, 0, 1]])

# Declare symbolic matrices
W = Matrix([[w00,w01,w02,w03],[w10,w11,w12,w13],[w20, w21,w22,w23],[w30,w31,w32,w33]])
K = Matrix([[k00,k01,k02],[k10,k11,k12],[k20,k21,k22]])
H = Matrix([[h00,h01,h02,h03],[h10,h11,h12,h13],[h20,h21,h22,h23],[h30,h31,h32,h33]])

# Compute
BtZB = B.T * W * B
GfGt = G * K * G.T
Hadamard = matrix_multiply_elementwise(GfGt, BtZB)
O = A.T * Hadamard * A
OH = A.T * H * A

print("--Space -> Winograd--")
print("\n")
S_BtZB = simplify(BtZB)
S_GfGt = simplify(GfGt)
S_hadamard = simplify(Hadamard)
S_OH = simplify(OH)
print("Input transform: ")
print(S_BtZB)
print("\n")
print("Kernel transform: ")
print(S_GfGt)
print("\n")
print("Hadamard: ")
print(S_hadamard)
print("OH: ")
print(S_OH)
print("\n\n")

print("--Simplificado--")
S0 = simplify(O)
print(S0[0,0])
print(S0[0,1])
print(S0[1,0])
print(S0[1,1])
