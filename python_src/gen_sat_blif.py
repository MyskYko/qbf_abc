import sys
import math
import itertools
import time
import os

def read_setting(setting_file):
    line = "1"

    num_cycle = 0
    num_node = 0
    num_spx = []
    init_assign = []
    fin_assign = []
    options = []

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
        elif line.rstrip('\n') == "implicit_reg":
            options.append("imp_reg")
        elif line.rstrip('\n') == "onehot_spx_in":
            options.append("onehot_spx_in")
        elif line.rstrip('\n') == "onehot_spx_out":
            options.append("onehot_spx_out")
        elif line.rstrip('\n') == "systolic":
            options.append("imp_reg")
            options.append("systolic")
        line = setting_file.readline()

    return num_cycle, num_node, num_spx, init_assign, fin_assign, options


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


def blif_generate(f, num_var, num_node, num_cycle, init_assign, num_spx, fin_assign, options):
    f.write(".model sat\n")
    
    num_con = 0
    for row in num_spx:
        for elem in row:
            if elem > 0:
                num_con += 1

    num_lit = 0
    #v in n at c
    num_assign = (num_cycle + 1) * num_node * num_var
    num_lit += num_assign
    #com v in k at c
    num_lit += num_cycle * num_con * num_var

    f.write(".inputs ")
    for k in range(0, num_cycle + 1):
        for j in range(0, num_node):
            for i in range(0, num_var):
                f.write("x" + str(i + j * num_var + k * num_node * num_var + 1) + " ")
    f.write("\\\n")
    for l in range(0, num_cycle):
        count = 0
        for k in range(0, num_node):
            for j in range(0, num_node):
                if num_spx[j][k] > 0:
                    for i in range(0, num_var):
                        f.write("c" + str(i + count * num_var + l * num_con * num_var + num_assign + 1) + " ")
                    count += 1
    f.write("\n")
    
    f.write(".outputs out\n")
    
    f.write("\n")

    #initial assignment
    f.write("#initial assignment\n")
    ncount = 0
    for j in range(0, num_node):
        f.write(".names ")
        for i in range(0, num_var):
            f.write("x" + str(i + j * num_var + 1) + " ")
        f.write("init" + str(ncount) + "\n")
        ncount += 1
        for i in range(0, num_var):
            if i in init_assign[j]:
                f.write("1")
            else:
                f.write("0")
        f.write(" 1\n")
    f.write(".names ")
    for i in range(0, ncount):
        f.write("init" + str(i) + " ")
    f.write("init\n")
    for i in range(0, ncount):
        f.write("1")
    f.write(" 1\n")

    f.write("\n")

    #communication data ready
    #com i from j to k at l
    f.write("#communication data ready\n")
    ncount = 0
    for l in range(0, num_cycle):
        count = 0
        for k in range(0, num_node):
            for j in range(0, num_node):
                if num_spx[j][k] > 0:
                    for i in range(0, num_var):
                        f.write(".names ")
                        f.write("c" + str(i + count * num_var + l * num_con * num_var + num_assign + 1) + " ")
                        f.write("x" + str(i + j * num_var + l * num_node * num_var + 1) + " ")
                        f.write("ready" + str(ncount) + "\n")
                        ncount += 1
                        f.write("10 0\n")
                    count += 1
    f.write(".names ")
    for i in range(0, ncount):
        f.write("ready" + str(i) + " ")
    f.write("ready\n")
    for i in range(0, ncount):
        f.write("1")
    f.write(" 1\n")
    f.write("\n")

    #communication
    #com i from j to k at l
    f.write("#communication\n")
    ncount = 0
    for l in range(0, num_cycle):
        count = 0
        for k in range(0, num_node):
            count_ = 0
            for i in range(0, num_var):
                count_ = 0
                f.write(".names ")
                f.write("x" + str(i + k * num_var + l * num_node * num_var + 1) + " ")
                f.write("x" + str(i + k * num_var + (l + 1) * num_node * num_var + 1) + " ")
                for j in range(0, num_node):
                    if num_spx[j][k] > 0:
                        f.write("c" + str(i + (count + count_) * num_var + l * num_con * num_var + num_assign + 1) + " ")
                        count_ += 1
                f.write("com" + str(ncount) + "\n")
                ncount += 1
                f.write("01")
                for j in range(0, count_):
                    f.write("0")
                f.write(" 0\n")
            count += count_
    f.write(".names ")
    for i in range(0, ncount):
        f.write("com" + str(i) + " ")
    f.write("com\n")
    for i in range(0, ncount):
        f.write("1")
    f.write(" 1\n")
    f.write("\n")

    #final assignment
    f.write("#final assignment\n")
    ncount = 0
    for j in range(0, num_node):
        if "imp_reg" not in options or True:        
            f.write(".names ")
            for i in range(0, num_var):
                f.write("x" + str(i + j * num_var + num_cycle * num_node * num_var + 1) + " ")
            f.write("fin" + str(ncount) + "\n")
            ncount += 1
            for i in range(0, num_var):
                if i in fin_assign[j]:
                    f.write("1")
                else:
                    f.write("0")
            f.write(" 1\n")
        else:
            for i in range(0, num_var):
                if i in fin_assign[j]:
                    f.write(".names ")                    
                    for k in range(0, num_cycle + 1):
                        f.write("x" + str(i + j * num_var + k * num_node * num_var + 1) + " ")
                    f.write("fin" + str(ncount) + "\n")
                    ncount += 1
                    for k in range(0, num_cycle + 1):
                        f.write("0")
                    f.write(" 0\n")
    f.write(".names ")
    for i in range(0, ncount):
        f.write("fin" + str(i) + " ")
    f.write("fin\n")
    for i in range(0, ncount):
        f.write("1")
    f.write(" 1\n")
    f.write("\n")
    
    #sinplex connection
    #com i from j to k at l
    f.write("#simplex connection\n")
    lt = []
    ncount = 0
    for l in range(0, num_cycle):
        count = 0
        for k in range(0, num_node):
            for j in range(0, num_node):
                if num_spx[j][k] > 0:
                    if (num_spx[j][k] + 1, num_var) not in lt:
                        lt.append((num_spx[j][k] + 1, num_var))
                    f.write(".subckt lt" + str(num_spx[j][k] + 1) + "_" + str(num_var) + " ")
                    for i in range(0, num_var):
                        f.write("in" + str(i) + "=c" + str(i + count * num_var + l * num_con * num_var + num_assign + 1) + " ")
                    f.write("out=con" + str(ncount) + "\n")
                    ncount += 1
                    count += 1
    f.write(".names ")
    for i in range(0, ncount):
        f.write("con" + str(i) + " ")
    f.write("con\n")
    for i in range(0, ncount):
        f.write("1")
    f.write(" 1\n")
    f.write("\n")

    f.write(".names init ready com fin con out\n11111 1\n")
    
    f.write(".end\n")

    for n in lt:
        f.write(".model lt" + str(n[0]) + "_" + str(n[1]) + "\n")
        f.write(".inputs")
        for i in range(0, n[1]):
            f.write(" in" + str(i))
        f.write("\n.outputs out\n")
        f.write(".names")
        for i in range(0, n[1]):
            f.write(" in" + str(i))
        f.write(" out\n")
        data = [i for i in range(0, n[1])]
        for pattern in itertools.combinations(data, n[0]):
            for i in range(0, n[1]):
                if i in pattern:
                    f.write("1")
                else:
                    f.write("-")
            f.write(" 0\n")
        f.write(".end")
    
    return num_lit
                    
    #onehot spx in
    #com i from j to k at l
    if "onehot_spx_in" in options:
        for l in range(0, num_cycle):
            count = 0
            for k in range(0, num_node):
                count_ = 0
                for j in range(0, num_node):
                    if num_spx[j][k]:
                        count_ += 1
                if count_ == 0:
                    continue
                data = [i for i in range(0, count_ * num_var)]
                for pattern in itertools.combinations(data, 2):
                    for i in pattern:
                        f.write("-" + str(i + count * num_var + l * num_con * num_var + num_assign + 1) + " ")
                    f.write("0\n")
                    num_cla += 1
                count += count_

    #onehot spx out
    #com i from j to k at l
    if "onehot_spx_out" in options:
        onehot_groups = [[] for i in range(0, num_node * num_cycle)]
        for l in range(0, num_cycle):
            count = 0
            for k in range(0, num_node):
                for j in range(0, num_node):
                    if num_spx[j][k]:
                        for i in range(0, num_var):
                            onehot_groups[j + l * num_node].append(i + count * num_var + l * num_con * num_var + num_assign + 1)
                        count += 1
        for onehot_group in onehot_groups:
            data = [i for i in range(0, len(onehot_group))]
            for pattern in itertools.combinations(data, 2):
                for i in pattern:
                    f.write("-" + str(onehot_group[i]) + " ")
                f.write("0\n")
                num_cla += 1

    #systolic
    if "systolic" in options:
        #onehot var in node per cycle
        for k in range(0, num_cycle + 1):
            for j in range(0, num_node):
                data = [i for i in range(0, num_var)]            
                for pattern in itertools.combinations(data, 2):            
                    for i in pattern:
                        f.write("-" + str(i + j * num_var + k * num_node * num_var + 1) + " ")
                    f.write("0\n")
                    num_cla += 1
                
    f.write("p cnf " + str(num_lit) + " " + str(num_cla) + "\n")
    return num_lit, num_cla

def text_omit(array):
    text = str(array)
    if len(text) > 20:
        return text[:20] + "..."
    return text

if __name__ == "__main__":
    argv = sys.argv
    if len(argv) < 2:
        print("setting file should be given as second command line parameter")
        exit()

    setting_file_name = argv[1]
    sat_file_name = setting_file_name + ".sat.blif"
    try:
        setting_file = open(setting_file_name, "r")
        sat_file = open(sat_file_name, "w")
    except:
        print("setting file should be given as second command line parameter")
        print("setting file or tmp_file(__[setting file].sat.blif) cannot be open")
        exit()

    num_cycle, num_node, num_spx, init_assign, fin_assign, options = read_setting(setting_file)
    print("setting file:", argv[1])
    print("num_cycle: ", num_cycle)
    print("num_node: ", num_node)
    print("num_spx: ", text_omit(num_spx))
    print("initial assignment: ", text_omit(init_assign))
    print("final assignment: ", text_omit(fin_assign))
    for option in options:
        print("options are now ignored")
#        print(option)
    
    setting_file.close()

    num_var, var_id, id_var, init_assign_id, fin_assign_id = identify_var(init_assign, fin_assign)
    print("num_var: ", num_var)
    print("var_id: ", var_id)

    time_start = time.time()
    num_in = blif_generate(sat_file, num_var, num_node, num_cycle, init_assign_id, num_spx, fin_assign_id, options)
    time_end = time.time()

    sat_file.close()
    
    print("SAT problem generation took " + str(round(time_end-time_start,5)) + "s in BLIF file")
    print("There are " + str(num_in) + " input")
    print("The BLIF file is " + str(os.path.getsize(sat_file_name)/1024) + "KB")
