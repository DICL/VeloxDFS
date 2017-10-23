release_no=$(grep -oP "release: \K(v\d*.\d*.\d*)" <(git log -1 --pretty=%B))

if [ "$?" = "0" ]; then
  cmd="git tag -a $release_no -m \"`git log -1 --pretty=%B`\"" 
  echo "$cmd"
  eval $cmd
fi
