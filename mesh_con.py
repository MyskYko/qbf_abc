#self.d_list = list()  # [node(str), time(int), place(int), (dir)(str)]
#self.xd_list = {'X':list(), 'D':list()}  # [node(str), time(int), place(int), (dir)(str)]
import sys
from PIL import Image, ImageDraw, ImageFont
import copy


def write_xd(x_list):
    meshx = 4
    d_list = list()
    # add X and X/D->D
    for i in x_list:
        hw_list = checkHW(i[2])
        for j in x_list:
            if j[0] == i[0] and j[1] == i[1] - 1 and j[2] in hw_list:
                for k in x_list:
                    if k[0] == i[0] and k[1] == i[1] - 1 and k[2] == i[2]:
                        break
                # D: D->D
                else:
                    d_list.append([i[0], i[1]-1, j[2], check_dir(j[2]-i[2], meshx)])
    d_list2 = copy.deepcopy(d_list)
    for i in d_list:
        delete_youso = None
        for j in d_list2:
            if i == j:
                break
        else:
            continue
        flag = False
        for j in d_list2:
            if j[1] == i[1] and not delete_youso:
                n_p, n_dir = rev_dir(i[2], i[3])
                if j[2] == n_p and j[3] == n_dir:
                    flag = True
        if flag:
            for j in d_list2:
                if j[0] == i[0] and j[1] == i[1]:
                    for k in checkHW_rev(i[2], i[3]):
                        if j[2] == k[0] and j[3] == k[1]:
                            delete_youso = copy.deepcopy(i)
                            break
        if delete_youso:
            d_list2.remove(delete_youso)
    d_list = copy.deepcopy(d_list2)
    for i in d_list2:
        delete_youso = None
        for j in d_list:
            if i == j:
                break
        else:
            continue
        for j in d_list:
            if j[0] == i[0] and j[1] == i[1]:
                for k in checkHW_rev(i[2], i[3]):
                    if j[2] == k[0] and j[3] == k[1]:
                        delete_youso = copy.deepcopy(i)
                        break
        if delete_youso:
            print(delete_youso)
            d_list.remove(delete_youso)
    return d_list


def rev_dir(p, dir_):
    x = 4
    if dir_ == 'N':
        return p-x, 'S'
    elif dir_ == 'W':
        return p-1, 'E'
    elif dir_ == 'S':
        return p+x, 'N'
    elif dir_ == 'E':
        return p+1, 'W'

def checkHW(p):
    x, y = 4, 4
    hw_list = list()
    if p > x:  # N
        hw_list.append(p-x)
    if p % x - 1:  # W
        hw_list.append(p-1)
    if p % x:  # E
        hw_list.append(p+1)
    if p <= x*(y-1):  # S
        hw_list.append(p+x)
    return hw_list

def checkHW2(p, dir_):
    x, y = 4, 4
    hw_list = list()
    if p > x and dir_ != 'S':  # N
        hw_list.append([p-x, 'S'])
    if p % x - 1 and dir_ != 'E':  # W
        hw_list.append([p-1, 'E'])
    if p % x and dir_ != 'W':  # E
        hw_list.append([p+1, 'W'])
    if p <= x*(y-1) and dir_ != 'N':  # S
        hw_list.append([p+x, 'N'])
    return hw_list

def checkHW_rev(p, dir_):
    x, y = 4, 4
    if dir_ == 'N':
        return checkHW2(p-x, dir_)
    elif dir_ == 'W':
        return checkHW2(p-1, dir_)
    elif dir_ == 'E':
        return checkHW2(p+1, dir_)
    elif dir_ == 'S':
        return checkHW2(p+x, dir_)

def check_dir(num, x):
    # num = previous position - next position
    if num == x:
        return 'N'
    elif num == 1:
        return 'W'
    elif num == -1:
        return 'E'
    elif num == -x:
        return 'S'

class genPNG:
    def __init__(self, x_list, d_list):
        self.xlist = x_list
        self.dlist = d_list
        self.meshx = 4
        self.meshy = 4
        self.time = 4
        self.font_pass = '/Library/Fonts/Arial.ttf'
        #'/usr/share/fonts/dejavu/DejaVuSerif.ttf'
        # each cycle
        for i in range(self.time):
            img = self.printPNG(i)
            img.save(f'./{i+1}.png')

    def printPNG(self, cycle):
        # color
        white = (255, 255, 255)
        black = (0, 0, 0)
        block_color = (29, 214, 128)  # green
        white_color = (255, 255, 255)  # white
        ioblock_color = (255, 170, 0)  # green
        # config
        img = Image.new('RGB', (200*(self.meshx+2), 200*(self.meshy+2)), white)
        draw = ImageDraw.Draw(img)
        font = ImageFont.truetype(self.font_pass, 30, encoding='utf-8')
        # base
        draw.rectangle((0, 0, 200*(self.meshx+2), 200*(self.meshy+2)), outline=black)
        draw.text((200*(self.meshx+2)-45, 200*(self.meshy+2)-45), str(cycle), font=font, fill=black)
        block_position = [(100*(self.meshx+2), 100*(self.meshy+2))]
        # background
        for i in range(self.meshy+2):
            for j in range(self.meshx+2):
                if i == 0 and j == 0:
                    draw.rectangle((50+200*j, 50+200*i, 150+200*j, 150+200*i), fill=white_color)
                elif i == 0 and j == self.meshx+1:
                    draw.rectangle((50+200*j, 50+200*i, 150+200*j, 150+200*i), fill=white_color)
                elif i == self.meshy+1 and j == 0:
                    draw.rectangle((50+200*j, 50+200*i, 150+200*j, 150+200*i), fill=white_color)
                elif i == self.meshy+1 and j == self.meshx+1:
                    draw.rectangle((50+200*j, 50+200*i, 150+200*j, 150+200*i), fill=white_color)
                elif i == 0:
                    draw.rectangle((50+200*j, 50+200*i, 150+200*j, 150+200*i), fill=ioblock_color)
                elif j == 0:
                    draw.rectangle((50+200*j, 50+200*i, 150+200*j, 150+200*i), fill=ioblock_color)
                elif j == self.meshx+1:
                    draw.rectangle((50+200*j, 50+200*i, 150+200*j, 150+200*i), fill=ioblock_color)
                elif i == self.meshy+1:
                    draw.rectangle((50+200*j, 50+200*i, 150+200*j, 150+200*i), fill=ioblock_color)
                else:
                    draw.rectangle((50+200*j, 50+200*i, 150+200*j, 150+200*i), fill=block_color)
                if i > 0 and i < self.meshy + 1 and j > 0 and j < self.meshx + 1:
                    center_block = (100+200*j, 100+200*i)
                    block_position.append(center_block)
        # text
        img = self.write_text(img, cycle, block_position)
        # arrows
        img = self.write_arrows(img, cycle, block_position)
        if cycle < 0:
            img = self.print_num(img, block_position)
        return img

    def write_text(self, img, cycle, block_position):
        black = (0, 0, 0)
        draw = ImageDraw.Draw(img)
        font = ImageFont.truetype(self.font_pass, 20, encoding='utf-8')
        for i in range(1, self.meshx*self.meshy+1):
            # NORMAL X
            text = ''
            newline_count = 0
            for j in self.xlist:
                if (j[1] == cycle or cycle < 0) and j[2] == i:
                    text += 'X' + str(j[0]) + ' '
                    newline_count += 1
                    if not newline_count % 2:
                        text += '\n'
            draw.multiline_text((block_position[i][0]-50, block_position[i][1]-50), text, fill=black, font=font)
        return img

    def write_arrows(self, img, cycle, block_position):
        arrow_color = (250, 107, 64)  # orange
        draw = ImageDraw.Draw(img)
        for i in self.dlist:
            if i[1] == cycle or cycle < 0:
                x = block_position[i[2]][0]
                y = block_position[i[2]][1]
                if i[3] == 'N':
                    draw.line((x-10, y-50, x-10, y-150), fill=arrow_color, width=5)
                    draw.line((x-10, y-150, x-20, y-140), fill=arrow_color, width=5)
                    draw.line((x-10, y-150, x, y-140), fill=arrow_color, width=5)
                elif i[3] == 'E':
                    draw.line((x+50, y-10, x+150, y-10), fill=arrow_color, width=5)
                    draw.line((x+150, y-10, x+140, y-20), fill=arrow_color, width=5)
                    draw.line((x+150, y-10, x+140, y), fill=arrow_color, width=5)
                elif i[3] == 'S':
                    draw.line((x+10, y+50, x+10, y+150), fill=arrow_color, width=5)
                    draw.line((x+10, y+150, x+20, y+140), fill=arrow_color, width=5)
                    draw.line((x+10, y+150, x, y+140), fill=arrow_color, width=5)
                elif i[3] == 'W':
                    draw.line((x-50, y+10, x-150, y+10), fill=arrow_color, width=5)
                    draw.line((x-150, y+10, x-140, y+20), fill=arrow_color, width=5)
                    draw.line((x-150, y+10, x-140, y), fill=arrow_color, width=5)
        return img

    def print_num(self, img, block_position):
        red = (255, 0, 0)
        draw = ImageDraw.Draw(img)
        font = ImageFont.truetype(self.font_pass, 40, encoding='utf-8')
        for i in range(1, self.meshx*self.meshy+1):
            count = 0
            tmp_list = list()
            for j in self.dlist:
                if i == j[2]:
                    if j[3][0] == 'B':
                        continue
                    elif j[0] in tmp_list:
                        continue
                    else:
                        tmp_list.append(j[0])
                        count += 1
            draw.multiline_text((block_position[i][0]+30, block_position[i][1]+30), str(count), fill=red, font=font)
        return img


if __name__ == '__main__':
    x_list__ = list()
    with open(sys.argv[1]) as lines:
        for line in lines:
            cycle_ = int(line.split()[0])
            place = int(line.split()[1]) + 1
            node = int(line.split()[2])
            x_list__.append([node, cycle_, place])
    x_list_ = [x for i, x in enumerate(x_list__) if i == x_list__.index(x)]
    d_list__ = list()
    with open(sys.argv[2]) as lines:
        for line in lines:
            cycle_ = int(line.split()[0])
            node = int(line.split()[1])
            place = int(line.split()[2]) + 1
            dire = (line.split()[3])
            d_list__.append([node, cycle_, place, dire])
    d_list_ = [x for i, x in enumerate(d_list__) if i == d_list__.index(x)]

#    d_list_ = write_xd(x_list_)
    with open('out.txt', 'w') as lines:
        for i in d_list_:
            lines.write(str(i)+'\n')
    png = genPNG(x_list_, d_list_)
