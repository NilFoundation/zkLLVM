SRC_DIR="build/examples/cpp"
DEST_DIR="$1"

if [ ! -d "$DEST_DIR" ]; then
    mkdir -p "$DEST_DIR"
fi

find "$SRC_DIR" -type f -name "*.crct*" -exec cp {} "$DEST_DIR" \;
find "$SRC_DIR" -type f -name "*.tbl*" -exec cp {} "$DEST_DIR" \;

echo "All .crct and .tbl files have been copied from $SRC_DIR to $DEST_DIR."
