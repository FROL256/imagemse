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

def eval_mse_for_images(a_list, folder1):

  myref = a_list[-1][1]

  mseList = []
  for (k,v) in a_list:
    mseFloat = mse(folder1 + "/" + v, folder1 + "/" + myref)
    mseList.append((v,mseFloat))

  return mseList 

def sorted_list_from_dir(folder1, ext):
  onlyfiles = [f for f in listdir(folder1) if isfile(join(folder1, f))]
  ldr_files = [f for f in onlyfiles if f.endswith(ext)]

  ldr_files_by_time = { int(re.search(r'\d+', fname).group()) : fname for fname in ldr_files } # {'8': aaa_8min.png, '16': aaa_16min.png }
  return sorted(ldr_files_by_time.items())
 

# ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INPUTFOLDER = "/home/frol/Dropbox/temp/L1.1/Hydra/PT/exp1"

ldr_dict_sorted1 = sorted_list_from_dir(INPUTFOLDER, ".png")
hdr_dict_sorted1 = sorted_list_from_dir(INPUTFOLDER, ".hdr")

ldr_ref = ldr_dict_sorted1[-1][1]
hdr_ref = hdr_dict_sorted1[-1][1]

ldr_ref2 = ldr_dict_sorted1[-2][1]
hdr_ref2 = hdr_dict_sorted1[-2][1]

refErrLdr = mse(INPUTFOLDER + "/" + ldr_ref, INPUTFOLDER + "/" + ldr_ref2)
refErrHdr = mse(INPUTFOLDER + "/" + hdr_ref, INPUTFOLDER + "/" + hdr_ref2)

csv_file = open('out.csv', mode='w')
writer   = csv.writer(csv_file, delimiter=';', quotechar='"', quoting=csv.QUOTE_MINIMAL)

arr1 = eval_mse_for_images(ldr_dict_sorted1, INPUTFOLDER)
arr2 = eval_mse_for_images(hdr_dict_sorted1, INPUTFOLDER)


writer.writerow(["time(min)", "mse_ldr", "mse_ref_ldr", "time(min)", "mse_hdr*256", "mse_ref_ldr*256"])

for i in range(0,len(arr1)):
  (name1,err1) = arr1[i]
  (name2,err2) = arr2[i]
  time1 = int(re.search(r'\d+', name1).group())
  time2 = int(re.search(r'\d+', name2).group())

  if time1 == 0:
    time1 = 0.5

  if time2 == 0:
    time2 = 0.5

  writer.writerow([time1, err1, refErrLdr, time2, 256.0*err2, 256.0*refErrHdr])

