# ExaminePaper_Detection
# Author: Weichen Xu
# Create Time: 2015/6
Detect answer in examine paper

Algorithm Steps:
1. preprocess
2. canny edge detection
3. line detection
4. box find
5. box validation

Preprocess:
  1.convert to Gray
  2.gauss blur
  3.binarization with OTSU
  
canny edge detection:
  1.opencv canny edge detection
  2.para: lowThresh, highThresh, kernelSize for gauss
  
line detection:
  1.opencv houghlinep 
  2.param: houghthresh, min line length, line gap

box find:
  1.filter lines by their angles, input: all lines, output: vertical lines, horizontal lines
  2.get box size: set box width with horizontal lines, set box height with vertical lines
  3.note all points of the lines, note hor/ver lines location and count

box validation:
  1.if box count < minimum || related points count < minimum, delete box
  2.sort all box, merge nearby ones
  3.delete odd box, considering the distance between pairs
