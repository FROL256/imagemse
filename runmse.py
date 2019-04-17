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

  ldr_files_by_time = { int(re.search(r'\d+min', fname).group()[:-3]) : fname for fname in ldr_files } # {'8': aaa_8min.png, '16': aaa_16min.png }
  return sorted(ldr_files_by_time.items())
 

def build_csv_table(imagefolder, csvfilename, algsuffix):
  
  ldr_dict_sorted1 = sorted_list_from_dir(imagefolder, ".png")
  #hdr_dict_sorted1 = sorted_list_from_dir(imagefolder, ".hdr")
  
  print (ldr_dict_sorted1)

  ldr_ref = ldr_dict_sorted1[-1][1]
  #hdr_ref = hdr_dict_sorted1[-1][1]

  ldr_ref2 = ldr_dict_sorted1[-2][1]
  #hdr_ref2 = hdr_dict_sorted1[-2][1]

  refErrLdr = mse(imagefolder + "/" + ldr_ref, imagefolder + "/" + ldr_ref2)
  #refErrHdr = mse(imagefolder + "/" + hdr_ref, imagefolder + "/" + hdr_ref2)

  csv_file = open(csvfilename, mode='w')
  writer   = csv.writer(csv_file, delimiter=';', quotechar='"', quoting=csv.QUOTE_MINIMAL)

  arr1 = eval_mse_for_images(ldr_dict_sorted1, imagefolder)
  #arr2 = eval_mse_for_images(hdr_dict_sorted1, imagefolder)

  writer.writerow(["time(min)", "mse_ldr" + algsuffix, "mse_ref_ldr"]) #, "time(min)", "256*mse_hdr" + algsuffix, "256*mse_ref_ldr"])

  for i in range(0,len(arr1)):
    (name1,err1) = arr1[i]
    time1 = int(re.search(r'\d+min', name1).group()[:-3])

    if time1 == 0:
      time1 = 0.5

    writer.writerow([time1, err1, refErrLdr]) #, time2, 256.0*err2, 256.0*refErrHdr])

# ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

build_csv_table("/home/frol/comp_input/L1.1/VRay/BF_LC",  "/home/frol/Dropbox/temp/vray_bf_lc.csv", "_vray1")
#build_csv_table("/home/frol/comp_input/L1.1/VRay/Irr_LC", "/home/frol/Dropbox/temp/vray_ic_lc.csv", "_vray2")
#build_csv_table("/home/frol/Dropbox/temp/L1.1/VRay/BF_LC",  "out_vray_bf_lc.csv", "_vray2")     

