#script that converts a dia diagram file to an XML file.
#usage: ./convert.sh mydiagram.dia
# you will get mydiagram.xml

filename=`basename $1 .dia`
if [ $filename == "$1" ];
 then 
    echo "not a Dia diagram file.";
    exit; 
 fi;
cp "$1" "$filename.gz"
gunzip -c "$filename.gz"  > "$filename.xml"
