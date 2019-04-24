if [ $# -lt 1 ]; then
    echo "setting file"
    exit 1
fi

path=$(cd $(dirname $0); pwd)
filename=$1

out_filename=${filename}.sat.out
log_filename=${filename}.sat.log
assign_filename=${out_filename}.assign.txt
com_filename=${out_filename}.com.txt
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
r=`python3 ${path}/python_src/parse_result.py $filename $out_filename`
    if [ -n "$r" ]; then
	echo $r
	exit 1
    fi

if [ ! -e $assign_filename -o ! -e $com_filename ]; then
    echo "synthesis failed"
    exit 0
fi

