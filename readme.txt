
	>>>> V E R S I O N    2  <<<<

INTRODUCTION:
Rh was written by Ken Stauffer to make the
job of finding files easier by allowing the
user to enter real C expressions. This notation is
much easier to master than the notation used by the
find(1) command, because most Unix users
already know C. In addition to being easier to use
than find(1), rh expressions can be used to select
the desired files.

CREDITS:
    Guy Harris
	- Corrected many portability problems.
    David MacKenzie
	- Manual revisions. Added getopt and regular expressions.
    Norm Hutchinson
	- Fixed ungetit().

COMPILING:
To make rh work on your system, you will need to change
some -D options in the Makefile. Define ONE of the
following in the definition of CFLAGS:

	-DBSD		- This would be used for most BSD systems.
	-DSYSV		- System V systems.

Also define one of the following:

	-DSUNOS_4	- SunOS 4.x (subclass of BSD)
	-DSYSVR3	- System V Release 3.x (subclass of SYSV)

In addition to the C source there is also a file called rh.man.
This is a nroff file and can be created by a command like:

		nroff -man rh.man > rh.cat

The resultant file (rh.cat) is sutable for general viewing.

RUNNING:
There is a file called rhrc. This file contains some
examples of things that can go into a $HOME/.rhrc file.
If the file "rhrc" is moved to your home directory and renamed
to ".rhrc" then a command like:

	% rh -l -e writable

Will do a search of the current directory, executing the function
"writable()", which finds files that other people have write access to.

Once rh is made, you can do what you want with it. A good test to
see if it is working is to do:

	% rh -vle 1 /

This will find all files that makes the constant expression '1' true.
So if your root, all the files on the system will be found.

PORTABILITY:
The file rhdir.c contains code that does directory reading.
This is most likely where problems will occur. These differences
have been taken into account for most versions of unix
and will hopefully work on your system.
So far 'rh' works on:
	SCO XENIX, BSD 4.3, and SUNOS 4.0

GRAMMER:
The following is the grammer that describes the input language
recognized by rh:

	<program> ==> <function list> <expression> EOF
		| <function list> <expression> ;

	<function list> ==> <function>
		| <function list> <function>
		| /* empty */

	<function> ==> <function heading> { RETURN <expression> ; }

	<function heading> ==> IDENTIFIER
		| IDENTIFIER ( )
		| IDENTIFIER ( <idlist> )

	<idlist> ==> IDENTIFIER <idtail>
	<idtail> ==> , <idlist>
		| /* empty */

	<expression> ==> <expr0> ? <expression> : <expression>

	<expr0> ==> <expr1> || <expr1>

	<expr1> ==> <expr2> && <expr2>

	<expr2> ==> <expr3> | <expr3>

	<expr3> ==> <expr4> ^ <expr4>

	<expr4> ==> <expr5> & <expr5>

	<expr5> ==> <expr6> == <expr6>
		|   <expr6> != <expr6>

	<expr6> ==> <expr7> < <expr7>
		|   <expr7> > <expr7>
		|   <expr7> <= <expr7>
		|   <expr7> >= <expr7>

	<expr7> ==> <expr8> >> <expr8>
		|   <expr8> << <expr8>

	<expr8> ==> <expr9> + <expr9>
		|   <expr9> - <expr9>

	<expr9> ==> <expr10> * <expr10>
		|   <expr10> / <expr10>
		|   <expr10> % <expr10>

	<expr10> ==> ~ <expr10>
		|    ! <expr10>
		|    - <expr10>
		|    <factor>

	<factor> ==> ( <expression> )
		|    NUMBER
		|    <function call>
		|    IDENTIFIER
		|    [ <date spec> ]
		|    STRING

	<function call> ==> IDENTIFIER
		| IDENTIFIER ( <exprlist> )
		| IDENTIFIER ( )

	<exprlist> ==> <expression> <exprtail>
	<exprtail> ==> , <exprlist>
		| /* empty */

	<datespec> ==> NUMBER / NUMBER / NUMBER

--------------------------------------------------------------------
Ken Stauffer.
root@sixk


I made some extension for DOS and OS/2 to give one access to the archive,
hidden and system attribute bits and made some cosmetic changes.

Note that stat.c is from the MS C 5.1 library source code and copyrighted
by Microsoft. I included it because the original version has a bug.

Kai Uwe Rommel
rommel@lan.informatik.tu-muenchen.dbp.de
