# dia2sql
Extract and generate the SQL code from a dia diagram that used the shapes database  menu item. The program is a console application. no GUI is involved.

To Build install the following packages.

apt install libpugixml-dev libgzstream-dev build-essential.

and run **make** in the same directory of the project. the build will produce a static standalone executable binary called *dia2sql.exe*

Usage: dia2sql  INPUT_FILE.dia  [ OUTPUT.SQL] [ --verbose ].

This command generates the SQL code for table creation from an INPUT_FILE.dia dia diagram file and save them in file OUTPUT.SQL

You have to use the shapes of [Database] in the menu of shapes .

If OUTPUT.SQL is not given, then the SQL code will be displayed on the terminal to stdout.
