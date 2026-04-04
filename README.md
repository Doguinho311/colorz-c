# colorz-c
A k-means algorithm to create palletes, inspired by pywal, made for fun and for learning C using the library stb_image for image processing: https://github.com/nothings/stb/tree/master.

The algorithms creates a random seed on the image and 17 more other cluster points based on the furthest color on RGB-spectrum between this point and all the previous ones while ignoring really bright or really dark points.

Then applies the k-means clusttering algorithm until it does reach the target point within a 1e-18 margin.

Finally, it sorts the colors based on luminance, according to the human eye perception of color, and prints to the terminal.

# Usage
Make sure you setup stb_image, then compile using "gcc -o colorz-c colorz-c.c -lm".

./colorz-c <path_to_image>

# Example
<img width="1920" height="1080" alt="2b" src="https://github.com/user-attachments/assets/7e14a131-93fd-4e57-9efc-20b0bdc72bd6" />

Pallete generated:
<img width="1598" height="383" alt="image" src="https://github.com/user-attachments/assets/9a846bc4-d004-4943-8c2a-55b10235b463" />


- Source: https://pt.wallpapers.com/foto/imagens-2b-1920-x-1080-0dtdxd6kpfxnd5sb.html

# To-do
Ready the output to be used for hyprland ricing.
