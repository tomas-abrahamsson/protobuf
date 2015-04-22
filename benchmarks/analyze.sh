#! /bin/bash
get-mbps() {
    awk '{print $NF}' | cut -dM -f1 | xargs printf " %s\n"
}

get-linepair() {
    awk -v n=$1 'BEGIN {i=1}
       /^Serialize/ { if (i==n) print $0;}
       /^Deserialize/ { if (i==n) print $0; i++}'
}

print-best() {
    awk -v inpuths="$1" -v outpuths="$2" 'BEGIN {
           nfiles=0;
           split(inpuths, headings);
           split(outpuths, outheadings);
           
         }
/^[a-z]/ { h=$0;
           if (h==headings[1]) { nfiles++; }
           if (!(h in headings)) { headings[length(headings)]=h; }
         }
/^ /     { ser[h][nfiles]=$0; getline; deser[h][nfiles]=$0; }
END      {
           for (hnum=1; hnum <= length(outheadings); hnum++)
           {
             h=outheadings[hnum];
             maxser=0;
             maxdeser=0;
             for (i=1; i <= nfiles; i++)
             {
               if (ser[h][i] > maxser)     { maxser = ser[h][i]; }
               if (deser[h][i] > maxdeser) { maxdeser = deser[h][i]; }
             }
             printf("%s (best of %d found values)\n", h, nfiles);
             if (maxser >= 100) printf("  %.1f\n", maxser);
             else               printf("  %.2f\n", maxser);
             if (maxdeser >= 100) printf("  %.1f\n", maxdeser);
             else                 printf("  %.2f\n", maxdeser);
           }
         }
'
}

get-java() {
    i=1
    inputf="$1"; shift
    for h in "$@"
    do
	echo "$h"
	egrep '(to|from) byte array' "$inputf" | get-linepair $i | get-mbps
	i=$(($i + 1))
    done
}

get-cc() {
    i=1
    inputf="$1"; shift
    for h in "$@"
    do
	echo "$h"
	egrep '(to|from) array' "$inputf" | get-linepair $i | get-mbps
	i=$(($i + 1))
    done
}

get-py() {
    i=1
    inputf="$1"; shift
    for h in "$@"
    do
	echo "$h"
	egrep '(to|from) string' "$inputf" | get-linepair $i | get-mbps
	i=$(($i + 1))
    done
}

analyze-it ()
case "$1" in
    java)
	shift
	for f in "$@"
	do
	    get-java  "$f" small,size small,speed large,size large,speed
	done | print-best \
	    "small,size small,speed large,size  large,speed" \
	    "small,size large,size  small,speed large,speed"
	;;
    cc)
	shift
	for f in "$@"
	do
	    get-cc  "$f" small,speed large,speed small,size large,size
	done | print-best \
	    "small,speed large,speed small,size large,size" \
	    "small,speed large,speed small,size large,size"
	;;
    cc-lite)
	shift
	for f in "$@"
	do
	    get-cc  "$f" small large
	done | print-best "small large" "small large"
	;;
    py)
	shift
	for f in "$@"
	do
	    get-py  "$f" small large
	done | print-best "small large" "small large"
	;;
esac

analyze-all () {
    echo C++ speed, size:
    analyze-it cc      log.benchmarks.cc-benchmarks.try-*.txt
    echo
    echo
    echo C++ lite:
    analyze-it cc-lite log.benchmarks.cc-lite-benchmarks.try-*.txt
    echo
    echo
    echo Python
    analyze-it py      log.benchmarks.python-benchmarks.try-*.txt
    echo
    echo
    echo Java
    analyze-it java    log.benchmarks.java-benchmarks.try-*.txt
    echo
    echo
}

benchmark-all () {
    n=$1
    [ -z "$n" ] && { echo "benchmark needs a number: num runs" >&2; exit 1; }

    echo "Running n=$n benchmarks"

    echo "Remember to fixate cpu speed!"
    printf "Type RET when done"
    read x

    for i in $(seq 1 $n)
    do
	bs="java-benchmarks cc-benchmarks cc-lite-benchmarks python-benchmarks"
	for b in $bs
	do
	    echo "Benchmarking ${b}..."
	    make $b 2>&1 | tee log.benchmarks.$b.try-$i.txt
	done
    done

    echo "All benchmarks done!"

    echo "Remember to un-fixate cpu speed!"
}

case $1 in
    analyze)
	analyze-all;;
    benchmark)
	benchmark-all "$2";;
    *)
	analyze-all;;
esac
