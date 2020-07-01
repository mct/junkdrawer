#!/usr/bin/env python3

'''

Takes as input a list of images files, and creates a new image of the
files concatenated vertically.  Useful for, say, stitching together a
series of phone screenshots.  The input images should all have the
same width.

                                -mct, Wed Jul  1 15:06:44 PDT 2020

'''

import argparse
from PIL import Image

def cat(files, output):
    images = []

    for i in files:
        images.append(Image.open(i))

    width = images[0].width
    height = sum(map(lambda x: x.height, images))

    img = Image.new('RGB', size=(width, height), color=(255,255,255))

    cursor = 0
    for i in images:
        img.paste(i, (0, cursor))
        cursor += i.height

    img.save(output)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', '-o', type=str, required=True)
    parser.add_argument('input',          type=str, nargs='+')
    args = parser.parse_args()

    cat(args.input, args.output)
