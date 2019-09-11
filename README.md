# GDS2MAT
Convert GDSII file features into (x,y) coordinates and layer number

Must know:
1. The input is the Binary file (GDSII), the output is a three column matrix (x-coordinates, y-coordinates, layer number & number of        points)
2. The last row contains the physical dimension in this form 0    0    1e-9 if the physical dimention is 1 nm.
3. Layer number is incremented by 1 than its real value, since zero is used for other purpose
4. Number of points is multiplied by -1 for sorting reason

5. let's take an example of a (square polygon of layer 1), since it is a square it has 4 edges,
   P1: (0,0)
   P2: (0,1)
   P3: (1,1)
   P4: (1,0)
   to complete the polygon, it should return to the first point so
   P5: (0,0)

   and we have another (square of layer 5)
   V1: (3,3)
   V2: (3,5)
   V3: (5,5)
   V4: (5,3)
   V5: (3,3)

   so the output file will be like that:
   0 0 2
   0 1 -5
   1 1 0
   1 1 0
   0 0 0
   3 3 6
   3 5 -5
   5 5 0
   5 3 0
   3 3 0
   0 0 1e-9
   
   a. The first two columns are clear. The third one started with (GDSII layer number + 1) so that's why we see 2 at the first row and 6    at row number 6 (does not mean anything, it is just a by luck :D). The negative number in the second and seventh row are the number      of points inside a polygon but in negative form.
   b. The zero is used to just to sort this matrix, as we can get easily the number of polygons (Positive number in column 3), and get      the indecies while searching for polygons in a specific layers (further processing)
   c. The last row is the physical unit at the last column.

6. The code can read Bondaries(Polygons), Routing(Paths [Actually pathtype 2 return to GDSII stream format manual] - Converted to          polygons), Structure Reference (Via is a multilayer boundaries).
7. It does not include AREF, Angle & STRANS because we just needed to use less complex masks as a start.
8. The GDSII file name should be the same as the name of Top-Level device(design/structure) as the code will not work correctly
9. for GDSII Stream Format Manual: http://bitsavers.informatik.uni-stuttgart.de/pdf/calma/GDS_II_Stream_Format_Manual_6.0_Feb87.pdf
