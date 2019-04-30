import re
import os
import subprocess

from os import listdir
from os.path import isfile, join

import csv

def mse(img1, img2):
  result = subprocess.run(["./imagemse", img1, img2], stdout=subprocess.PIPE)
  mseStr = result.stdout.decode('utf-8')
  reRes  = re.search(r'^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$', mseStr)
  if reRes != None:
    return float(reRes.group())
  else:
    return 0.0


def cut(img1, img2, bminx, bminy, bsizex, bsizey):
  subprocess.run(["./imagecut", img1, img2, str(bminx), str(bminy), str(bsizex), str(bsizey)], stdout=subprocess.PIPE)


boxMinX_input = 295
boxMinY_input = 1024 - 935

imgHydra = ["" for x in range(0,4)]
imgHydra[0] = "/media/frol/886234F06234E49A/yandexdisk/2019/orb_2019_images/L1.2/Hydra/PT/ldr_pt_4min.png"
imgHydra[1] = "/media/frol/886234F06234E49A/yandexdisk/2019/orb_2019_images/L1.2/Hydra/PT_QMC/ldr_pt_4min.png"
imgHydra[2] = "/media/frol/886234F06234E49A/yandexdisk/2019/orb_2019_images/L1.2/Hydra/IBPT/ldr_ibpt_4min.png"
imgHydra[3] = "/media/frol/886234F06234E49A/yandexdisk/2019/orb_2019_images/L1.2/Hydra/MMLT/ldr_mmlt_4min.png"

os.rmdir("outcuts")
os.mkdir("outcuts")

cut(imgHydra[0], "outcuts/01_cut.png", boxMinX_input, boxMinY_input, 100, 100)
cut(imgHydra[1], "outcuts/02_cut.png", boxMinX_input, boxMinY_input, 100, 100)
cut(imgHydra[2], "outcuts/03_cut.png", boxMinX_input, boxMinY_input, 100, 100)
cut(imgHydra[3], "outcuts/04_cut.png", boxMinX_input, boxMinY_input, 100, 100)
