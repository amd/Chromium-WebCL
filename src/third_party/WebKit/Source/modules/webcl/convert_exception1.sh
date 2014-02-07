for f in `ls *.h *.cpp`
do
	cat $f | sed 's/ec.throwDOMException(\(.*\));/ec.throwDOMException(\1, "\1");/g' > tmp/$f;
	mv tmp/$f $f
done
