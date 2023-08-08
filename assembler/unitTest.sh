#!/bin/bash

c=0
s=0
t=0

make -s

for f in test/*.asm ; do
echo "--Testing $f" ;

((c=c+1))

if [ -f "${f%.asm}.hack" ] ; then
	rm "${f%.asm}.hack"
fi

./asm "$f"

if [ -f "${f%.asm}_gen.hack" ] ;
then
	cmp "${f%.asm}.hack" "${f%.asm}_gen.hack" && ((s=s+1))
	((t=t+1))
fi

done

echo "$c files assembled"
echo "$s/$t tests passed"
