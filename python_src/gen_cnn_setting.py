import sys
import math
import itertools
import time
import os

argv = sys.argv
if len(argv) < 8:
    print("matrix_row matrix_column window_row window_column cycle pad_row pad_col (option)")
    exit()

mat_row = int(argv[1])
mat_col = int(argv[2])
win_row = int(argv[3])
win_col = int(argv[4])
min_cycle = int(argv[5])
pad_row = int(argv[6])
pad_col = int(argv[7])

file_name = "cnn_m" + str(mat_row) + "x" + str(mat_col) + "_w" + str(win_row) + "x" + str(win_col) + "_c" + str(min_cycle) + "_p" + str(pad_row) + "x" + str(pad_col);
for i in range(8, len(argv)):
    file_name += "_" + argv[i]
file_name += ".txt"
print(file_name)
f = open(file_name, "w")

f.write("cycle\n" + str(min_cycle) + "\n")

num_node = (mat_row + pad_row) * (mat_col + pad_col)
f.write("\nnode\n" + str(num_node) + "\n")

f.write("\nspx\n")
for i in range(0, num_node):
    for j in range(0, num_node):
        if (i % (mat_col + pad_col) != mat_col + pad_col - 1 and j - i == 1) or (i // (mat_col + pad_col) != mat_row + pad_row - 1 and j - i == mat_col + pad_col) or (i % (mat_col + pad_col) != 0 and i - j == 1) or (i // (mat_col + pad_col) != 0 and i - j == mat_col + pad_col):
            f.write("1 ")
        else:
            f.write("0 ")
    f.seek(f.tell() - 1)
    f.write("\n")

f.write("\ninitial\n")
for i in range(0, mat_row + pad_row):
    for j in range(0, mat_col + pad_col):
        if i < pad_row or j < pad_col:
            f.write("\n")
            continue
        f.write("m[" + str(i - pad_row) + "][" + str(j - pad_col) + "]\n")

f.write("\nfinal\n")
for i in range(0, mat_row + pad_row):
    for j in range(0, mat_col + pad_col):
        if i < pad_row or j < pad_col:
            f.write("\n")
            continue
        pos = f.tell()
        for k in range(0, win_row):
            for l in range(0, win_col):
                if i + k < mat_row + pad_row and j + l < mat_col + pad_col:
                    f.write("m[" + str(i + k - pad_row) + "][" + str(j + l - pad_col) + "] ")
                    pos = f.tell() - 1
        f.seek(pos)
        f.write("\n")

for i in range(8, len(argv)):
    f.write("\n" + argv[i] + "\n")

