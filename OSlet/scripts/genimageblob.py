#!/usr/bin/python

#
# Convert images (given as arguments) to a C source file blob
#

import argparse
from PIL import Image
import sys
import os.path as path
import re

parser = argparse.ArgumentParser(
		description='Create C source from image files.')

parser.add_argument('images', metavar = '<FILE>', nargs = '+',
                    help = 'Image file to add to blob')

parser.add_argument('-o', metavar = '<OUT>', required = True,
		help = 'output prefix (will generate OUT.c and OUT.h)')

parser.add_argument('-b', metavar = '<BLOBNAME>', required = True,
		help = 'name of blob object')

args = parser.parse_args()

cFileName = "%s.c" % args.o 
hFileName = "%s.h" % args.o

outCSource = open(cFileName, "w")
outHeader = open(hFileName, "w")

# write headers

outCSource.write("/*\n")
outCSource.write("\t%s : %s\n" % (cFileName, args.b))
outCSource.write("\n\t--- AUTOMATICALLY GENERATED --\n")
outCSource.write("\t\t\t\t\t\t*/\n\n")

outHeader.write("/*\n")
outHeader.write("\t%s : %s\n" % (hFileName, args.b))
outHeader.write("\n\t--- AUTOMATICALLY GENERATED --\n")
outHeader.write("\t\t\t\t\t\t*/\n\n")

outHeader.write("extern unsigned char %s[];" % args.b);

blobData = []

blobOffset = 0

for imgFile in args.images:

	im = Image.open(imgFile)
	pixels = im.load()

	if im.mode != "RGBA":
		print "%s: Incorrect format: %s" % (imgFile, im.mode)
		sys.exit(1)

	fileBasename = path.basename(imgFile)
	fileBasenameNoExt = path.splitext(fileBasename)[0] 

	imageName = re.sub("[-\s]", "_", fileBasenameNoExt.lower())
	
	# write out header data

	outHeader.write("\n\n");
	outHeader.write("#define img_%s_width %d\n" % (imageName, im.width))
	outHeader.write("#define img_%s_height %d\n" % (imageName, im.height))
	outHeader.write("#define img_%s_blobOffset 0x%x\n" % (imageName, blobOffset))

	for j in xrange(im.height):
	    for i in xrange(im.width):
		blobData.append(pixels[i,j][2])
		blobData.append(pixels[i,j][1])
		blobData.append(pixels[i,j][0])
		blobData.append(pixels[i,j][3])

	blobOffset += im.width * im.height * 4

# write out blob data

outHeader.write("\n#define %s_sizeInBytes %d\n\n" % (args.b, len(blobData)));

outCSource.write("\n\nunsigned char %s[] = {\n" % args.b)

outCSource.write("\t0x%02x" % blobData[0])

for i in range(1, len(blobData)):

	if (i % 12 == 0):
		outCSource.write(",\n\t")
	else:
		outCSource.write(",")

	outCSource.write("0x%02x" % blobData[i])
	

outCSource.write("\n\t};\n");

outCSource.close()
outHeader.close()

