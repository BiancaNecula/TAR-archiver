Computer Programming Course \
HOMEWORK 3 - TAR archiver 

January, 2020 

Bianca Necula \
Faculty of Automatic Control and Computer Science \
315CA 

# Info
```bash
git clone https://github.com/BiancaNecula/TAR-archiver.git
```
Run ./check for testing with the checker. \
See "Programare_2019__Tema_3.pdf" for information about the homework. \
**Tasks with Files, Strings and Structures in C language.**

# About the code:
Implementation in C language of a program that simulates an utility that creates a tar archive. The program
is represented by an executable file named archiver, which when running can interpret certain
commands from standard input (stdin). The commands that the program can interpret are:

* create <archive_name> <directory_name> - creates a tar archive with the name archive_name that contains
file from directory directory_name.
* list <archive_name> - displays the names of the files contained in the archive_name archive.
* extract <filename> <archive_name> - extract the contents of the filename saved in the archive
archive_name.
* exit - ends the program execution. 
	
If the command is incorrect or has a smaller or larger number of parameters, "Wrong command!" will be displayed on the screen, otherwise "Done!".
	

* Functions used: 

	- write_meta: writes the metadata corresponding to each given file through the header in the record union
	- write_data: write character by character the information in the files and fill the remaining space up to a multiple of 512 with '/ 0'
	- calculate_checksum: calculates checksum between bytes in the header
	- calculate_time: calculates the timestamp of the files with the mktime () function

* Tar header used: https://www.gnu.org/software/tar/manual/html_node/Standard.html
* Other files used:
	- files.txt: contains the files to be archived. He counts a subset of lines in the result
applying the command "ls -la --time-style = full-iso | grep ^ -" to the directory containing the files
	- usermap.txt: it has information about the users of the system, and more precisely, it contains the result of the order
"cat / etc / passwd". The / etc / passwd file is a text file, which contains a list of everyone's accounts
users, providing for each account some useful information such as user ID, group ID,
and so on.
