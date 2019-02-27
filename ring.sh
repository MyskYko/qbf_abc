sed -i -e "s/ 3 / 4 /g" $1
sed -i -e "s/ 2 / 3 /g" $1
sed -i -e "s/ 4 / 2 /g" $1
python3 mesh.py $1
