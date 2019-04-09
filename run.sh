if [ $# -lt 7 ]; then
    echo "QBF_or_SAT mat_row mat_col win_row win_col cycle hasPadding (option)"
    exit 1
fi
cd tests
filename=`python3 ../python_src/gen_cnn_setting.py "${@:2}"`
extension=`echo $filename | sed 's/^.*\.\([^\.]*\)$/\1/'`
if [ $extension != "txt" ]; then
    echo "generate setting file failed because : " $filename
    exit 1
fi

assign_filename=""
com_filename=""
if [ $1 = "QBF" -o $1 = "0" ]; then
    assign_filename=${filename}.qbf.out.blif.assign.txt
    com_filename= ${filename}.qbf.out.blif.com.txt
    if [ -e $assign_filename ]; then
	rm $assign_filename
    fi
    if [ -e $com_filename ]; then
	rm $com_filename
    fi
    
    ../bin/qbf_syn $filename

elif [ $1 = "SAT" -o $1 = "1" ]; then
    cnf_filename=${filename}.sat.cnf
    out_filename=${filename}.sat.cnf.out
    assign_filename=${out_filename}.assign.txt
    com_filename=${out_filename}.com.txt
    if [ -e $assign_filename ]; then
	rm $assign_filename
    fi
    if [ -e $com_filename ]; then
	rm $com_filename
    fi

    python3 ../python_src/gen_cnf.py $filename
    minisat $cnf_filename $out_filename
    r=`python3 ../python_src/parse_result.py $filename $out_filename`
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

python3 ../python_src/gen_cnn_png.py $2 $3 $6 $assign_filename $com_filename
