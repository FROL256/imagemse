# imagemse
A simple tool for calculating images metrics and cutouts.

* MSE (both LDR and HDR).

Compute RGB difference and divide it with width\*height. Not width\*height\*3.

imagemse image1.png image2.png

* DSSIM metric (for LDR):

imagedssim image1.png image2.png

* Image cut (subimage):

imagecut imageIn.png subImageOut.png 20 50 100 100

imagecut input.png output.png boxMinX boxMinY boxSizeX boxSizeY
