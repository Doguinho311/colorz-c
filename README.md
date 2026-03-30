# colorz-c
A k-means algorithm to create palletes, made for fun and for learning C.

The algorithms creates a random seed on the image and 17, for now, other cluster points based on the most "distance" on RGB-spectrum between this point and the previous ones.

Then applies the k-means clusttering algorithm until it does reach the target point within a 1e-18 margin.

Finally, it sorts the colors based on luminance, according to the human eye perception of color, and prints to the terminal.

# Usage
./colorz-c <path_to_image>
