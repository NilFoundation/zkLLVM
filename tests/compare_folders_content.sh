# check that the number of files in both directories is equal
count1=$(find "$1" -type f | wc -l)
count2=$(find "$2" -type f | wc -l)

if [ "$count1" -eq "$count2" ]; then
    echo "The number of files in both directories is equal."
else
    echo "The number of files differs: $1 has $count1 files, $2 has $count2 files."
    exit 1
fi

# Check that both directories contain identical list of files
diff -qr "$1" "$2" | grep 'Only in' && exit 1
echo "Both directories contain files with the same names."

diff -r "$1" "$2"
if [ $? -ne 0 ]; then
    exit 1
fi
echo "Comparison completed."
