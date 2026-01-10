# Adjust environment setting to enable PODIO at runtime
#
# The PODIO environment variable must be set to use this
# script
if [ -z "$PODIO" ]; then
  echo "Please set the PODIO environment variable to the install location before sourcing this script."
  return
fi

unamestr=`uname`
if [[ "$unamestr" = 'Linux' ]]; then
  if ! echo $LD_LIBRARY_PATH | grep $PODIO/lib > /dev/null 2>&1; then
    # RedHat based put the libraries into lib64
    if [ -d $PODIO/lib64 ]; then
      export LD_LIBRARY_PATH=$PODIO/lib64:$LD_LIBRARY_PATH
    else
      export LD_LIBRARY_PATH=$PODIO/lib:$LD_LIBRARY_PATH
    fi
  fi
elif [[ "$unamestr" = 'Darwin' ]]; then
  # This currently does not work on OS X as DYLD_LIBRARY_PATH is ignored
  # in recent OS X versions
  if ! echo $DYLD_LIBRARY_PATH | grep -o $PODIO/lib > /dev/null 2>&1; then
      export DYLD_LIBRARY_PATH=$PODIO/lib:$DYLD_LIBRARY_PATH
    fi
fi

python_stem=python3.$(python -c 'import sys; print(str(sys.version_info[1]) + sys.abiflags )')/site-packages
if [ -d $PODIO/lib64/${python_stem} ]; then
  if ! echo $PYTHONPATH | grep -o $PODIO/lib64/${python_stem} > /dev/null 2>&1; then
    export PYTHONPATH=$PODIO/lib64/${python_stem}:$PYTHONPATH
  fi
else
  if ! echo $PYTHONPATH | grep -o $PODIO/lib/${python_stem} > /dev/null 2>&1; then
    export PYTHONPATH=$PODIO/lib/${python_stem}:$PYTHONPATH
  fi
fi

if ! echo $ROOT_INCLUDE_PATH | grep -o $PODIO/include > /dev/null 2>&1; then
  export ROOT_INCLUDE_PATH=$PODIO/include:$ROOT_INCLUDE_PATH
fi

if ! echo $PATH | grep $PODIO/bin > /dev/null 2>&1; then
  export PATH=$PODIO/bin:$PATH
fi

if ! echo $CMAKE_PREFIX_PATH | grep $PODIO > /dev/null 2>&1; then
  export CMAKE_PREFIX_PATH=$PODIO:$CMAKE_PREFIX_PATH
fi
