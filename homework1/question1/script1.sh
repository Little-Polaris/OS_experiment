#!/bin/bash
#声明解释器，用bash解释下列命令
generate_script() {						#定义一个函数，用于生成一个脚本
    script_file=$1						#该函数的第一个参数script_file
    flag=$2							#干函数的第二个参数flag

    echo '#!/bin/bash' > "$script_file"				#向script_file里写入'#!/bin/bash'	
    echo "" >> "$script_file"					#向script_file里空出一行
    echo "file=\$1" >> "$script_file"				#向script_file里写入"file=$1" 
    echo "flag=\$2" >> "$script_file"				#向script_file里写入"flag=$2"
    echo "" >> "$script_file"					#向script_file里空出一行
    echo "if [ \$flag = \"r\" ]; then" >> "$script_file"	#向script_file里写入"if [ $flag = "r" ]; then" 
    echo "    cat \$file" >> "$script_file"			#向script_file里写入"    cat $file"
    echo "elif [ \$flag = \"w\" ]; then" >> "$script_file"	#向script_file里写入"elif [ $flag = "w" ]; then"
    echo "    echo \"175 MYFILE\" > \$file" >> "$script_file"	#向script_file里写入"    echo "175 MYFILE\ > $file"
    echo "fi" >> "$script_file"					#向script_file里写入"fi" 
    chmod +x "$script_file"					#添加script_file的执行权限
}

script_file="script2.sh"					#将变量 script_file赋值为"script2.sh"
generate_script "$script_file" "w"				#执行generate_script函数，两个参数分别为script_file和"w"

./"$script_file" file.txt w					#执行生成的脚本script_file，两个参数分别为 "file.txt"和"w"

