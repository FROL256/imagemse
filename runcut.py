import re
import os
import sys
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


def sorted_list_from_dir(folder1, ext):
  onlyfiles = [f for f in listdir(folder1) if isfile(join(folder1, f))]
  ldr_files = [f for f in onlyfiles if f.endswith(ext)]

  ldr_files_by_time = { int(re.search(r'\d+min', fname).group()[:-3]) : fname for fname in ldr_files } # {'8': aaa_8min.png, '16': aaa_16min.png }
  return sorted(ldr_files_by_time.items())


def list_dirs(path):
  dirs = []
  for root, subdirs, files in os.walk(path):
    for file in os.listdir(root):
      filePath = os.path.join(root, file)
      if os.path.isdir(filePath):
        dirs.append(filePath)
  return dirs
        


boxMinX_input = 295
boxMinY_input = 1024 - 935

timepoint     = 3 
orb_path      = "/media/frol/886234F06234E49A/yandexdisk/2019/orb_2019_images/L1.2"

mydirs = list_dirs(orb_path)
mydirs = sorted(mydirs[4:]) # discard first 4 folders due to they are just render system folders, does not contain images


counter = 0
for folder in mydirs:
  splitted          = folder.split(os.sep)
  (renderer,method) = (splitted[-2], splitted[-1])
  allimages         = sorted_list_from_dir(folder, ".png")
  targetimage       = allimages[timepoint][1]
  cut(folder + "/" + targetimage, "outcuts/{0}_{1}_{2}.png".format(counter,renderer,method), boxMinX_input, boxMinY_input, 100, 100)
  counter = counter+1

"""

images_hydra_pt   = sorted_list_from_dir(orb_path + "Hydra/PT",     ".png")
images_hydra_qmc  = sorted_list_from_dir(orb_path + "Hydra/PT_QMC", ".png")
images_hydra_ibpt = sorted_list_from_dir(orb_path + "Hydra/IBPT",   ".png")
images_hydra_mmlt = sorted_list_from_dir(orb_path + "Hydra/MMLT",   ".png")

images_octane_pmc = sorted_list_from_dir(orb_path + "Octane/PMC",   ".png") 
images_vray_pt_lc = sorted_list_from_dir(orb_path + "VRay/BF_LC",   ".png") 

images_corona_pt_hd = sorted_list_from_dir(orb_path + "Corona/PT_UHD",   ".png") 
images_corona_vcm   = sorted_list_from_dir(orb_path + "Corona/VCM",   ".png") 

cut(orb_path + "Hydra/PT"     + "/" + images_hydra_pt[timepoint][1],   "outcuts/01_cut_hydra_pt.png",     boxMinX_input, boxMinY_input, 100, 100)
cut(orb_path + "Hydra/PT_QMC" + "/" + images_hydra_qmc[timepoint][1],  "outcuts/02_cut_hydra_pt_qmc.png", boxMinX_input, boxMinY_input, 100, 100)
cut(orb_path + "Hydra/IBPT"   + "/" + images_hydra_ibpt[timepoint][1], "outcuts/03_cut_hydra_ibpt.png",   boxMinX_input, boxMinY_input, 100, 100)
cut(orb_path + "Hydra/MMLT"   + "/" + images_hydra_mmlt[timepoint][1], "outcuts/03_cut_hydra_mmlt.png",   boxMinX_input, boxMinY_input, 100, 100)
cut(orb_path + "Octane/PMC"   + "/" + images_octane_pmc[timepoint][1], "outcuts/04_octane_pmc.png",       boxMinX_input, boxMinY_input, 100, 100)

cut(orb_path + "VRay/BF_LC"   + "/" + images_vray_pt_lc[timepoint][1], "outcuts/05_vray_pt_lc.png",       boxMinX_input, boxMinY_input, 100, 100)

cut(orb_path + "Corona/PT_UHD" + "/" + images_corona_pt_hd[timepoint][1], "outcuts/06_corona_pt_hd.png",  boxMinX_input, boxMinY_input, 100, 100)
cut(orb_path + "Corona/VCM"    + "/" + images_corona_vcm[timepoint][1],   "outcuts/07_corona_vcm.png",    boxMinX_input, boxMinY_input, 100, 100)

"""
