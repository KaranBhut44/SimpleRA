## Non-Sparse Matrix :
### Page Layout
 - In `matrix.h`, we have specified variable M = 32, that is number of rows of submatrix. So, submatrix size in 1 page = 32*32.(By Assuming `int` takes 4 byte, PageSize = 32\*32\*4 bytes = atmost 4KB per page). We can also change it for testing purpose.
 - so, basically we are storing submatrices of original matrix into different pages(page size<8KB). below is the example containing submatrices of atmost 2\*2, original matrix size is 5\*5.
<img src="docs/img1.PNG" alt="Figure1"/>

### Transpose Operation:
 - At first, perform transpose operation on entries of individual pages.(internally)
 - Interchange page names(such that we don't have to copy values from 1 page to another). For example, page_1_2 will be changed to page_2_1 and vice versa. Diagonal matrices will stay as it is. (i.e. page_2_2).
 - Below is the example of 5\*5 matrix transpose using atmost 2\*2 submatrices.(pages)
<img src="docs/img2.PNG" alt="Figure2"/>

## Sparse Matrix:
### Compression Technique:
 - For sparse matrix, We can't store the matrix, submatrix wise. It will consume unneccesary space.
 - so, We have stored non-zero values diagonally into different pages.
 - If there are so much values on single diagonal, then we have seperated them into multiple pages. 
### Page Layout:
 - we are storing 3 values for each non-zero element: (row no., col no, value) inside respected page.
 - If particular diagonal `don't contain any non-zero value` then it's corresponding `page won't be created`.
 - Below is the example of 8\*8 matrix containing 14 non-zero elements.(Sparseness > 60%)
<img src="docs/img3.PNG" alt="Figure3"/>

### Transpose Operation:
 - Swap first 2 values(row and column number) for each cell in each page,
 - Change Page-name. For example, Page_1_0 to Page_-1_0 and vice versa.
 - Principle diagonal pages won't require any change. (i.e. Page_0_0).

## Compression ratio as a function of the percentage of sparseness of the matrix:

