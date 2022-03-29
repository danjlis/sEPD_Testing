for i in {0..31}
do
    if (( $i > 9 ))
    then
	dir="s$i"
        mkdir $dir
        cd $dir
        touch log.log
        cd ..
    else
	dir="s0$i"
	mkdir $dir
	cd $dir
	touch log.log
	cd ..
    fi
done

