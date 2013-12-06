Matt Dumford - mdumford
mdumfo2@uic.edu

findit.c

Known issues:
	When using more than one expression there is no guarantee that the
	decision tree created will accurately reflect your intentions. It
	does, however, try its best.


findit [list of paths] [-expressions]


All paths are expected to come before any expressions. The first argument
starting with a '-' marks the beginning of the expression list. All
expressions are expected to start with the '-' character except for the
aditional arguments provided to an expression such as "-size 1024" or "-access
r".


Single Expressions:
These expressions must be the first expression. All other expressions will be
ignored if these expressions are seen.

-treedir
	Prints the directory tree with the number of sub directories and files in
	each directory, along with the directory's size.

-summarize 
	Tracks the number of each type of file in the given paths and how much
	space each type takes up.

-largest type
	Finds the largest type where type can be file, dir, or tree.

-smallest type
	Finds the smallest type where type can be file, dir, or tree.

-help 
	Display this help page.


Other Expressions:
These expressions can be chained together with -and, -or, and -not with -and
as the default. Any files that meet the description of the expression chain
will be printed.

-size [+-]n
	Returns true if a file's size is less than (-), greater than (+), or
	exactly (no prefix) n.

-access type
	Returns true if the user has access of type specified by type to a file.
	type can be r for read, w for write, x for execute, or a for all.

-sparse
	Returns true if a file's size is bigger than indicated by the number of
	blocks used.

-print
	Always returns true. Prints files. The program will do this anyway.
