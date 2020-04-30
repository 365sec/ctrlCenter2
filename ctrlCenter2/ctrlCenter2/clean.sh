#rm -f ./CMakeCache.txt
#rm -f ./cmake_install.cmake
#rm -rf ./CMakeFiles

function cleancmake()
{
	#做清除操作
	echo $1
	rm -f  $1/CMakeCache.txt
	rm -f  $1/cmake_install.cmake
	rm -rf $1/CMakeFiles

	#遍历
    for element in `ls $1`
    do  
		STR=$1
		FINAL=${STR: -1}
		#判断最后一个字符是否是分隔符，若不是则添加
		if [ ${FINAL} = "/" ];then 
			dir_or_file=$1""$element
		else
			dir_or_file=$1"/"$element
        fi  
		
		#判读是文件夹
        if [ -d $dir_or_file ];then 
            cleancmake $dir_or_file
		# else
			# echo $dir_or_file
        fi  
    done
}

root_dir="./"
cleancmake $root_dir