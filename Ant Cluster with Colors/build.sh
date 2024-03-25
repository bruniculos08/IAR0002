# $n is n-th arg (like string) passed in "bash build.sh $1 $2 ... $n":
filename=$1
# basename receives 2 args, the filename and the extension (to remove it):
executable=$(basename "$filename" .cpp)
# Source: https://stackoverflow.com/questions/12152626/how-can-i-remove-the-extension-of-a-filename-in-a-shell-script
g++ "$executable".cpp -lSDL2 -o $executable && ./$executable
# The grep -v says "only allow filenames that don't contain a dot", and...
# ... the xargs rm says "then pass the list of filenames to rm":
ls | grep -v "\." | xargs rm
# Source: https://stackoverflow.com/questions/856463/how-to-remove-delete-executable-files-aka-files-without-extension-only