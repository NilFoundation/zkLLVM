SRC_DIR="$1"
DEST_DIR="$2"

if [ ! -d "$DEST_DIR" ]; then
    mkdir -p "$DEST_DIR"
fi

find "$SRC_DIR" -type f -name "*.crct*" -exec mv {} "$DEST_DIR" \;
find "$SRC_DIR" -type f -name "*.tbl*" -exec mv {} "$DEST_DIR" \;

echo "All .crct and .tbl files have been moved from $SRC_DIR to $DEST_DIR."
