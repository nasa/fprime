cp ../build-fprime-automatic-native/Ref/Top/RefTopologyAppAi.xml .
mkdir visual
cd visual
fpl-extract-xml < ../RefTopologyAppAi.xml
mkdir Ref
for file in `ls *.xml`
do
echo "laying out $file"
base=`basename $file .xml`
fpl-convert-xml $file | fpl-layout > Ref/$base.json
done