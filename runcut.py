import re
import os
import sys
import subprocess

from os import listdir
from os.path import isfile, join

import fileinput
import io

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


def list_dirs_deep(path):
  dirs = []
  for root, subdirs, files in os.walk(path):
    for file in os.listdir(root):
      filePath = os.path.join(root, file)
      if os.path.isdir(filePath):
        dirs.append(filePath)
  return dirs
        

class TestCase():
  def __init__(self, a_folder, a_timePoint, a_bMinX, a_bMinY):
    self.folder  = a_folder
    self.timept  = a_timePoint
    self.boxMinX = a_bMinX
    self.boxMinY = 1024 - a_bMinY

    splitted    = a_folder.split(os.sep)
    self.engine = splitted[-2]
    self.method = splitted[-1]
    self.outdir = splitted[-3]


def run_cut(orb_path, timepoint, boxMinX_input, boxMinY_input):
  
  images = []

  mydirs = list_dirs_deep(orb_path)
  mydirs = sorted(mydirs[4:]) # discard first 4 folders due to they are just render system folders, does not contain images

  for folder in mydirs:
    testc     = TestCase(folder, timepoint, boxMinX_input, boxMinY_input);
    allimages = sorted_list_from_dir(folder, ".png")
    if len(allimages) < 1:
      continue
    targetimage = allimages[timepoint][1]
    outFolder   = testc.outdir + "_out";
    if not os.path.exists(outFolder):
      os.mkdir(outFolder)
    outFileName = outFolder + "/{0}_{1}.png".format(testc.engine, testc.method)
    cut(folder + "/" + targetimage, outFileName, testc.boxMinX, testc.boxMinY, 100, 100)
    images.append(outFileName)
  return images

def array_to_matrix(inputArr, colNum):
  inputArr2 = inputArr[:]
  result    = []

  while len(inputArr2) > 0:
    result.append(inputArr2[:colNum])
    inputArr2 = inputArr2[colNum:]
  return result


#\includegraphics[width=0.23\linewidth]{01_glossy_cornell1_cut/zcut_1_LDR_PT_10min.png}

myimages1   = run_cut("/media/frol/886234F06234E49A/yandexdisk/2019/orb_2019_images/L1.1", 3, 265, 950)
#myimages1_g = ["\\includegraphics[width=0.24\\linewidth]{" + fname + "}" for fname in myimages1]
myimages2   = array_to_matrix(myimages1, 4)

tabstrPrinter = io.StringIO()
print("\\begin{tabular}{c c c c}", file=tabstrPrinter)

for lineId in range(0, len(myimages2)):
  line = myimages2[lineId]
  
  for columnId in range(0, len(line)):
    column = line[columnId]
    print("\\includegraphics[width=0.24\\linewidth]{" + column +"}", file=tabstrPrinter, end='')
    if columnId != len(line)-1:
      print(" & ", file=tabstrPrinter)
  
  print("\\\\", file=tabstrPrinter)
  for columnId in range(0, len(line)):
    column = line[columnId].split(os.sep)[1]
    name   = column.replace("_", "\\_")
    name   = name.replace(".png","")
    print(name, file=tabstrPrinter, end='')
    if columnId != len(line)-1:
      print(" & ", file=tabstrPrinter)
  
  if lineId != len(myimages2)-1:    
    print("\\\\", file=tabstrPrinter)
  
  print("", file=tabstrPrinter)

print("", file=tabstrPrinter)
print("\\end{tabular}", file=tabstrPrinter)





tabstr = tabstrPrinter.getvalue()

for line in fileinput.FileInput("zz_comp.tex", inplace=1):
  if "REPLACEME" in line:
    line = line.replace(line, tabstr)
  print(line, end='')

