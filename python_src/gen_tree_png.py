import sys
import math
import itertools
import time
import os
from PIL import Image, ImageDraw, ImageFont

white_color = (255, 255, 255)
black_color = (0, 0, 0)

block_color = (29, 214, 128)  # green
arrow_color = (250, 107, 64)  # orange

fontsize = 10
font = ImageFont.truetype('/usr/share/fonts/dejavu/DejaVuSerif.ttf', fontsize, encoding='utf-8')

rectsize = 60
arrowsize = 10

def parse_assign(f, num_node, num_cycle):
    assign = []
    for k in range(0, num_cycle + 1):
        assign_ = []
        for i in range(0, num_node):
            assign__ = []
            assign_.append(assign__)
        assign.append(assign_)
        
    line = f.readline()
    while line:
        data = line.split()
        cycle = int(data[0])
        node = int(data[1])
        var = data[2]
        if var not in assign[cycle][node]:
            assign[cycle][node].append(var)
        line = f.readline()

    return assign

def parse_com(f, num_node, num_cycle, assign):
    com = []
    for k in range(0, num_cycle):
        com_ = []
        for i in range(0, num_node):
            com__ = []            
            for j in range(0, num_node):
                com___ = []
                com__.append(com___)
            com_.append(com__)
        com.append(com_)
        
    line = f.readline()
    while line:
        data = line.split()
        cycle = int(data[0])
        node_from = int(data[1])
        node_to = int(data[2])
        for i in range(3, len(data)):
            if data[i] not in com[cycle][node_from][node_to]:
                if data[i] not in assign[cycle][node_to] and data[i] in assign[cycle + 1][node_to]:
                    com[cycle][node_from][node_to].append(data[i])
        line = f.readline()

    return com

#direction ... U, R, D, L (up, right, down, left)
def write_arrow(draw, x, y, length, direction):
    if direction == 'U':
        draw.line((x, y, x, y - length), fill=arrow_color, width=5)
        draw.line((x - arrowsize, y - length + arrowsize, x, y - length), fill=arrow_color, width=5)
        draw.line((x + arrowsize, y - length + arrowsize, x, y - length), fill=arrow_color, width=5)
    elif direction == 'R':
        draw.line((x, y, x + length, y), fill=arrow_color, width=5)
        draw.line((x + length - arrowsize, y - arrowsize, x + length, y), fill=arrow_color, width=5)
        draw.line((x + length - arrowsize, y + arrowsize, x + length, y), fill=arrow_color, width=5)
    elif direction == 'D':
        draw.line((x, y, x, y + length), fill=arrow_color, width=5)
        draw.line((x - arrowsize, y + length - arrowsize, x, y + length), fill=arrow_color, width=5)
        draw.line((x + arrowsize, y + length - arrowsize, x, y + length), fill=arrow_color, width=5)
    elif direction == 'L':
        draw.line((x, y, x - length, y), fill=arrow_color, width=5)
        draw.line((x - length + arrowsize, y - arrowsize, x - length, y), fill=arrow_color, width=5)
        draw.line((x - length + arrowsize, y + arrowsize, x - length, y), fill=arrow_color, width=5)

def write_cluster_rec(draw, x, y, size, assign, index, num_elem):
    if num_elem == 1:
        draw.rectangle((x, y, x + size, y + size), fill=block_color)
        dx = 0
        dy = 0
        next_dx = 0
        for var in assign[index]:
            tsize = draw.textsize(var)
            if dy + tsize[1] > size:
                dx += next_dx
                dy = 0
                next_dx = 0
            if dx + tsize[0] > size:
                print("oversize or too many vars in node, wrapped as ...")
                draw.text((x + dx, y + dy), "...", font=font, fill=black_color)
                break
            draw.text((x + dx, y + dy), var, font=font, fill=black_color)
            dy += tsize[1]
            if tsize[0] > next_dx:
                next_dx = tsize[0]
    else:
        next_size = size // 4
        next_num_elem = num_elem // 4
        write_cluster_rec(draw, x + next_size, y + next_size, next_size * 2, assign, index + num_elem - 1, 1)

        write_cluster_rec(draw, x, y, next_size, assign, index, next_num_elem)
        write_cluster_rec(draw, x + next_size * 3, y, next_size, assign, index + next_num_elem, next_num_elem)
        write_cluster_rec(draw, x, y + next_size * 3, next_size, assign, index + next_num_elem * 2, next_num_elem)
        write_cluster_rec(draw, x + next_size * 3, y + next_size * 3, next_size, assign, index + next_num_elem * 3, next_num_elem)

def write_cluster_com_rec(draw, x, y, size, com, index, num_elem):
    if num_elem > 1:
        next_size = size // 4
        next_num_elem = num_elem // 4
        write_cluster_com_rec(draw, x, y, next_size, com, index, next_num_elem)
        write_cluster_com_rec(draw, x + next_size * 3, y, next_size, com, index + next_num_elem, next_num_elem)
        write_cluster_com_rec(draw, x, y + next_size * 3, next_size, com, index + next_num_elem * 2, next_num_elem)
        write_cluster_com_rec(draw, x + next_size * 3, y + next_size * 3, next_size, com, index + next_num_elem * 3, next_num_elem)
        
        node_a = index + num_elem - 1
        a_x = x + next_size * 3 // 2
        a_y = y + next_size * 3 // 2
        node_b = index + next_num_elem - 1
        b_x = x + next_size // 2
        b_y = y + next_size // 2
        if len(com[node_a][node_b]) > 0:
            write_arrow(draw, b_x, a_y, next_size // 2, 'U')
            draw.line((a_x - next_size // 2, a_y, b_x, a_y), fill=arrow_color, width=5)
            dy = 0
            for var in com[node_a][node_b]:
                tsize = draw.textsize(var)
                if dy + tsize[1] > next_size:
                    print("oversize or too many vars in node, wrapped as ...")
                    draw.text((b_x - next_size // 2, b_y + next_size // 2 + dy), "...", font=font, fill=black_color)
                    break
                draw.text((b_x - next_size // 2, b_y + next_size // 2 + dy), var, font=font, fill=black_color)
                dy += tsize[1]
        if len(com[node_b][node_a]) > 0:
            write_arrow(draw, a_x, b_y, next_size // 2, 'D')
            draw.line((b_x + next_size // 2, b_y, a_x, b_y), fill=arrow_color, width=5)
            dy = 0
            for var in com[node_b][node_a]:
                tsize = draw.textsize(var)
                if dy + tsize[1] > next_size:
                    print("oversize or too many vars in node, wrapped as ...")
                    draw.text((b_x + next_size // 2, b_y - next_size // 2 + dy), "...", font=font, fill=black_color)
                    break
                draw.text((b_x + next_size // 2, b_y - next_size // 2 + dy), var, font=font, fill=black_color)
                dy += tsize[1]
                
        node_a = index + num_elem - 1
        a_x = x + next_size * 5 // 2
        a_y = y + next_size * 3 // 2
        node_b = index + next_num_elem * 2 - 1
        b_x = x + next_size * 7 // 2
        b_y = y + next_size // 2
        if len(com[node_a][node_b]) > 0:
            write_arrow(draw, b_x, a_y, next_size // 2, 'U')
            draw.line((a_x + next_size // 2, a_y, b_x, a_y), fill=arrow_color, width=5)
            dy = 0
            for var in com[node_a][node_b]:
                tsize = draw.textsize(var)
                if dy + tsize[1] > next_size:
                    print("oversize or too many vars in node, wrapped as ...")
                    draw.text((b_x - next_size // 2, b_y + next_size // 2 + dy), "...", font=font, fill=black_color)
                    break
                draw.text((b_x - next_size // 2, b_y + next_size // 2 + dy), var, font=font, fill=black_color)
                dy += tsize[1]
        if len(com[node_b][node_a]) > 0:
            write_arrow(draw, a_x, b_y, next_size // 2, 'D')
            draw.line((b_x - next_size // 2, b_y, a_x, b_y), fill=arrow_color, width=5)
            dy = 0
            for var in com[node_b][node_a]:
                tsize = draw.textsize(var)
                if dy + tsize[1] > next_size:
                    print("oversize or too many vars in node, wrapped as ...")
                    draw.text((a_x - next_size // 2, b_y - next_size // 2 + dy), "...", font=font, fill=black_color)
                    break
                draw.text((a_x - next_size // 2, b_y - next_size // 2 + dy), var, font=font, fill=black_color)
                dy += tsize[1]
                
        node_a = index + num_elem - 1
        a_x = x + next_size * 3 // 2
        a_y = y + next_size * 5 // 2
        node_b = index + next_num_elem * 3 - 1
        b_x = x + next_size // 2
        b_y = y + next_size * 7 // 2
        if len(com[node_a][node_b]) > 0:
            write_arrow(draw, b_x, a_y, next_size // 2, 'D')
            draw.line((a_x - next_size // 2, a_y, b_x, a_y), fill=arrow_color, width=5)
            dy = 0
            for var in com[node_a][node_b]:
                tsize = draw.textsize(var)
                if dy + tsize[1] > next_size:
                    print("oversize or too many vars in node, wrapped as ...")
                    draw.text((b_x - next_size // 2, a_y - next_size // 2 + dy), "...", font=font, fill=black_color)
                    break
                draw.text((b_x - next_size // 2, a_y - next_size // 2 + dy), var, font=font, fill=black_color)
                dy += tsize[1]
        if len(com[node_b][node_a]) > 0:
            write_arrow(draw, a_x, b_y, next_size // 2, 'U')
            draw.line((b_x + next_size // 2, b_y, a_x, b_y), fill=arrow_color, width=5)
            dy = 0
            for var in com[node_b][node_a]:
                tsize = draw.textsize(var)
                if dy + tsize[1] > next_size:
                    print("oversize or too many vars in node, wrapped as ...")
                    draw.text((a_x - next_size // 2, b_y - next_size // 2 + dy), "...", font=font, fill=black_color)
                    break
                draw.text((a_x - next_size // 2, b_y - next_size // 2 + dy), var, font=font, fill=black_color)
                dy += tsize[1]
            
        node_a = index + num_elem - 1
        a_x = x + next_size * 5 // 2
        a_y = y + next_size * 5 // 2
        node_b = index + next_num_elem * 4 - 1
        b_x = x + next_size * 7 // 2
        b_y = y + next_size * 7 // 2
        if len(com[node_a][node_b]) > 0:
            write_arrow(draw, b_x, a_y, next_size // 2, 'D')
            draw.line((a_x + next_size // 2, a_y, b_x, a_y), fill=arrow_color, width=5)
            dy = 0
            for var in com[node_a][node_b]:
                tsize = draw.textsize(var)
                if dy + tsize[1] > next_size:
                    print("oversize or too many vars in node, wrapped as ...")
                    draw.text((b_x - next_size // 2, a_y - next_size // 2 + dy), "...", font=font, fill=black_color)
                    break
                draw.text((b_x - next_size // 2, a_y - next_size // 2 + dy), var, font=font, fill=black_color)
                dy += tsize[1]
        if len(com[node_b][node_a]) > 0:
            write_arrow(draw, a_x, b_y, next_size // 2, 'U')
            draw.line((b_x - next_size // 2, b_y, a_x, b_y), fill=arrow_color, width=5)
            dy = 0
            for var in com[node_b][node_a]:
                tsize = draw.textsize(var)
                if dy + tsize[1] > next_size:
                    print("oversize or too many vars in node, wrapped as ...")
                    draw.text((a_x - next_size // 2, b_y - next_size // 2 + dy), "...", font=font, fill=black_color)
                    break
                draw.text((a_x - next_size // 2, b_y - next_size // 2 + dy), var, font=font, fill=black_color)
                dy += tsize[1]
                
                
def gen_img(num_node, num_cycle, assign, com):
    # config
    margin = rectsize // 2
    num_hierarchy = 0
    tmp_num_node = num_node
    while True:
        tmp_num_node = tmp_num_node // 4
        if tmp_num_node == 0:
            break
        num_hierarchy += 1

    content_size = rectsize * (4 ** num_hierarchy)
    img = [Image.new('RGB', (content_size + margin * 2, content_size + margin * 2), white_color) for i in range(0, num_cycle + 1)]
    draw = [ImageDraw.Draw(img[i]) for i in range(0, num_cycle + 1)]

    for k in range(0, num_cycle + 1):
        draw[k].text((0, 0), "cycle:" + str(k), font=font, fill=black_color)
        write_cluster_rec(draw[k], margin, margin, content_size, assign[k], 0, num_node)

    if com:
        for k in range(0, num_cycle):
            write_cluster_com_rec(draw[k], margin, margin, content_size, com[k], 0, num_node)            
#            for i_from in range(0, num_node_row):
#                for j_from in range(0, num_node_col):
#                    node_from = j_from + i_from * num_node_col
#                    x_from = margin + (rectsize + padding) * j_from
#                    y_from = margin + (rectsize + padding) * i_from
#                    direction = 'R'
#                    node_to = node_from + 1
#                    if node_to >= num_node:
#                        node_to = 0
#                    if len(com[k][node_from][node_to]) > 0:
#                        write_arrow(draw[k], x_from + rectsize, y_from + rectsize // 2 - arrowsize, padding, arrowsize, direction)
#                        dy = -arrowsize * 2
#                        for var in com[k][node_from][node_to]:
#                            size = draw[k].textsize(var)
#                            dy += -size[1]
#                            if size[0] > padding or -dy > rectsize // 2:
#                                print("oversize or too many vars in communication, wrapped as ...")
#                                draw[k].text((x_from + rectsize, y_from + rectsize // 2 + dy), "...", font=font, fill=black_color)
#                                break
#                            draw[k].text((x_from + rectsize, y_from + rectsize // 2 + dy), var, font=font, fill=black_color)
#                    direction = 'L'
#                    node_to = node_from - 1
#                    if node_to < 0:
#                        node_to = num_node - 1
#                    if len(com[k][node_from][node_to]) > 0:
#                        write_arrow(draw[k], x_from, y_from + rectsize // 2 + arrowsize, padding, arrowsize, direction)
#                        dy = arrowsize * 2
#                        for var in com[k][node_from][node_to]:
#                            size = draw[k].textsize(var)
#                            if size[0] > padding or dy > rectsize // 2:
#                                print("oversize or too many vars in communication, wrapped as ...")
#                                draw[k].text((x_from - padding, y_from + rectsize // 2 + dy), "...", font=font, fill=black_color)
#                                break
#                            draw[k].text((x_from - padding, y_from + rectsize // 2 + dy), var, font=font, fill=black_color)
#                            dy += size[1]

    return img

if __name__ == "__main__":
    argv = sys.argv
    if len(argv) < 4:
        print("num_node num_cycle assignment_file (communication_file)")
        exit()

    num_node = int(argv[1])
    num_cycle = int(argv[2])
    assign_file_name = argv[3]
    com_file_name = False
    if len(argv) == 5:
        com_file_name = argv[4]

    try:
        assign_file = open(assign_file_name, "r")
        com_file = False
        if com_file_name:
            com_file = open(com_file_name, "r")
    except:
        print("num_node num_cycle assignment_file (communication_file)")
        print("assignment file (or commnication file) cannot be open")
        exit()
        
    assign = parse_assign(assign_file, num_node, num_cycle)
    com = False
    if com_file:
        com = parse_com(com_file, num_node, num_cycle, assign)

    img = gen_img(num_node, num_cycle, assign, com)
    
    try:
        img[0].save(assign_file_name + ".gif", save_all=True, append_images=img[1:], optimize=False, duration=1000, loop=0)
        for i in range(0, num_cycle + 1):
            img[i].save(assign_file_name + str(i) + ".png")
    except:
        print("image file ([assignment file name].png) cannot be open")
        exit()
        
exit()