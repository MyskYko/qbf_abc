import sys
import math
import itertools
import time
import os
from PIL import Image, ImageDraw, ImageFont

def read_setting(setting_file):
    line = "1"

    num_cycle = 0
    num_node = 0
    num_spx = []
    init_assign = []
    fin_assign = []
    onehot_spx_in = False
    systolic = False

    while line:
        if line.rstrip('\n') == "cycle":
            num_cycle = int(setting_file.readline().rstrip('\n'))
        elif line.rstrip('\n') == "node":
            num_node = int(setting_file.readline().rstrip('\n'))
        elif line.rstrip('\n') == "spx":
            for i in range(0, num_node):
                num_spx.append([int(i) for i in setting_file.readline().split()])
        elif line.rstrip('\n') == "initial":
            for i in range(0, num_node):
                init_assign.append(setting_file.readline().split())
        elif line.rstrip('\n') == "final":
            for i in range(0, num_node):
                fin_assign.append(setting_file.readline().split())
        elif line.rstrip('\n') == "onehot_spx_in":
            onehot_spx_in = True
        elif line.rstrip('\n') == "systolic":
            systolic = True
        line = setting_file.readline()

    return num_cycle, num_node, num_spx, init_assign, fin_assign, onehot_spx_in, systolic


def identify_var(init_assign, fin_assign):
    var_id = {}
    id_var = {}
    num_var = 0
    init_assign_id = []
    fin_assign_id = []
    
    for row in init_assign:
        init_assign_id_row = []
        for elem in row:
            if elem not in var_id:
                var_id[elem] = num_var
                id_var[num_var] = elem
                num_var += 1
            init_assign_id_row.append(var_id[elem])
        init_assign_id.append(init_assign_id_row)

    for row in fin_assign:
        fin_assign_id_row = []
        for elem in row:
            fin_assign_id_row.append(var_id[elem])
        fin_assign_id.append(fin_assign_id_row)
        
    return num_var, var_id, id_var, init_assign_id, fin_assign_id


def parse_result(f, fc, result, num_var, num_node, num_cycle, num_spx, id_var):
    num_lit = 0

    num_con = 0
    for row in num_spx:
        for elem in row:
            if elem > 0:
                num_con += 1

    #v in n at c
    num_assign = (num_cycle + 1) * num_node * num_var
    num_lit += num_assign
    #com v in k at c
    num_lit += num_cycle * num_con * num_var

    for k in range(0, num_cycle + 1):
        for j in range(0, num_node):
            for i in range(0, num_var):
                if int(result[i + j * num_var + k * num_node * num_var]) > 0:
                    f.write(str(k) + " " + str(j) + " " + id_var[i] + "\n")

    #com i from j to k at l
    for l in range(0, num_cycle):
        count = 0
        for k in range(0, num_node):
            for j in range(0, num_node):
                if num_spx[j][k] > 0:
                    var_names = []
                    for i in range(0, num_var):
                        if int(result[i + count * num_var + l * num_con * num_var + num_assign]) > 0:
                            var_names.append(id_var[i])
                    if len(var_names) != 0:
                        fc.write(str(l) + " " + str(j) + " " + str(k) + " ")
                        for var in var_names:
                            fc.write(" " + var)
                        fc.write("\n")
                    count += 1
                    
    return


if __name__ == "__main__":
    argv = sys.argv
    if len(argv) < 3:
        print("setting_file result_file")
        exit()

    setting_file_name = argv[1]
    result_file_name = argv[2]
    assign_file_name = result_file_name + ".assign.txt"
    com_file_name = result_file_name + ".com.txt"
    
    try:
        setting_file = open(setting_file_name, "r")
        result_file = open(result_file_name, "r")
        assign_file = open(assign_file_name, "w")
        com_file = open(com_file_name, "w")
    except:
        print("setting_file result_file")
        print("setting file or result file or assignment file([setting file].assign.txt) or communication file([setting file].com.txt) cannot be open")
        exit()

    num_cycle, num_node, num_spx, init_assign, fin_assign, onehot_spx_in, systolic = read_setting(setting_file)
    setting_file.close()
    num_var, var_id, id_var, init_assign_id, fin_assign_id = identify_var(init_assign, fin_assign)

    if result_file.readline().rstrip('\n') != "SAT":
        print("error: UNSAT")
        exit()
    result = result_file.readline().split()
    parse_result(assign_file, com_file, result, num_var, num_node, num_cycle, num_spx, id_var)

    assign_file.close()
    com_file.close()
    result_file.close()

