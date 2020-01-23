if exist dir-two\linked-file.txt del dir-two\linked-file.txt

mklink /h dir-two\linked-file.txt dir-one\file.txt
