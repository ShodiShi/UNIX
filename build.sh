#!/bin/sh

if [ $# -ne 1 ]; then
    echo "Usage: $0 source_file" >&2
    exit 1
fi

SOURCE_FILE=$(realpath "$1")
SOURCE_DIR=$(dirname "$SOURCE_FILE")
SOURCE_NAME=$(basename "$SOURCE_FILE")

if [ ! -f "$SOURCE_FILE" ]; then
    echo "Error: File '$SOURCE_FILE' does not exist" >&2
    exit 2
fi

TEMP_DIR=$(mktemp -d) || {
    echo "Error: Failed to create temp dir" >&2
    exit 3
}

clean_dir() {
    rm -rf "$TEMP_DIR"
    exit "$1"
}

trap 'clean_dir 130' INT
trap 'clean_dir 143' TERM
trap 'clean_dir $?' EXIT

case "$SOURCE_NAME" in
    *.c)   COMPILER="cc" ;;
    *.cpp) COMPILER="g++" ;;
    *.tex) COMPILER="pdflatex" ;;
    *)
        echo "Error: Unsupported file type" >&2
        clean_dir 4
        ;;
esac

OUTPUT=$(grep "Output:" "$SOURCE_FILE" | sed 's/.*Output:[[:space:]]*//' | head -n 1)

if [ -z "$OUTPUT" ]; then
    echo "Error: No output filename specified with Output:" >&2
    clean_dir 5
fi

cp "$SOURCE_FILE" "$TEMP_DIR/" || clean_dir 6
cd "$TEMP_DIR" || clean_dir 7

if [ "$COMPILER" = "pdflatex" ]; then

    $COMPILER -interaction=nonstopmode "$SOURCE_NAME" > /dev/null 2>compile_err
    RESULT_FILE="${SOURCE_NAME%.*}.pdf"
else
    # Для C/C++
    $COMPILER "$SOURCE_NAME" -o "$OUTPUT" 2>compile_err
    RESULT_FILE="$OUTPUT"
fi

if [ $? -ne 0 ]; then
    echo "Compilation failed:" >&2
    cat compile_err >&2
    clean_dir 8
fi

if [ "$COMPILER" = "pdflatex" ] && [ -f "$RESULT_FILE" ]; then
    mv "$RESULT_FILE" "$OUTPUT"
fi

if [ -f "$OUTPUT" ]; then
    mv "$OUTPUT" "$SOURCE_DIR/" || clean_dir 11
    echo "Success: $SOURCE_DIR/$OUTPUT created"
else
    echo "Error: Output file not found after compilation" >&2
    clean_dir 9
fi

clean_dir 0
