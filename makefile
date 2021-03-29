all:
	g++ -O2 -std=c++11 imagemse.cpp imageutils.cpp -lfreeimage -o imagemse 
	g++ -O2 -std=c++11 imagecut.cpp imageutils.cpp -lfreeimage -o imagecut
	g++ -O2 -std=c++11 imagedssim.cpp imageutils.cpp -lfreeimage -o imagedssim
