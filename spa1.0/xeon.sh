
for i in cg.A is.A lu.A mg.A sp.A ep.A
	
	do 
	echo "starts *********$i**********"
	micnativeloadex ./$i -e "OMP_NUM_THREADS=40" -d 1&

	done;