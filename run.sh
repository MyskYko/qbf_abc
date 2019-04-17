if [ $# -lt 1 ]; then
    echo "QBF_or_SAT"
    exit 1
    
fi

if [ $1 != "QBF" -a $1 != "SAT" ]; then
    echo "QBF_or_SAT"
    exit 1
fi

path=$(cd $(dirname $0); pwd)

cd ${path}/tests
filename=`python3 ../python_src/gen_cnn_setting.py "${@:2}"`
newdirname=`echo $filename | sed 's/\.[^\.]*$//'`
extension=`echo $filename | sed 's/^.*\.\([^\.]*\)$/\1/'`
if [ "$extension" != "txt" ]; then
    echo "generate setting file failed because : " $filename
    exit 1
fi

if [ ! -e $newdirname ]; then
    mkdir $newdirname
fi
mv $filename ${newdirname}/.
cd $newdirname

assign_filename=""
com_filename=""
if [ $1 = "QBF" -o $1 = "0" ]; then
    assign_filename=${filename}.qbf.out.blif.assign.txt
    com_filename=${filename}.qbf.out.blif.com.txt
    if [ -e $assign_filename ]; then
	rm $assign_filename
    fi
    if [ -e $com_filename ]; then
	rm $com_filename
    fi
    
    unbuffer ${path}/bin/qbf_syn $filename

elif [ $1 = "SAT" -o $1 = "1" ]; then
    cnf_filename=${filename}.sat.cnf
    out_filename=${filename}.sat.cnf.out
    log_filename=${filename}.sat.cnf.log
    assign_filename=${out_filename}.assign.txt
    com_filename=${out_filename}.com.txt
    if [ -e $cnf_filename ]; then
	rm $cnf_filename
    fi
    if [ -e $out_filename ]; then
	rm $out_filename
    fi
    if [ -e $log_filename ]; then
	rm $log_filename
    fi
    if [ -e $assign_filename ]; then
	rm $assign_filename
    fi
    if [ -e $com_filename ]; then
	rm $com_filename
    fi

    unbuffer ${path}/python_src/gen_cnf $filename | tee $log_filename
    unbuffer minisat $cnf_filename $out_filename | tee -a $log_filename
    r=`python3 ${path}/python_src/parse_result.py $filename $out_filename`
    if [ -n "$r" ]; then
	echo $r
	exit 1
    fi
else
    echo "QBF_or_SAT mat_row mat_col win_row win_col cycle hasPadding (option)"
fi

if [ ! -e $assign_filename -o ! -e $com_filename ]; then
    echo "synthesis failed"
    exit 0
fi

python3 ${path}/python_src/gen_cnn_png.py `expr $2 + $7` `expr $3 + $8` $6 $assign_filename $com_filename
echo "synthesis succeeded"
