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


def cnf_generate(f, num_var, num_node, num_cycle, init_assign, num_spx, fin_assign, options):
    num_lit = 0
    num_cla = 0

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

    #initial assignment
    for j in range(0, num_node):
        if len(init_assign) <= j:
            break
        for i in range(0, num_var):
            if i not in init_assign[j]:
                f.write("-")
            f.write(str(i + j * num_var + 1))
            f.write(" 0\n")
    num_cla += num_node * num_var

    #communication data ready
    #com i from j to k at l
    for l in range(0, num_cycle):
        count = 0
        for k in range(0, num_node):
            for j in range(0, num_node):
                if num_spx[j][k] > 0:
                    for i in range(0, num_var):
                        f.write("-" + str(i + count * num_var + l * num_con * num_var + num_assign + 1) + " ")
                        f.write(str(i + j * num_var + l * num_node * num_var + 1) + " 0\n")
                    count += 1
    num_cla += num_cycle * num_con * num_var

    #communication
    #com i from j to k at l
    for l in range(0, num_cycle):
        count = 0
        for k in range(0, num_node):
            count_ = 0
            for i in range(0, num_var):
                count_ = 0
                f.write(str(i + k * num_var + l * num_node * num_var + 1) + " ")
                f.write("-" + str(i + k * num_var + (l + 1) * num_node * num_var + 1) + " ")
                for j in range(0, num_node):
                    if num_spx[j][k] > 0:
                        f.write(str(i + (count + count_) * num_var + l * num_con * num_var + num_assign + 1) + " ")
                        count_ += 1
                f.write("0\n")
            count += count_
    num_cla += num_cycle * num_node * num_var

    #final assignment
    for j in range(0, num_node):
        for i in range(0, num_var):
            if i in fin_assign[j]:
                if "imp_reg" not in options:
                    f.write(str(i + j * num_var + num_cycle * num_node * num_var + 1) + " 0\n")
                else:
                    for k in range(0, num_cycle + 1):
                        f.write(str(i + j * num_var + k * num_node * num_var + 1) + " ")
                    f.write("0\n")
            else:
                if "imp_reg" not in options:                
                    f.write("-" + str(i + j * num_var + num_cycle * num_node * num_var + 1) + " 0\n")
                
    for j in range(0, num_node):
        if "imp_reg" not in options:        
            num_cla += num_var
        else:
            num_cla += len(fin_assign[j])

    #sinplex connection
    #com i from j to k at l
    for l in range(0, num_cycle):
        count = 0
        for k in range(0, num_node):
            for j in range(0, num_node):
                if num_spx[j][k] > 0:
                    data = [i for i in range(0, num_var)]
                    for pattern in itertools.combinations(data, num_spx[j][k] + 1):
                        for i in pattern:
                            f.write("-" + str(i + count * num_var + l * num_con * num_var + num_assign + 1) + " ")
                        f.write("0\n")
                        num_cla += 1
                    count += 1
                    
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
    sat_file_name = setting_file_name + ".sat.cnf"
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
    print("num_spx: " + text_omit(num_spx))
    print("initial assignment: " + text_omit(init_assign))
    print("final assignment: "+ text_omit(fin_assign))
    for option in options:
        print(option)
    
    setting_file.close()

    num_var, var_id, id_var, init_assign_id, fin_assign_id = identify_var(init_assign, fin_assign)
    print("num_var: ", num_var)
#    print("var_id: ", var_id)

    time_start = time.time()
    num_lit, num_cla = cnf_generate(sat_file, num_var, num_node, num_cycle, init_assign_id, num_spx, fin_assign_id, options)
    time_end = time.time()

    sat_file.close()
    
    print("SAN problem generation took " + str(round(time_end-time_start,5)) + "s in CNF file")
    print("There are " + str(num_lit) + " literals, " + str(num_cla) + " clauses")
    print("The CNF file is " + str(os.path.getsize(sat_file_name)/1024) + "KB")
