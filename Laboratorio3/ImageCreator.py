import numpy as np
import PIL
from PIL import Image
import matplotlib

an_image = PIL.Image.open("./Clock/Side76x100.png")

image_sequence = an_image.getdata()
image_array = np.array(image_sequence)
image_list = image_array.tolist()

print(image_list)

def rgb_hex565(red, green, blue):
    rgb565 = ((int(blue / 255 * 31) << 11) | (int(green / 255 * 63) << 5) | (int(red / 255 * 31)))
    return rgb565

def rgbto565(r,g,b):
    rgb = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
    return rgb


lst = list()
for i in range(len(image_list)):
    print(image_list[i])
    if(image_list[i][3] != 0):
        a=rgbto565(image_list[i][0],image_list[i][1],image_list[i][2])
        a=hex(a)
    else:
        a=-1
    lst.append(a)

size= len(lst)
print(size)
np_array=np.asarray(lst)
reshaped_array = np.reshape(np_array,(int(size/10), 10))
# open file in write mode

with open(r'C:\Users\Yarib\Desktop\Embebidos\Lab Embebidos\Clock\File\sales.txt', 'w') as fp:
    for item in reshaped_array:
        for i in item:
            fp.write("%s," % i)
        # write each item on a new line
        fp.write("\n")
    print('Done')
