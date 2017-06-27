for i in 1 2
		do
				sed -i '25d' conf.h
				sed -i '25i#	define POOL_COUNT			'$i'' conf.h

				for j in 24 12 6 3 1
				do 
					sed -i '32d' conf.h
				    sed -i '32i#	define CLUSTER_COUNT		'$j'' conf.h

					do
						./spa a 24 b 24 c 24 d 24 e 24 f 24 g 24 h 24 i 24 j 24 >> output.txt
						
					done;
				done;
		done;
done;

for i in 3
		do
				sed -i '25d' conf.h
				sed -i '25i#	define POOL_COUNT			'$i'' conf.h

				for j in 16 8 4 2 1
				do 
					sed -i '32d' conf.h
				    sed -i '32i#	define CLUSTER_COUNT		'$j'' conf.h

					do
						./spa a 24 b 24 c 24 d 24 e 24 f 24 g 24 h 24 i 24 j 24 >> output.txt
						
					done;
				done;
		done;
done;