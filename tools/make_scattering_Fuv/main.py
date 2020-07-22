import scipy.integrate as integrate
import scipy.special as special
import scipy.misc
import math
from functools import partial
import numpy as np
from PIL import Image
import cv2
import sys

FileName = "Fuv.hdr" if len(sys.argv) <= 1 else sys.argv[1]
Resolution = 256 if len(sys.argv) <= 2 else int(sys.argv[2])
URange = 20 if len(sys.argv) <= 3 else int(sys.argv[3])

def integrand(u, xi):
    return math.exp(-u * math.tan(xi))

image = np.zeros((Resolution, Resolution), np.float32)

gamma_inc = math.pi / 2 / Resolution

u_index = 0
v_index = 0


for u in np.arange(0, URange, URange / Resolution):
    integrand_partial = partial(integrand, u)
    print (str(u_index) + '/' + str(Resolution))

    for v in np.arange(0.0, math.pi / 2, gamma_inc):
        result = integrate.quad(integrand_partial, 0, v)[0]
        image[v_index, u_index] = result
        v_index += 1

    v_index = 0
    u_index += 1

cv2.imwrite(FileName, image)

# for u in range(0, Resolution):
#     for v in range(0, Resolution):
#         print (image[u, v])
