../meshgen $1
sed -i -e "s/ 3 / 4 /g" __mesh
sed -i -e "s/ 2 / 3 /g" __mesh
sed -i -e "s/ 4 / 2 /g" __mesh
python3 ../mesh4.py __mesh $2
