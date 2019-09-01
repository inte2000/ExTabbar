
echo copy %1 to project directory
if not exist %2 mkdir %2
copy /y %1 %2
